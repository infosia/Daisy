/* Copyright 2014-2015 Samsung Electronics Co., Ltd.
 * Copyright 2015 University of Szeged.
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
#include "ecma-array-object.h"
#include "ecma-builtin-helpers.h"
#include "ecma-builtins.h"
#include "ecma-conversion.h"
#include "ecma-exceptions.h"
#include "ecma-gc.h"
#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "ecma-objects.h"
#include "ecma-objects-general.h"
#include "ecma-try-catch-macro.h"
#include "jrt.h"

#define ECMA_BUILTINS_INTERNAL
#include "ecma-builtins-internal.h"

#define BUILTIN_INC_HEADER_NAME "ecma-builtin-object.inc.h"
#define BUILTIN_UNDERSCORED_ID object
#include "ecma-builtin-internal-routines-template.inc.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmabuiltins
 * @{
 *
 * \addtogroup object ECMA Object object built-in
 * @{
 */

/**
 * Handle calling [[Call]] of built-in Object object
 *
 * @return completion-value
 */
ecma_completion_value_t
ecma_builtin_object_dispatch_call (const ecma_value_t *arguments_list_p, /**< arguments list */
                                   ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (arguments_list_len == 0
      || ecma_is_value_undefined (arguments_list_p[0])
      || ecma_is_value_null (arguments_list_p[0]))
  {
    ret_value = ecma_builtin_object_dispatch_construct (arguments_list_p, arguments_list_len);
  }
  else
  {
    ret_value = ecma_op_to_object (arguments_list_p[0]);
  }

  return ret_value;
} /* ecma_builtin_object_dispatch_call */

/**
 * Handle calling [[Construct]] of built-in Object object
 *
 * @return completion-value
 */
ecma_completion_value_t
ecma_builtin_object_dispatch_construct (const ecma_value_t *arguments_list_p, /**< arguments list */
                                        ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  if (arguments_list_len == 0)
  {
    ecma_object_t *obj_p = ecma_op_create_object_object_noarg ();

    return ecma_make_normal_completion_value (ecma_make_object_value (obj_p));
  }
  else
  {
    ecma_completion_value_t new_obj_value = ecma_op_create_object_object_arg (arguments_list_p[0]);

    if (!ecma_is_completion_value_normal (new_obj_value))
    {
      return new_obj_value;
    }
    else
    {
      return ecma_make_normal_completion_value (ecma_get_completion_value_value (new_obj_value));
    }
  }
} /* ecma_builtin_object_dispatch_construct */

/**
 * The Object object's 'getPrototypeOf' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.2
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_get_prototype_of (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                             ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  /* 1. */
  if (!ecma_is_value_object (arg))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    /* 2. */
    ecma_object_t *obj_p = ecma_get_object_from_value (arg);
    ecma_object_t *prototype_p = ecma_get_object_prototype (obj_p);

    if (prototype_p)
    {
      ret_value = ecma_make_normal_completion_value (ecma_make_object_value (prototype_p));
      ecma_ref_object (prototype_p);
    }
    else
    {
      ret_value = ecma_make_simple_completion_value (ECMA_SIMPLE_VALUE_NULL);
    }
  }

  return ret_value;
} /* ecma_builtin_object_object_get_prototype_of */

/**
 * The Object object's 'getOwnPropertyNames' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.4
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_get_own_property_names (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                                   ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (!ecma_is_value_object (arg))
  {
    /* 1. */
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (arg);
    /* 2-5. */
    ret_value = ecma_builtin_helper_object_get_properties (obj_p, false);
  }

  return ret_value;
} /* ecma_builtin_object_object_get_own_property_names */

