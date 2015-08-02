/* Copyright 2014-2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ecma-alloc.h"
#include "ecma-builtins.h"
#include "ecma-exceptions.h"
#include "ecma-function-object.h"
#include "ecma-gc.h"
#include "ecma-helpers.h"
#include "ecma-lex-env.h"
#include "ecma-objects.h"
#include "ecma-objects-general.h"
#include "ecma-objects-arguments.h"
#include "ecma-try-catch-macro.h"

#define JERRY_INTERNAL
#include "jerry-internal.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmafunctionobject ECMA Function object related routines
 * @{
 */

/**
 * Pack 'is_strict', 'do_instantiate_arguments_object' flags and instruction position to value
 * that can be stored in an [[Code]] internal property.
 *
 * @return packed value
 */
static uint32_t
ecma_pack_code_internal_property_value (bool is_strict, /**< is code strict? */
                                        bool do_instantiate_args_obj, /**< should an Arguments object be
                                                                       *   instantiated for the code */
                                        vm_instr_counter_t instr_oc) /**< position of first instruction */
{
  uint32_t value = instr_oc;
  const uint32_t is_strict_bit_offset = (uint32_t) (sizeof (value) * JERRY_BITSINBYTE - 1);
  const uint32_t do_instantiate_arguments_object_bit_offset = (uint32_t) (sizeof (value) * JERRY_BITSINBYTE - 2);

  JERRY_ASSERT (((value) & (1u << is_strict_bit_offset)) == 0);
  JERRY_ASSERT (((value) & (1u << do_instantiate_arguments_object_bit_offset)) == 0);

  if (is_strict)
  {
    value |= (1u << is_strict_bit_offset);
  }

  if (do_instantiate_args_obj)
  {
    value |= (1u << do_instantiate_arguments_object_bit_offset);
  }

  return value;
} /* ecma_pack_code_internal_property_value */

/**
 * Unpack 'is_strict', 'do_instantiate_arguments_object' flags and instruction position from value
 * that can be stored in an [[Code]] internal property.
 *
 * @return instruction position
 */
static vm_instr_counter_t
ecma_unpack_code_internal_property_value (uint32_t value, /**< packed value */
                                          bool* out_is_strict_p, /**< out: is code strict? */
                                          bool* out_do_instantiate_args_obj_p) /**< should an Arguments object be
                                                                                *   instantiated for the code */
{
  JERRY_ASSERT (out_is_strict_p != NULL);
  JERRY_ASSERT (out_do_instantiate_args_obj_p != NULL);

  const uint32_t is_strict_bit_offset = (uint32_t) (sizeof (value) * JERRY_BITSINBYTE - 1);
  const uint32_t do_instantiate_arguments_object_bit_offset = (uint32_t) (sizeof (value) * JERRY_BITSINBYTE - 2);

  *out_is_strict_p = ((value & (1u << is_strict_bit_offset)) != 0);
  *out_do_instantiate_args_obj_p = ((value & (1u << do_instantiate_arguments_object_bit_offset)) != 0);
  value &= ~((1u << is_strict_bit_offset) | (1u << do_instantiate_arguments_object_bit_offset));

  return (vm_instr_counter_t) value;
} /* ecma_unpack_code_internal_property_value */

/**
 * IsCallable operation.
 *
 * See also: ECMA-262 v5, 9.11
 *
 * @return true, if value is callable object;
 *         false - otherwise.
 */
bool
ecma_op_is_callable (ecma_value_t value) /**< ecma-value */
{
  if (!ecma_is_value_object (value))
  {
    return false;
  }

  ecma_object_t *obj_p = ecma_get_object_from_value (value);

  JERRY_ASSERT (obj_p != NULL);
  JERRY_ASSERT (!ecma_is_lexical_environment (obj_p));

  return (ecma_get_object_type (obj_p) == ECMA_OBJECT_TYPE_FUNCTION
          || ecma_get_object_type (obj_p) == ECMA_OBJECT_TYPE_BOUND_FUNCTION
          || ecma_get_object_type (obj_p) == ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION
          || ecma_get_object_type (obj_p) == ECMA_OBJECT_TYPE_BUILT_IN_FUNCTION);
} /* ecma_op_is_callable */

/**
 * Check whether the value is Object that implements [[Construct]].
 *
 * @return true, if value is constructor object;
 *         false - otherwise.
 */
bool
ecma_is_constructor (ecma_value_t value) /**< ecma-value */
{
  if (!ecma_is_value_object (value))
  {
    return false;
  }

  ecma_object_t *obj_p = ecma_get_object_from_value (value);

  JERRY_ASSERT (obj_p != NULL);
  JERRY_ASSERT (!ecma_is_lexical_environment (obj_p));

  return (ecma_get_object_type (obj_p) == ECMA_OBJECT_TYPE_FUNCTION
          || ecma_get_object_type (obj_p) == ECMA_OBJECT_TYPE_BOUND_FUNCTION
          || ecma_get_object_type (obj_p) == ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION);
} /* ecma_is_constructor */

