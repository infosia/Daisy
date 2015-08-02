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

/**
 * Implementation of ECMA-defined conversion routines
 */

#include "ecma-alloc.h"
#include "ecma-boolean-object.h"
#include "ecma-conversion.h"
#include "ecma-exceptions.h"
#include "ecma-function-object.h"
#include "ecma-gc.h"
#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "ecma-number-object.h"
#include "ecma-objects.h"
#include "ecma-objects-general.h"
#include "ecma-string-object.h"
#include "ecma-try-catch-macro.h"
#include "jrt-libc-includes.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmaconversion ECMA conversion routines
 * @{
 */

/**
 * CheckObjectCoercible operation.
 *
 * See also:
 *          ECMA-262 v5, 9.10
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_op_check_object_coercible (ecma_value_t value) /**< ecma-value */
{
  ecma_check_value_type_is_spec_defined (value);

  if (ecma_is_value_undefined (value)
      || ecma_is_value_null (value))
  {
    return ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    return ecma_make_empty_completion_value ();
  }
} /* ecma_op_check_object_coercible */

/**
 * SameValue operation.
 *
 * See also:
 *          ECMA-262 v5, 9.12
 *
 * @return true - if the value are same according to ECMA-defined SameValue algorithm,
 *         false - otherwise.
 */
bool
ecma_op_same_value (ecma_value_t x, /**< ecma-value */
                    ecma_value_t y) /**< ecma-value */
{
  const bool is_x_undefined = ecma_is_value_undefined (x);
  const bool is_x_null = ecma_is_value_null (x);
  const bool is_x_boolean = ecma_is_value_boolean (x);
  const bool is_x_number = ecma_is_value_number (x);
  const bool is_x_string = ecma_is_value_string (x);
  const bool is_x_object = ecma_is_value_object (x);

  const bool is_y_undefined = ecma_is_value_undefined (y);
  const bool is_y_null = ecma_is_value_null (y);
  const bool is_y_boolean = ecma_is_value_boolean (y);
  const bool is_y_number = ecma_is_value_number (y);
  const bool is_y_string = ecma_is_value_string (y);
  const bool is_y_object = ecma_is_value_object (y);

  const bool is_types_equal = ((is_x_undefined && is_y_undefined)
                               || (is_x_null && is_y_null)
                               || (is_x_boolean && is_y_boolean)
                               || (is_x_number && is_y_number)
                               || (is_x_string && is_y_string)
                               || (is_x_object && is_y_object));

  if (!is_types_equal)
  {
    return false;
  }
  else if (is_x_undefined || is_x_null)
  {
    return true;
  }
  else if (is_x_number)
  {
    ecma_number_t *x_num_p = ecma_get_number_from_value (x);
    ecma_number_t *y_num_p = ecma_get_number_from_value (y);

    bool is_x_nan = ecma_number_is_nan (*x_num_p);
    bool is_y_nan = ecma_number_is_nan (*y_num_p);

    if (is_x_nan || is_y_nan)
    {
      /*
       * If both are NaN
       *   return true;
       * else
       *   // one of the numbers is NaN, and another - is not
       *   return false;
       */
      return (is_x_nan && is_y_nan);
    }
    else if (ecma_number_is_zero (*x_num_p)
             && ecma_number_is_zero (*y_num_p)
             && ecma_number_is_negative (*x_num_p) != ecma_number_is_negative (*y_num_p))
    {
      return false;
    }
    else
    {
      return (*x_num_p == *y_num_p);
    }
  }
  else if (is_x_string)
  {
    ecma_string_t* x_str_p = ecma_get_string_from_value (x);
    ecma_string_t* y_str_p = ecma_get_string_from_value (y);

    return ecma_compare_ecma_strings (x_str_p, y_str_p);
  }
  else if (is_x_boolean)
  {
    return (ecma_is_value_true (x) == ecma_is_value_true (y));
  }
  else
  {
    JERRY_ASSERT (is_x_object);

    return (ecma_get_object_from_value (x) == ecma_get_object_from_value (y));
  }
} /* ecma_op_same_value */

/**
 * ToPrimitive operation.
 *
 * See also:
 *          ECMA-262 v5, 9.1
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_op_to_primitive (ecma_value_t value, /**< ecma-value */
                      ecma_preferred_type_hint_t preferred_type) /**< preferred type hint */
{
  ecma_check_value_type_is_spec_defined (value);

  if (ecma_is_value_object (value))
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (value);

    return ecma_op_object_default_value (obj_p, preferred_type);
  }
  else
  {
    return ecma_make_normal_completion_value (ecma_copy_value (value, true));
  }
} /* ecma_op_to_primitive */