/**
 * The Object object's 'seal' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.8
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_seal (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                 ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  // 1.
  if (!ecma_is_value_object (arg))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    // 2.
    ecma_object_t *obj_p = ecma_get_object_from_value (arg);

    ecma_property_t *property_p;
    for (property_p = ecma_get_property_list (obj_p);
         property_p != NULL && ecma_is_completion_value_empty (ret_value);
         property_p = ECMA_GET_POINTER (ecma_property_t, property_p->next_property_p))
    {
      ecma_string_t *property_name_p;

      if (property_p->type == ECMA_PROPERTY_NAMEDDATA)
      {
        property_name_p = ECMA_GET_NON_NULL_POINTER (ecma_string_t,
                                                     property_p->u.named_data_property.name_p);
      }
      else if (property_p->type == ECMA_PROPERTY_NAMEDACCESSOR)
      {
        property_name_p = ECMA_GET_NON_NULL_POINTER (ecma_string_t,
                                                     property_p->u.named_accessor_property.name_p);
      }
      else
      {
        continue;
      }

      // 2.a
      ecma_property_descriptor_t prop_desc = ecma_get_property_descriptor_from_property (property_p);

      // 2.b
      if (ecma_is_property_configurable (property_p))
      {
        prop_desc.is_configurable = false;
      }

      // 2.c
      ECMA_TRY_CATCH (define_own_prop_ret,
                      ecma_op_object_define_own_property (obj_p,
                                                          property_name_p,
                                                          &prop_desc,
                                                          true),
                      ret_value);
      ECMA_FINALIZE (define_own_prop_ret);

      ecma_free_property_descriptor (&prop_desc);
    }

    if (ecma_is_completion_value_empty (ret_value))
    {
      // 3.
      ecma_set_object_extensible (obj_p, false);

      // 4.
      ret_value = ecma_make_normal_completion_value (ecma_copy_value (arg, true));
    }
  }

  return ret_value;
} /* ecma_builtin_object_object_seal */

/**
 * The Object object's 'freeze' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.9
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_freeze (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                   ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  // 1.
  if (!ecma_is_value_object (arg))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    // 2.
    ecma_object_t *obj_p = ecma_get_object_from_value (arg);

    ecma_property_t *property_p;
    for (property_p = ecma_get_property_list (obj_p);
         property_p != NULL && ecma_is_completion_value_empty (ret_value);
         property_p = ECMA_GET_POINTER (ecma_property_t, property_p->next_property_p))
    {
      ecma_string_t *property_name_p;

      if (property_p->type == ECMA_PROPERTY_NAMEDDATA)
      {
        property_name_p = ECMA_GET_NON_NULL_POINTER (ecma_string_t,
                                                     property_p->u.named_data_property.name_p);
      }
      else
      if (property_p->type == ECMA_PROPERTY_NAMEDACCESSOR)
      {
        property_name_p = ECMA_GET_NON_NULL_POINTER (ecma_string_t,
                                                     property_p->u.named_accessor_property.name_p);
      }
      else
      {
        continue;
      }

      // 2.a
      ecma_property_descriptor_t prop_desc = ecma_get_property_descriptor_from_property (property_p);

      // 2.b
      if (property_p->type == ECMA_PROPERTY_NAMEDDATA && ecma_is_property_writable (property_p))
      {
        prop_desc.is_writable = false;
      }

      // 2.c
      if (ecma_is_property_configurable (property_p))
      {
        prop_desc.is_configurable = false;
      }

      // 2.d
      ECMA_TRY_CATCH (define_own_prop_ret,
                      ecma_op_object_define_own_property (obj_p,
                                                          property_name_p,
                                                          &prop_desc,
                                                          true),
                      ret_value);
      ECMA_FINALIZE (define_own_prop_ret);

      ecma_free_property_descriptor (&prop_desc);
    }

    if (ecma_is_completion_value_empty (ret_value))
    {
      // 3.
      ecma_set_object_extensible (obj_p, false);

      // 4.
      ret_value = ecma_make_normal_completion_value (ecma_copy_value (arg, true));
    }
  }

  return ret_value;
} /* ecma_builtin_object_object_freeze */

/**
 * The Object object's 'preventExtensions' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.10
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_prevent_extensions (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                               ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (!ecma_is_value_object (arg))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (arg);
    ecma_set_object_extensible (obj_p, false);

    ret_value = ecma_make_normal_completion_value (ecma_copy_value (arg, true));
  }

  return ret_value;
} /* ecma_builtin_object_object_prevent_extensions */

