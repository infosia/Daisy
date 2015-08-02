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

#ifndef LIT_ID_HASH_TABLE
#define LIT_ID_HASH_TABLE

#include "jrt.h"
#include "ecma-globals.h"
#include "opcodes.h"
#include "lit-literal.h"

typedef struct
{
  size_t current_bucket_pos;
  lit_cpointer_t *raw_buckets;
  lit_cpointer_t **buckets;
} lit_id_hash_table;

lit_id_hash_table *lit_id_hash_table_init (uint8_t*, size_t, size_t, size_t);
size_t lit_id_hash_table_get_size_for_table (size_t, size_t);
void lit_id_hash_table_free (lit_id_hash_table *);
void lit_id_hash_table_insert (lit_id_hash_table *, idx_t, vm_instr_counter_t, lit_cpointer_t);
lit_cpointer_t lit_id_hash_table_lookup (lit_id_hash_table *, idx_t, vm_instr_counter_t);

#endif /* LIT_ID_HASH_TABLE */