/**
 * Helper function to merge argument lists
 *
 * See also:
 *          ECMA-262 v5, 15.3.4.5.1 step 4
 *          ECMA-262 v5, 15.3.4.5.2 step 4
 *
 * Used by:
 *         - [[Call]] implementation for Function objects.
 *         - [[Construct]] implementation for Function objects.
 *
 * @return ecma_value_t* - pointer to the merged argument list.
 */
static ecma_value_t*
ecma_function_bind_merge_arg_lists (ecma_object_t *func_obj_p, /**< Function object */
                                    const ecma_value_t *arguments_list_p, /**< arguments list */
                                    ecma_length_t arguments_list_len, /**< length of arguments list */
                                    ecma_length_t *total_args_count) /**< length of the merged argument list */
{
  ecma_value_t *arg_list_p;
  ecma_length_t bound_args_count = 0;

  ecma_property_t *bound_args_prop_p;
  bound_args_prop_p = ecma_find_internal_property (func_obj_p, ECMA_INTERNAL_PROPERTY_BOUND_FUNCTION_BOUND_ARGS);

  if (bound_args_prop_p != NULL)
  {
    ecma_collection_header_t *bound_arg_list_p = ECMA_GET_POINTER (ecma_collection_header_t,
                                                                   bound_args_prop_p->u.internal_property.value);

    ecma_collection_iterator_t bound_args_iterator;
    ecma_collection_iterator_init (&bound_args_iterator, bound_arg_list_p);

    bound_args_count = bound_arg_list_p->unit_number;

    *total_args_count = bound_args_count + arguments_list_len;

    const size_t arg_list_size = (size_t) *total_args_count * sizeof (ecma_value_t);
    arg_list_p = static_cast <ecma_value_t *> (mem_heap_alloc_block (arg_list_size, MEM_HEAP_ALLOC_SHORT_TERM));

    for (ecma_length_t i = 0; i < bound_args_count; i++)
    {
      bool is_moved = ecma_collection_iterator_next (&bound_args_iterator);
      JERRY_ASSERT (is_moved);

      arg_list_p[i] = *bound_args_iterator.current_value_p;
    }
  }
  else
  {
    *total_args_count = arguments_list_len;

    const size_t arg_list_size = (size_t) *total_args_count * sizeof (ecma_value_t);
    arg_list_p = static_cast <ecma_value_t *> (mem_heap_alloc_block (arg_list_size, MEM_HEAP_ALLOC_SHORT_TERM));
  }

  for (ecma_length_t i = 0; i < arguments_list_len; i++)
  {
    arg_list_p[i + bound_args_count] = arguments_list_p[i];
  }

  return arg_list_p;
} /* ecma_function_bind_merge_arg_lists */

/**
 * Function object creation operation.
 *
 * See also: ECMA-262 v5, 13.2
 *
 * @return pointer to newly created Function object
 */
