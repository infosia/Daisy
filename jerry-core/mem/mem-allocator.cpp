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
 * Allocator implementation
 */

#include "jrt.h"
#include "jrt-libc-includes.h"
#include "mem-allocator.h"
#include "mem-heap.h"
#include "mem-poolman.h"

#define MEM_ALLOCATOR_INTERNAL

#include "mem-allocator-internal.h"

/**
 * Area for heap
 */
static uint8_t mem_heap_area[ MEM_HEAP_AREA_SIZE ] __attribute__ ((aligned (JERRY_MAX (MEM_ALIGNMENT,
                                                                                       MEM_HEAP_CHUNK_SIZE))));

/**
 * The 'try to give memory back' callback
 */
static mem_try_give_memory_back_callback_t mem_try_give_memory_back_callback = NULL;

/**
 * Initialize memory allocators.
 */
void
mem_init (void)
{
  mem_heap_init (mem_heap_area, sizeof (mem_heap_area));
  mem_pools_init ();
} /* mem_init */

/**
 * Finalize memory allocators.
 */
void
mem_finalize (bool is_show_mem_stats) /**< show heap memory stats
                                           before finalization? */
{
  mem_pools_finalize ();

  if (is_show_mem_stats)
  {
    mem_heap_print (false, false, true);

#ifdef MEM_STATS
    mem_pools_stats_t stats;
    mem_pools_get_stats (&stats);

    printf ("Pools stats:\n");
    printf (" Chunk size: %zu\n"
            "  Pools: %zu\n"
            "  Allocated chunks: %zu\n"
            "  Free chunks: %zu\n"
            "  Peak pools: %zu\n"
            "  Peak allocated chunks: %zu\n\n",
            MEM_POOL_CHUNK_SIZE,
            stats.pools_count,
            stats.allocated_chunks,
            stats.free_chunks,
            stats.peak_pools_count,
            stats.peak_allocated_chunks);
#endif /* MEM_STATS */
  }

  mem_heap_finalize ();
} /* mem_finalize */

/**
 * Get base pointer for allocation area.
 */
static uintptr_t
mem_get_base_pointer (void)
{
  return (uintptr_t) mem_heap_area;
} /* mem_get_base_pointer */

/**
 * Compress pointer.
 */
uintptr_t
mem_compress_pointer (const void *pointer) /**< pointer to compress */
{
  JERRY_ASSERT (pointer != NULL);

  uintptr_t int_ptr = (uintptr_t) pointer;

  JERRY_ASSERT (int_ptr % MEM_ALIGNMENT == 0);

  int_ptr -= mem_get_base_pointer ();
  int_ptr >>= MEM_ALIGNMENT_LOG;

  JERRY_ASSERT ((int_ptr & ~((1u << MEM_HEAP_OFFSET_LOG) - 1)) == 0);

  JERRY_ASSERT (int_ptr != MEM_CP_NULL);

  return int_ptr;
} /* mem_compress_pointer */

/**
 * Decompress pointer.
 */
void*
mem_decompress_pointer (uintptr_t compressed_pointer) /**< pointer to decompress */
{
  JERRY_ASSERT (compressed_pointer != MEM_CP_NULL);

  uintptr_t int_ptr = compressed_pointer;

  int_ptr <<= MEM_ALIGNMENT_LOG;
  int_ptr += mem_get_base_pointer ();

  return (void*) int_ptr;
} /* mem_decompress_pointer */

/**
 * Register specified 'try to give memory back' callback routine
 */
void
mem_register_a_try_give_memory_back_callback (mem_try_give_memory_back_callback_t callback) /* callback routine */
{
  /* Currently only one callback is supported */
  JERRY_ASSERT (mem_try_give_memory_back_callback == NULL);

  mem_try_give_memory_back_callback = callback;
} /* mem_register_a_try_give_memory_back_callback */

/**
 * Unregister specified 'try to give memory back' callback routine
 */
void
mem_unregister_a_try_give_memory_back_callback (mem_try_give_memory_back_callback_t callback) /* callback routine */
{
  /* Currently only one callback is supported */
  JERRY_ASSERT (mem_try_give_memory_back_callback == callback);

  mem_try_give_memory_back_callback = NULL;
} /* mem_unregister_a_try_give_memory_back_callback */

/**
 * Run 'try to give memory back' callbacks with specified severity
 */
void
mem_run_try_to_give_memory_back_callbacks (mem_try_give_memory_back_severity_t severity) /**< severity of
                                                                                              the request */
{
  if (mem_try_give_memory_back_callback != NULL)
  {
    mem_try_give_memory_back_callback (severity);
  }
} /* mem_run_try_to_give_memory_back_callbacks */

#ifndef JERRY_NDEBUG
/**
 * Check whether the pointer points to the heap
 *
 * Note:
 *      the routine should be used only for assertion checks
 *
 * @return true - if pointer points to the heap,
 *         false - otherwise
 */
bool
mem_is_heap_pointer (void *pointer) /**< pointer */
{
  uint8_t *uint8_pointer = (uint8_t*) pointer;

  return (uint8_pointer >= mem_heap_area && uint8_pointer <= (mem_heap_area + MEM_HEAP_AREA_SIZE));
} /* mem_is_heap_pointer */
#endif /* !JERRY_NDEBUG */

#ifdef MEM_STATS
/**
 * Reset peak values in memory usage statistics
 */
void
mem_stats_reset_peak (void)
{
  mem_heap_stats_reset_peak ();
  mem_pools_stats_reset_peak ();
} /* mem_stats_reset_peak */

/**
 * Print memory usage statistics
 */
void
mem_stats_print (void)
{
  mem_heap_print (false, false, true);

  mem_pools_stats_t stats;
  mem_pools_get_stats (&stats);

  printf ("Pools stats:\n");
  printf (" Chunk size: %zu\n"
          "  Pools: %zu\n"
          "  Allocated chunks: %zu\n"
          "  Free chunks: %zu\n"
          "  Peak pools: %zu\n"
          "  Peak allocated chunks: %zu\n\n",
          MEM_POOL_CHUNK_SIZE,
          stats.pools_count,
          stats.allocated_chunks,
          stats.free_chunks,
          stats.peak_pools_count,
          stats.peak_allocated_chunks);
} /* mem_stats_print */
#endif /* MEM_STATS */
