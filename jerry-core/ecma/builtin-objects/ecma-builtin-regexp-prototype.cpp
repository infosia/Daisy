/* Copyright 2015 Samsung Electronics Co., Ltd.
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
#include "ecma-builtins.h"
#include "ecma-conversion.h"
#include "ecma-exceptions.h"
#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "ecma-objects.h"
#include "ecma-try-catch-macro.h"

#ifndef CONFIG_ECMA_COMPACT_PROFILE_DISABLE_REGEXP_BUILTIN
#include "ecma-regexp-object.h"
#include "re-compiler.h"

#define ECMA_BUILTINS_INTERNAL
#include "ecma-builtins-internal.h"

#define BUILTIN_INC_HEADER_NAME "ecma-builtin-regexp-prototype.inc.h"
#define BUILTIN_UNDERSCORED_ID regexp_prototype
#include "ecma-builtin-internal-routines-template.inc.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmabuiltins
 * @{
 *
 * \addtogroup regexp ECMA RegExp.prototype object built-in
 * @{
 */

/**
 * The RegExp.prototype object's 'exec' routine
 *
 * See also:
 *          ECMA-262 v5, 15.10.6.2
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_regexp_prototype_exec (ecma_value_t this_arg, /**< this argument */
                                    ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (ecma_object_get_class_name (ecma_get_object_from_value (this_arg)) != LIT_MAGIC_STRING_REGEXP_UL)
  {
    ret_value = ecma_raise_type_error ("Incomplete RegExp type");
  }
  else
  {
    ECMA_TRY_CATCH (obj_this, ecma_op_to_object (this_arg), ret_value);

    ECMA_TRY_CATCH (input_str_value,
                    ecma_op_to_string (arg),
                    ret_value);

    ret_value = ecma_regexp_exec_helper (obj_this, input_str_value, false);

    ECMA_FINALIZE (input_str_value);
    ECMA_FINALIZE (obj_this);
  }

  return ret_value;
} /* ecma_builtin_regexp_prototype_exec */

/**
 * The RegExp.prototype object's 'test' routine
 *
 * See also:
 *          ECMA-262 v5, 15.10.6.3
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_regexp_prototype_test (ecma_value_t this_arg, /**< this argument */
                                    ecma_value_t arg) /**< routine's argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  ECMA_TRY_CATCH (match_value,
                  ecma_builtin_regexp_prototype_exec (this_arg, arg),
                  ret_value);

  if (ecma_is_value_null (match_value))
  {
    ret_value = ecma_make_simple_completion_value (ECMA_SIMPLE_VALUE_FALSE);
  }
  else
  {
    ret_value = ecma_make_simple_completion_value (ECMA_SIMPLE_VALUE_TRUE);
  }

  ECMA_FINALIZE (match_value);

  return ret_value;
} /* ecma_builtin_regexp_prototype_test */

/**
 * The RegExp.prototype object's 'toString' routine
 *
 * See also:
 *          ECMA-262 v5, 15.10.6.4
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_regexp_prototype_to_string (ecma_value_t this_arg) /**< this argument */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (ecma_object_get_class_name (ecma_get_object_from_value (this_arg)) != LIT_MAGIC_STRING_REGEXP_UL)
  {
    ret_value = ecma_raise_type_error ("Incomplete RegExp type");
  }
  else
  {
    ECMA_TRY_CATCH (obj_this,
                    ecma_op_to_object (this_arg),
                    ret_value);

    ecma_object_t *obj_p = ecma_get_object_from_value (obj_this);

    /* Get RegExp source from the source property */
    ecma_string_t *magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_SOURCE);
    ecma_property_t *source_prop_p = ecma_op_object_get_property (obj_p, magic_string_p);
    ecma_deref_ecma_string (magic_string_p);

    ecma_string_t *src_sep_str_p = ecma_get_magic_string (LIT_MAGIC_STRING_SLASH_CHAR);
    ecma_string_t *source_str_p = ecma_get_string_from_value (source_prop_p->u.named_data_property.value);
    ecma_string_t *output_str_p = ecma_concat_ecma_strings (src_sep_str_p, ecma_copy_or_ref_ecma_string (source_str_p));
    ecma_deref_ecma_string (source_str_p);

    ecma_string_t *concat_p = ecma_concat_ecma_strings (output_str_p, src_sep_str_p);
    ecma_deref_ecma_string (src_sep_str_p);
    ecma_deref_ecma_string (output_str_p);
    output_str_p = concat_p;

    /* Check the global flag */
    magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_GLOBAL);
    ecma_property_t *global_prop_p = ecma_op_object_get_property (obj_p, magic_string_p);
    ecma_deref_ecma_string (magic_string_p);

    if (ecma_is_value_true (global_prop_p->u.named_data_property.value))
    {
      ecma_string_t *g_flag_str_p = ecma_get_magic_string (LIT_MAGIC_STRING_G_CHAR);
      concat_p = ecma_concat_ecma_strings (output_str_p, g_flag_str_p);
      ecma_deref_ecma_string (output_str_p);
      ecma_deref_ecma_string (g_flag_str_p);
      output_str_p = concat_p;
    }

    /* Check the ignoreCase flag */
    magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_IGNORECASE_UL);
    ecma_property_t *ignorecase_prop_p = ecma_op_object_get_property (obj_p, magic_string_p);
    ecma_deref_ecma_string (magic_string_p);

    if (ecma_is_value_true (ignorecase_prop_p->u.named_data_property.value))
    {
      ecma_string_t *ic_flag_str_p = ecma_get_magic_string (LIT_MAGIC_STRING_I_CHAR);
      concat_p = ecma_concat_ecma_strings (output_str_p, ic_flag_str_p);
      ecma_deref_ecma_string (output_str_p);
      ecma_deref_ecma_string (ic_flag_str_p);
      output_str_p = concat_p;
    }

    /* Check the global flag */
    magic_string_p = ecma_get_magic_string (LIT_MAGIC_STRING_MULTILINE);
    ecma_property_t *multiline_prop_p = ecma_op_object_get_property (obj_p, magic_string_p);
    ecma_deref_ecma_string (magic_string_p);

    if (ecma_is_value_true (multiline_prop_p->u.named_data_property.value))
    {
      ecma_string_t *m_flag_str_p = ecma_get_magic_string (LIT_MAGIC_STRING_M_CHAR);
      concat_p = ecma_concat_ecma_strings (output_str_p, m_flag_str_p);
      ecma_deref_ecma_string (output_str_p);
      ecma_deref_ecma_string (m_flag_str_p);
      output_str_p = concat_p;
    }

    ret_value = ecma_make_normal_completion_value (ecma_make_string_value (output_str_p));

    ECMA_FINALIZE (obj_this);
  }

  return ret_value;
} /* ecma_builtin_regexp_prototype_to_string */

/**
 * @}
 * @}
 * @}
 */

#endif /* CONFIG_ECMA_COMPACT_PROFILE_DISABLE_REGEXP_BUILTIN */