/**
 * ToBoolean operation.
 *
 * See also:
 *          ECMA-262 v5, 9.2
 *
 * @return completion value
 *         Returned value is simple and so need not be freed.
 *         However, ecma_free_completion_value may be called for it, but it is a no-op.
 */
ecma_completion_value_t
ecma_op_to_boolean (ecma_value_t value) /**< ecma-value */
{
  ecma_check_value_type_is_spec_defined (value);

  ecma_simple_value_t ret_value;

  if (ecma_is_value_boolean (value))
  {
    ret_value = (ecma_is_value_true (value) ?
                 ECMA_SIMPLE_VALUE_TRUE : ECMA_SIMPLE_VALUE_FALSE);
  }
  else if (ecma_is_value_undefined (value)
           || ecma_is_value_null (value))
  {
    ret_value = ECMA_SIMPLE_VALUE_FALSE;
  }
  else if (ecma_is_value_number (value))
  {
    ecma_number_t *num_p = ecma_get_number_from_value (value);

    if (ecma_number_is_nan (*num_p)
        || ecma_number_is_zero (*num_p))
    {
      ret_value = ECMA_SIMPLE_VALUE_FALSE;
    }
    else
    {
      ret_value = ECMA_SIMPLE_VALUE_TRUE;
    }
  }
  else if (ecma_is_value_string (value))
  {
    ecma_string_t *str_p = ecma_get_string_from_value (value);

    if (ecma_string_get_length (str_p) == 0)
    {
      ret_value = ECMA_SIMPLE_VALUE_FALSE;
    }
    else
    {
      ret_value = ECMA_SIMPLE_VALUE_TRUE;
    }
  }
  else
  {
    JERRY_ASSERT (ecma_is_value_object (value));

    ret_value = ECMA_SIMPLE_VALUE_TRUE;
  }

  return ecma_make_simple_completion_value (ret_value);
} /* ecma_op_to_boolean */

/**
 * ToNumber operation.
 *
 * See also:
 *          ECMA-262 v5, 9.3
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_op_to_number (ecma_value_t value) /**< ecma-value */
{
  ecma_check_value_type_is_spec_defined (value);

  if (ecma_is_value_number (value))
  {
    return ecma_make_normal_completion_value (ecma_copy_value (value, true));
  }
  else if (ecma_is_value_string (value))
  {
    ecma_string_t *str_p = ecma_get_string_from_value (value);

    ecma_number_t *num_p = ecma_alloc_number ();
    *num_p = ecma_string_to_number (str_p);

    return ecma_make_normal_completion_value (ecma_make_number_value (num_p));
  }
  else if (ecma_is_value_object (value))
  {
    ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

    ECMA_TRY_CATCH (primitive_value,
                    ecma_op_to_primitive (value, ECMA_PREFERRED_TYPE_NUMBER),
                    ret_value);

    ret_value = ecma_op_to_number (primitive_value);

    ECMA_FINALIZE (primitive_value);

    return ret_value;
  }
  else
  {
    ecma_number_t *num_p = ecma_alloc_number ();

    if (ecma_is_value_undefined (value))
    {
      *num_p = ecma_number_make_nan ();
    }
    else if (ecma_is_value_null (value))
    {
      *num_p = ECMA_NUMBER_ZERO;
    }
    else
    {
      JERRY_ASSERT (ecma_is_value_boolean (value));

      if (ecma_is_value_true (value))
      {
        *num_p = ECMA_NUMBER_ONE;
      }
      else
      {
        *num_p = ECMA_NUMBER_ZERO;
      }
    }

    return ecma_make_normal_completion_value (ecma_make_number_value (num_p));
  }
} /* ecma_op_to_number */