/**
 * The Object object's 'isSealed' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.11
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_is_sealed (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                      ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  // 1.
  if (!ecma_is_value_object (arg))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (arg);
    ecma_property_t *property_p;

    // This will be the result if the other steps doesn't change it.
    bool sealed = false;

    // 3.
    // The pseudo code contains multiple early return but this way we get the same
    // result.
    if (!ecma_get_object_extensible (obj_p))
    {
      sealed = true;
    }

    // 2.
    for (property_p = ecma_get_property_list (obj_p);
         property_p != NULL && sealed;
         property_p = ECMA_GET_POINTER (ecma_property_t, property_p->next_property_p))
    {
      if (property_p->type == ECMA_PROPERTY_INTERNAL)
      {
        continue;
      }

      JERRY_ASSERT (property_p->type == ECMA_PROPERTY_NAMEDDATA || property_p->type == ECMA_PROPERTY_NAMEDACCESSOR);

      // 2.b
      if (ecma_is_property_configurable (property_p))
      {
        sealed = false;
        break;
      }
    }

    // 4.
    ret_value = ecma_make_simple_completion_value (sealed
                                                   ? ECMA_SIMPLE_VALUE_TRUE
                                                   : ECMA_SIMPLE_VALUE_FALSE);
  }

  return ret_value;
} /* ecma_builtin_object_object_is_sealed */

/**
 * The Object object's 'isFrozen' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.12
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_is_frozen (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                      ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  // 1.
  if (!ecma_is_value_object (arg))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (arg);
    ecma_property_t *property_p;

    // This will be the result if the other steps doesn't change it.
    bool frozen = false;

    // 3.
    // The pseudo code contains multiple early return but this way we get the same
    // result.
    if (!ecma_get_object_extensible (obj_p))
    {
      frozen = true;
    }

    // 2.
    for (property_p = ecma_get_property_list (obj_p);
         property_p != NULL && frozen;
         property_p = ECMA_GET_POINTER (ecma_property_t, property_p->next_property_p))
    {
      if (property_p->type == ECMA_PROPERTY_INTERNAL)
      {
        continue;
      }

      JERRY_ASSERT (property_p->type == ECMA_PROPERTY_NAMEDDATA || property_p->type == ECMA_PROPERTY_NAMEDACCESSOR);

      // 2.b
      if (property_p->type == ECMA_PROPERTY_NAMEDDATA && ecma_is_property_writable (property_p))
      {
        frozen = false;
        break;
      }

      // 2.c
      if (ecma_is_property_configurable (property_p))
      {
        frozen = false;
        break;
      }
    }

    // 4.
    ret_value = ecma_make_simple_completion_value (frozen
                                                   ? ECMA_SIMPLE_VALUE_TRUE
                                                   : ECMA_SIMPLE_VALUE_FALSE);
  }

  return ret_value;
} /* ecma_builtin_object_object_is_frozen */

/**
 * The Object object's 'isExtensible' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.13
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_is_extensible (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                          ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (!ecma_is_value_object (arg))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (arg);

    bool extensible = ecma_get_object_extensible (obj_p);

    ret_value = ecma_make_simple_completion_value (extensible
                                                   ? ECMA_SIMPLE_VALUE_TRUE
                                                   : ECMA_SIMPLE_VALUE_FALSE);
  }

  return ret_value;
} /* ecma_builtin_object_object_is_extensible */

/**
 * The Object object's 'keys' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.14
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_keys (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                 ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (!ecma_is_value_object (arg))
  {
    /* 1. */
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (arg);
    /* 3-6. */
    ret_value = ecma_builtin_helper_object_get_properties (obj_p, true);
  }

  return ret_value;
} /* ecma_builtin_object_object_keys */

