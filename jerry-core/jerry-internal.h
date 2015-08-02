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

#ifndef JERRY_INTERNAL
 # error "The header is for Jerry's internal interfaces"
#endif

#ifndef JERRY_INTERNAL_H
#define JERRY_INTERNAL_H

#include "ecma-globals.h"
#include "jerry-api.h"

extern ecma_completion_value_t
jerry_dispatch_external_function (ecma_object_t *function_object_p,
                                  ecma_external_pointer_t handler_p,
                                  ecma_value_t this_arg_value,
                                  const ecma_value_t args_p[],
                                  ecma_length_t args_count);

extern void
jerry_dispatch_object_free_callback (ecma_external_pointer_t freecb_p,
                                     ecma_external_pointer_t native_p);

extern bool
jerry_is_abort_on_fail (void);

#endif /* !JERRY_INTERNAL_H */