/**
 * ToString operation.
 *
 * See also:
 *          ECMA-262 v5, 9.8
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_op_to_string (ecma_value_t value) /**< ecma-value */
{
  ecma_check_value_type_is_spec_defined (value);

  if (unlikely (ecma_is_value_object (value)))
  {
    ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

    ECMA_TRY_CATCH (prim_value,
                    ecma_op_to_primitive (value, ECMA_PREFERRED_TYPE_STRING),
                    ret_value);

    ret_value = ecma_op_to_string (prim_value);

    ECMA_FINALIZE (prim_value);

    return ret_value;
  }
  else
  {
    ecma_string_t *res_p = NULL;

    if (ecma_is_value_string (value))
    {
      res_p = ecma_get_string_from_value (value);
      res_p = ecma_copy_or_ref_ecma_string (res_p);
    }
    else if (ecma_is_value_number (value))
    {
      ecma_number_t *num_p = ecma_get_number_from_value (value);
      res_p = ecma_new_ecma_string_from_number (*num_p);
    }
    else if (ecma_is_value_undefined (value))
    {
      res_p = ecma_get_magic_string (LIT_MAGIC_STRING_UNDEFINED);
    }
    else if (ecma_is_value_null (value))
    {
      res_p = ecma_get_magic_string (LIT_MAGIC_STRING_NULL);
    }
    else
    {
      JERRY_ASSERT (ecma_is_value_boolean (value));

      if (ecma_is_value_true (value))
      {
        res_p = ecma_get_magic_string (LIT_MAGIC_STRING_TRUE);
      }
      else
      {
        res_p = ecma_get_magic_string (LIT_MAGIC_STRING_FALSE);
      }
    }

    return ecma_make_normal_completion_value (ecma_make_string_value (res_p));
  }
} /* ecma_op_to_string */

/**
 * ToObject operation.
 *
 * See also:
 *          ECMA-262 v5, 9.9
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_op_to_object (ecma_value_t value) /**< ecma-value */
{
  ecma_check_value_type_is_spec_defined (value);

  if (ecma_is_value_number (value))
  {
    return ecma_op_create_number_object (value);
  }
  else if (ecma_is_value_string (value))
  {
    return ecma_op_create_string_object (&value, 1);
  }
  else if (ecma_is_value_object (value))
  {
    return ecma_make_normal_completion_value (ecma_copy_value (value, true));
  }
  else
  {
    if (ecma_is_value_undefined (value)
        || ecma_is_value_null (value))
    {
      return ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
    }
    else
    {
      JERRY_ASSERT (ecma_is_value_boolean (value));

      return ecma_op_create_boolean_object (value);
    }
  }
} /* ecma_op_to_object */

/**
 * FromPropertyDescriptor operation.
 *
 * See also:
 *          ECMA-262 v5, 8.10.4
 *
 * @return constructed object
 */