ecma_object_t*
ecma_op_create_function_object (ecma_string_t* formal_parameter_list_p[], /**< formal parameters list */
                                ecma_length_t formal_parameters_number, /**< formal parameters list's length */
                                ecma_object_t *scope_p, /**< function's scope */
                                bool is_strict, /**< 'strict' flag */
                                bool do_instantiate_arguments_object, /**< should an Arguments object be instantiated
                                                                       *   for the function object upon call */
                                const vm_instr_t *instrs_p, /**< byte-code array */
                                vm_instr_counter_t first_instr_pos) /**< position of first instruction
                                                                     *   of function's body */
{
  // 1., 4., 13.
  ecma_object_t *prototype_obj_p = ecma_builtin_get (ECMA_BUILTIN_ID_FUNCTION_PROTOTYPE);

  ecma_object_t *f = ecma_create_object (prototype_obj_p, true, ECMA_OBJECT_TYPE_FUNCTION);

  ecma_deref_object (prototype_obj_p);

  // 2., 6., 7., 8.
  /*
   * We don't setup [[Get]], [[Call]], [[Construct]], [[HasInstance]] for each function object.
   * Instead we set the object's type to ECMA_OBJECT_TYPE_FUNCTION
   * that defines which version of the routine should be used on demand.
   */

  // 3.
  /*
   * [[Class]] property is not stored explicitly for objects of ECMA_OBJECT_TYPE_FUNCTION type.
   *
   * See also: ecma_object_get_class_name
   */

  // 9.
  ecma_property_t *scope_prop_p = ecma_create_internal_property (f, ECMA_INTERNAL_PROPERTY_SCOPE);
  ECMA_SET_POINTER (scope_prop_p->u.internal_property.value, scope_p);

  // 10., 11.
  ecma_property_t *formal_parameters_prop_p = ecma_create_internal_property (f,
                                                                             ECMA_INTERNAL_PROPERTY_FORMAL_PARAMETERS);
  if (formal_parameters_number != 0)
  {
    /*
     * Reverse formal parameter list
     */
    for (ecma_length_t i = 0; i < formal_parameters_number / 2; i++)
    {
      ecma_string_t *tmp_p = formal_parameter_list_p[i];
      formal_parameter_list_p[i] = formal_parameter_list_p[formal_parameters_number - 1u - i];
      formal_parameter_list_p[formal_parameters_number - 1u - i] = tmp_p;
    }

    ecma_collection_header_t *formal_parameters_collection_p = ecma_new_strings_collection (formal_parameter_list_p,
                                                                                            formal_parameters_number);
    ECMA_SET_POINTER (formal_parameters_prop_p->u.internal_property.value, formal_parameters_collection_p);
  }
  else
  {
    JERRY_ASSERT (formal_parameters_prop_p->u.internal_property.value == ECMA_NULL_POINTER);
  }

  // 12.
  ecma_property_t *bytecode_prop_p = ecma_create_internal_property (f, ECMA_INTERNAL_PROPERTY_CODE_BYTECODE);
  MEM_CP_SET_NON_NULL_POINTER (bytecode_prop_p->u.internal_property.value, instrs_p);

  ecma_property_t *code_prop_p = ecma_create_internal_property (f, ECMA_INTERNAL_PROPERTY_CODE_FLAGS_AND_OFFSET);
  code_prop_p->u.internal_property.value = ecma_pack_code_internal_property_value (is_strict,
                                                                                   do_instantiate_arguments_object,
                                                                                   first_instr_pos);

  // 14.
  ecma_number_t* len_p = ecma_alloc_number ();
  *len_p = ecma_uint32_to_number (formal_parameters_number);

  // 15.
  ecma_property_descriptor_t length_prop_desc = ecma_make_empty_property_descriptor ();
  length_prop_desc.is_value_defined = true;
  length_prop_desc.value = ecma_make_number_value (len_p);

  ecma_string_t* magic_string_length_p = ecma_get_magic_string (LIT_MAGIC_STRING_LENGTH);
  ecma_completion_value_t completion = ecma_op_object_define_own_property (f,
                                                                           magic_string_length_p,
                                                                           &length_prop_desc,
                                                                           false);
  ecma_deref_ecma_string (magic_string_length_p);

  JERRY_ASSERT (ecma_is_completion_value_normal_true (completion)
                || ecma_is_completion_value_normal_false (completion));

  ecma_dealloc_number (len_p);
  len_p = NULL;

  // 16.
  ecma_object_t *proto_p = ecma_op_create_object_object_noarg ();

  // 17.
  ecma_property_descriptor_t prop_desc = ecma_make_empty_property_descriptor ();
  {
    prop_desc.is_value_defined = true;
    prop_desc.value = ecma_make_object_value (f);

    prop_desc.is_writable_defined = true;
    prop_desc.is_writable = true;

    prop_desc.is_enumerable_defined = true;
    prop_desc.is_enumerable = false;

    prop_desc.is_configurable_defined = true;
    prop_desc.is_configurable = true;
  }

  ecma_string_t *magic_string_constructor_p = ecma_get_magic_string (LIT_MAGIC_STRING_CONSTRUCTOR);
  ecma_op_object_define_own_property (proto_p,
                                      magic_string_constructor_p,
                                      &prop_desc,
                                      false);
  ecma_deref_ecma_string (magic_string_constructor_p);

  // 18.
  prop_desc.value = ecma_make_object_value (proto_p);
  prop_desc.is_configurable = false;
  ecma_string_t *magic_string_prototype_p = ecma_get_magic_string (LIT_MAGIC_STRING_PROTOTYPE);
  ecma_op_object_define_own_property (f,
                                      magic_string_prototype_p,
                                      &prop_desc,
                                      false);
  ecma_deref_ecma_string (magic_string_prototype_p);

  ecma_deref_object (proto_p);

  // 19.
  if (is_strict)
  {
    ecma_object_t *thrower_p = ecma_builtin_get (ECMA_BUILTIN_ID_TYPE_ERROR_THROWER);

    prop_desc = ecma_make_empty_property_descriptor ();
    {
      prop_desc.is_enumerable_defined = true;
      prop_desc.is_enumerable = false;

      prop_desc.is_configurable_defined = true;
      prop_desc.is_configurable = false;

      prop_desc.is_get_defined = true;
      prop_desc.get_p = thrower_p;

      prop_desc.is_set_defined = true;
      prop_desc.set_p = thrower_p;
    }

    ecma_string_t *magic_string_caller_p = ecma_get_magic_string (LIT_MAGIC_STRING_CALLER);
    ecma_op_object_define_own_property (f,
                                        magic_string_caller_p,
                                        &prop_desc,
                                        false);
    ecma_deref_ecma_string (magic_string_caller_p);

    ecma_string_t *magic_string_arguments_p = ecma_get_magic_string (LIT_MAGIC_STRING_ARGUMENTS);
    ecma_op_object_define_own_property (f,
                                        magic_string_arguments_p,
                                        &prop_desc,
                                        false);
    ecma_deref_ecma_string (magic_string_arguments_p);

    ecma_deref_object (thrower_p);
  }

  return f;
} /* ecma_op_create_function_object */

/**
 * External function object creation operation.
 *
 * Note:
 *      external function object is implementation-defined object type
 *      that represent functions implemented in native code, using Embedding API
 *
 * @return pointer to newly created external function object
 */
