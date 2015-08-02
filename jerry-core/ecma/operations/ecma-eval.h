/* Copyright 2015 Samsung Electronics Co., Ltd.
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

#ifndef ECMA_EVAL_H
#define ECMA_EVAL_H

#include "ecma-globals.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup eval eval
 */

extern ecma_completion_value_t
ecma_op_eval (ecma_string_t *code_p,
              bool is_direct,
              bool is_called_from_strict_mode_code);

extern ecma_completion_value_t
ecma_op_eval_chars_buffer (const jerry_api_char_t *code_p,
                           size_t code_buffer_size,
                           bool is_direct,
                           bool is_called_from_strict_mode_code);

/**
 * @}
 * @}
 */

#endif /* !ECMA_EVAL_H */