ecma_object_t*
ecma_op_from_property_descriptor (const ecma_property_descriptor_t* src_prop_desc_p) /**< property descriptor */
{
  // 2.
  ecma_object_t *obj_p = ecma_op_create_object_object_noarg ();

  ecma_completion_value_t completion;
  ecma_property_descriptor_t prop_desc = ecma_make_empty_property_descriptor ();
  {
    prop_desc.is_value_defined = true;

    prop_desc.is_writable_defined = true;
    prop_desc.is_writable = true;

    prop_desc.is_enumerable_defined = true;
    prop_desc.is_enumerable = true;

    prop_desc.is_configurable_defined = true;
    prop_desc.is_configurable = true;
  }

  // 3.
  if (src_prop_desc_p->is_value_defined
      || src_prop_desc_p->is_writable_defined)
  {
    JERRY_ASSERT (prop_desc.is_value_defined && prop_desc.is_writable_defined);

    // a.
    prop_desc.value = src_prop_desc_p->value;

    ecma_string_t *value_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_VALUE);
    completion = ecma_op_object_define_own_property (obj_p,
                                                     value_magic_string_p,
                                                     &prop_desc,
                                                     false);
    ecma_deref_ecma_string (value_magic_string_p);
    JERRY_ASSERT (ecma_is_completion_value_normal_true (completion));

    // b.
    const bool is_writable = (src_prop_desc_p->is_writable);
    prop_desc.value = ecma_make_simple_value (is_writable ? ECMA_SIMPLE_VALUE_TRUE
                                                          : ECMA_SIMPLE_VALUE_FALSE);

    ecma_string_t *writable_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_WRITABLE);
    completion = ecma_op_object_define_own_property (obj_p,
                                                     writable_magic_string_p,
                                                     &prop_desc,
                                                     false);
    ecma_deref_ecma_string (writable_magic_string_p);
    JERRY_ASSERT (ecma_is_completion_value_normal_true (completion));
  }
  else
  {
    // 4.
    JERRY_ASSERT (src_prop_desc_p->is_get_defined
                  || src_prop_desc_p->is_set_defined);

    // a.
    if (src_prop_desc_p->get_p == NULL)
    {
      prop_desc.value = ecma_make_simple_value (ECMA_SIMPLE_VALUE_UNDEFINED);
    }
    else
    {
      prop_desc.value = ecma_make_object_value (src_prop_desc_p->get_p);
    }

    ecma_string_t *get_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_GET);
    completion = ecma_op_object_define_own_property (obj_p,
                                                     get_magic_string_p,
                                                     &prop_desc,
                                                     false);
    ecma_deref_ecma_string (get_magic_string_p);
    JERRY_ASSERT (ecma_is_completion_value_normal_true (completion));

    // b.
    if (src_prop_desc_p->set_p == NULL)
    {
      prop_desc.value = ecma_make_simple_value (ECMA_SIMPLE_VALUE_UNDEFINED);
    }
    else
    {
      prop_desc.value = ecma_make_object_value (src_prop_desc_p->set_p);
    }

    ecma_string_t *set_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_SET);
    completion = ecma_op_object_define_own_property (obj_p,
                                                     set_magic_string_p,
                                                     &prop_desc,
                                                     false);
    ecma_deref_ecma_string (set_magic_string_p);
    JERRY_ASSERT (ecma_is_completion_value_normal_true (completion));
  }

  const bool is_enumerable = src_prop_desc_p->is_enumerable;
  prop_desc.value = ecma_make_simple_value (is_enumerable ? ECMA_SIMPLE_VALUE_TRUE
                                            : ECMA_SIMPLE_VALUE_FALSE);

  ecma_string_t *enumerable_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_ENUMERABLE);
  completion = ecma_op_object_define_own_property (obj_p,
                                                   enumerable_magic_string_p,
                                                   &prop_desc,
                                                   false);
  ecma_deref_ecma_string (enumerable_magic_string_p);
  JERRY_ASSERT (ecma_is_completion_value_normal_true (completion));

  const bool is_configurable = src_prop_desc_p->is_configurable;
  prop_desc.value = ecma_make_simple_value (is_configurable ? ECMA_SIMPLE_VALUE_TRUE
                                            : ECMA_SIMPLE_VALUE_FALSE);

  ecma_string_t *configurable_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_CONFIGURABLE);
  completion = ecma_op_object_define_own_property (obj_p,
                                                   configurable_magic_string_p,
                                                   &prop_desc,
                                                   false);
  ecma_deref_ecma_string (configurable_magic_string_p);
  JERRY_ASSERT (ecma_is_completion_value_normal_true (completion));

  return obj_p;
} /* ecma_op_from_property_descriptor */