ecma_object_t*
ecma_op_create_external_function_object (ecma_external_pointer_t code_p) /**< pointer to external native handler */
{
  ecma_object_t *prototype_obj_p = ecma_builtin_get (ECMA_BUILTIN_ID_FUNCTION_PROTOTYPE);

  ecma_object_t *function_obj_p = ecma_create_object (prototype_obj_p, true, ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION);

  ecma_deref_object (prototype_obj_p);

  /*
   * [[Class]] property is not stored explicitly for objects of ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION type.
   *
   * See also: ecma_object_get_class_name
   */

  bool is_created = ecma_create_external_pointer_property (function_obj_p,
                                                           ECMA_INTERNAL_PROPERTY_NATIVE_CODE,
                                                           (ecma_external_pointer_t) code_p);
  JERRY_ASSERT (is_created);

  ecma_property_descriptor_t prop_desc = ecma_make_empty_property_descriptor ();
  {
    prop_desc.is_value_defined = true;
    prop_desc.value = ecma_make_simple_value (ECMA_SIMPLE_VALUE_UNDEFINED);

    prop_desc.is_writable_defined = true;
    prop_desc.is_writable = true;

    prop_desc.is_enumerable_defined = true;
    prop_desc.is_enumerable = false;

    prop_desc.is_configurable_defined = true;
    prop_desc.is_configurable = false;
  }

  ecma_string_t *magic_string_prototype_p = ecma_get_magic_string (LIT_MAGIC_STRING_PROTOTYPE);
  ecma_op_object_define_own_property (function_obj_p,
                                      magic_string_prototype_p,
                                      &prop_desc,
                                      false);
  ecma_deref_ecma_string (magic_string_prototype_p);

  return function_obj_p;
} /* ecma_op_create_external_function_object */

/**
 * Setup variables for arguments listed in formal parameter list,
 * and, if necessary, Arguments object with 'arguments' binding.
 *
 * See also:
 *          Declaration binding instantiation (ECMA-262 v5, 10.5), block 4 and 7
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
static ecma_completion_value_t
ecma_function_call_setup_args_variables (ecma_object_t *func_obj_p, /**< Function object */
                                         ecma_object_t *env_p, /**< lexical environment */
                                         const ecma_value_t *arguments_list_p, /**< arguments list */
                                         ecma_length_t arguments_list_len, /**< length of argument list */
                                         bool is_strict, /**< flag indicating strict mode */
                                         bool do_instantiate_arguments_object) /**< flag indicating whether
                                                                                *   Arguments object should be
                                                                                *   instantiated */
{
  ecma_property_t *formal_parameters_prop_p = ecma_get_internal_property (func_obj_p,
                                                                          ECMA_INTERNAL_PROPERTY_FORMAL_PARAMETERS);
  ecma_collection_header_t *formal_parameters_p;
  formal_parameters_p = ECMA_GET_POINTER (ecma_collection_header_t,
                                          formal_parameters_prop_p->u.internal_property.value);

  if (formal_parameters_p != NULL)
  {
    ecma_length_t formal_parameters_count = formal_parameters_p->unit_number;

    ecma_collection_iterator_t formal_params_iterator;
    ecma_collection_iterator_init (&formal_params_iterator, formal_parameters_p);

    /*
     * Formal parameter list is stored in reversed order
     *
     * Although, specification defines ascending order of formal parameters list enumeration,
     * implementation enumerates the parameters in descending order.
     *
     * In the case, redundant SetMutableBinding invocation could be avoided.
     */
    for (ssize_t n = (ssize_t) formal_parameters_count - 1;
         n >= 0;
         n--)
    {
      ecma_value_t v;
      if (n >= (ssize_t) arguments_list_len)
      {
        v = ecma_make_simple_value (ECMA_SIMPLE_VALUE_UNDEFINED);
      }
      else
      {
        v = arguments_list_p[n];
      }

      bool is_moved = ecma_collection_iterator_next (&formal_params_iterator);
      JERRY_ASSERT (is_moved);

      ecma_value_t formal_parameter_name_value = *formal_params_iterator.current_value_p;
      ecma_string_t *formal_parameter_name_string_p = ecma_get_string_from_value (formal_parameter_name_value);

      bool arg_already_declared = ecma_op_has_binding (env_p, formal_parameter_name_string_p);
      if (!arg_already_declared)
      {
        ecma_completion_value_t completion = ecma_op_create_mutable_binding (env_p,
                                                                             formal_parameter_name_string_p,
                                                                             false);
        if (ecma_is_completion_value_throw (completion))
        {
          return completion;
        }

        JERRY_ASSERT (ecma_is_completion_value_empty (completion));

        completion = ecma_op_set_mutable_binding (env_p,
                                                  formal_parameter_name_string_p,
                                                  v,
                                                  is_strict);

        if (ecma_is_completion_value_throw (completion))
        {
          return completion;
        }

        JERRY_ASSERT (ecma_is_completion_value_empty (completion));
      }
    }
  }

  if (do_instantiate_arguments_object)
  {
    /*
     * According to ECMA-262 v5, 10.5, the Arguments object should be instantiated
     * after instantiating declared functions, and only if there is no binding named 'arguments'
     * by that time.
     *
     * However, we can setup Arguments object and 'arguments' binding here, because:
     *  - instantiation of Arguments object itself doesn't have any side effects;
     *  - if 'arguments' is name of a declared function in current scope,
     *    value of the binding would be overwritten, execution would proceed in correct state.
     *  - declaration of function, named 'arguments', is considered to be unrecommended (and so, rare) case,
     *    so instantiation of Arguments object here, in general, is supposed to not affect resource consumption
     *    significantly.
     */

    ecma_string_t *arguments_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_ARGUMENTS);

    bool binding_already_declared = ecma_op_has_binding (env_p, arguments_string_p);

    if (!binding_already_declared)
    {
      ecma_object_t *args_obj_p = ecma_op_create_arguments_object (func_obj_p,
                                                                   env_p,
                                                                   formal_parameters_p,
                                                                   arguments_list_p,
                                                                   arguments_list_len,
                                                                   is_strict);

      if (is_strict)
      {
        ecma_op_create_immutable_binding (env_p, arguments_string_p);
        ecma_op_initialize_immutable_binding (env_p, arguments_string_p, ecma_make_object_value (args_obj_p));
      }
      else
      {
        ecma_completion_value_t completion = ecma_op_create_mutable_binding (env_p,
                                                                             arguments_string_p,
                                                                             false);
        JERRY_ASSERT (ecma_is_completion_value_empty (completion));

        completion = ecma_op_set_mutable_binding (env_p,
                                                  arguments_string_p,
                                                  ecma_make_object_value (args_obj_p),
                                                  false);
        JERRY_ASSERT (ecma_is_completion_value_empty (completion));
      }

      ecma_deref_object (args_obj_p);
    }

    ecma_deref_ecma_string (arguments_string_p);
  }

  return ecma_make_empty_completion_value ();
} /* ecma_function_call_setup_args_variables */

