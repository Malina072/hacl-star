/*
  Copyright (c) INRIA and Microsoft Corporation. All rights reserved.
  Licensed under the Apache 2.0 License.
*/


#ifndef __FStar_UInt128_H
#define __FStar_UInt128_H
#include <inttypes.h>
#include <stdbool.h>
#include "lowstar_endianness.h"
#include "types.h"

static inline void store128_le(uint8_t *x0, FStar_UInt128_uint128 x1);

static inline FStar_UInt128_uint128 load128_le(uint8_t *x0);

static inline void store128_be(uint8_t *x0, FStar_UInt128_uint128 x1);

static inline FStar_UInt128_uint128 load128_be(uint8_t *x0);

static inline FStar_UInt128_uint128
FStar_UInt128_add(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_add_underspec(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_add_mod(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_sub(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_sub_underspec(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_sub_mod(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_logand(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_logxor(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_logor(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128 FStar_UInt128_lognot(FStar_UInt128_uint128 a);

static inline FStar_UInt128_uint128
FStar_UInt128_shift_left(FStar_UInt128_uint128 a, uint32_t s);

static inline FStar_UInt128_uint128
FStar_UInt128_shift_right(FStar_UInt128_uint128 a, uint32_t s);

static inline bool FStar_UInt128_eq(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline bool FStar_UInt128_gt(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline bool FStar_UInt128_lt(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline bool FStar_UInt128_gte(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline bool FStar_UInt128_lte(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_eq_mask(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128
FStar_UInt128_gte_mask(FStar_UInt128_uint128 a, FStar_UInt128_uint128 b);

static inline FStar_UInt128_uint128 FStar_UInt128_uint64_to_uint128(uint64_t a);

static inline uint64_t FStar_UInt128_uint128_to_uint64(FStar_UInt128_uint128 a);

static inline FStar_UInt128_uint128 FStar_UInt128_mul32(uint64_t x, uint32_t y);

static inline FStar_UInt128_uint128 FStar_UInt128_mul_wide(uint64_t x, uint64_t y);


#define __FStar_UInt128_H_DEFINED
#endif