/**
 * ToPropertyDescriptor operation.
 *
 * See also:
 *          ECMA-262 v5, 8.10.5
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_op_to_property_descriptor (ecma_value_t obj_value, /**< object value */
                                ecma_property_descriptor_t *out_prop_desc_p) /**< out: filled property descriptor
                                                                                  if return value is normal
                                                                                  empty completion value */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  // 1.
  if (!ecma_is_value_object (obj_value))
  {
    ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
  }
  else
  {
    ecma_object_t *obj_p = ecma_get_object_from_value (obj_value);

    // 2.
    ecma_property_descriptor_t prop_desc = ecma_make_empty_property_descriptor ();

    // 3.
    ecma_string_t *enumerable_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_ENUMERABLE);

    if (ecma_op_object_get_property (obj_p, enumerable_magic_string_p) != NULL)
    {
      ECMA_TRY_CATCH (enumerable_prop_value,
                      ecma_op_object_get (obj_p, enumerable_magic_string_p),
                      ret_value);
      ECMA_TRY_CATCH (boolean_enumerable_prop_value,
                      ecma_op_to_boolean (enumerable_prop_value),
                      ret_value);

      prop_desc.is_enumerable_defined = true;
      if (ecma_is_value_true (boolean_enumerable_prop_value))
      {
        prop_desc.is_enumerable = true;
      }
      else
      {
        JERRY_ASSERT (ecma_is_value_boolean (boolean_enumerable_prop_value));

        prop_desc.is_enumerable = false;
      }

      ECMA_FINALIZE (boolean_enumerable_prop_value);
      ECMA_FINALIZE (enumerable_prop_value);
    }

    ecma_deref_ecma_string (enumerable_magic_string_p);

    if (!ecma_is_completion_value_throw (ret_value))
    {
      JERRY_ASSERT (ecma_is_completion_value_empty (ret_value));

      // 4.
      ecma_string_t *configurable_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_CONFIGURABLE);

      if (ecma_op_object_get_property (obj_p, configurable_magic_string_p) != NULL)
      {
        ECMA_TRY_CATCH (configurable_prop_value,
                        ecma_op_object_get (obj_p, configurable_magic_string_p),
                        ret_value);
        ECMA_TRY_CATCH (boolean_configurable_prop_value,
                        ecma_op_to_boolean (configurable_prop_value),
                        ret_value);

        prop_desc.is_configurable_defined = true;
        if (ecma_is_value_true (boolean_configurable_prop_value))
        {
          prop_desc.is_configurable = true;
        }
        else
        {
          JERRY_ASSERT (ecma_is_value_boolean (boolean_configurable_prop_value));

          prop_desc.is_configurable = false;
        }

        ECMA_FINALIZE (boolean_configurable_prop_value);
        ECMA_FINALIZE (configurable_prop_value);
      }

      ecma_deref_ecma_string (configurable_magic_string_p);
    }

    if (!ecma_is_completion_value_throw (ret_value))
    {
      JERRY_ASSERT (ecma_is_completion_value_empty (ret_value));

      // 5.
      ecma_string_t *value_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_VALUE);

      if (ecma_op_object_get_property (obj_p, value_magic_string_p) != NULL)
      {
        ECMA_TRY_CATCH (value_prop_value,
                        ecma_op_object_get (obj_p, value_magic_string_p),
                        ret_value);

        prop_desc.is_value_defined = true;
        prop_desc.value = ecma_copy_value (value_prop_value, true);

        ECMA_FINALIZE (value_prop_value);
      }

      ecma_deref_ecma_string (value_magic_string_p);
    }

    if (!ecma_is_completion_value_throw (ret_value))
    {
      JERRY_ASSERT (ecma_is_completion_value_empty (ret_value));

      // 6.
      ecma_string_t *writable_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_WRITABLE);

      if (ecma_op_object_get_property (obj_p, writable_magic_string_p) != NULL)
      {
        ECMA_TRY_CATCH (writable_prop_value,
                        ecma_op_object_get (obj_p, writable_magic_string_p),
                        ret_value);
        ECMA_TRY_CATCH (boolean_writable_prop_value,
                        ecma_op_to_boolean (writable_prop_value),
                        ret_value);

        prop_desc.is_writable_defined = true;
        if (ecma_is_value_true (boolean_writable_prop_value))
        {
          prop_desc.is_writable = true;
        }
        else
        {
          JERRY_ASSERT (ecma_is_value_boolean (boolean_writable_prop_value));

          prop_desc.is_writable = false;
        }

        ECMA_FINALIZE (boolean_writable_prop_value);
        ECMA_FINALIZE (writable_prop_value);
      }

      ecma_deref_ecma_string (writable_magic_string_p);
    }

    if (!ecma_is_completion_value_throw (ret_value))
    {
      JERRY_ASSERT (ecma_is_completion_value_empty (ret_value));

      // 7.
      ecma_string_t *get_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_GET);

      if (ecma_op_object_get_property (obj_p, get_magic_string_p) != NULL)
      {
        ECMA_TRY_CATCH (get_prop_value,
                        ecma_op_object_get (obj_p, get_magic_string_p),
                        ret_value);

        if (!ecma_op_is_callable (get_prop_value)
            && !ecma_is_value_undefined (get_prop_value))
        {
          ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
        }
        else
        {
          prop_desc.is_get_defined = true;

          if (ecma_is_value_undefined (get_prop_value))
          {
            prop_desc.get_p = NULL;
          }
          else
          {
            JERRY_ASSERT (ecma_is_value_object (get_prop_value));

            ecma_object_t *get_p = ecma_get_object_from_value (get_prop_value);
            ecma_ref_object (get_p);

            prop_desc.get_p = get_p;
          }
        }

        ECMA_FINALIZE (get_prop_value);
      }

      ecma_deref_ecma_string (get_magic_string_p);
    }

    if (!ecma_is_completion_value_throw (ret_value))
    {
      JERRY_ASSERT (ecma_is_completion_value_empty (ret_value));

      // 8.

      ecma_string_t *set_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_SET);

      if (ecma_op_object_get_property (obj_p, set_magic_string_p) != NULL)
      {
        ECMA_TRY_CATCH (set_prop_value,
                        ecma_op_object_get (obj_p, set_magic_string_p),
                        ret_value);

        if (!ecma_op_is_callable (set_prop_value)
            && !ecma_is_value_undefined (set_prop_value))
        {
          ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
        }
        else
        {
          prop_desc.is_set_defined = true;

          if (ecma_is_value_undefined (set_prop_value))
          {
            prop_desc.set_p = NULL;
          }
          else
          {
            JERRY_ASSERT (ecma_is_value_object (set_prop_value));

            ecma_object_t *set_p = ecma_get_object_from_value (set_prop_value);
            ecma_ref_object (set_p);

            prop_desc.set_p = set_p;
          }
        }

        ECMA_FINALIZE (set_prop_value);
      }

      ecma_deref_ecma_string (set_magic_string_p);
    }

    if (!ecma_is_completion_value_throw (ret_value))
    {
      JERRY_ASSERT (ecma_is_completion_value_empty (ret_value));

      // 9.
      if (prop_desc.is_get_defined
          || prop_desc.is_set_defined)
      {
        if (prop_desc.is_value_defined
            || prop_desc.is_writable_defined)
        {
          ret_value = ecma_make_throw_obj_completion_value (ecma_new_standard_error (ECMA_ERROR_TYPE));
        }
      }
    }

    if (!ecma_is_completion_value_throw (ret_value))
    {
      JERRY_ASSERT (ecma_is_completion_value_empty (ret_value));
    }
    else
    {
      ecma_free_property_descriptor (&prop_desc);
    }

    *out_prop_desc_p = prop_desc;
  }

  return ret_value;
} /* ecma_op_to_property_descriptor */

/**
 * @}
 * @}
 */