/**
 * The Object object's 'getOwnPropertyDescriptor' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.3
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_get_own_property_descriptor (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                                        ecma_value_t arg1, /**< routine's first argument */
                                                        ecma_value_t arg2) /**< routine's second argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  // 1.
  if (!ecma_is_value_object (arg1))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
    return ret_value;
  }

  ecma_object_t *obj_p = ecma_get_object_from_value (arg1);

  // 2.
  ECMA_TRY_CATCH (name_str_value,
                  ecma_op_to_string (arg2),
                  ret_value);

  ecma_string_t *name_str_p = ecma_get_string_from_value (name_str_value);

  // 3.
  ecma_property_t *prop_p = ecma_op_object_get_own_property (obj_p, name_str_p);

  if (prop_p != NULL)
  {
    ecma_property_descriptor_t prop_desc = ecma_get_property_descriptor_from_property (prop_p);

    // 4.
    ecma_object_t* desc_obj_p = ecma_op_from_property_descriptor (&prop_desc);

    ecma_free_property_descriptor (&prop_desc);

    ret_value = ecma_make_normal_completion_value (ecma_make_object_value (desc_obj_p));
  }
  else
  {
    ret_value = ecma_make_simple_completion_value (ECMA_SIMPLE_VALUE_UNDEFINED);
  }

  ECMA_FINALIZE (name_str_value);

  return ret_value;
} /* ecma_builtin_object_object_get_own_property_descriptor */

/**
 * The Object object's 'create' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.5
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_create (ecma_value_t this_arg, /**< 'this' argument */
                                   ecma_value_t arg1, /**< routine's first argument */
                                   ecma_value_t arg2) /**< routine's second argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  // 1.
  if (!ecma_is_value_object (arg1) && !ecma_is_value_null (arg1))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = NULL;

    if (!ecma_is_value_null (arg1))
    {
      obj_p = ecma_get_object_from_value (arg1);
    }
    // 2-3.
    ecma_object_t *result_obj_p = ecma_op_create_object_object_noarg_and_set_prototype (obj_p);

    // 4.
    if (!ecma_is_value_undefined (arg2))
    {
      ECMA_TRY_CATCH (obj,
                      ecma_builtin_object_object_define_properties (this_arg,
                                                                    ecma_make_object_value (result_obj_p),
                                                                    arg2),
                      ret_value);
      ECMA_FINALIZE (obj);
    }

    // 5.
    if (ecma_is_completion_value_empty (ret_value))
    {
      ret_value = ecma_make_normal_completion_value (ecma_copy_value (ecma_make_object_value (result_obj_p),
                                                                      true));
    }

    ecma_deref_object (result_obj_p);
  }

  return ret_value;
} /* ecma_builtin_object_object_create */

