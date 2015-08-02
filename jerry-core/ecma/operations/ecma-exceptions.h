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

#ifndef ECMA_EXCEPTIONS_H
#define ECMA_EXCEPTIONS_H

#include "ecma-globals.h"
#include "jrt.h"

/** \addtogroup ecma ECMA
 * @{
 */

/**
 * \addtogroup exceptions Exceptions
 * @{
 */

/**
 * Native errors.
 *
 * See also: 15.11.1, 15.11.6
 */
typedef enum
{
  ECMA_ERROR_COMMON, /**< Error */
  ECMA_ERROR_EVAL, /**< EvalError */
  ECMA_ERROR_RANGE, /**< RangeError */
  ECMA_ERROR_REFERENCE, /**< ReferenceError */
  ECMA_ERROR_SYNTAX, /**< SyntaxError */
  ECMA_ERROR_TYPE, /**< TypeError */
  ECMA_ERROR_URI /**< URIError */
} ecma_standard_error_t;

extern ecma_object_t *ecma_new_standard_error (ecma_standard_error_t error_type);
extern ecma_object_t *ecma_new_standard_error_with_message (ecma_standard_error_t error_type,
                                                            ecma_string_t *message_string_p);
extern ecma_completion_value_t ecma_raise_standard_error (ecma_standard_error_t error_type,
                                                          const lit_utf8_byte_t *msg_p);
extern ecma_completion_value_t ecma_raise_common_error (const char *msg_p);
extern ecma_completion_value_t ecma_raise_eval_error (const char *msg_p);
extern ecma_completion_value_t ecma_raise_range_error (const char *msg_p);
extern ecma_completion_value_t ecma_raise_reference_error (const char *msg_p);
extern ecma_completion_value_t ecma_raise_syntax_error (const char *msg_p);
extern ecma_completion_value_t ecma_raise_type_error (const char *msg_p);
extern ecma_completion_value_t ecma_raise_uri_error (const char *msg_p);

/**
 * @}
 * @}
 */

#endif /* !ECMA_EXCEPTIONS_H */
