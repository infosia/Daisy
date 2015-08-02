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

#ifndef LIT_MAGIC_STRINGS_H
#define LIT_MAGIC_STRINGS_H

#include "lit-globals.h"

/**
 * Limit for magic string length
 */
#define LIT_MAGIC_STRING_LENGTH_LIMIT 32

/**
 * Identifiers of ECMA and implementation-defined magic string constants
 */
typedef enum
{
#define LIT_MAGIC_STRING_DEF(id, ascii_zt_string) \
     id,
#include "lit-magic-strings.inc.h"
#undef LIT_MAGIC_STRING_DEF

  LIT_MAGIC_STRING__COUNT /**< number of magic strings */
} lit_magic_string_id_t;

/**
 * Identifiers of implementation-defined external magic string constants
 */
typedef uint32_t lit_magic_string_ex_id_t;

extern void lit_magic_strings_init (void);
extern void lit_magic_strings_ex_init (void);

extern uint32_t lit_get_magic_string_ex_count (void);

extern const lit_utf8_byte_t *lit_get_magic_string_utf8 (lit_magic_string_id_t);
extern lit_utf8_size_t lit_get_magic_string_size (lit_magic_string_id_t);

extern const lit_utf8_byte_t *lit_get_magic_string_ex_utf8 (lit_magic_string_ex_id_t);
extern lit_utf8_size_t lit_get_magic_string_ex_size (lit_magic_string_ex_id_t);

extern void lit_magic_strings_ex_set (const lit_utf8_byte_t **,
                                      uint32_t count,
                                      const lit_utf8_size_t *);

extern  bool lit_is_utf8_string_magic (const lit_utf8_byte_t *,
                                       lit_utf8_size_t,
                                       lit_magic_string_id_t *);
extern  bool lit_is_ex_utf8_string_magic (const lit_utf8_byte_t *,
                                          lit_utf8_size_t,
                                          lit_magic_string_ex_id_t *);

extern bool lit_compare_utf8_string_and_magic_string (const lit_utf8_byte_t *,
                                                      lit_utf8_size_t,
                                                      lit_magic_string_id_t);

extern bool lit_compare_utf8_string_and_magic_string_ex (const lit_utf8_byte_t *,
                                                         lit_utf8_size_t,
                                                         lit_magic_string_ex_id_t);

extern lit_utf8_byte_t *lit_copy_magic_string_to_buffer (lit_magic_string_id_t,
                                                         lit_utf8_byte_t *buffer_p,
                                                         ssize_t buffer_size);

#endif /* LIT_MAGIC_STRINGS_H */
