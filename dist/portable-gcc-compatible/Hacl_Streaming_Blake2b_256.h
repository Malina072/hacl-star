/* MIT License
 *
 * Copyright (c) 2016-2020 INRIA, CMU and Microsoft Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef __Hacl_Streaming_Blake2b_256_H
#define __Hacl_Streaming_Blake2b_256_H

#if defined(__cplusplus)
extern "C" {
#endif




#include "Hacl_Streaming_Blake2.h"
#include "Hacl_Spec.h"
#include "Hacl_Kremlib.h"
#include "Hacl_Hash_Blake2b_256.h"
#include "Hacl_Hash_Blake2.h"
#include "evercrypt_targetconfig.h"
#include "libintvector.h"
#include "kremlin/internal/types.h"
#include "kremlin/lowstar_endianness.h"
#include <string.h>
#include "kremlin/internal/target.h"
/* SNIPPET_START: Hacl_Streaming_Blake2b_256_blake2b_256_block_state */

typedef struct Hacl_Streaming_Blake2b_256_blake2b_256_block_state_s
{
  Lib_IntVector_Intrinsics_vec256 *fst;
  Lib_IntVector_Intrinsics_vec256 *snd;
}
Hacl_Streaming_Blake2b_256_blake2b_256_block_state;

/* SNIPPET_END: Hacl_Streaming_Blake2b_256_blake2b_256_block_state */

/* SNIPPET_START: Hacl_Streaming_Blake2b_256_blake2b_256_state */

typedef struct Hacl_Streaming_Blake2b_256_blake2b_256_state_s
{
  Hacl_Streaming_Blake2b_256_blake2b_256_block_state block_state;
  uint8_t *buf;
  uint64_t total_len;
}
Hacl_Streaming_Blake2b_256_blake2b_256_state;

/* SNIPPET_END: Hacl_Streaming_Blake2b_256_blake2b_256_state */

/* SNIPPET_START: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_create_in */

/*
  State allocation function when there is no key
*/
Hacl_Streaming_Blake2b_256_blake2b_256_state
*Hacl_Streaming_Blake2b_256_blake2b_256_no_key_create_in();

/* SNIPPET_END: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_create_in */

/* SNIPPET_START: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_init */

/*
  (Re-)initialization function when there is no key
*/
void
Hacl_Streaming_Blake2b_256_blake2b_256_no_key_init(
  Hacl_Streaming_Blake2b_256_blake2b_256_state *s
);

/* SNIPPET_END: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_init */

/* SNIPPET_START: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_update */

/*
  Update function when there is no key
*/
void
Hacl_Streaming_Blake2b_256_blake2b_256_no_key_update(
  Hacl_Streaming_Blake2b_256_blake2b_256_state *p,
  uint8_t *data,
  uint32_t len
);

/* SNIPPET_END: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_update */

/* SNIPPET_START: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_finish */

/*
  Finish function when there is no key
*/
void
Hacl_Streaming_Blake2b_256_blake2b_256_no_key_finish(
  Hacl_Streaming_Blake2b_256_blake2b_256_state *p,
  uint8_t *dst
);

/* SNIPPET_END: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_finish */

/* SNIPPET_START: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_free */

/*
  Free state function when there is no key
*/
void
Hacl_Streaming_Blake2b_256_blake2b_256_no_key_free(
  Hacl_Streaming_Blake2b_256_blake2b_256_state *s
);

/* SNIPPET_END: Hacl_Streaming_Blake2b_256_blake2b_256_no_key_free */

#if defined(__cplusplus)
}
#endif

#define __Hacl_Streaming_Blake2b_256_H_DEFINED
#endif
