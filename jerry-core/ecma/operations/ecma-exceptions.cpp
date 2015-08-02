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

#include "ecma-builtins.h"
#include "ecma-exceptions.h"
#include "ecma-gc.h"
#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "jrt.h"

/** \addtogroup ecma ECMA
 * @{
 */

/**
 * \addtogroup exceptions Exceptions
 * @{
 */

/**
 * Standard ecma-error object constructor.
 *
 * @return pointer to ecma-object representing specified error
 *         with reference counter set to one.
 */
ecma_object_t*
ecma_new_standard_error (ecma_standard_error_t error_type) /**< native error type */
{
#ifndef CONFIG_ECMA_COMPACT_PROFILE_DISABLE_ERROR_BUILTINS
  ecma_builtin_id_t prototype_id = ECMA_BUILTIN_ID__COUNT;

  switch (error_type)
  {
    case ECMA_ERROR_COMMON:
    {
      prototype_id = ECMA_BUILTIN_ID_ERROR_PROTOTYPE;
      break;
    }

    case ECMA_ERROR_EVAL:
    {
      prototype_id = ECMA_BUILTIN_ID_EVAL_ERROR_PROTOTYPE;
      break;
    }

    case ECMA_ERROR_RANGE:
    {
      prototype_id = ECMA_BUILTIN_ID_RANGE_ERROR_PROTOTYPE;
      break;
    }

    case ECMA_ERROR_REFERENCE:
    {
      prototype_id = ECMA_BUILTIN_ID_REFERENCE_ERROR_PROTOTYPE;
      break;
    }

    case ECMA_ERROR_TYPE:
    {
      prototype_id = ECMA_BUILTIN_ID_TYPE_ERROR_PROTOTYPE;
      break;
    }

    case ECMA_ERROR_URI:
    {
      prototype_id = ECMA_BUILTIN_ID_URI_ERROR_PROTOTYPE;
      break;
    }

    case ECMA_ERROR_SYNTAX:
    {
      prototype_id = ECMA_BUILTIN_ID_SYNTAX_ERROR_PROTOTYPE;
      break;
    }
  }

  ecma_object_t *prototype_obj_p = ecma_builtin_get (prototype_id);

  ecma_object_t *new_error_obj_p = ecma_create_object (prototype_obj_p,
                                                       true,
                                                       ECMA_OBJECT_TYPE_GENERAL);

  ecma_deref_object (prototype_obj_p);

  ecma_property_t *class_prop_p = ecma_create_internal_property (new_error_obj_p,
                                                                 ECMA_INTERNAL_PROPERTY_CLASS);
  class_prop_p->u.internal_property.value = LIT_MAGIC_STRING_ERROR_UL;

  return new_error_obj_p;
#else /* !CONFIG_ECMA_COMPACT_PROFILE_DISABLE_ERROR_BUILTINS */
  (void) error_type;

  return ecma_builtin_get (ECMA_BUILTIN_ID_COMPACT_PROFILE_ERROR);
#endif /* CONFIG_ECMA_COMPACT_PROFILE_DISABLE_ERROR_BUILTINS */
} /* ecma_new_standard_error */

/**
 * Standard ecma-error object constructor.
 *
 * @return pointer to ecma-object representing specified error
 *         with reference counter set to one.
 */
ecma_object_t*
ecma_new_standard_error_with_message (ecma_standard_error_t error_type, /**< native error type */
                                      ecma_string_t* message_string_p) /**< message string */
{
  ecma_object_t *new_error_obj_p = ecma_new_standard_error (error_type);

  ecma_string_t *message_magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_MESSAGE);
  ecma_property_t *prop_p = ecma_create_named_data_property (new_error_obj_p,
                                                             message_magic_string_p,
                                                             true, false, true);

  ecma_set_named_data_property_value (prop_p,
                                      ecma_make_string_value (ecma_copy_or_ref_ecma_string (message_string_p)));
  ecma_deref_ecma_string (message_magic_string_p);

  return new_error_obj_p;
} /* ecma_new_standard_error_with_message */

/**
 * Raise a standard ecma-error with the given type and message.
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_raise_standard_error (ecma_standard_error_t error_type, /**< error type */
                           const lit_utf8_byte_t *msg_p) /**< error message */
{
  ecma_string_t *error_msg_p = ecma_new_ecma_string_from_utf8 (msg_p,
                                                               lit_zt_utf8_string_size (msg_p));
  ecma_object_t *error_obj_p = ecma_new_standard_error_with_message (error_type, error_msg_p);
  ecma_deref_ecma_string (error_msg_p);
  return ecma_make_throw_obj_completion_value (error_obj_p);
} /* ecma_raise_standard_error */

/**
 * Raise a common error with the given message.
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_raise_common_error (const char *msg_p) /**< error message */
{
  return ecma_raise_standard_error (ECMA_ERROR_COMMON, (const lit_utf8_byte_t *) msg_p);
} /* ecma_raise_common_error */

/**
 * Raise an EvalError with the given message.
 *
 * See also: ECMA-262 v5, 15.11.6.1
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_raise_eval_error (const char *msg_p) /**< error message */
{
  return ecma_raise_standard_error (ECMA_ERROR_EVAL, (const lit_utf8_byte_t *) msg_p);
} /* ecma_raise_eval_error */

/**
 * Raise a RangeError with the given message.
 *
 * See also: ECMA-262 v5, 15.11.6.2
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_raise_range_error (const char *msg_p) /**< error message */
{
  return ecma_raise_standard_error (ECMA_ERROR_RANGE, (const lit_utf8_byte_t *) msg_p);
} /* ecma_raise_range_error */

/**
 * Raise a ReferenceError with the given message.
 *
 * See also: ECMA-262 v5, 15.11.6.3
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_raise_reference_error (const char *msg_p) /**< error message */
{
  return ecma_raise_standard_error (ECMA_ERROR_REFERENCE, (const lit_utf8_byte_t *) msg_p);
} /* ecma_raise_reference_error */

/**
 * Raise a SyntaxError with the given message.
 *
 * See also: ECMA-262 v5, 15.11.6.4
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_raise_syntax_error (const char *msg_p) /**< error message */
{
  return ecma_raise_standard_error (ECMA_ERROR_SYNTAX, (const lit_utf8_byte_t *) msg_p);
} /* ecma_raise_syntax_error */

/**
 * Raise a TypeError with the given message.
 *
* See also: ECMA-262 v5, 15.11.6.5
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_raise_type_error (const char *msg_p) /**< error message */
{
  return ecma_raise_standard_error (ECMA_ERROR_TYPE, (const lit_utf8_byte_t *) msg_p);
} /* ecma_raise_type_error */

/**
 * Raise a URIError with the given message.
 *
* See also: ECMA-262 v5, 15.11.6.6
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value
 */
ecma_completion_value_t
ecma_raise_uri_error (const char *msg_p) /**< error message */
{
  return ecma_raise_standard_error (ECMA_ERROR_URI, (const lit_utf8_byte_t *) msg_p);
} /* ecma_raise_uri_error */

/**
 * @}
 * @}
 */
