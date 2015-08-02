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
#include "ecma-conversion.h"
#include "ecma-exceptions.h"
#include "ecma-gc.h"
#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "ecma-objects.h"
#include "ecma-string-object.h"
#include "ecma-try-catch-macro.h"
#include "jrt.h"

#ifndef CONFIG_ECMA_COMPACT_PROFILE_DISABLE_STRING_BUILTIN

#define ECMA_BUILTINS_INTERNAL
#include "ecma-builtins-internal.h"

#define BUILTIN_INC_HEADER_NAME "ecma-builtin-string.inc.h"
#define BUILTIN_UNDERSCORED_ID string
#include "ecma-builtin-internal-routines-template.inc.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmabuiltins
 * @{
 *
 * \addtogroup string ECMA String object built-in
 * @{
 */

/**
 * The String object's 'fromCharCode' routine
 *
 * See also:
 *          ECMA-262 v5, 15.5.3.2
 *
 * @return completion value
 *         Returned value must be freed with ecma_free_completion_value.
 */
static ecma_completion_value_t
ecma_builtin_string_object_from_char_code (ecma_value_t this_arg __attr_unused___, /**< 'this' argument */
                                           const ecma_value_t args[], /**< arguments list */
                                           ecma_length_t args_number) /**< number of arguments */
{
  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (args_number == 0)
  {
    ecma_string_t *ret_str_p = ecma_new_ecma_string_from_utf8 (NULL, 0);
    return ecma_make_normal_completion_value (ecma_make_string_value (ret_str_p));
  }

  lit_utf8_size_t utf8_buf_size = args_number * LIT_UTF8_MAX_BYTES_IN_CODE_UNIT;
  ecma_string_t *ret_str_p;
  MEM_DEFINE_LOCAL_ARRAY (utf8_buf_p, utf8_buf_size, lit_utf8_byte_t);

  lit_utf8_size_t utf8_buf_used = 0;

  FIXME ("Support surrogate pairs");
  for (ecma_length_t arg_index = 0;
       arg_index < args_number;
       arg_index++)
  {
    ECMA_OP_TO_NUMBER_TRY_CATCH (arg_num, args[arg_index], ret_value);

    uint32_t uint32_char_code = ecma_number_to_uint32 (arg_num);
    ecma_char_t code_unit = (uint16_t) uint32_char_code;

    JERRY_ASSERT (utf8_buf_used <= utf8_buf_size - LIT_UTF8_MAX_BYTES_IN_CODE_UNIT);
    utf8_buf_used += lit_code_unit_to_utf8 (code_unit, utf8_buf_p + utf8_buf_used);
    JERRY_ASSERT (utf8_buf_used <= utf8_buf_size);

    ECMA_OP_TO_NUMBER_FINALIZE (arg_num);

    if (ecma_is_completion_value_throw (ret_value))
    {
      mem_heap_free_block (utf8_buf_p);

      return ret_value;
    }

    JERRY_ASSERT (ecma_is_completion_value_empty (ret_value));
  }

  ret_str_p = ecma_new_ecma_string_from_utf8 (utf8_buf_p, utf8_buf_used);

  MEM_FINALIZE_LOCAL_ARRAY (utf8_buf_p);

  return ecma_make_normal_completion_value (ecma_make_string_value (ret_str_p));
} /* ecma_builtin_string_object_from_char_code */

/**
 * Handle calling [[Call]] of built-in String object
 *
 * @return completion-value
 */
ecma_completion_value_t
ecma_builtin_string_dispatch_call (const ecma_value_t *arguments_list_p, /**< arguments list */
                                   ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  if (arguments_list_len == 0)
  {
    ecma_string_t *str_p = ecma_new_ecma_string_from_magic_string_id (LIT_MAGIC_STRING__EMPTY);
    ecma_value_t str_value = ecma_make_string_value (str_p);

    ret_value = ecma_make_normal_completion_value (str_value);
  }
  else
  {
    ret_value = ecma_op_to_string (arguments_list_p[0]);
  }

  return ret_value;
} /* ecma_builtin_string_dispatch_call */

/**
 * Handle calling [[Construct]] of built-in String object
 *
 * @return completion-value
 */
ecma_completion_value_t
ecma_builtin_string_dispatch_construct (const ecma_value_t *arguments_list_p, /**< arguments list */
                                        ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  return ecma_op_create_string_object (arguments_list_p, arguments_list_len);
} /* ecma_builtin_string_dispatch_construct */

/**
 * @}
 * @}
 * @}
 */

#endif /* !CONFIG_ECMA_COMPACT_PROFILE_DISABLE_STRING_BUILTIN */