/**
 * The Object object's 'defineProperties' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.7
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_define_properties (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                              ecma_value_t arg1, /**< routine's first argument */
                                              ecma_value_t arg2) /**< routine's second argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  // 1.
  if (!ecma_is_value_object (arg1))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (arg1);

    // 2.
    ECMA_TRY_CATCH (props,
                    ecma_op_to_object (arg2),
                    ret_value);

    ecma_object_t *props_p = ecma_get_object_from_value (props);
    ecma_property_t *property_p;

    // First we need to know how many properties should be stored
    uint32_t property_number = 0;
    for (property_p = ecma_get_property_list (props_p);
         property_p != NULL;
         property_p = ECMA_GET_POINTER (ecma_property_t, property_p->next_property_p))
    {
      if ((property_p->type == ECMA_PROPERTY_NAMEDDATA || property_p->type == ECMA_PROPERTY_NAMEDACCESSOR)
          && ecma_is_property_enumerable (property_p))
      {
        property_number++;
      }
    }

    // 3.
    MEM_DEFINE_LOCAL_ARRAY (property_names_p, property_number, ecma_string_t*);

    uint32_t index = 0;
    for (property_p = ecma_get_property_list (props_p);
         property_p != NULL;
         property_p = ECMA_GET_POINTER (ecma_property_t, property_p->next_property_p))
    {
      ecma_string_t *property_name_p;

      if (property_p->type == ECMA_PROPERTY_NAMEDDATA)
      {
        property_name_p = ECMA_GET_NON_NULL_POINTER (ecma_string_t,
                                                     property_p->u.named_data_property.name_p);
      }
      else if (property_p->type == ECMA_PROPERTY_NAMEDACCESSOR)
      {
        property_name_p = ECMA_GET_NON_NULL_POINTER (ecma_string_t,
                                                     property_p->u.named_accessor_property.name_p);
      }
      else
      {
        continue;
      }

      if (ecma_is_property_enumerable (property_p))
      {
        property_names_p[index++] = ecma_copy_or_ref_ecma_string (property_name_p);
      }
    }

    // 4.
    MEM_DEFINE_LOCAL_ARRAY (property_descriptors, property_number, ecma_property_descriptor_t);
    uint32_t property_descriptor_number = 0;

    for (index = 0;
         index < property_number && ecma_is_completion_value_empty (ret_value);
         index++)
    {
      // 5.a
      ECMA_TRY_CATCH (desc_obj,
                      ecma_op_object_get (props_p, property_names_p[index]),
                      ret_value);

      // 5.b
      ECMA_TRY_CATCH (conv_result,
                      ecma_op_to_property_descriptor (desc_obj,
                                                      &property_descriptors[index]),
                      ret_value);

      property_descriptor_number++;

      ECMA_FINALIZE (conv_result);
      ECMA_FINALIZE (desc_obj);
    }

    // 6.
    for (index = 0;
         index < property_number && ecma_is_completion_value_empty (ret_value);
         index++)
    {
      ECMA_TRY_CATCH (define_own_prop_ret,
                      ecma_op_object_define_own_property (obj_p,
                                                          property_names_p[index],
                                                          &property_descriptors[index],
                                                          true),
                      ret_value);

      ECMA_FINALIZE (define_own_prop_ret);
    }

    // Clean up
    for (index = 0;
         index < property_descriptor_number;
         index++)
    {
      ecma_free_property_descriptor (&property_descriptors[index]);
    }

    MEM_FINALIZE_LOCAL_ARRAY (property_descriptors);

    for (index = 0;
         index < property_number;
         index++)
    {
      ecma_deref_ecma_string (property_names_p[index]);
    }

    MEM_FINALIZE_LOCAL_ARRAY (property_names_p);

    // 7.
    if (ecma_is_completion_value_empty (ret_value))
    {
      ret_value = ecma_make_normal_completion_value (ecma_copy_value (arg1, true));
    }

    ECMA_FINALIZE (props);
  }

  return ret_value;
} /* ecma_builtin_object_object_define_properties */

/**
 * The Object object's 'defineProperty' routine
 *
 * See also:
 *          ECMA-262 v5, 15.2.3.6
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_object_object_define_property (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                            ecma_value_t arg1, /**< routine's first argument */
                                            ecma_value_t arg2, /**< routine's second argument */
                                            ecma_value_t arg3) /**< routine's third argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (!ecma_is_value_object (arg1))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (arg1);

    ECMA_TRY_CATCH (name_str_value,
                    ecma_op_to_string (arg2),
                    ret_value);

    ecma_string_t *name_str_p = ecma_get_string_from_value (name_str_value);

    ecma_property_descriptor_t prop_desc;

    ECMA_TRY_CATCH (conv_result,
                    ecma_op_to_property_descriptor (arg3, &prop_desc),
                    ret_value);

    ECMA_TRY_CATCH (define_own_prop_ret,
                    ecma_op_object_define_own_property (obj_p,
                                                        name_str_p,
                                                        &prop_desc,
                                                        true),
                    ret_value);

    ret_value = ecma_make_normal_completion_value (ecma_copy_value (arg1, true));

    ECMA_FINALIZE (define_own_prop_ret);
    ecma_free_property_descriptor (&prop_desc);
    ECMA_FINALIZE (conv_result);
    ECMA_FINALIZE (name_str_value);
  }

  return ret_value;
} /* ecma_builtin_object_object_define_property */

/**
 * @}
 * @}
 * @}
 */
