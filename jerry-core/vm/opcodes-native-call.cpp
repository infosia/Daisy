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

#include "opcodes-ecma-support.h"

#include "jrt.h"
#include "vm.h"
#include "opcodes.h"

#include "opcodes-native-call.h"

#include "jrt-libc-includes.h"

/**
 * 'Native call' opcode handler.
 */
ecma_completion_value_t
opfunc_native_call (vm_instr_t instr, /**< instruction */
                    vm_frame_ctx_t *frame_ctx_p) /**< interpreter context */
{
  const idx_t dst_var_idx = instr.data.native_call.lhs;
  const idx_t native_call_id_idx = instr.data.native_call.name;
  const idx_t args_number = instr.data.native_call.arg_list;
  const vm_instr_counter_t lit_oc = frame_ctx_p->pos;

  JERRY_ASSERT (native_call_id_idx < OPCODE_NATIVE_CALL__COUNT);

  frame_ctx_p->pos++;

  JERRY_STATIC_ASSERT (OPCODE_NATIVE_CALL__COUNT < (1u << (sizeof (native_call_id_idx) * JERRY_BITSINBYTE)));

  ecma_completion_value_t ret_value = ecma_make_empty_completion_value ();

  MEM_DEFINE_LOCAL_ARRAY (arg_values, args_number, ecma_value_t);

  ecma_length_t args_read;
  ecma_completion_value_t get_arg_completion = fill_varg_list (frame_ctx_p,
                                                               args_number,
                                                               arg_values,
                                                               &args_read);

  if (ecma_is_completion_value_empty (get_arg_completion))
  {
    JERRY_ASSERT (args_read == args_number);

    switch ((opcode_native_call_t)native_call_id_idx)
    {
      case OPCODE_NATIVE_CALL_LED_TOGGLE:
      case OPCODE_NATIVE_CALL_LED_ON:
      case OPCODE_NATIVE_CALL_LED_OFF:
      case OPCODE_NATIVE_CALL_LED_ONCE:
      case OPCODE_NATIVE_CALL_WAIT:
      {
        JERRY_UNIMPLEMENTED ("Device operations are not implemented.");
      }

      case OPCODE_NATIVE_CALL_PRINT:
      {
        for (ecma_length_t arg_index = 0;
             ecma_is_completion_value_empty (ret_value) && arg_index < args_read;
             arg_index++)
        {
          ECMA_TRY_CATCH (str_value,
                          ecma_op_to_string (arg_values[arg_index]),
                          ret_value);

          ecma_string_t *str_p = ecma_get_string_from_value (str_value);

          lit_utf8_size_t bytes = ecma_string_get_size (str_p);

          ssize_t utf8_str_size = (ssize_t) (bytes + 1);
          lit_utf8_byte_t *utf8_str_p = (lit_utf8_byte_t*) mem_heap_alloc_block ((size_t) utf8_str_size,
                                                                               MEM_HEAP_ALLOC_SHORT_TERM);
          if (utf8_str_p == NULL)
          {
            jerry_fatal (ERR_OUT_OF_MEMORY);
          }

          ecma_string_to_utf8_string (str_p, utf8_str_p, utf8_str_size);
          utf8_str_p[utf8_str_size - 1] = 0;

          FIXME ("Support unicode in printf.");
          if (arg_index < args_read - 1)
          {
            printf ("%s ", (char*) utf8_str_p);
          }
          else
          {
            printf ("%s", (char*) utf8_str_p);
          }

          mem_heap_free_block (utf8_str_p);

          ret_value = set_variable_value (frame_ctx_p, lit_oc, dst_var_idx,
                                          ecma_make_simple_value (ECMA_SIMPLE_VALUE_UNDEFINED));

          ECMA_FINALIZE (str_value);
        }
        printf ("\n");
        break;
      }

      case OPCODE_NATIVE_CALL__COUNT:
      {
        JERRY_UNREACHABLE ();
      }
    }
  }
  else
  {
    JERRY_ASSERT (!ecma_is_completion_value_normal (get_arg_completion));

    ret_value = get_arg_completion;
  }

  for (ecma_length_t arg_index = 0;
       arg_index < args_read;
       arg_index++)
  {
    ecma_free_value (arg_values[arg_index], true);
  }

  MEM_FINALIZE_LOCAL_ARRAY (arg_values);

  return ret_value;
} /* opfunc_native_call */