/**
 * [[Call]] implementation for Function objects,
 * created through 13.2 (ECMA_OBJECT_TYPE_FUNCTION)
 * or 15.3.4.5 (ECMA_OBJECT_TYPE_BOUND_FUNCTION),
 * and for built-in Function objects
 * from section 15 (ECMA_OBJECT_TYPE_BUILT_IN_FUNCTION).
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_op_function_has_instance (ecma_object_t *func_obj_p, /**< Function object */
                               ecma_value_t value) /**< argument 'V' */
{
  JERRY_ASSERT (func_obj_p != NULL
                && !ecma_is_lexical_environment (func_obj_p));

  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_FUNCTION)
  {
    if (!ecma_is_value_object (value))
    {
      return ecma_make_simple_completion_value (ECMA_SIMPLE_VALUE_FALSE);
    }

    ecma_object_t* v_obj_p = ecma_get_object_from_value (value);

    ecma_string_t *prototype_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_PROTOTYPE);

    ECMA_TRY_CATCH (prototype_obj_value,
                    ecma_op_object_get (func_obj_p, prototype_magic_string_p),
                    ret_value);

    if (!ecma_is_value_object (prototype_obj_value))
    {
      ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
    }
    else
    {
      ecma_object_t *prototype_obj_p = ecma_get_object_from_value (prototype_obj_value);
      JERRY_ASSERT (prototype_obj_p != NULL);

      do
      {
        v_obj_p = ecma_get_object_prototype (v_obj_p);

        if (v_obj_p == NULL)
        {
          ret_value = ecma_make_simple_completion_value (ECMA_SIMPLE_VALUE_FALSE);

          break;
        }
        else if (v_obj_p == prototype_obj_p)
        {
          ret_value = ecma_make_simple_completion_value (ECMA_SIMPLE_VALUE_TRUE);

          break;
        }
      } while (true);
    }

    ECMA_FINALIZE (prototype_obj_value);

    ecma_deref_ecma_string (prototype_magic_string_p);
  }
  else if (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_BUILT_IN_FUNCTION)
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    JERRY_ASSERT (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_BOUND_FUNCTION);

    /* 1. */
    ecma_property_t *target_function_prop_p;
    target_function_prop_p = ecma_get_internal_property (func_obj_p,
                                                         ECMA_INTERNAL_PROPERTY_BOUND_FUNCTION_TARGET_FUNCTION);

    ecma_object_t *target_func_obj_p = ECMA_GET_NON_NULL_POINTER (ecma_object_t,
                                                                  target_function_prop_p->u.internal_property.value);

    /* 3. */
    ret_value = ecma_op_object_has_instance (target_func_obj_p, value);
  }

  return ret_value;
} /* ecma_op_function_has_instance */

/**
 * [[Call]] implementation for Function objects,
 * created through 13.2 (ECMA_OBJECT_TYPE_FUNCTION)
 * or 15.3.4.5 (ECMA_OBJECT_TYPE_BOUND_FUNCTION),
 * and for built-in Function objects
 * from section 15 (ECMA_OBJECT_TYPE_BUILT_IN_FUNCTION).
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_op_function_call (ecma_object_t *func_obj_p, /**< Function object */
                       ecma_value_t this_arg_value, /**< 'this' argument's value */
                       const ecma_value_t* arguments_list_p, /**< arguments list */
                       ecma_length_t arguments_list_len) /**< length of arguments list */
{
  JERRY_ASSERT (func_obj_p != NULL
                && !ecma_is_lexical_environment (func_obj_p));
  JERRY_ASSERT (ecma_op_is_callable (ecma_make_object_value (func_obj_p)));
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_FUNCTION)
  {
    if (unlikely (ecma_get_object_is_builtin (func_obj_p)))
    {
      ret_value = ecma_builtin_dispatch_call (func_obj_p,
                                              this_arg_value,
                                              arguments_list_p,
                                              arguments_list_len);
    }
    else
    {
      /* Entering Function Code (ECMA-262 v5, 10.4.3) */
      ecma_property_t *scope_prop_p = ecma_get_internal_property (func_obj_p, ECMA_INTERNAL_PROPERTY_SCOPE);
      ecma_property_t *bytecode_prop_p = ecma_get_internal_property (func_obj_p, ECMA_INTERNAL_PROPERTY_CODE_BYTECODE);
      ecma_property_t *code_prop_p = ecma_get_internal_property (func_obj_p,
                                                                 ECMA_INTERNAL_PROPERTY_CODE_FLAGS_AND_OFFSET);

      ecma_object_t *scope_p = ECMA_GET_NON_NULL_POINTER (ecma_object_t,
                                                          scope_prop_p->u.internal_property.value);
      uint32_t code_prop_value = code_prop_p->u.internal_property.value;

      // 8.
      bool is_strict;
      bool do_instantiate_args_obj;
      const vm_instr_t *instrs_p = MEM_CP_GET_POINTER (const vm_instr_t, bytecode_prop_p->u.internal_property.value);
      vm_instr_counter_t code_first_instr_pos = ecma_unpack_code_internal_property_value (code_prop_value,
                                                                                          &is_strict,
                                                                                          &do_instantiate_args_obj);

      ecma_value_t this_binding;
      // 1.
      if (is_strict)
      {
        this_binding = ecma_copy_value (this_arg_value, true);
      }
      else if (ecma_is_value_undefined (this_arg_value)
               || ecma_is_value_null (this_arg_value))
      {
        // 2.
        this_binding = ecma_make_object_value (ecma_builtin_get (ECMA_BUILTIN_ID_GLOBAL));
      }
      else
      {
        // 3., 4.
        ecma_completion_value_t completion = ecma_op_to_object (this_arg_value);
        JERRY_ASSERT (ecma_is_completion_value_normal (completion));

        this_binding = ecma_get_completion_value_value (completion);
      }

      // 5.
      ecma_object_t *local_env_p = ecma_create_decl_lex_env (scope_p);

      // 9.
      ECMA_TRY_CATCH (args_var_declaration_ret,
                      ecma_function_call_setup_args_variables (func_obj_p,
                                                               local_env_p,
                                                               arguments_list_p,
                                                               arguments_list_len,
                                                               is_strict,
                                                               do_instantiate_args_obj),
                      ret_value);

      ecma_completion_value_t completion = vm_run_from_pos (instrs_p,
                                                            code_first_instr_pos,
                                                            this_binding,
                                                            local_env_p,
                                                            is_strict,
                                                            false);

      if (ecma_is_completion_value_return (completion))
      {
        ret_value = ecma_make_normal_completion_value (ecma_get_completion_value_value (completion));
      }
      else
      {
        ret_value = completion;
      }

      ECMA_FINALIZE (args_var_declaration_ret);

      ecma_deref_object (local_env_p);
      ecma_free_value (this_binding, true);
    }
  }
  else if (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_BUILT_IN_FUNCTION)
  {
    ret_value = ecma_builtin_dispatch_call (func_obj_p,
                                            this_arg_value,
                                            arguments_list_p,
                                            arguments_list_len);
  }
  else if (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION)
  {
    ecma_external_pointer_t handler_p;
    bool is_retrieved = ecma_get_external_pointer_value (func_obj_p,
                                                         ECMA_INTERNAL_PROPERTY_NATIVE_CODE,
                                                         &handler_p);
    JERRY_ASSERT (is_retrieved);

    ret_value = jerry_dispatch_external_function (func_obj_p,
                                                  handler_p,
                                                  this_arg_value,
                                                  arguments_list_p,
                                                  arguments_list_len);
  }
  else
  {
    JERRY_ASSERT (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_BOUND_FUNCTION);

    /* 2-3. */
    ecma_property_t *bound_this_prop_p;
    ecma_property_t *target_function_prop_p;

    bound_this_prop_p = ecma_get_internal_property (func_obj_p, ECMA_INTERNAL_PROPERTY_BOUND_FUNCTION_BOUND_THIS);
    target_function_prop_p = ecma_get_internal_property (func_obj_p,
                                                         ECMA_INTERNAL_PROPERTY_BOUND_FUNCTION_TARGET_FUNCTION);

    ecma_object_t *target_func_obj_p = ECMA_GET_NON_NULL_POINTER (ecma_object_t,
                                                                  target_function_prop_p->u.internal_property.value);

    ecma_length_t total_args_count;

    /* 4. */
    ecma_value_t *arg_list_p = ecma_function_bind_merge_arg_lists (func_obj_p,
                                                                   arguments_list_p,
                                                                   arguments_list_len,
                                                                   &total_args_count);

    ecma_value_t bound_this_value = bound_this_prop_p->u.internal_property.value;

    /* 5. */
    ret_value = ecma_op_function_call (target_func_obj_p,
                                       bound_this_value,
                                       arg_list_p,
                                       total_args_count);

    if (arg_list_p != NULL)
    {
      mem_heap_free_block (arg_list_p);
    }
  }

  JERRY_ASSERT (!ecma_is_completion_value_empty (ret_value));
  return ret_value;
} /* ecma_op_function_call */

/**
 * [[Construct]] implementation for Function objects (13.2.2),
 * created through 13.2 (ECMA_OBJECT_TYPE_FUNCTION) and
 * externally defined (host) functions (ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION).
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
static ecma_completion_value_t
ecma_op_function_construct_simple_or_external (ecma_object_t *func_obj_p, /**< Function object */
                                               const ecma_value_t* arguments_list_p, /**< arguments list */
                                               ecma_length_t arguments_list_len) /**< length of arguments list */
{
  JERRY_ASSERT (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_FUNCTION
                || ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION);

  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  ecma_string_t *prototype_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_PROTOTYPE);

  // 5.
  ECMA_TRY_CATCH (func_obj_prototype_prop_value,
                  ecma_op_object_get (func_obj_p,
                                      prototype_magic_string_p),
                  ret_value);

  // 1., 2., 4.
  ecma_object_t *obj_p;
  if (ecma_is_value_object (func_obj_prototype_prop_value))
  {
    //  6.
    obj_p = ecma_create_object (ecma_get_object_from_value (func_obj_prototype_prop_value),
                                true,
                                ECMA_OBJECT_TYPE_GENERAL);
  }
  else
  {
    // 7.
    ecma_object_t *prototype_p = ecma_builtin_get (ECMA_BUILTIN_ID_OBJECT_PROTOTYPE);

    obj_p = ecma_create_object (prototype_p, true, ECMA_OBJECT_TYPE_GENERAL);

    ecma_deref_object (prototype_p);
  }

  // 3.
  /*
   * [[Class]] property of ECMA_OBJECT_TYPE_GENERAL type objects
   * without ECMA_INTERNAL_PROPERTY_CLASS internal property
   * is "Object".
   *
   * See also: ecma_object_get_class_name.
   */

  // 8.
  ECMA_TRY_CATCH (call_completion,
                  ecma_op_function_call (func_obj_p,
                                         ecma_make_object_value (obj_p),
                                         arguments_list_p,
                                         arguments_list_len),
                  ret_value);

  ecma_value_t obj_value;

  // 9.
  if (ecma_is_value_object (call_completion))
  {
    obj_value = ecma_copy_value (call_completion, true);
  }
  else
  {
    // 10.
    ecma_ref_object (obj_p);
    obj_value = ecma_make_object_value (obj_p);
  }

  ret_value = ecma_make_normal_completion_value (obj_value);

  ECMA_FINALIZE (call_completion);

  ecma_deref_object (obj_p);

  ECMA_FINALIZE (func_obj_prototype_prop_value);

  ecma_deref_ecma_string (prototype_magic_string_p);

  return ret_value;
} /* ecma_op_function_construct_simple_or_external */

/**
 * [[Construct]] implementation:
 *   13.2.2 - for Function objects, created through 13.2 (ECMA_OBJECT_TYPE_FUNCTION),
 *            and externally defined host functions (ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION);
 *   15.3.4.5.1 - for Function objects, created through 15.3.4.5 (ECMA_OBJECT_TYPE_BOUND_FUNCTION).
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_op_function_construct (ecma_object_t *func_obj_p, /**< Function object */
                            const ecma_value_t* arguments_list_p, /**< arguments list */
                            ecma_length_t arguments_list_len) /**< length of arguments list */
{
  JERRY_ASSERT (func_obj_p != NULL
                && !ecma_is_lexical_environment (func_obj_p));
  JERRY_ASSERT (ecma_is_constructor (ecma_make_object_value (func_obj_p)));
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_FUNCTION)
  {
    if (unlikely (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_FUNCTION
                  && ecma_get_object_is_builtin (func_obj_p)))
    {
      ret_value = ecma_builtin_dispatch_construct (func_obj_p, arguments_list_p, arguments_list_len);
    }
    else
    {
      ret_value = ecma_op_function_construct_simple_or_external (func_obj_p, arguments_list_p, arguments_list_len);
    }
  }
  else if (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_EXTERNAL_FUNCTION)
  {
    ret_value = ecma_op_function_construct_simple_or_external (func_obj_p, arguments_list_p, arguments_list_len);
  }
  else
  {
    JERRY_ASSERT (ecma_get_object_type (func_obj_p) == ECMA_OBJECT_TYPE_BOUND_FUNCTION);

    /* 1. */
    ecma_property_t *target_function_prop_p;
    target_function_prop_p = ecma_get_internal_property (func_obj_p,
                                                         ECMA_INTERNAL_PROPERTY_BOUND_FUNCTION_TARGET_FUNCTION);

    ecma_object_t *target_func_obj_p = ECMA_GET_NON_NULL_POINTER (ecma_object_t,
                                                                  target_function_prop_p->u.internal_property.value);

    /* 2. */
    if (!ecma_is_constructor (ecma_make_object_value (target_func_obj_p)))
    {
      ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
    }
    else
    {
      ecma_length_t total_args_count;

      /* 4. */
      ecma_value_t *arg_list_p = ecma_function_bind_merge_arg_lists (func_obj_p,
                                                                     arguments_list_p,
                                                                     arguments_list_len,
                                                                     &total_args_count);

      /* 5. */
      ret_value = ecma_op_function_construct (target_func_obj_p,
                                              arg_list_p,
                                              total_args_count);
      if (arg_list_p != NULL)
      {
        mem_heap_free_block (arg_list_p);
      }
    }
  }

  return ret_value;
} /* ecma_op_function_construct */

/**
 * Function declaration.
 *
 * See also: ECMA-262 v5, 10.5 - Declaration binding instantiation (block 5).
 *
 * @return completion value
 *         returned value must be freed with ecma_free_completion_value.
 */
ecma_completion_value_t
ecma_op_function_declaration (ecma_object_t *lex_env_p, /**< lexical environment */
                              ecma_string_t *function_name_p, /**< function name */
                              const vm_instr_t *instrs_p, /**< byte-code array */
                              vm_instr_counter_t function_first_instr_pos, /**< position of first instruction
                                                                            *   of function code */
                              ecma_string_t* formal_parameter_list_p[], /**< formal parameters list */
                              ecma_length_t formal_parameter_list_length, /**< length of formal parameters list */
                              bool is_strict, /**< flag indicating if function is declared in strict mode code */
                              bool do_instantiate_arguments_object, /**< flag, indicating whether an Arguments object
                                                                     *   should be instantiated for the function object
                                                                     *   upon call */
                              bool is_configurable_bindings) /**< flag indicating whether function
                                                              *   is declared in eval code */
{
  // b.
  ecma_object_t *func_obj_p = ecma_op_create_function_object (formal_parameter_list_p,
                                                              formal_parameter_list_length,
                                                              lex_env_p,
                                                              is_strict,
                                                              do_instantiate_arguments_object,
                                                              instrs_p,
                                                              function_first_instr_pos);

  // c.
  bool func_already_declared = ecma_op_has_binding (lex_env_p, function_name_p);

  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  // d.
  if (!func_already_declared)
  {
    ecma_completion_value_t completion = ecma_op_create_mutable_binding (lex_env_p,
                                                                         function_name_p,
                                                                         is_configurable_bindings);

    JERRY_ASSERT (ecma_is_completion_value_empty (completion));
  }
  else if (ecma_is_lexical_environment_global (lex_env_p))
  {
    // e.
    ecma_object_t *glob_obj_p = ecma_builtin_get (ECMA_BUILTIN_ID_GLOBAL);

    ecma_property_t *existing_prop_p = ecma_op_object_get_property (glob_obj_p, function_name_p);

    if (ecma_is_property_configurable (existing_prop_p))
    {
      ecma_property_descriptor_t property_desc = ecma_make_empty_property_descriptor ();
      {
        property_desc.is_value_defined = true;
        property_desc.value = ecma_make_simple_value (ECMA_SIMPLE_VALUE_UNDEFINED);

        property_desc.is_writable_defined = true;
        property_desc.is_writable = true;

        property_desc.is_enumerable_defined = true;
        property_desc.is_enumerable = true;

        property_desc.is_configurable_defined = true;
        property_desc.is_configurable = is_configurable_bindings;
      }

      ecma_completion_value_t completion = ecma_op_object_define_own_property (glob_obj_p,
                                                                               function_name_p,
                                                                               &property_desc,
                                                                               true);
      JERRY_ASSERT (ecma_is_completion_value_normal_true (completion));
    }
    else if (existing_prop_p->type == ECMA_PROPERTY_NAMEDACCESSOR)
    {
      ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
    }
    else
    {
      JERRY_ASSERT (existing_prop_p->type == ECMA_PROPERTY_NAMEDDATA);

      if (!ecma_is_property_writable (existing_prop_p)
          || !ecma_is_property_enumerable (existing_prop_p))
      {
        ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
      }
    }

    ecma_deref_object (glob_obj_p);
  }

  if (ecma_is_completion_value_empty (ret_value))
  {
    // f.
    ret_value = ecma_op_set_mutable_binding (lex_env_p,
                                             function_name_p,
                                             ecma_make_object_value (func_obj_p),
                                             is_strict);
  }
  else
  {
    JERRY_ASSERT (ecma_is_completion_value_throw (ret_value));
  }

  ecma_deref_object (func_obj_p);

  return ret_value;
} /* ecma_op_function_declaration */

/**
 * @}
 * @}
 */
