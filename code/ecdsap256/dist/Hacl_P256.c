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


#include "Hacl_P256.h"

static inline void cmovznz4(uint64_t *out, uint64_t *x, uint64_t *y, uint64_t mask)
{
  uint64_t mask1 = FStar_UInt64_eq_mask(mask, (uint64_t)0U);
  uint64_t r0 = (x[0U] & mask1) | (y[0U] & ~mask1);
  uint64_t r1 = (x[1U] & mask1) | (y[1U] & ~mask1);
  uint64_t r2 = (x[2U] & mask1) | (y[2U] & ~mask1);
  uint64_t r3 = (x[3U] & mask1) | (y[3U] & ~mask1);
  out[0U] = r0;
  out[1U] = r1;
  out[2U] = r2;
  out[3U] = r3;
}

static inline void copy_conditional(uint64_t *out, uint64_t *x, uint64_t mask)
{
  uint64_t out_0 = out[0U];
  uint64_t out_1 = out[1U];
  uint64_t out_2 = out[2U];
  uint64_t out_3 = out[3U];
  uint64_t x_0 = x[0U];
  uint64_t x_1 = x[1U];
  uint64_t x_2 = x[2U];
  uint64_t x_3 = x[3U];
  uint64_t r_0 = out_0 ^ (mask & (out_0 ^ x_0));
  uint64_t r_1 = out_1 ^ (mask & (out_1 ^ x_1));
  uint64_t r_2 = out_2 ^ (mask & (out_2 ^ x_2));
  uint64_t r_3 = out_3 ^ (mask & (out_3 ^ x_3));
  out[0U] = r_0;
  out[1U] = r_1;
  out[2U] = r_2;
  out[3U] = r_3;
}

static inline void mul64(uint64_t x, uint64_t y, uint64_t *result, uint64_t *temp)
{
  uint128_t res = (uint128_t)x * y;
  uint64_t l0 = (uint64_t)res;
  uint64_t h0 = (uint64_t)(res >> (uint32_t)64U);
  result[0U] = l0;
  temp[0U] = h0;
}

static const
uint64_t
prime256_buffer[4U] =
  {
    (uint64_t)0xffffffffffffffffU,
    (uint64_t)0xffffffffU,
    (uint64_t)0U,
    (uint64_t)0xffffffff00000001U
  };

static inline void p256_add(uint64_t *arg1, uint64_t *arg2, uint64_t *out)
{
  uint64_t *r00 = out;
  uint64_t *r10 = out + (uint32_t)1U;
  uint64_t *r20 = out + (uint32_t)2U;
  uint64_t *r30 = out + (uint32_t)3U;
  uint64_t cc0 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, arg1[0U], arg2[0U], r00);
  uint64_t cc1 = Lib_IntTypes_Intrinsics_add_carry_u64(cc0, arg1[1U], arg2[1U], r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_add_carry_u64(cc1, arg1[2U], arg2[2U], r20);
  uint64_t cc3 = Lib_IntTypes_Intrinsics_add_carry_u64(cc2, arg1[3U], arg2[3U], r30);
  uint64_t t = cc3;
  uint64_t tempBuffer[4U] = { 0U };
  uint64_t tempBufferForSubborrow = (uint64_t)0U;
  uint64_t *r0 = tempBuffer;
  uint64_t *r1 = tempBuffer + (uint32_t)1U;
  uint64_t *r2 = tempBuffer + (uint32_t)2U;
  uint64_t *r3 = tempBuffer + (uint32_t)3U;
  uint64_t
  cc =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      out[0U],
      (uint64_t)0xffffffffffffffffU,
      r0);
  uint64_t cc10 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, out[1U], (uint64_t)0xffffffffU, r1);
  uint64_t cc20 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc10, out[2U], (uint64_t)0U, r2);
  uint64_t
  cc30 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc20, out[3U], (uint64_t)0xffffffff00000001U, r3);
  uint64_t c = cc30;
  uint64_t
  carry = Lib_IntTypes_Intrinsics_sub_borrow_u64(c, t, (uint64_t)0U, &tempBufferForSubborrow);
  cmovznz4(out, tempBuffer, out, carry);
}

static inline void p256_double(uint64_t *arg1, uint64_t *out)
{
  uint64_t *r00 = out;
  uint64_t *r10 = out + (uint32_t)1U;
  uint64_t *r20 = out + (uint32_t)2U;
  uint64_t *r30 = out + (uint32_t)3U;
  uint64_t cc0 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, arg1[0U], arg1[0U], r00);
  uint64_t cc1 = Lib_IntTypes_Intrinsics_add_carry_u64(cc0, arg1[1U], arg1[1U], r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_add_carry_u64(cc1, arg1[2U], arg1[2U], r20);
  uint64_t cc3 = Lib_IntTypes_Intrinsics_add_carry_u64(cc2, arg1[3U], arg1[3U], r30);
  uint64_t t = cc3;
  uint64_t tempBuffer[4U] = { 0U };
  uint64_t tempBufferForSubborrow = (uint64_t)0U;
  uint64_t *r0 = tempBuffer;
  uint64_t *r1 = tempBuffer + (uint32_t)1U;
  uint64_t *r2 = tempBuffer + (uint32_t)2U;
  uint64_t *r3 = tempBuffer + (uint32_t)3U;
  uint64_t
  cc =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      out[0U],
      (uint64_t)0xffffffffffffffffU,
      r0);
  uint64_t cc10 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, out[1U], (uint64_t)0xffffffffU, r1);
  uint64_t cc20 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc10, out[2U], (uint64_t)0U, r2);
  uint64_t
  cc30 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc20, out[3U], (uint64_t)0xffffffff00000001U, r3);
  uint64_t c = cc30;
  uint64_t
  carry = Lib_IntTypes_Intrinsics_sub_borrow_u64(c, t, (uint64_t)0U, &tempBufferForSubborrow);
  cmovznz4(out, tempBuffer, out, carry);
}

static inline void p256_sub(uint64_t *arg1, uint64_t *arg2, uint64_t *out)
{
  uint64_t *r00 = out;
  uint64_t *r10 = out + (uint32_t)1U;
  uint64_t *r20 = out + (uint32_t)2U;
  uint64_t *r30 = out + (uint32_t)3U;
  uint64_t cc = Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U, arg1[0U], arg2[0U], r00);
  uint64_t cc1 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, arg1[1U], arg2[1U], r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc1, arg1[2U], arg2[2U], r20);
  uint64_t cc3 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc2, arg1[3U], arg2[3U], r30);
  uint64_t t = cc3;
  uint64_t t0 = (uint64_t)0U - t;
  uint64_t t1 = ((uint64_t)0U - t) >> (uint32_t)32U;
  uint64_t t2 = (uint64_t)0U;
  uint64_t t3 = t - (t << (uint32_t)32U);
  uint64_t *r0 = out;
  uint64_t *r1 = out + (uint32_t)1U;
  uint64_t *r2 = out + (uint32_t)2U;
  uint64_t *r3 = out + (uint32_t)3U;
  uint64_t cc0 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, out[0U], t0, r0);
  uint64_t cc10 = Lib_IntTypes_Intrinsics_add_carry_u64(cc0, out[1U], t1, r1);
  uint64_t cc20 = Lib_IntTypes_Intrinsics_add_carry_u64(cc10, out[2U], t2, r2);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc20, out[3U], t3, r3);
}

static void p256_neg(uint64_t *arg2, uint64_t *out)
{
  uint64_t *r00 = out;
  uint64_t *r10 = out + (uint32_t)1U;
  uint64_t *r20 = out + (uint32_t)2U;
  uint64_t *r30 = out + (uint32_t)3U;
  uint64_t
  cc = Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U, (uint64_t)0U, arg2[0U], r00);
  uint64_t cc1 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, (uint64_t)0U, arg2[1U], r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc1, (uint64_t)0U, arg2[2U], r20);
  uint64_t cc3 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc2, (uint64_t)0U, arg2[3U], r30);
  uint64_t t = cc3;
  uint64_t t0 = (uint64_t)0U - t;
  uint64_t t1 = ((uint64_t)0U - t) >> (uint32_t)32U;
  uint64_t t2 = (uint64_t)0U;
  uint64_t t3 = t - (t << (uint32_t)32U);
  uint64_t *r0 = out;
  uint64_t *r1 = out + (uint32_t)1U;
  uint64_t *r2 = out + (uint32_t)2U;
  uint64_t *r3 = out + (uint32_t)3U;
  uint64_t cc0 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, out[0U], t0, r0);
  uint64_t cc10 = Lib_IntTypes_Intrinsics_add_carry_u64(cc0, out[1U], t1, r1);
  uint64_t cc20 = Lib_IntTypes_Intrinsics_add_carry_u64(cc10, out[2U], t2, r2);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc20, out[3U], t3, r3);
}

static inline void montgomery_multiplication_buffer(uint64_t *a, uint64_t *b, uint64_t *result)
{
  uint64_t t[8U] = { 0U };
  uint64_t round[8U] = { 0U };
  uint64_t t1[8U] = { 0U };
  uint64_t t2[8U] = { 0U };
  uint64_t f00 = a[0U];
  uint64_t f10 = a[1U];
  uint64_t f2 = a[2U];
  uint64_t f30 = a[3U];
  uint64_t *b00 = t;
  uint64_t temp2 = (uint64_t)0U;
  uint64_t f110 = b[1U];
  uint64_t f210 = b[2U];
  uint64_t f310 = b[3U];
  uint64_t *o00 = b00;
  uint64_t *o10 = b00 + (uint32_t)1U;
  uint64_t *o20 = b00 + (uint32_t)2U;
  uint64_t *o30 = b00 + (uint32_t)3U;
  uint64_t f020 = b[0U];
  mul64(f020, f00, o00, &temp2);
  uint64_t h0 = temp2;
  mul64(f110, f00, o10, &temp2);
  uint64_t l0 = o10[0U];
  uint64_t c10 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l0, h0, o10);
  uint64_t h1 = temp2;
  mul64(f210, f00, o20, &temp2);
  uint64_t l1 = o20[0U];
  uint64_t c2 = Lib_IntTypes_Intrinsics_add_carry_u64(c10, l1, h1, o20);
  uint64_t h2 = temp2;
  mul64(f310, f00, o30, &temp2);
  uint64_t l2 = o30[0U];
  uint64_t c3 = Lib_IntTypes_Intrinsics_add_carry_u64(c2, l2, h2, o30);
  uint64_t temp00 = temp2;
  uint64_t c00 = c3 + temp00;
  t[4U] = c00;
  uint64_t *b10 = t + (uint32_t)1U;
  uint64_t temp3[4U] = { 0U };
  uint64_t temp10 = (uint64_t)0U;
  uint64_t f111 = b[1U];
  uint64_t f211 = b[2U];
  uint64_t f311 = b[3U];
  uint64_t *o01 = temp3;
  uint64_t *o11 = temp3 + (uint32_t)1U;
  uint64_t *o21 = temp3 + (uint32_t)2U;
  uint64_t *o31 = temp3 + (uint32_t)3U;
  uint64_t f021 = b[0U];
  mul64(f021, f10, o01, &temp10);
  uint64_t h3 = temp10;
  mul64(f111, f10, o11, &temp10);
  uint64_t l3 = o11[0U];
  uint64_t c11 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l3, h3, o11);
  uint64_t h4 = temp10;
  mul64(f211, f10, o21, &temp10);
  uint64_t l4 = o21[0U];
  uint64_t c20 = Lib_IntTypes_Intrinsics_add_carry_u64(c11, l4, h4, o21);
  uint64_t h5 = temp10;
  mul64(f311, f10, o31, &temp10);
  uint64_t l5 = o31[0U];
  uint64_t c30 = Lib_IntTypes_Intrinsics_add_carry_u64(c20, l5, h5, o31);
  uint64_t temp01 = temp10;
  uint64_t c = c30 + temp01;
  uint64_t *r00 = b10;
  uint64_t *r10 = b10 + (uint32_t)1U;
  uint64_t *r20 = b10 + (uint32_t)2U;
  uint64_t *r30 = b10 + (uint32_t)3U;
  uint64_t cc00 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, temp3[0U], b10[0U], r00);
  uint64_t cc1 = Lib_IntTypes_Intrinsics_add_carry_u64(cc00, temp3[1U], b10[1U], r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_add_carry_u64(cc1, temp3[2U], b10[2U], r20);
  uint64_t cc3 = Lib_IntTypes_Intrinsics_add_carry_u64(cc2, temp3[3U], b10[3U], r30);
  uint64_t c31 = cc3;
  uint64_t c12 = c + c31;
  t[5U] = c12;
  uint64_t *b2 = t + (uint32_t)2U;
  uint64_t temp4[4U] = { 0U };
  uint64_t temp11 = (uint64_t)0U;
  uint64_t f112 = b[1U];
  uint64_t f212 = b[2U];
  uint64_t f312 = b[3U];
  uint64_t *o02 = temp4;
  uint64_t *o12 = temp4 + (uint32_t)1U;
  uint64_t *o22 = temp4 + (uint32_t)2U;
  uint64_t *o32 = temp4 + (uint32_t)3U;
  uint64_t f022 = b[0U];
  mul64(f022, f2, o02, &temp11);
  uint64_t h6 = temp11;
  mul64(f112, f2, o12, &temp11);
  uint64_t l6 = o12[0U];
  uint64_t c110 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l6, h6, o12);
  uint64_t h7 = temp11;
  mul64(f212, f2, o22, &temp11);
  uint64_t l7 = o22[0U];
  uint64_t c21 = Lib_IntTypes_Intrinsics_add_carry_u64(c110, l7, h7, o22);
  uint64_t h8 = temp11;
  mul64(f312, f2, o32, &temp11);
  uint64_t l8 = o32[0U];
  uint64_t c32 = Lib_IntTypes_Intrinsics_add_carry_u64(c21, l8, h8, o32);
  uint64_t temp02 = temp11;
  uint64_t c4 = c32 + temp02;
  uint64_t *r01 = b2;
  uint64_t *r11 = b2 + (uint32_t)1U;
  uint64_t *r21 = b2 + (uint32_t)2U;
  uint64_t *r31 = b2 + (uint32_t)3U;
  uint64_t cc01 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, temp4[0U], b2[0U], r01);
  uint64_t cc10 = Lib_IntTypes_Intrinsics_add_carry_u64(cc01, temp4[1U], b2[1U], r11);
  uint64_t cc20 = Lib_IntTypes_Intrinsics_add_carry_u64(cc10, temp4[2U], b2[2U], r21);
  uint64_t cc30 = Lib_IntTypes_Intrinsics_add_carry_u64(cc20, temp4[3U], b2[3U], r31);
  uint64_t c33 = cc30;
  uint64_t c22 = c4 + c33;
  t[6U] = c22;
  uint64_t *b3 = t + (uint32_t)3U;
  uint64_t temp5[4U] = { 0U };
  uint64_t temp1 = (uint64_t)0U;
  uint64_t f11 = b[1U];
  uint64_t f21 = b[2U];
  uint64_t f31 = b[3U];
  uint64_t *o03 = temp5;
  uint64_t *o13 = temp5 + (uint32_t)1U;
  uint64_t *o23 = temp5 + (uint32_t)2U;
  uint64_t *o33 = temp5 + (uint32_t)3U;
  uint64_t f02 = b[0U];
  mul64(f02, f30, o03, &temp1);
  uint64_t h9 = temp1;
  mul64(f11, f30, o13, &temp1);
  uint64_t l9 = o13[0U];
  uint64_t c111 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l9, h9, o13);
  uint64_t h10 = temp1;
  mul64(f21, f30, o23, &temp1);
  uint64_t l10 = o23[0U];
  uint64_t c210 = Lib_IntTypes_Intrinsics_add_carry_u64(c111, l10, h10, o23);
  uint64_t h11 = temp1;
  mul64(f31, f30, o33, &temp1);
  uint64_t l11 = o33[0U];
  uint64_t c310 = Lib_IntTypes_Intrinsics_add_carry_u64(c210, l11, h11, o33);
  uint64_t temp03 = temp1;
  uint64_t c5 = c310 + temp03;
  uint64_t *r02 = b3;
  uint64_t *r12 = b3 + (uint32_t)1U;
  uint64_t *r22 = b3 + (uint32_t)2U;
  uint64_t *r32 = b3 + (uint32_t)3U;
  uint64_t cc02 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, temp5[0U], b3[0U], r02);
  uint64_t cc11 = Lib_IntTypes_Intrinsics_add_carry_u64(cc02, temp5[1U], b3[1U], r12);
  uint64_t cc21 = Lib_IntTypes_Intrinsics_add_carry_u64(cc11, temp5[2U], b3[2U], r22);
  uint64_t cc31 = Lib_IntTypes_Intrinsics_add_carry_u64(cc21, temp5[3U], b3[3U], r32);
  uint64_t c311 = cc31;
  uint64_t c34 = c5 + c311;
  t[7U] = c34;
  uint64_t t11 = t[0U];
  uint64_t temp6 = (uint64_t)0U;
  uint64_t f01 = (uint64_t)0xffffffffffffffffU;
  uint64_t f12 = (uint64_t)0xffffffffU;
  uint64_t f32 = (uint64_t)0xffffffff00000001U;
  uint64_t *o04 = t2;
  uint64_t *o14 = t2 + (uint32_t)1U;
  uint64_t *o24 = t2 + (uint32_t)2U;
  uint64_t *o34 = t2 + (uint32_t)3U;
  mul64(f01, t11, o04, &temp6);
  uint64_t h12 = temp6;
  mul64(f12, t11, o14, &temp6);
  uint64_t l12 = o14[0U];
  uint64_t c13 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l12, h12, o14);
  uint64_t h13 = temp6;
  temp6 = (uint64_t)0U;
  o24[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c13, (uint64_t)0U, h13, o24);
  uint64_t h40 = temp6;
  mul64(f32, t11, o34, &temp6);
  uint64_t l13 = o34[0U];
  uint64_t c35 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l13, h40, o34);
  uint64_t temp04 = temp6;
  t2[4U] = c35 + temp04;
  uint64_t *a0 = t;
  uint64_t *a10 = t + (uint32_t)4U;
  uint64_t *b01 = t2;
  uint64_t *b11 = t2 + (uint32_t)4U;
  uint64_t *c01 = t1;
  uint64_t *c14 = t1 + (uint32_t)4U;
  uint64_t *r03 = c01;
  uint64_t *r13 = c01 + (uint32_t)1U;
  uint64_t *r23 = c01 + (uint32_t)2U;
  uint64_t *r33 = c01 + (uint32_t)3U;
  uint64_t cc03 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a0[0U], b01[0U], r03);
  uint64_t cc12 = Lib_IntTypes_Intrinsics_add_carry_u64(cc03, a0[1U], b01[1U], r13);
  uint64_t cc22 = Lib_IntTypes_Intrinsics_add_carry_u64(cc12, a0[2U], b01[2U], r23);
  uint64_t cc32 = Lib_IntTypes_Intrinsics_add_carry_u64(cc22, a0[3U], b01[3U], r33);
  uint64_t carry0 = cc32;
  uint64_t *r04 = c14;
  uint64_t *r14 = c14 + (uint32_t)1U;
  uint64_t *r24 = c14 + (uint32_t)2U;
  uint64_t *r34 = c14 + (uint32_t)3U;
  uint64_t cc04 = Lib_IntTypes_Intrinsics_add_carry_u64(carry0, a10[0U], b11[0U], r04);
  uint64_t cc13 = Lib_IntTypes_Intrinsics_add_carry_u64(cc04, a10[1U], b11[1U], r14);
  uint64_t cc23 = Lib_IntTypes_Intrinsics_add_carry_u64(cc13, a10[2U], b11[2U], r24);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc23, a10[3U], b11[3U], r34);
  uint64_t t12 = t1[1U];
  uint64_t t210 = t1[2U];
  uint64_t t30 = t1[3U];
  uint64_t t40 = t1[4U];
  uint64_t t50 = t1[5U];
  uint64_t t60 = t1[6U];
  uint64_t t70 = t1[7U];
  t1[0U] = t12;
  t1[1U] = t210;
  t1[2U] = t30;
  t1[3U] = t40;
  t1[4U] = t50;
  t1[5U] = t60;
  t1[6U] = t70;
  t1[7U] = (uint64_t)0U;
  uint64_t t110 = t1[0U];
  uint64_t temp7 = (uint64_t)0U;
  uint64_t f03 = (uint64_t)0xffffffffffffffffU;
  uint64_t f13 = (uint64_t)0xffffffffU;
  uint64_t f33 = (uint64_t)0xffffffff00000001U;
  uint64_t *o05 = t2;
  uint64_t *o15 = t2 + (uint32_t)1U;
  uint64_t *o25 = t2 + (uint32_t)2U;
  uint64_t *o35 = t2 + (uint32_t)3U;
  mul64(f03, t110, o05, &temp7);
  uint64_t h14 = temp7;
  mul64(f13, t110, o15, &temp7);
  uint64_t l14 = o15[0U];
  uint64_t c15 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l14, h14, o15);
  uint64_t h15 = temp7;
  temp7 = (uint64_t)0U;
  o25[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c15, (uint64_t)0U, h15, o25);
  uint64_t h41 = temp7;
  mul64(f33, t110, o35, &temp7);
  uint64_t l15 = o35[0U];
  uint64_t c36 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l15, h41, o35);
  uint64_t temp05 = temp7;
  t2[4U] = c36 + temp05;
  uint64_t *a00 = t1;
  uint64_t *a11 = t1 + (uint32_t)4U;
  uint64_t *b02 = t2;
  uint64_t *b12 = t2 + (uint32_t)4U;
  uint64_t *c02 = round;
  uint64_t *c16 = round + (uint32_t)4U;
  uint64_t *r05 = c02;
  uint64_t *r15 = c02 + (uint32_t)1U;
  uint64_t *r25 = c02 + (uint32_t)2U;
  uint64_t *r35 = c02 + (uint32_t)3U;
  uint64_t cc05 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a00[0U], b02[0U], r05);
  uint64_t cc14 = Lib_IntTypes_Intrinsics_add_carry_u64(cc05, a00[1U], b02[1U], r15);
  uint64_t cc24 = Lib_IntTypes_Intrinsics_add_carry_u64(cc14, a00[2U], b02[2U], r25);
  uint64_t cc33 = Lib_IntTypes_Intrinsics_add_carry_u64(cc24, a00[3U], b02[3U], r35);
  uint64_t carry00 = cc33;
  uint64_t *r06 = c16;
  uint64_t *r16 = c16 + (uint32_t)1U;
  uint64_t *r26 = c16 + (uint32_t)2U;
  uint64_t *r36 = c16 + (uint32_t)3U;
  uint64_t cc06 = Lib_IntTypes_Intrinsics_add_carry_u64(carry00, a11[0U], b12[0U], r06);
  uint64_t cc15 = Lib_IntTypes_Intrinsics_add_carry_u64(cc06, a11[1U], b12[1U], r16);
  uint64_t cc25 = Lib_IntTypes_Intrinsics_add_carry_u64(cc15, a11[2U], b12[2U], r26);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc25, a11[3U], b12[3U], r36);
  uint64_t t120 = round[1U];
  uint64_t t211 = round[2U];
  uint64_t t31 = round[3U];
  uint64_t t41 = round[4U];
  uint64_t t51 = round[5U];
  uint64_t t61 = round[6U];
  uint64_t t71 = round[7U];
  round[0U] = t120;
  round[1U] = t211;
  round[2U] = t31;
  round[3U] = t41;
  round[4U] = t51;
  round[5U] = t61;
  round[6U] = t71;
  round[7U] = (uint64_t)0U;
  uint64_t t111 = round[0U];
  uint64_t temp8 = (uint64_t)0U;
  uint64_t f04 = (uint64_t)0xffffffffffffffffU;
  uint64_t f14 = (uint64_t)0xffffffffU;
  uint64_t f34 = (uint64_t)0xffffffff00000001U;
  uint64_t *o06 = t2;
  uint64_t *o16 = t2 + (uint32_t)1U;
  uint64_t *o26 = t2 + (uint32_t)2U;
  uint64_t *o36 = t2 + (uint32_t)3U;
  mul64(f04, t111, o06, &temp8);
  uint64_t h16 = temp8;
  mul64(f14, t111, o16, &temp8);
  uint64_t l16 = o16[0U];
  uint64_t c17 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l16, h16, o16);
  uint64_t h17 = temp8;
  temp8 = (uint64_t)0U;
  o26[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c17, (uint64_t)0U, h17, o26);
  uint64_t h42 = temp8;
  mul64(f34, t111, o36, &temp8);
  uint64_t l17 = o36[0U];
  uint64_t c37 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l17, h42, o36);
  uint64_t temp06 = temp8;
  t2[4U] = c37 + temp06;
  uint64_t *a01 = round;
  uint64_t *a12 = round + (uint32_t)4U;
  uint64_t *b03 = t2;
  uint64_t *b13 = t2 + (uint32_t)4U;
  uint64_t *c03 = t1;
  uint64_t *c18 = t1 + (uint32_t)4U;
  uint64_t *r07 = c03;
  uint64_t *r17 = c03 + (uint32_t)1U;
  uint64_t *r27 = c03 + (uint32_t)2U;
  uint64_t *r37 = c03 + (uint32_t)3U;
  uint64_t cc07 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a01[0U], b03[0U], r07);
  uint64_t cc16 = Lib_IntTypes_Intrinsics_add_carry_u64(cc07, a01[1U], b03[1U], r17);
  uint64_t cc26 = Lib_IntTypes_Intrinsics_add_carry_u64(cc16, a01[2U], b03[2U], r27);
  uint64_t cc34 = Lib_IntTypes_Intrinsics_add_carry_u64(cc26, a01[3U], b03[3U], r37);
  uint64_t carry01 = cc34;
  uint64_t *r08 = c18;
  uint64_t *r18 = c18 + (uint32_t)1U;
  uint64_t *r28 = c18 + (uint32_t)2U;
  uint64_t *r38 = c18 + (uint32_t)3U;
  uint64_t cc08 = Lib_IntTypes_Intrinsics_add_carry_u64(carry01, a12[0U], b13[0U], r08);
  uint64_t cc17 = Lib_IntTypes_Intrinsics_add_carry_u64(cc08, a12[1U], b13[1U], r18);
  uint64_t cc27 = Lib_IntTypes_Intrinsics_add_carry_u64(cc17, a12[2U], b13[2U], r28);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc27, a12[3U], b13[3U], r38);
  uint64_t t121 = t1[1U];
  uint64_t t212 = t1[2U];
  uint64_t t32 = t1[3U];
  uint64_t t42 = t1[4U];
  uint64_t t52 = t1[5U];
  uint64_t t62 = t1[6U];
  uint64_t t72 = t1[7U];
  t1[0U] = t121;
  t1[1U] = t212;
  t1[2U] = t32;
  t1[3U] = t42;
  t1[4U] = t52;
  t1[5U] = t62;
  t1[6U] = t72;
  t1[7U] = (uint64_t)0U;
  uint64_t t112 = t1[0U];
  uint64_t temp = (uint64_t)0U;
  uint64_t f0 = (uint64_t)0xffffffffffffffffU;
  uint64_t f1 = (uint64_t)0xffffffffU;
  uint64_t f3 = (uint64_t)0xffffffff00000001U;
  uint64_t *o0 = t2;
  uint64_t *o1 = t2 + (uint32_t)1U;
  uint64_t *o2 = t2 + (uint32_t)2U;
  uint64_t *o3 = t2 + (uint32_t)3U;
  mul64(f0, t112, o0, &temp);
  uint64_t h18 = temp;
  mul64(f1, t112, o1, &temp);
  uint64_t l18 = o1[0U];
  uint64_t c19 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l18, h18, o1);
  uint64_t h = temp;
  temp = (uint64_t)0U;
  o2[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c19, (uint64_t)0U, h, o2);
  uint64_t h43 = temp;
  mul64(f3, t112, o3, &temp);
  uint64_t l = o3[0U];
  uint64_t c38 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l, h43, o3);
  uint64_t temp0 = temp;
  t2[4U] = c38 + temp0;
  uint64_t *a02 = t1;
  uint64_t *a1 = t1 + (uint32_t)4U;
  uint64_t *b0 = t2;
  uint64_t *b1 = t2 + (uint32_t)4U;
  uint64_t *c0 = round;
  uint64_t *c1 = round + (uint32_t)4U;
  uint64_t *r09 = c0;
  uint64_t *r19 = c0 + (uint32_t)1U;
  uint64_t *r29 = c0 + (uint32_t)2U;
  uint64_t *r39 = c0 + (uint32_t)3U;
  uint64_t cc09 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a02[0U], b0[0U], r09);
  uint64_t cc18 = Lib_IntTypes_Intrinsics_add_carry_u64(cc09, a02[1U], b0[1U], r19);
  uint64_t cc28 = Lib_IntTypes_Intrinsics_add_carry_u64(cc18, a02[2U], b0[2U], r29);
  uint64_t cc35 = Lib_IntTypes_Intrinsics_add_carry_u64(cc28, a02[3U], b0[3U], r39);
  uint64_t carry02 = cc35;
  uint64_t *r010 = c1;
  uint64_t *r110 = c1 + (uint32_t)1U;
  uint64_t *r210 = c1 + (uint32_t)2U;
  uint64_t *r310 = c1 + (uint32_t)3U;
  uint64_t cc0 = Lib_IntTypes_Intrinsics_add_carry_u64(carry02, a1[0U], b1[0U], r010);
  uint64_t cc19 = Lib_IntTypes_Intrinsics_add_carry_u64(cc0, a1[1U], b1[1U], r110);
  uint64_t cc29 = Lib_IntTypes_Intrinsics_add_carry_u64(cc19, a1[2U], b1[2U], r210);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc29, a1[3U], b1[3U], r310);
  uint64_t t122 = round[1U];
  uint64_t t21 = round[2U];
  uint64_t t3 = round[3U];
  uint64_t t4 = round[4U];
  uint64_t t5 = round[5U];
  uint64_t t6 = round[6U];
  uint64_t t7 = round[7U];
  round[0U] = t122;
  round[1U] = t21;
  round[2U] = t3;
  round[3U] = t4;
  round[4U] = t5;
  round[5U] = t6;
  round[6U] = t7;
  round[7U] = (uint64_t)0U;
  uint64_t tempBuffer[4U] = { 0U };
  uint64_t tempBufferForSubborrow = (uint64_t)0U;
  uint64_t cin = round[4U];
  uint64_t *x_ = round;
  uint64_t *r0 = tempBuffer;
  uint64_t *r1 = tempBuffer + (uint32_t)1U;
  uint64_t *r2 = tempBuffer + (uint32_t)2U;
  uint64_t *r3 = tempBuffer + (uint32_t)3U;
  uint64_t
  cc =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      x_[0U],
      (uint64_t)0xffffffffffffffffU,
      r0);
  uint64_t cc110 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, x_[1U], (uint64_t)0xffffffffU, r1);
  uint64_t cc210 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc110, x_[2U], (uint64_t)0U, r2);
  uint64_t
  cc36 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc210, x_[3U], (uint64_t)0xffffffff00000001U, r3);
  uint64_t c6 = cc36;
  uint64_t
  carry = Lib_IntTypes_Intrinsics_sub_borrow_u64(c6, cin, (uint64_t)0U, &tempBufferForSubborrow);
  cmovznz4(result, tempBuffer, x_, carry);
}

static inline void montgomery_square_buffer(uint64_t *a, uint64_t *result)
{
  uint64_t t[8U] = { 0U };
  uint64_t t1[8U] = { 0U };
  uint64_t t2[8U] = { 0U };
  uint64_t round[8U] = { 0U };
  uint64_t wb[17U] = { 0U };
  uint64_t *tb = wb;
  uint64_t *memory = wb + (uint32_t)5U;
  uint64_t *b00 = t;
  uint64_t f01 = a[0U];
  uint64_t f310 = a[3U];
  uint64_t *o30 = b00 + (uint32_t)3U;
  uint64_t *temp1 = tb;
  uint64_t f02 = a[0U];
  uint64_t f12 = a[1U];
  uint64_t f22 = a[2U];
  uint64_t *o01 = b00;
  uint64_t *o10 = b00 + (uint32_t)1U;
  uint64_t *o20 = b00 + (uint32_t)2U;
  mul64(f02, f02, o01, temp1);
  uint64_t h_00 = temp1[0U];
  mul64(f02, f12, o10, temp1);
  uint64_t l0 = o10[0U];
  memory[0U] = l0;
  memory[1U] = temp1[0U];
  uint64_t c10 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l0, h_00, o10);
  uint64_t h_10 = temp1[0U];
  mul64(f02, f22, o20, temp1);
  uint64_t l10 = o20[0U];
  memory[2U] = l10;
  memory[3U] = temp1[0U];
  uint64_t c2 = Lib_IntTypes_Intrinsics_add_carry_u64(c10, l10, h_10, o20);
  uint64_t h_20 = temp1[0U];
  mul64(f01, f310, o30, temp1);
  uint64_t l3 = o30[0U];
  memory[4U] = l3;
  memory[5U] = temp1[0U];
  uint64_t c3 = Lib_IntTypes_Intrinsics_add_carry_u64(c2, l3, h_20, o30);
  uint64_t temp00 = temp1[0U];
  uint64_t c00 = c3 + temp00;
  t[4U] = c00;
  uint64_t *b10 = t + (uint32_t)1U;
  uint64_t *temp2 = tb;
  uint64_t *tempBufferResult0 = tb + (uint32_t)1U;
  uint64_t f11 = a[1U];
  uint64_t f210 = a[2U];
  uint64_t f311 = a[3U];
  uint64_t *o00 = tempBufferResult0;
  uint64_t *o11 = tempBufferResult0 + (uint32_t)1U;
  uint64_t *o21 = tempBufferResult0 + (uint32_t)2U;
  uint64_t *o31 = tempBufferResult0 + (uint32_t)3U;
  o00[0U] = memory[0U];
  uint64_t h_01 = memory[1U];
  mul64(f11, f11, o11, temp2);
  uint64_t l4 = o11[0U];
  uint64_t c11 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l4, h_01, o11);
  uint64_t h_11 = temp2[0U];
  mul64(f11, f210, o21, temp2);
  uint64_t l11 = o21[0U];
  memory[6U] = l11;
  memory[7U] = temp2[0U];
  uint64_t c20 = Lib_IntTypes_Intrinsics_add_carry_u64(c11, l11, h_11, o21);
  uint64_t h_21 = temp2[0U];
  mul64(f11, f311, o31, temp2);
  uint64_t l20 = o31[0U];
  memory[8U] = l20;
  memory[9U] = temp2[0U];
  uint64_t c30 = Lib_IntTypes_Intrinsics_add_carry_u64(c20, l20, h_21, o31);
  uint64_t h_30 = temp2[0U];
  uint64_t *r00 = b10;
  uint64_t *r10 = b10 + (uint32_t)1U;
  uint64_t *r20 = b10 + (uint32_t)2U;
  uint64_t *r30 = b10 + (uint32_t)3U;
  uint64_t
  cc00 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, tempBufferResult0[0U], b10[0U], r00);
  uint64_t
  cc1 = Lib_IntTypes_Intrinsics_add_carry_u64(cc00, tempBufferResult0[1U], b10[1U], r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_add_carry_u64(cc1, tempBufferResult0[2U], b10[2U], r20);
  uint64_t cc3 = Lib_IntTypes_Intrinsics_add_carry_u64(cc2, tempBufferResult0[3U], b10[3U], r30);
  uint64_t c4 = cc3;
  uint64_t c12 = c30 + h_30 + c4;
  t[5U] = c12;
  uint64_t *b2 = t + (uint32_t)2U;
  uint64_t *temp3 = tb;
  uint64_t *tempBufferResult1 = tb + (uint32_t)1U;
  uint64_t f21 = a[2U];
  uint64_t f312 = a[3U];
  uint64_t *o02 = tempBufferResult1;
  uint64_t *o12 = tempBufferResult1 + (uint32_t)1U;
  uint64_t *o22 = tempBufferResult1 + (uint32_t)2U;
  uint64_t *o32 = tempBufferResult1 + (uint32_t)3U;
  o02[0U] = memory[2U];
  uint64_t h_0 = memory[3U];
  o12[0U] = memory[6U];
  uint64_t l5 = o12[0U];
  uint64_t c110 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l5, h_0, o12);
  uint64_t h_1 = memory[7U];
  mul64(f21, f21, o22, temp3);
  uint64_t l12 = o22[0U];
  uint64_t c21 = Lib_IntTypes_Intrinsics_add_carry_u64(c110, l12, h_1, o22);
  uint64_t h_2 = temp3[0U];
  mul64(f21, f312, o32, temp3);
  uint64_t l21 = o32[0U];
  memory[10U] = l21;
  memory[11U] = temp3[0U];
  uint64_t c31 = Lib_IntTypes_Intrinsics_add_carry_u64(c21, l21, h_2, o32);
  uint64_t h_31 = temp3[0U];
  uint64_t *r01 = b2;
  uint64_t *r11 = b2 + (uint32_t)1U;
  uint64_t *r21 = b2 + (uint32_t)2U;
  uint64_t *r31 = b2 + (uint32_t)3U;
  uint64_t
  cc01 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, tempBufferResult1[0U], b2[0U], r01);
  uint64_t
  cc10 = Lib_IntTypes_Intrinsics_add_carry_u64(cc01, tempBufferResult1[1U], b2[1U], r11);
  uint64_t
  cc20 = Lib_IntTypes_Intrinsics_add_carry_u64(cc10, tempBufferResult1[2U], b2[2U], r21);
  uint64_t
  cc30 = Lib_IntTypes_Intrinsics_add_carry_u64(cc20, tempBufferResult1[3U], b2[3U], r31);
  uint64_t c40 = cc30;
  uint64_t c22 = c31 + h_31 + c40;
  t[6U] = c22;
  uint64_t *b3 = t + (uint32_t)3U;
  uint64_t *temp4 = tb;
  uint64_t *tempBufferResult = tb + (uint32_t)1U;
  uint64_t f31 = a[3U];
  uint64_t *o03 = tempBufferResult;
  uint64_t *o13 = tempBufferResult + (uint32_t)1U;
  uint64_t *o23 = tempBufferResult + (uint32_t)2U;
  uint64_t *o33 = tempBufferResult + (uint32_t)3U;
  o03[0U] = memory[4U];
  uint64_t h0 = memory[5U];
  o13[0U] = memory[8U];
  uint64_t l6 = o13[0U];
  uint64_t c111 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l6, h0, o13);
  uint64_t h4 = memory[9U];
  o23[0U] = memory[10U];
  uint64_t l1 = o23[0U];
  uint64_t c210 = Lib_IntTypes_Intrinsics_add_carry_u64(c111, l1, h4, o23);
  uint64_t h5 = memory[11U];
  mul64(f31, f31, o33, temp4);
  uint64_t l2 = o33[0U];
  uint64_t c310 = Lib_IntTypes_Intrinsics_add_carry_u64(c210, l2, h5, o33);
  uint64_t h_3 = temp4[0U];
  uint64_t *r02 = b3;
  uint64_t *r12 = b3 + (uint32_t)1U;
  uint64_t *r22 = b3 + (uint32_t)2U;
  uint64_t *r32 = b3 + (uint32_t)3U;
  uint64_t
  cc02 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, tempBufferResult[0U], b3[0U], r02);
  uint64_t cc11 = Lib_IntTypes_Intrinsics_add_carry_u64(cc02, tempBufferResult[1U], b3[1U], r12);
  uint64_t cc21 = Lib_IntTypes_Intrinsics_add_carry_u64(cc11, tempBufferResult[2U], b3[2U], r22);
  uint64_t cc31 = Lib_IntTypes_Intrinsics_add_carry_u64(cc21, tempBufferResult[3U], b3[3U], r32);
  uint64_t c41 = cc31;
  uint64_t c32 = c310 + h_3 + c41;
  t[7U] = c32;
  uint64_t t11 = t[0U];
  uint64_t temp5 = (uint64_t)0U;
  uint64_t f00 = (uint64_t)0xffffffffffffffffU;
  uint64_t f10 = (uint64_t)0xffffffffU;
  uint64_t f30 = (uint64_t)0xffffffff00000001U;
  uint64_t *o04 = t2;
  uint64_t *o14 = t2 + (uint32_t)1U;
  uint64_t *o24 = t2 + (uint32_t)2U;
  uint64_t *o34 = t2 + (uint32_t)3U;
  mul64(f00, t11, o04, &temp5);
  uint64_t h1 = temp5;
  mul64(f10, t11, o14, &temp5);
  uint64_t l7 = o14[0U];
  uint64_t c13 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l7, h1, o14);
  uint64_t h2 = temp5;
  temp5 = (uint64_t)0U;
  o24[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c13, (uint64_t)0U, h2, o24);
  uint64_t h40 = temp5;
  mul64(f30, t11, o34, &temp5);
  uint64_t l8 = o34[0U];
  uint64_t c33 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l8, h40, o34);
  uint64_t temp01 = temp5;
  t2[4U] = c33 + temp01;
  uint64_t *a0 = t;
  uint64_t *a10 = t + (uint32_t)4U;
  uint64_t *b01 = t2;
  uint64_t *b11 = t2 + (uint32_t)4U;
  uint64_t *c01 = t1;
  uint64_t *c14 = t1 + (uint32_t)4U;
  uint64_t *r03 = c01;
  uint64_t *r13 = c01 + (uint32_t)1U;
  uint64_t *r23 = c01 + (uint32_t)2U;
  uint64_t *r33 = c01 + (uint32_t)3U;
  uint64_t cc03 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a0[0U], b01[0U], r03);
  uint64_t cc12 = Lib_IntTypes_Intrinsics_add_carry_u64(cc03, a0[1U], b01[1U], r13);
  uint64_t cc22 = Lib_IntTypes_Intrinsics_add_carry_u64(cc12, a0[2U], b01[2U], r23);
  uint64_t cc32 = Lib_IntTypes_Intrinsics_add_carry_u64(cc22, a0[3U], b01[3U], r33);
  uint64_t carry0 = cc32;
  uint64_t *r04 = c14;
  uint64_t *r14 = c14 + (uint32_t)1U;
  uint64_t *r24 = c14 + (uint32_t)2U;
  uint64_t *r34 = c14 + (uint32_t)3U;
  uint64_t cc04 = Lib_IntTypes_Intrinsics_add_carry_u64(carry0, a10[0U], b11[0U], r04);
  uint64_t cc13 = Lib_IntTypes_Intrinsics_add_carry_u64(cc04, a10[1U], b11[1U], r14);
  uint64_t cc23 = Lib_IntTypes_Intrinsics_add_carry_u64(cc13, a10[2U], b11[2U], r24);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc23, a10[3U], b11[3U], r34);
  uint64_t t12 = t1[1U];
  uint64_t t210 = t1[2U];
  uint64_t t30 = t1[3U];
  uint64_t t40 = t1[4U];
  uint64_t t50 = t1[5U];
  uint64_t t60 = t1[6U];
  uint64_t t70 = t1[7U];
  t1[0U] = t12;
  t1[1U] = t210;
  t1[2U] = t30;
  t1[3U] = t40;
  t1[4U] = t50;
  t1[5U] = t60;
  t1[6U] = t70;
  t1[7U] = (uint64_t)0U;
  uint64_t t110 = t1[0U];
  uint64_t temp6 = (uint64_t)0U;
  uint64_t f03 = (uint64_t)0xffffffffffffffffU;
  uint64_t f13 = (uint64_t)0xffffffffU;
  uint64_t f32 = (uint64_t)0xffffffff00000001U;
  uint64_t *o05 = t2;
  uint64_t *o15 = t2 + (uint32_t)1U;
  uint64_t *o25 = t2 + (uint32_t)2U;
  uint64_t *o35 = t2 + (uint32_t)3U;
  mul64(f03, t110, o05, &temp6);
  uint64_t h3 = temp6;
  mul64(f13, t110, o15, &temp6);
  uint64_t l9 = o15[0U];
  uint64_t c15 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l9, h3, o15);
  uint64_t h6 = temp6;
  temp6 = (uint64_t)0U;
  o25[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c15, (uint64_t)0U, h6, o25);
  uint64_t h41 = temp6;
  mul64(f32, t110, o35, &temp6);
  uint64_t l13 = o35[0U];
  uint64_t c34 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l13, h41, o35);
  uint64_t temp02 = temp6;
  t2[4U] = c34 + temp02;
  uint64_t *a00 = t1;
  uint64_t *a11 = t1 + (uint32_t)4U;
  uint64_t *b02 = t2;
  uint64_t *b12 = t2 + (uint32_t)4U;
  uint64_t *c02 = round;
  uint64_t *c16 = round + (uint32_t)4U;
  uint64_t *r05 = c02;
  uint64_t *r15 = c02 + (uint32_t)1U;
  uint64_t *r25 = c02 + (uint32_t)2U;
  uint64_t *r35 = c02 + (uint32_t)3U;
  uint64_t cc05 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a00[0U], b02[0U], r05);
  uint64_t cc14 = Lib_IntTypes_Intrinsics_add_carry_u64(cc05, a00[1U], b02[1U], r15);
  uint64_t cc24 = Lib_IntTypes_Intrinsics_add_carry_u64(cc14, a00[2U], b02[2U], r25);
  uint64_t cc33 = Lib_IntTypes_Intrinsics_add_carry_u64(cc24, a00[3U], b02[3U], r35);
  uint64_t carry00 = cc33;
  uint64_t *r06 = c16;
  uint64_t *r16 = c16 + (uint32_t)1U;
  uint64_t *r26 = c16 + (uint32_t)2U;
  uint64_t *r36 = c16 + (uint32_t)3U;
  uint64_t cc06 = Lib_IntTypes_Intrinsics_add_carry_u64(carry00, a11[0U], b12[0U], r06);
  uint64_t cc15 = Lib_IntTypes_Intrinsics_add_carry_u64(cc06, a11[1U], b12[1U], r16);
  uint64_t cc25 = Lib_IntTypes_Intrinsics_add_carry_u64(cc15, a11[2U], b12[2U], r26);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc25, a11[3U], b12[3U], r36);
  uint64_t t120 = round[1U];
  uint64_t t211 = round[2U];
  uint64_t t31 = round[3U];
  uint64_t t41 = round[4U];
  uint64_t t51 = round[5U];
  uint64_t t61 = round[6U];
  uint64_t t71 = round[7U];
  round[0U] = t120;
  round[1U] = t211;
  round[2U] = t31;
  round[3U] = t41;
  round[4U] = t51;
  round[5U] = t61;
  round[6U] = t71;
  round[7U] = (uint64_t)0U;
  uint64_t t111 = round[0U];
  uint64_t temp7 = (uint64_t)0U;
  uint64_t f04 = (uint64_t)0xffffffffffffffffU;
  uint64_t f14 = (uint64_t)0xffffffffU;
  uint64_t f33 = (uint64_t)0xffffffff00000001U;
  uint64_t *o06 = t2;
  uint64_t *o16 = t2 + (uint32_t)1U;
  uint64_t *o26 = t2 + (uint32_t)2U;
  uint64_t *o36 = t2 + (uint32_t)3U;
  mul64(f04, t111, o06, &temp7);
  uint64_t h7 = temp7;
  mul64(f14, t111, o16, &temp7);
  uint64_t l14 = o16[0U];
  uint64_t c17 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l14, h7, o16);
  uint64_t h8 = temp7;
  temp7 = (uint64_t)0U;
  o26[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c17, (uint64_t)0U, h8, o26);
  uint64_t h42 = temp7;
  mul64(f33, t111, o36, &temp7);
  uint64_t l15 = o36[0U];
  uint64_t c35 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l15, h42, o36);
  uint64_t temp03 = temp7;
  t2[4U] = c35 + temp03;
  uint64_t *a01 = round;
  uint64_t *a12 = round + (uint32_t)4U;
  uint64_t *b03 = t2;
  uint64_t *b13 = t2 + (uint32_t)4U;
  uint64_t *c03 = t1;
  uint64_t *c18 = t1 + (uint32_t)4U;
  uint64_t *r07 = c03;
  uint64_t *r17 = c03 + (uint32_t)1U;
  uint64_t *r27 = c03 + (uint32_t)2U;
  uint64_t *r37 = c03 + (uint32_t)3U;
  uint64_t cc07 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a01[0U], b03[0U], r07);
  uint64_t cc16 = Lib_IntTypes_Intrinsics_add_carry_u64(cc07, a01[1U], b03[1U], r17);
  uint64_t cc26 = Lib_IntTypes_Intrinsics_add_carry_u64(cc16, a01[2U], b03[2U], r27);
  uint64_t cc34 = Lib_IntTypes_Intrinsics_add_carry_u64(cc26, a01[3U], b03[3U], r37);
  uint64_t carry01 = cc34;
  uint64_t *r08 = c18;
  uint64_t *r18 = c18 + (uint32_t)1U;
  uint64_t *r28 = c18 + (uint32_t)2U;
  uint64_t *r38 = c18 + (uint32_t)3U;
  uint64_t cc08 = Lib_IntTypes_Intrinsics_add_carry_u64(carry01, a12[0U], b13[0U], r08);
  uint64_t cc17 = Lib_IntTypes_Intrinsics_add_carry_u64(cc08, a12[1U], b13[1U], r18);
  uint64_t cc27 = Lib_IntTypes_Intrinsics_add_carry_u64(cc17, a12[2U], b13[2U], r28);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc27, a12[3U], b13[3U], r38);
  uint64_t t121 = t1[1U];
  uint64_t t212 = t1[2U];
  uint64_t t32 = t1[3U];
  uint64_t t42 = t1[4U];
  uint64_t t52 = t1[5U];
  uint64_t t62 = t1[6U];
  uint64_t t72 = t1[7U];
  t1[0U] = t121;
  t1[1U] = t212;
  t1[2U] = t32;
  t1[3U] = t42;
  t1[4U] = t52;
  t1[5U] = t62;
  t1[6U] = t72;
  t1[7U] = (uint64_t)0U;
  uint64_t t112 = t1[0U];
  uint64_t temp = (uint64_t)0U;
  uint64_t f0 = (uint64_t)0xffffffffffffffffU;
  uint64_t f1 = (uint64_t)0xffffffffU;
  uint64_t f3 = (uint64_t)0xffffffff00000001U;
  uint64_t *o0 = t2;
  uint64_t *o1 = t2 + (uint32_t)1U;
  uint64_t *o2 = t2 + (uint32_t)2U;
  uint64_t *o3 = t2 + (uint32_t)3U;
  mul64(f0, t112, o0, &temp);
  uint64_t h9 = temp;
  mul64(f1, t112, o1, &temp);
  uint64_t l16 = o1[0U];
  uint64_t c19 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l16, h9, o1);
  uint64_t h = temp;
  temp = (uint64_t)0U;
  o2[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c19, (uint64_t)0U, h, o2);
  uint64_t h43 = temp;
  mul64(f3, t112, o3, &temp);
  uint64_t l = o3[0U];
  uint64_t c36 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l, h43, o3);
  uint64_t temp0 = temp;
  t2[4U] = c36 + temp0;
  uint64_t *a02 = t1;
  uint64_t *a1 = t1 + (uint32_t)4U;
  uint64_t *b0 = t2;
  uint64_t *b1 = t2 + (uint32_t)4U;
  uint64_t *c0 = round;
  uint64_t *c1 = round + (uint32_t)4U;
  uint64_t *r09 = c0;
  uint64_t *r19 = c0 + (uint32_t)1U;
  uint64_t *r29 = c0 + (uint32_t)2U;
  uint64_t *r39 = c0 + (uint32_t)3U;
  uint64_t cc09 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a02[0U], b0[0U], r09);
  uint64_t cc18 = Lib_IntTypes_Intrinsics_add_carry_u64(cc09, a02[1U], b0[1U], r19);
  uint64_t cc28 = Lib_IntTypes_Intrinsics_add_carry_u64(cc18, a02[2U], b0[2U], r29);
  uint64_t cc35 = Lib_IntTypes_Intrinsics_add_carry_u64(cc28, a02[3U], b0[3U], r39);
  uint64_t carry02 = cc35;
  uint64_t *r010 = c1;
  uint64_t *r110 = c1 + (uint32_t)1U;
  uint64_t *r210 = c1 + (uint32_t)2U;
  uint64_t *r310 = c1 + (uint32_t)3U;
  uint64_t cc0 = Lib_IntTypes_Intrinsics_add_carry_u64(carry02, a1[0U], b1[0U], r010);
  uint64_t cc19 = Lib_IntTypes_Intrinsics_add_carry_u64(cc0, a1[1U], b1[1U], r110);
  uint64_t cc29 = Lib_IntTypes_Intrinsics_add_carry_u64(cc19, a1[2U], b1[2U], r210);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc29, a1[3U], b1[3U], r310);
  uint64_t t122 = round[1U];
  uint64_t t21 = round[2U];
  uint64_t t3 = round[3U];
  uint64_t t4 = round[4U];
  uint64_t t5 = round[5U];
  uint64_t t6 = round[6U];
  uint64_t t7 = round[7U];
  round[0U] = t122;
  round[1U] = t21;
  round[2U] = t3;
  round[3U] = t4;
  round[4U] = t5;
  round[5U] = t6;
  round[6U] = t7;
  round[7U] = (uint64_t)0U;
  uint64_t tempBuffer[4U] = { 0U };
  uint64_t tempBufferForSubborrow = (uint64_t)0U;
  uint64_t cin = round[4U];
  uint64_t *x_ = round;
  uint64_t *r0 = tempBuffer;
  uint64_t *r1 = tempBuffer + (uint32_t)1U;
  uint64_t *r2 = tempBuffer + (uint32_t)2U;
  uint64_t *r3 = tempBuffer + (uint32_t)3U;
  uint64_t
  cc =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      x_[0U],
      (uint64_t)0xffffffffffffffffU,
      r0);
  uint64_t cc110 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, x_[1U], (uint64_t)0xffffffffU, r1);
  uint64_t cc210 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc110, x_[2U], (uint64_t)0U, r2);
  uint64_t
  cc36 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc210, x_[3U], (uint64_t)0xffffffff00000001U, r3);
  uint64_t c = cc36;
  uint64_t
  carry = Lib_IntTypes_Intrinsics_sub_borrow_u64(c, cin, (uint64_t)0U, &tempBufferForSubborrow);
  cmovznz4(result, tempBuffer, x_, carry);
}

static inline void exponent(uint64_t *t, uint64_t *result, uint64_t *tempBuffer)
{
  uint64_t *t0 = tempBuffer;
  uint64_t *t1 = tempBuffer + (uint32_t)4U;
  uint64_t *t2 = tempBuffer + (uint32_t)8U;
  uint64_t *t3 = tempBuffer + (uint32_t)12U;
  uint64_t *t4 = tempBuffer + (uint32_t)16U;
  uint64_t *t5 = tempBuffer + (uint32_t)20U;
  uint64_t *t6 = tempBuffer + (uint32_t)24U;
  uint64_t *t7 = tempBuffer + (uint32_t)28U;
  montgomery_square_buffer(t, t0);
  montgomery_multiplication_buffer(t0, t, t2);
  montgomery_square_buffer(t2, t0);
  montgomery_square_buffer(t0, t0);
  montgomery_multiplication_buffer(t0, t2, t6);
  montgomery_square_buffer(t6, t0);
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  montgomery_multiplication_buffer(t0, t6, t7);
  montgomery_square_buffer(t7, t0);
  montgomery_square_buffer(t0, t0);
  montgomery_multiplication_buffer(t0, t2, t1);
  montgomery_square_buffer(t1, t0);
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  montgomery_multiplication_buffer(t0, t1, t3);
  montgomery_square_buffer(t3, t0);
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  montgomery_multiplication_buffer(t0, t1, t4);
  montgomery_square_buffer(t4, t0);
  montgomery_square_buffer(t0, t0);
  montgomery_multiplication_buffer(t0, t2, t5);
  montgomery_square_buffer(t5, t0);
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)31U; i++)
  {
    montgomery_square_buffer(t0, t0);
  }
  montgomery_multiplication_buffer(t0, t, t0);
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)128U; i++)
  {
    montgomery_square_buffer(t0, t0);
  }
  montgomery_multiplication_buffer(t0, t5, t0);
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)32U; i++)
  {
    montgomery_square_buffer(t0, t0);
  }
  montgomery_multiplication_buffer(t0, t5, t0);
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)30U; i++)
  {
    montgomery_square_buffer(t0, t0);
  }
  montgomery_multiplication_buffer(t0, t4, t0);
  {
    montgomery_square_buffer(t0, t0);
  }
  {
    montgomery_square_buffer(t0, t0);
  }
  montgomery_multiplication_buffer(t0, t, result);
}

static void solinas_reduction_impl(uint64_t *i, uint64_t *o)
{
  uint64_t tempBuffer[36U] = { 0U };
  uint64_t i0 = i[0U];
  uint64_t i1 = i[1U];
  uint64_t i2 = i[2U];
  uint64_t i3 = i[3U];
  uint64_t i4 = i[4U];
  uint64_t i5 = i[5U];
  uint64_t i6 = i[6U];
  uint64_t i7 = i[7U];
  uint32_t c0 = (uint32_t)i0;
  uint32_t c1 = (uint32_t)(i0 >> (uint32_t)32U);
  uint32_t c2 = (uint32_t)i1;
  uint32_t c3 = (uint32_t)(i1 >> (uint32_t)32U);
  uint32_t c4 = (uint32_t)i2;
  uint32_t c5 = (uint32_t)(i2 >> (uint32_t)32U);
  uint32_t c6 = (uint32_t)i3;
  uint32_t c7 = (uint32_t)(i3 >> (uint32_t)32U);
  uint32_t c8 = (uint32_t)i4;
  uint32_t c9 = (uint32_t)(i4 >> (uint32_t)32U);
  uint32_t c10 = (uint32_t)i5;
  uint32_t c11 = (uint32_t)(i5 >> (uint32_t)32U);
  uint32_t c12 = (uint32_t)i6;
  uint32_t c13 = (uint32_t)(i6 >> (uint32_t)32U);
  uint32_t c14 = (uint32_t)i7;
  uint32_t c15 = (uint32_t)(i7 >> (uint32_t)32U);
  uint64_t *t01 = tempBuffer;
  uint64_t *t110 = tempBuffer + (uint32_t)4U;
  uint64_t *t210 = tempBuffer + (uint32_t)8U;
  uint64_t *t310 = tempBuffer + (uint32_t)12U;
  uint64_t *t410 = tempBuffer + (uint32_t)16U;
  uint64_t *t510 = tempBuffer + (uint32_t)20U;
  uint64_t *t610 = tempBuffer + (uint32_t)24U;
  uint64_t *t710 = tempBuffer + (uint32_t)28U;
  uint64_t *t810 = tempBuffer + (uint32_t)32U;
  uint64_t as_uint64_high0 = (uint64_t)c1;
  uint64_t as_uint64_high1 = as_uint64_high0 << (uint32_t)32U;
  uint64_t as_uint64_low0 = (uint64_t)c0;
  uint64_t b0 = as_uint64_low0 ^ as_uint64_high1;
  uint64_t as_uint64_high2 = (uint64_t)c3;
  uint64_t as_uint64_high10 = as_uint64_high2 << (uint32_t)32U;
  uint64_t as_uint64_low1 = (uint64_t)c2;
  uint64_t b1 = as_uint64_low1 ^ as_uint64_high10;
  uint64_t as_uint64_high3 = (uint64_t)c5;
  uint64_t as_uint64_high11 = as_uint64_high3 << (uint32_t)32U;
  uint64_t as_uint64_low2 = (uint64_t)c4;
  uint64_t b20 = as_uint64_low2 ^ as_uint64_high11;
  uint64_t as_uint64_high4 = (uint64_t)c7;
  uint64_t as_uint64_high12 = as_uint64_high4 << (uint32_t)32U;
  uint64_t as_uint64_low3 = (uint64_t)c6;
  uint64_t b3 = as_uint64_low3 ^ as_uint64_high12;
  t01[0U] = b0;
  t01[1U] = b1;
  t01[2U] = b20;
  t01[3U] = b3;
  uint64_t tempBuffer1[4U] = { 0U };
  uint64_t *r00 = tempBuffer1;
  uint64_t *r10 = tempBuffer1 + (uint32_t)1U;
  uint64_t *r20 = tempBuffer1 + (uint32_t)2U;
  uint64_t *r30 = tempBuffer1 + (uint32_t)3U;
  uint64_t
  cc0 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      t01[0U],
      (uint64_t)0xffffffffffffffffU,
      r00);
  uint64_t
  cc1 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc0, t01[1U], (uint64_t)0xffffffffU, r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc1, t01[2U], (uint64_t)0U, r20);
  uint64_t
  cc3 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc2, t01[3U], (uint64_t)0xffffffff00000001U, r30);
  uint64_t c = cc3;
  cmovznz4(t01, tempBuffer1, t01, c);
  uint64_t b00 = (uint64_t)0U;
  uint64_t as_uint64_high5 = (uint64_t)c11;
  uint64_t as_uint64_high13 = as_uint64_high5 << (uint32_t)32U;
  uint64_t as_uint64_low4 = (uint64_t)(uint32_t)0U;
  uint64_t b10 = as_uint64_low4 ^ as_uint64_high13;
  uint64_t as_uint64_high6 = (uint64_t)c13;
  uint64_t as_uint64_high14 = as_uint64_high6 << (uint32_t)32U;
  uint64_t as_uint64_low5 = (uint64_t)c12;
  uint64_t b21 = as_uint64_low5 ^ as_uint64_high14;
  uint64_t as_uint64_high7 = (uint64_t)c15;
  uint64_t as_uint64_high15 = as_uint64_high7 << (uint32_t)32U;
  uint64_t as_uint64_low6 = (uint64_t)c14;
  uint64_t b30 = as_uint64_low6 ^ as_uint64_high15;
  t110[0U] = b00;
  t110[1U] = b10;
  t110[2U] = b21;
  t110[3U] = b30;
  uint64_t tempBuffer10[4U] = { 0U };
  uint64_t *r01 = tempBuffer10;
  uint64_t *r11 = tempBuffer10 + (uint32_t)1U;
  uint64_t *r21 = tempBuffer10 + (uint32_t)2U;
  uint64_t *r31 = tempBuffer10 + (uint32_t)3U;
  uint64_t
  cc4 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      t110[0U],
      (uint64_t)0xffffffffffffffffU,
      r01);
  uint64_t
  cc10 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc4, t110[1U], (uint64_t)0xffffffffU, r11);
  uint64_t cc20 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc10, t110[2U], (uint64_t)0U, r21);
  uint64_t
  cc30 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64(cc20,
      t110[3U],
      (uint64_t)0xffffffff00000001U,
      r31);
  uint64_t c16 = cc30;
  cmovznz4(t110, tempBuffer10, t110, c16);
  uint64_t b01 = (uint64_t)0U;
  uint64_t as_uint64_high8 = (uint64_t)c12;
  uint64_t as_uint64_high16 = as_uint64_high8 << (uint32_t)32U;
  uint64_t as_uint64_low7 = (uint64_t)(uint32_t)0U;
  uint64_t b11 = as_uint64_low7 ^ as_uint64_high16;
  uint64_t as_uint64_high9 = (uint64_t)c14;
  uint64_t as_uint64_high17 = as_uint64_high9 << (uint32_t)32U;
  uint64_t as_uint64_low8 = (uint64_t)c13;
  uint64_t b22 = as_uint64_low8 ^ as_uint64_high17;
  uint64_t as_uint64_high18 = (uint64_t)(uint32_t)0U;
  uint64_t as_uint64_high19 = as_uint64_high18 << (uint32_t)32U;
  uint64_t as_uint64_low9 = (uint64_t)c15;
  uint64_t b31 = as_uint64_low9 ^ as_uint64_high19;
  t210[0U] = b01;
  t210[1U] = b11;
  t210[2U] = b22;
  t210[3U] = b31;
  uint64_t as_uint64_high20 = (uint64_t)c9;
  uint64_t as_uint64_high110 = as_uint64_high20 << (uint32_t)32U;
  uint64_t as_uint64_low10 = (uint64_t)c8;
  uint64_t b02 = as_uint64_low10 ^ as_uint64_high110;
  uint64_t as_uint64_high21 = (uint64_t)(uint32_t)0U;
  uint64_t as_uint64_high111 = as_uint64_high21 << (uint32_t)32U;
  uint64_t as_uint64_low11 = (uint64_t)c10;
  uint64_t b12 = as_uint64_low11 ^ as_uint64_high111;
  uint64_t b23 = (uint64_t)0U;
  uint64_t as_uint64_high22 = (uint64_t)c15;
  uint64_t as_uint64_high112 = as_uint64_high22 << (uint32_t)32U;
  uint64_t as_uint64_low12 = (uint64_t)c14;
  uint64_t b32 = as_uint64_low12 ^ as_uint64_high112;
  t310[0U] = b02;
  t310[1U] = b12;
  t310[2U] = b23;
  t310[3U] = b32;
  uint64_t tempBuffer11[4U] = { 0U };
  uint64_t *r02 = tempBuffer11;
  uint64_t *r12 = tempBuffer11 + (uint32_t)1U;
  uint64_t *r22 = tempBuffer11 + (uint32_t)2U;
  uint64_t *r32 = tempBuffer11 + (uint32_t)3U;
  uint64_t
  cc5 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      t310[0U],
      (uint64_t)0xffffffffffffffffU,
      r02);
  uint64_t
  cc11 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc5, t310[1U], (uint64_t)0xffffffffU, r12);
  uint64_t cc21 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc11, t310[2U], (uint64_t)0U, r22);
  uint64_t
  cc31 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64(cc21,
      t310[3U],
      (uint64_t)0xffffffff00000001U,
      r32);
  uint64_t c17 = cc31;
  cmovznz4(t310, tempBuffer11, t310, c17);
  uint64_t as_uint64_high23 = (uint64_t)c10;
  uint64_t as_uint64_high113 = as_uint64_high23 << (uint32_t)32U;
  uint64_t as_uint64_low13 = (uint64_t)c9;
  uint64_t b03 = as_uint64_low13 ^ as_uint64_high113;
  uint64_t as_uint64_high24 = (uint64_t)c13;
  uint64_t as_uint64_high114 = as_uint64_high24 << (uint32_t)32U;
  uint64_t as_uint64_low14 = (uint64_t)c11;
  uint64_t b13 = as_uint64_low14 ^ as_uint64_high114;
  uint64_t as_uint64_high25 = (uint64_t)c15;
  uint64_t as_uint64_high115 = as_uint64_high25 << (uint32_t)32U;
  uint64_t as_uint64_low15 = (uint64_t)c14;
  uint64_t b24 = as_uint64_low15 ^ as_uint64_high115;
  uint64_t as_uint64_high26 = (uint64_t)c8;
  uint64_t as_uint64_high116 = as_uint64_high26 << (uint32_t)32U;
  uint64_t as_uint64_low16 = (uint64_t)c13;
  uint64_t b33 = as_uint64_low16 ^ as_uint64_high116;
  t410[0U] = b03;
  t410[1U] = b13;
  t410[2U] = b24;
  t410[3U] = b33;
  uint64_t tempBuffer12[4U] = { 0U };
  uint64_t *r03 = tempBuffer12;
  uint64_t *r13 = tempBuffer12 + (uint32_t)1U;
  uint64_t *r23 = tempBuffer12 + (uint32_t)2U;
  uint64_t *r33 = tempBuffer12 + (uint32_t)3U;
  uint64_t
  cc6 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      t410[0U],
      (uint64_t)0xffffffffffffffffU,
      r03);
  uint64_t
  cc12 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc6, t410[1U], (uint64_t)0xffffffffU, r13);
  uint64_t cc22 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc12, t410[2U], (uint64_t)0U, r23);
  uint64_t
  cc32 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64(cc22,
      t410[3U],
      (uint64_t)0xffffffff00000001U,
      r33);
  uint64_t c18 = cc32;
  cmovznz4(t410, tempBuffer12, t410, c18);
  uint64_t as_uint64_high27 = (uint64_t)c12;
  uint64_t as_uint64_high117 = as_uint64_high27 << (uint32_t)32U;
  uint64_t as_uint64_low17 = (uint64_t)c11;
  uint64_t b04 = as_uint64_low17 ^ as_uint64_high117;
  uint64_t as_uint64_high28 = (uint64_t)(uint32_t)0U;
  uint64_t as_uint64_high118 = as_uint64_high28 << (uint32_t)32U;
  uint64_t as_uint64_low18 = (uint64_t)c13;
  uint64_t b14 = as_uint64_low18 ^ as_uint64_high118;
  uint64_t b25 = (uint64_t)0U;
  uint64_t as_uint64_high29 = (uint64_t)c10;
  uint64_t as_uint64_high119 = as_uint64_high29 << (uint32_t)32U;
  uint64_t as_uint64_low19 = (uint64_t)c8;
  uint64_t b34 = as_uint64_low19 ^ as_uint64_high119;
  t510[0U] = b04;
  t510[1U] = b14;
  t510[2U] = b25;
  t510[3U] = b34;
  uint64_t tempBuffer13[4U] = { 0U };
  uint64_t *r04 = tempBuffer13;
  uint64_t *r14 = tempBuffer13 + (uint32_t)1U;
  uint64_t *r24 = tempBuffer13 + (uint32_t)2U;
  uint64_t *r34 = tempBuffer13 + (uint32_t)3U;
  uint64_t
  cc7 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      t510[0U],
      (uint64_t)0xffffffffffffffffU,
      r04);
  uint64_t
  cc13 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc7, t510[1U], (uint64_t)0xffffffffU, r14);
  uint64_t cc23 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc13, t510[2U], (uint64_t)0U, r24);
  uint64_t
  cc33 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64(cc23,
      t510[3U],
      (uint64_t)0xffffffff00000001U,
      r34);
  uint64_t c19 = cc33;
  cmovznz4(t510, tempBuffer13, t510, c19);
  uint64_t as_uint64_high30 = (uint64_t)c13;
  uint64_t as_uint64_high120 = as_uint64_high30 << (uint32_t)32U;
  uint64_t as_uint64_low20 = (uint64_t)c12;
  uint64_t b05 = as_uint64_low20 ^ as_uint64_high120;
  uint64_t as_uint64_high31 = (uint64_t)c15;
  uint64_t as_uint64_high121 = as_uint64_high31 << (uint32_t)32U;
  uint64_t as_uint64_low21 = (uint64_t)c14;
  uint64_t b15 = as_uint64_low21 ^ as_uint64_high121;
  uint64_t b2 = (uint64_t)0U;
  uint64_t as_uint64_high32 = (uint64_t)c11;
  uint64_t as_uint64_high122 = as_uint64_high32 << (uint32_t)32U;
  uint64_t as_uint64_low22 = (uint64_t)c9;
  uint64_t b35 = as_uint64_low22 ^ as_uint64_high122;
  t610[0U] = b05;
  t610[1U] = b15;
  t610[2U] = b2;
  t610[3U] = b35;
  uint64_t tempBuffer14[4U] = { 0U };
  uint64_t *r05 = tempBuffer14;
  uint64_t *r15 = tempBuffer14 + (uint32_t)1U;
  uint64_t *r25 = tempBuffer14 + (uint32_t)2U;
  uint64_t *r35 = tempBuffer14 + (uint32_t)3U;
  uint64_t
  cc8 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      t610[0U],
      (uint64_t)0xffffffffffffffffU,
      r05);
  uint64_t
  cc14 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc8, t610[1U], (uint64_t)0xffffffffU, r15);
  uint64_t cc24 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc14, t610[2U], (uint64_t)0U, r25);
  uint64_t
  cc34 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64(cc24,
      t610[3U],
      (uint64_t)0xffffffff00000001U,
      r35);
  uint64_t c20 = cc34;
  cmovznz4(t610, tempBuffer14, t610, c20);
  uint64_t as_uint64_high33 = (uint64_t)c14;
  uint64_t as_uint64_high123 = as_uint64_high33 << (uint32_t)32U;
  uint64_t as_uint64_low23 = (uint64_t)c13;
  uint64_t b06 = as_uint64_low23 ^ as_uint64_high123;
  uint64_t as_uint64_high34 = (uint64_t)c8;
  uint64_t as_uint64_high124 = as_uint64_high34 << (uint32_t)32U;
  uint64_t as_uint64_low24 = (uint64_t)c15;
  uint64_t b16 = as_uint64_low24 ^ as_uint64_high124;
  uint64_t as_uint64_high35 = (uint64_t)c10;
  uint64_t as_uint64_high125 = as_uint64_high35 << (uint32_t)32U;
  uint64_t as_uint64_low25 = (uint64_t)c9;
  uint64_t b26 = as_uint64_low25 ^ as_uint64_high125;
  uint64_t as_uint64_high36 = (uint64_t)c12;
  uint64_t as_uint64_high126 = as_uint64_high36 << (uint32_t)32U;
  uint64_t as_uint64_low26 = (uint64_t)(uint32_t)0U;
  uint64_t b36 = as_uint64_low26 ^ as_uint64_high126;
  t710[0U] = b06;
  t710[1U] = b16;
  t710[2U] = b26;
  t710[3U] = b36;
  uint64_t tempBuffer15[4U] = { 0U };
  uint64_t *r06 = tempBuffer15;
  uint64_t *r16 = tempBuffer15 + (uint32_t)1U;
  uint64_t *r26 = tempBuffer15 + (uint32_t)2U;
  uint64_t *r36 = tempBuffer15 + (uint32_t)3U;
  uint64_t
  cc9 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      t710[0U],
      (uint64_t)0xffffffffffffffffU,
      r06);
  uint64_t
  cc15 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc9, t710[1U], (uint64_t)0xffffffffU, r16);
  uint64_t cc25 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc15, t710[2U], (uint64_t)0U, r26);
  uint64_t
  cc35 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64(cc25,
      t710[3U],
      (uint64_t)0xffffffff00000001U,
      r36);
  uint64_t c21 = cc35;
  cmovznz4(t710, tempBuffer15, t710, c21);
  uint64_t as_uint64_high37 = (uint64_t)c15;
  uint64_t as_uint64_high127 = as_uint64_high37 << (uint32_t)32U;
  uint64_t as_uint64_low27 = (uint64_t)c14;
  uint64_t b07 = as_uint64_low27 ^ as_uint64_high127;
  uint64_t as_uint64_high38 = (uint64_t)c9;
  uint64_t as_uint64_high128 = as_uint64_high38 << (uint32_t)32U;
  uint64_t as_uint64_low28 = (uint64_t)(uint32_t)0U;
  uint64_t b17 = as_uint64_low28 ^ as_uint64_high128;
  uint64_t as_uint64_high39 = (uint64_t)c11;
  uint64_t as_uint64_high129 = as_uint64_high39 << (uint32_t)32U;
  uint64_t as_uint64_low29 = (uint64_t)c10;
  uint64_t b27 = as_uint64_low29 ^ as_uint64_high129;
  uint64_t as_uint64_high = (uint64_t)c13;
  uint64_t as_uint64_high130 = as_uint64_high << (uint32_t)32U;
  uint64_t as_uint64_low = (uint64_t)(uint32_t)0U;
  uint64_t b37 = as_uint64_low ^ as_uint64_high130;
  t810[0U] = b07;
  t810[1U] = b17;
  t810[2U] = b27;
  t810[3U] = b37;
  uint64_t tempBuffer16[4U] = { 0U };
  uint64_t *r0 = tempBuffer16;
  uint64_t *r1 = tempBuffer16 + (uint32_t)1U;
  uint64_t *r2 = tempBuffer16 + (uint32_t)2U;
  uint64_t *r3 = tempBuffer16 + (uint32_t)3U;
  uint64_t
  cc =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      t810[0U],
      (uint64_t)0xffffffffffffffffU,
      r0);
  uint64_t
  cc16 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, t810[1U], (uint64_t)0xffffffffU, r1);
  uint64_t cc26 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc16, t810[2U], (uint64_t)0U, r2);
  uint64_t
  cc36 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64(cc26,
      t810[3U],
      (uint64_t)0xffffffff00000001U,
      r3);
  uint64_t c22 = cc36;
  cmovznz4(t810, tempBuffer16, t810, c22);
  uint64_t *t010 = tempBuffer;
  uint64_t *t11 = tempBuffer + (uint32_t)4U;
  uint64_t *t21 = tempBuffer + (uint32_t)8U;
  uint64_t *t31 = tempBuffer + (uint32_t)12U;
  uint64_t *t41 = tempBuffer + (uint32_t)16U;
  uint64_t *t51 = tempBuffer + (uint32_t)20U;
  uint64_t *t61 = tempBuffer + (uint32_t)24U;
  uint64_t *t71 = tempBuffer + (uint32_t)28U;
  uint64_t *t81 = tempBuffer + (uint32_t)32U;
  p256_double(t21, t21);
  p256_double(t11, t11);
  p256_add(t010, t11, o);
  p256_add(t21, o, o);
  p256_add(t31, o, o);
  p256_add(t41, o, o);
  p256_sub(o, t51, o);
  p256_sub(o, t61, o);
  p256_sub(o, t71, o);
  p256_sub(o, t81, o);
}

static void point_double(uint64_t *p, uint64_t *result, uint64_t *tempBuffer)
{
  uint64_t *pY = p + (uint32_t)4U;
  uint64_t *pZ = p + (uint32_t)8U;
  uint64_t *x3 = result;
  uint64_t *y3 = result + (uint32_t)4U;
  uint64_t *z3 = result + (uint32_t)8U;
  uint64_t *delta = tempBuffer;
  uint64_t *gamma = tempBuffer + (uint32_t)4U;
  uint64_t *beta = tempBuffer + (uint32_t)8U;
  uint64_t *alpha = tempBuffer + (uint32_t)16U;
  uint64_t *fourBeta = tempBuffer + (uint32_t)20U;
  uint64_t *eightBeta = tempBuffer + (uint32_t)24U;
  uint64_t *eightGamma = tempBuffer + (uint32_t)28U;
  uint64_t *tmp = tempBuffer + (uint32_t)32U;
  uint64_t *pX1 = p;
  uint64_t *pY1 = p + (uint32_t)4U;
  uint64_t *pZ1 = p + (uint32_t)8U;
  uint64_t *a0 = tmp;
  uint64_t *a1 = tmp + (uint32_t)4U;
  uint64_t *alpha0 = tmp + (uint32_t)8U;
  montgomery_square_buffer(pZ1, delta);
  montgomery_square_buffer(pY1, gamma);
  montgomery_multiplication_buffer(pX1, gamma, beta);
  p256_sub(pX1, delta, a0);
  p256_add(pX1, delta, a1);
  montgomery_multiplication_buffer(a0, a1, alpha0);
  p256_add(alpha0, alpha0, alpha);
  p256_add(alpha0, alpha, alpha);
  montgomery_square_buffer(alpha, x3);
  p256_add(beta, beta, fourBeta);
  p256_add(fourBeta, fourBeta, fourBeta);
  p256_add(fourBeta, fourBeta, eightBeta);
  p256_sub(x3, eightBeta, x3);
  p256_add(pY, pZ, z3);
  montgomery_square_buffer(z3, z3);
  p256_sub(z3, gamma, z3);
  p256_sub(z3, delta, z3);
  p256_sub(fourBeta, x3, y3);
  montgomery_multiplication_buffer(alpha, y3, y3);
  montgomery_square_buffer(gamma, gamma);
  p256_add(gamma, gamma, eightGamma);
  p256_add(eightGamma, eightGamma, eightGamma);
  p256_add(eightGamma, eightGamma, eightGamma);
  p256_sub(y3, eightGamma, y3);
}

static void point_add(uint64_t *p, uint64_t *q, uint64_t *result, uint64_t *tempBuffer)
{
  uint64_t *tempBuffer16 = tempBuffer;
  uint64_t *u1 = tempBuffer + (uint32_t)16U;
  uint64_t *u2 = tempBuffer + (uint32_t)20U;
  uint64_t *s1 = tempBuffer + (uint32_t)24U;
  uint64_t *s2 = tempBuffer + (uint32_t)28U;
  uint64_t *h = tempBuffer + (uint32_t)32U;
  uint64_t *r = tempBuffer + (uint32_t)36U;
  uint64_t *uh = tempBuffer + (uint32_t)40U;
  uint64_t *hCube = tempBuffer + (uint32_t)44U;
  uint64_t *tempBuffer28 = tempBuffer + (uint32_t)60U;
  uint64_t *pX = p;
  uint64_t *pY = p + (uint32_t)4U;
  uint64_t *pZ = p + (uint32_t)8U;
  uint64_t *qX = q;
  uint64_t *qY = q + (uint32_t)4U;
  uint64_t *qZ0 = q + (uint32_t)8U;
  uint64_t *z2Square = tempBuffer16;
  uint64_t *z1Square = tempBuffer16 + (uint32_t)4U;
  uint64_t *z2Cube = tempBuffer16 + (uint32_t)8U;
  uint64_t *z1Cube = tempBuffer16 + (uint32_t)12U;
  montgomery_square_buffer(qZ0, z2Square);
  montgomery_square_buffer(pZ, z1Square);
  montgomery_multiplication_buffer(z2Square, qZ0, z2Cube);
  montgomery_multiplication_buffer(z1Square, pZ, z1Cube);
  montgomery_multiplication_buffer(z2Square, pX, u1);
  montgomery_multiplication_buffer(z1Square, qX, u2);
  montgomery_multiplication_buffer(z2Cube, pY, s1);
  montgomery_multiplication_buffer(z1Cube, qY, s2);
  uint64_t *temp = tempBuffer16;
  p256_sub(u2, u1, h);
  p256_sub(s2, s1, r);
  montgomery_square_buffer(h, temp);
  montgomery_multiplication_buffer(temp, u1, uh);
  montgomery_multiplication_buffer(temp, h, hCube);
  uint64_t *pZ0 = p + (uint32_t)8U;
  uint64_t *qZ = q + (uint32_t)8U;
  uint64_t *tempBuffer161 = tempBuffer28;
  uint64_t *x3_out1 = tempBuffer28 + (uint32_t)16U;
  uint64_t *y3_out1 = tempBuffer28 + (uint32_t)20U;
  uint64_t *z3_out1 = tempBuffer28 + (uint32_t)24U;
  uint64_t *rSquare = tempBuffer161;
  uint64_t *rH = tempBuffer161 + (uint32_t)4U;
  uint64_t *twoUh = tempBuffer161 + (uint32_t)8U;
  montgomery_square_buffer(r, rSquare);
  p256_sub(rSquare, hCube, rH);
  p256_add(uh, uh, twoUh);
  p256_sub(rH, twoUh, x3_out1);
  uint64_t *s1hCube = tempBuffer161;
  uint64_t *u1hx3 = tempBuffer161 + (uint32_t)4U;
  uint64_t *ru1hx3 = tempBuffer161 + (uint32_t)8U;
  montgomery_multiplication_buffer(s1, hCube, s1hCube);
  p256_sub(uh, x3_out1, u1hx3);
  montgomery_multiplication_buffer(u1hx3, r, ru1hx3);
  p256_sub(ru1hx3, s1hCube, y3_out1);
  uint64_t *z1z2 = tempBuffer161;
  montgomery_multiplication_buffer(pZ0, qZ, z1z2);
  montgomery_multiplication_buffer(z1z2, h, z3_out1);
  uint64_t *z = p + (uint32_t)8U;
  uint64_t a00 = z[0U];
  uint64_t a10 = z[1U];
  uint64_t a20 = z[2U];
  uint64_t a30 = z[3U];
  uint64_t r00 = FStar_UInt64_eq_mask(a00, (uint64_t)0U);
  uint64_t r10 = FStar_UInt64_eq_mask(a10, (uint64_t)0U);
  uint64_t r20 = FStar_UInt64_eq_mask(a20, (uint64_t)0U);
  uint64_t r30 = FStar_UInt64_eq_mask(a30, (uint64_t)0U);
  uint64_t r010 = r00 & r10;
  uint64_t r230 = r20 & r30;
  uint64_t mask = r010 & r230;
  uint64_t *p_x0 = q;
  uint64_t *p_y0 = q + (uint32_t)4U;
  uint64_t *p_z0 = q + (uint32_t)8U;
  copy_conditional(x3_out1, p_x0, mask);
  copy_conditional(y3_out1, p_y0, mask);
  copy_conditional(z3_out1, p_z0, mask);
  uint64_t *z0 = q + (uint32_t)8U;
  uint64_t a0 = z0[0U];
  uint64_t a1 = z0[1U];
  uint64_t a2 = z0[2U];
  uint64_t a3 = z0[3U];
  uint64_t r0 = FStar_UInt64_eq_mask(a0, (uint64_t)0U);
  uint64_t r1 = FStar_UInt64_eq_mask(a1, (uint64_t)0U);
  uint64_t r2 = FStar_UInt64_eq_mask(a2, (uint64_t)0U);
  uint64_t r3 = FStar_UInt64_eq_mask(a3, (uint64_t)0U);
  uint64_t r01 = r0 & r1;
  uint64_t r23 = r2 & r3;
  uint64_t mask0 = r01 & r23;
  uint64_t *p_x = p;
  uint64_t *p_y = p + (uint32_t)4U;
  uint64_t *p_z = p + (uint32_t)8U;
  copy_conditional(x3_out1, p_x, mask0);
  copy_conditional(y3_out1, p_y, mask0);
  copy_conditional(z3_out1, p_z, mask0);
  memcpy(result, x3_out1, (uint32_t)4U * sizeof (uint64_t));
  memcpy(result + (uint32_t)4U, y3_out1, (uint32_t)4U * sizeof (uint64_t));
  memcpy(result + (uint32_t)8U, z3_out1, (uint32_t)4U * sizeof (uint64_t));
}

static uint64_t *const_to_buffer__uint64_t(const uint64_t *b)
{
  return (uint64_t *)b;
}

static uint64_t *const_to_lbuffer__uint64_t(const uint64_t *b)
{
  return const_to_buffer__uint64_t(b);
}

static uint64_t *const_to_ibuffer__uint64_t(const uint64_t *b)
{
  return (uint64_t *)b;
}

static uint64_t *const_to_ilbuffer__uint64_t(const uint64_t *b)
{
  return const_to_ibuffer__uint64_t(b);
}

static void
point_add_mixed(
  Lib_Buffer_buftype buftype,
  uint64_t *p,
  void *q,
  uint64_t *result,
  uint64_t *tempBuffer
)
{
  uint64_t *tempBuffer28 = tempBuffer;
  uint64_t *tempBuffer16 = tempBuffer;
  uint64_t *u1 = tempBuffer + (uint32_t)28U;
  uint64_t *u2 = tempBuffer + (uint32_t)32U;
  uint64_t *s1 = tempBuffer + (uint32_t)36U;
  uint64_t *s2 = tempBuffer + (uint32_t)40U;
  uint64_t *h = tempBuffer + (uint32_t)44U;
  uint64_t *r = tempBuffer + (uint32_t)48U;
  uint64_t *uh = tempBuffer + (uint32_t)52U;
  uint64_t *hCube = tempBuffer + (uint32_t)56U;
  uint64_t *pX = p;
  uint64_t *pY = p + (uint32_t)4U;
  uint64_t *pZ = p + (uint32_t)8U;
  void *qX0;
  switch (buftype)
  {
    case Lib_Buffer_MUT:
      {
        qX0 = (void *)(uint64_t *)q;
        break;
      }
    case Lib_Buffer_IMMUT:
      {
        qX0 = (void *)(uint64_t *)q;
        break;
      }
    case Lib_Buffer_CONST:
      {
        qX0 = (void *)(const uint64_t *)q;
        break;
      }
    default:
      {
        KRML_HOST_EPRINTF("KreMLin incomplete match at %s:%d\n", __FILE__, __LINE__);
        KRML_HOST_EXIT(253U);
      }
  }
  void *qY0;
  switch (buftype)
  {
    case Lib_Buffer_MUT:
      {
        qY0 = (void *)((uint64_t *)q + (uint32_t)4U);
        break;
      }
    case Lib_Buffer_IMMUT:
      {
        qY0 = (void *)((uint64_t *)q + (uint32_t)4U);
        break;
      }
    case Lib_Buffer_CONST:
      {
        qY0 = (void *)((const uint64_t *)q + (uint32_t)4U);
        break;
      }
    default:
      {
        KRML_HOST_EPRINTF("KreMLin incomplete match at %s:%d\n", __FILE__, __LINE__);
        KRML_HOST_EXIT(253U);
      }
  }
  uint64_t *z2Square = tempBuffer16;
  uint64_t *z1Square = tempBuffer16 + (uint32_t)4U;
  uint64_t *z2Cube = tempBuffer16 + (uint32_t)8U;
  uint64_t *z1Cube = tempBuffer16 + (uint32_t)12U;
  uint64_t *qX10 = const_to_lbuffer__uint64_t((const uint64_t *)qX0);
  uint64_t *qY10 = const_to_lbuffer__uint64_t((const uint64_t *)qY0);
  z2Square[0U] = (uint64_t)0x000000300000000U;
  z2Square[1U] = (uint64_t)0x00000001FFFFFFFEU;
  z2Square[2U] = (uint64_t)0xFFFFFFFD00000002U;
  z2Square[3U] = (uint64_t)0xFFFFFFFE00000003U;
  z2Cube[0U] = (uint64_t)0x0000000CFFFFFFF7U;
  z2Cube[1U] = (uint64_t)0xFFFFFFF800000007U;
  z2Cube[2U] = (uint64_t)0xFFFFFFFB0000000FU;
  z2Cube[3U] = (uint64_t)0x00000005FFFFFFFFU;
  montgomery_square_buffer(pZ, z1Square);
  montgomery_multiplication_buffer(z1Square, pZ, z1Cube);
  montgomery_multiplication_buffer(z2Square, pX, u1);
  montgomery_multiplication_buffer(z1Square, qX10, u2);
  montgomery_multiplication_buffer(z2Cube, pY, s1);
  montgomery_multiplication_buffer(z1Cube, qY10, s2);
  uint64_t *temp1 = tempBuffer16;
  p256_sub(u2, u1, h);
  p256_sub(s2, s1, r);
  montgomery_square_buffer(h, temp1);
  montgomery_multiplication_buffer(temp1, u1, uh);
  montgomery_multiplication_buffer(temp1, h, hCube);
  uint64_t *pZ0 = p + (uint32_t)8U;
  uint64_t *tempBuffer161 = tempBuffer28;
  uint64_t *pointOut = tempBuffer28 + (uint32_t)16U;
  uint64_t *xOut = tempBuffer28 + (uint32_t)16U;
  uint64_t *yOut = tempBuffer28 + (uint32_t)20U;
  uint64_t *zOut = tempBuffer28 + (uint32_t)24U;
  uint64_t *rSquare = tempBuffer161;
  uint64_t *rH = tempBuffer161 + (uint32_t)4U;
  uint64_t *twoUh = tempBuffer161 + (uint32_t)8U;
  montgomery_square_buffer(r, rSquare);
  p256_sub(rSquare, hCube, rH);
  p256_add(uh, uh, twoUh);
  p256_sub(rH, twoUh, xOut);
  uint64_t *s1hCube = tempBuffer161;
  uint64_t *u1hx3 = tempBuffer161 + (uint32_t)4U;
  uint64_t *ru1hx3 = tempBuffer161 + (uint32_t)8U;
  montgomery_multiplication_buffer(s1, hCube, s1hCube);
  p256_sub(uh, xOut, u1hx3);
  montgomery_multiplication_buffer(u1hx3, r, ru1hx3);
  p256_sub(ru1hx3, s1hCube, yOut);
  uint64_t *z1z2 = tempBuffer161;
  uint64_t t[8U] = { 0U };
  uint64_t *t_low = t;
  uint64_t round[8U] = { 0U };
  uint64_t t1[8U] = { 0U };
  uint64_t t2[8U] = { 0U };
  memcpy(t_low, pZ0, (uint32_t)4U * sizeof (uint64_t));
  uint64_t t11 = t[0U];
  uint64_t temp2 = (uint64_t)0U;
  uint64_t f00 = (uint64_t)0xffffffffffffffffU;
  uint64_t f10 = (uint64_t)0xffffffffU;
  uint64_t f30 = (uint64_t)0xffffffff00000001U;
  uint64_t *o00 = t2;
  uint64_t *o10 = t2 + (uint32_t)1U;
  uint64_t *o20 = t2 + (uint32_t)2U;
  uint64_t *o30 = t2 + (uint32_t)3U;
  mul64(f00, t11, o00, &temp2);
  uint64_t h20 = temp2;
  mul64(f10, t11, o10, &temp2);
  uint64_t l0 = o10[0U];
  uint64_t c10 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l0, h20, o10);
  uint64_t h30 = temp2;
  temp2 = (uint64_t)0U;
  o20[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c10, (uint64_t)0U, h30, o20);
  uint64_t h4 = temp2;
  mul64(f30, t11, o30, &temp2);
  uint64_t l1 = o30[0U];
  uint64_t c3 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l1, h4, o30);
  uint64_t temp00 = temp2;
  t2[4U] = c3 + temp00;
  uint64_t *a00 = t;
  uint64_t *a10 = t + (uint32_t)4U;
  uint64_t *b00 = t2;
  uint64_t *b10 = t2 + (uint32_t)4U;
  uint64_t *c00 = t1;
  uint64_t *c11 = t1 + (uint32_t)4U;
  uint64_t *r00 = c00;
  uint64_t *r10 = c00 + (uint32_t)1U;
  uint64_t *r20 = c00 + (uint32_t)2U;
  uint64_t *r30 = c00 + (uint32_t)3U;
  uint64_t cc00 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a00[0U], b00[0U], r00);
  uint64_t cc1 = Lib_IntTypes_Intrinsics_add_carry_u64(cc00, a00[1U], b00[1U], r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_add_carry_u64(cc1, a00[2U], b00[2U], r20);
  uint64_t cc3 = Lib_IntTypes_Intrinsics_add_carry_u64(cc2, a00[3U], b00[3U], r30);
  uint64_t carry0 = cc3;
  uint64_t *r02 = c11;
  uint64_t *r11 = c11 + (uint32_t)1U;
  uint64_t *r21 = c11 + (uint32_t)2U;
  uint64_t *r31 = c11 + (uint32_t)3U;
  uint64_t cc01 = Lib_IntTypes_Intrinsics_add_carry_u64(carry0, a10[0U], b10[0U], r02);
  uint64_t cc10 = Lib_IntTypes_Intrinsics_add_carry_u64(cc01, a10[1U], b10[1U], r11);
  uint64_t cc20 = Lib_IntTypes_Intrinsics_add_carry_u64(cc10, a10[2U], b10[2U], r21);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc20, a10[3U], b10[3U], r31);
  uint64_t t12 = t1[1U];
  uint64_t t210 = t1[2U];
  uint64_t t30 = t1[3U];
  uint64_t t40 = t1[4U];
  uint64_t t50 = t1[5U];
  uint64_t t60 = t1[6U];
  uint64_t t70 = t1[7U];
  t1[0U] = t12;
  t1[1U] = t210;
  t1[2U] = t30;
  t1[3U] = t40;
  t1[4U] = t50;
  t1[5U] = t60;
  t1[6U] = t70;
  t1[7U] = (uint64_t)0U;
  uint64_t t110 = t1[0U];
  uint64_t temp3 = (uint64_t)0U;
  uint64_t f01 = (uint64_t)0xffffffffffffffffU;
  uint64_t f11 = (uint64_t)0xffffffffU;
  uint64_t f31 = (uint64_t)0xffffffff00000001U;
  uint64_t *o01 = t2;
  uint64_t *o11 = t2 + (uint32_t)1U;
  uint64_t *o21 = t2 + (uint32_t)2U;
  uint64_t *o31 = t2 + (uint32_t)3U;
  mul64(f01, t110, o01, &temp3);
  uint64_t h2 = temp3;
  mul64(f11, t110, o11, &temp3);
  uint64_t l2 = o11[0U];
  uint64_t c12 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l2, h2, o11);
  uint64_t h31 = temp3;
  temp3 = (uint64_t)0U;
  o21[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c12, (uint64_t)0U, h31, o21);
  uint64_t h40 = temp3;
  mul64(f31, t110, o31, &temp3);
  uint64_t l3 = o31[0U];
  uint64_t c30 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l3, h40, o31);
  uint64_t temp01 = temp3;
  t2[4U] = c30 + temp01;
  uint64_t *a01 = t1;
  uint64_t *a11 = t1 + (uint32_t)4U;
  uint64_t *b01 = t2;
  uint64_t *b11 = t2 + (uint32_t)4U;
  uint64_t *c01 = round;
  uint64_t *c13 = round + (uint32_t)4U;
  uint64_t *r03 = c01;
  uint64_t *r12 = c01 + (uint32_t)1U;
  uint64_t *r22 = c01 + (uint32_t)2U;
  uint64_t *r32 = c01 + (uint32_t)3U;
  uint64_t cc02 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a01[0U], b01[0U], r03);
  uint64_t cc11 = Lib_IntTypes_Intrinsics_add_carry_u64(cc02, a01[1U], b01[1U], r12);
  uint64_t cc21 = Lib_IntTypes_Intrinsics_add_carry_u64(cc11, a01[2U], b01[2U], r22);
  uint64_t cc30 = Lib_IntTypes_Intrinsics_add_carry_u64(cc21, a01[3U], b01[3U], r32);
  uint64_t carry00 = cc30;
  uint64_t *r04 = c13;
  uint64_t *r13 = c13 + (uint32_t)1U;
  uint64_t *r24 = c13 + (uint32_t)2U;
  uint64_t *r33 = c13 + (uint32_t)3U;
  uint64_t cc03 = Lib_IntTypes_Intrinsics_add_carry_u64(carry00, a11[0U], b11[0U], r04);
  uint64_t cc12 = Lib_IntTypes_Intrinsics_add_carry_u64(cc03, a11[1U], b11[1U], r13);
  uint64_t cc22 = Lib_IntTypes_Intrinsics_add_carry_u64(cc12, a11[2U], b11[2U], r24);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc22, a11[3U], b11[3U], r33);
  uint64_t t120 = round[1U];
  uint64_t t211 = round[2U];
  uint64_t t31 = round[3U];
  uint64_t t41 = round[4U];
  uint64_t t51 = round[5U];
  uint64_t t61 = round[6U];
  uint64_t t71 = round[7U];
  round[0U] = t120;
  round[1U] = t211;
  round[2U] = t31;
  round[3U] = t41;
  round[4U] = t51;
  round[5U] = t61;
  round[6U] = t71;
  round[7U] = (uint64_t)0U;
  uint64_t t111 = round[0U];
  uint64_t temp4 = (uint64_t)0U;
  uint64_t f02 = (uint64_t)0xffffffffffffffffU;
  uint64_t f12 = (uint64_t)0xffffffffU;
  uint64_t f32 = (uint64_t)0xffffffff00000001U;
  uint64_t *o02 = t2;
  uint64_t *o12 = t2 + (uint32_t)1U;
  uint64_t *o22 = t2 + (uint32_t)2U;
  uint64_t *o32 = t2 + (uint32_t)3U;
  mul64(f02, t111, o02, &temp4);
  uint64_t h32 = temp4;
  mul64(f12, t111, o12, &temp4);
  uint64_t l4 = o12[0U];
  uint64_t c14 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l4, h32, o12);
  uint64_t h33 = temp4;
  temp4 = (uint64_t)0U;
  o22[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c14, (uint64_t)0U, h33, o22);
  uint64_t h41 = temp4;
  mul64(f32, t111, o32, &temp4);
  uint64_t l5 = o32[0U];
  uint64_t c31 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l5, h41, o32);
  uint64_t temp02 = temp4;
  t2[4U] = c31 + temp02;
  uint64_t *a02 = round;
  uint64_t *a12 = round + (uint32_t)4U;
  uint64_t *b02 = t2;
  uint64_t *b12 = t2 + (uint32_t)4U;
  uint64_t *c02 = t1;
  uint64_t *c15 = t1 + (uint32_t)4U;
  uint64_t *r05 = c02;
  uint64_t *r14 = c02 + (uint32_t)1U;
  uint64_t *r25 = c02 + (uint32_t)2U;
  uint64_t *r34 = c02 + (uint32_t)3U;
  uint64_t cc04 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a02[0U], b02[0U], r05);
  uint64_t cc13 = Lib_IntTypes_Intrinsics_add_carry_u64(cc04, a02[1U], b02[1U], r14);
  uint64_t cc23 = Lib_IntTypes_Intrinsics_add_carry_u64(cc13, a02[2U], b02[2U], r25);
  uint64_t cc31 = Lib_IntTypes_Intrinsics_add_carry_u64(cc23, a02[3U], b02[3U], r34);
  uint64_t carry01 = cc31;
  uint64_t *r06 = c15;
  uint64_t *r15 = c15 + (uint32_t)1U;
  uint64_t *r26 = c15 + (uint32_t)2U;
  uint64_t *r35 = c15 + (uint32_t)3U;
  uint64_t cc05 = Lib_IntTypes_Intrinsics_add_carry_u64(carry01, a12[0U], b12[0U], r06);
  uint64_t cc14 = Lib_IntTypes_Intrinsics_add_carry_u64(cc05, a12[1U], b12[1U], r15);
  uint64_t cc24 = Lib_IntTypes_Intrinsics_add_carry_u64(cc14, a12[2U], b12[2U], r26);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc24, a12[3U], b12[3U], r35);
  uint64_t t121 = t1[1U];
  uint64_t t212 = t1[2U];
  uint64_t t32 = t1[3U];
  uint64_t t42 = t1[4U];
  uint64_t t52 = t1[5U];
  uint64_t t62 = t1[6U];
  uint64_t t72 = t1[7U];
  t1[0U] = t121;
  t1[1U] = t212;
  t1[2U] = t32;
  t1[3U] = t42;
  t1[4U] = t52;
  t1[5U] = t62;
  t1[6U] = t72;
  t1[7U] = (uint64_t)0U;
  uint64_t t112 = t1[0U];
  uint64_t temp = (uint64_t)0U;
  uint64_t f0 = (uint64_t)0xffffffffffffffffU;
  uint64_t f1 = (uint64_t)0xffffffffU;
  uint64_t f3 = (uint64_t)0xffffffff00000001U;
  uint64_t *o0 = t2;
  uint64_t *o1 = t2 + (uint32_t)1U;
  uint64_t *o2 = t2 + (uint32_t)2U;
  uint64_t *o3 = t2 + (uint32_t)3U;
  mul64(f0, t112, o0, &temp);
  uint64_t h34 = temp;
  mul64(f1, t112, o1, &temp);
  uint64_t l6 = o1[0U];
  uint64_t c16 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l6, h34, o1);
  uint64_t h3 = temp;
  temp = (uint64_t)0U;
  o2[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c16, (uint64_t)0U, h3, o2);
  uint64_t h42 = temp;
  mul64(f3, t112, o3, &temp);
  uint64_t l = o3[0U];
  uint64_t c32 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l, h42, o3);
  uint64_t temp0 = temp;
  t2[4U] = c32 + temp0;
  uint64_t *a03 = t1;
  uint64_t *a13 = t1 + (uint32_t)4U;
  uint64_t *b0 = t2;
  uint64_t *b1 = t2 + (uint32_t)4U;
  uint64_t *c0 = round;
  uint64_t *c1 = round + (uint32_t)4U;
  uint64_t *r07 = c0;
  uint64_t *r16 = c0 + (uint32_t)1U;
  uint64_t *r27 = c0 + (uint32_t)2U;
  uint64_t *r36 = c0 + (uint32_t)3U;
  uint64_t cc06 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a03[0U], b0[0U], r07);
  uint64_t cc15 = Lib_IntTypes_Intrinsics_add_carry_u64(cc06, a03[1U], b0[1U], r16);
  uint64_t cc25 = Lib_IntTypes_Intrinsics_add_carry_u64(cc15, a03[2U], b0[2U], r27);
  uint64_t cc32 = Lib_IntTypes_Intrinsics_add_carry_u64(cc25, a03[3U], b0[3U], r36);
  uint64_t carry02 = cc32;
  uint64_t *r08 = c1;
  uint64_t *r17 = c1 + (uint32_t)1U;
  uint64_t *r28 = c1 + (uint32_t)2U;
  uint64_t *r37 = c1 + (uint32_t)3U;
  uint64_t cc0 = Lib_IntTypes_Intrinsics_add_carry_u64(carry02, a13[0U], b1[0U], r08);
  uint64_t cc16 = Lib_IntTypes_Intrinsics_add_carry_u64(cc0, a13[1U], b1[1U], r17);
  uint64_t cc26 = Lib_IntTypes_Intrinsics_add_carry_u64(cc16, a13[2U], b1[2U], r28);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc26, a13[3U], b1[3U], r37);
  uint64_t t122 = round[1U];
  uint64_t t21 = round[2U];
  uint64_t t3 = round[3U];
  uint64_t t4 = round[4U];
  uint64_t t5 = round[5U];
  uint64_t t6 = round[6U];
  uint64_t t7 = round[7U];
  round[0U] = t122;
  round[1U] = t21;
  round[2U] = t3;
  round[3U] = t4;
  round[4U] = t5;
  round[5U] = t6;
  round[6U] = t7;
  round[7U] = (uint64_t)0U;
  uint64_t tempBuffer1[4U] = { 0U };
  uint64_t tempBufferForSubborrow = (uint64_t)0U;
  uint64_t cin = round[4U];
  uint64_t *x_ = round;
  uint64_t *r09 = tempBuffer1;
  uint64_t *r18 = tempBuffer1 + (uint32_t)1U;
  uint64_t *r29 = tempBuffer1 + (uint32_t)2U;
  uint64_t *r38 = tempBuffer1 + (uint32_t)3U;
  uint64_t
  cc =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      x_[0U],
      (uint64_t)0xffffffffffffffffU,
      r09);
  uint64_t cc17 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, x_[1U], (uint64_t)0xffffffffU, r18);
  uint64_t cc27 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc17, x_[2U], (uint64_t)0U, r29);
  uint64_t
  cc33 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc27, x_[3U], (uint64_t)0xffffffff00000001U, r38);
  uint64_t c = cc33;
  uint64_t
  carry = Lib_IntTypes_Intrinsics_sub_borrow_u64(c, cin, (uint64_t)0U, &tempBufferForSubborrow);
  cmovznz4(z1z2, tempBuffer1, x_, carry);
  montgomery_multiplication_buffer(z1z2, h, zOut);
  uint64_t *pZ1 = p + (uint32_t)8U;
  uint64_t a04 = pZ1[0U];
  uint64_t a14 = pZ1[1U];
  uint64_t a20 = pZ1[2U];
  uint64_t a30 = pZ1[3U];
  uint64_t r010 = FStar_UInt64_eq_mask(a04, (uint64_t)0U);
  uint64_t r19 = FStar_UInt64_eq_mask(a14, (uint64_t)0U);
  uint64_t r210 = FStar_UInt64_eq_mask(a20, (uint64_t)0U);
  uint64_t r39 = FStar_UInt64_eq_mask(a30, (uint64_t)0U);
  uint64_t r011 = r010 & r19;
  uint64_t r230 = r210 & r39;
  uint64_t mask = r011 & r230;
  uint64_t *xOut1 = pointOut;
  uint64_t *yOut1 = pointOut + (uint32_t)4U;
  uint64_t *zOut1 = pointOut + (uint32_t)8U;
  void *qX;
  switch (buftype)
  {
    case Lib_Buffer_MUT:
      {
        qX = (void *)(uint64_t *)q;
        break;
      }
    case Lib_Buffer_IMMUT:
      {
        qX = (void *)(uint64_t *)q;
        break;
      }
    case Lib_Buffer_CONST:
      {
        qX = (void *)(const uint64_t *)q;
        break;
      }
    default:
      {
        KRML_HOST_EPRINTF("KreMLin incomplete match at %s:%d\n", __FILE__, __LINE__);
        KRML_HOST_EXIT(253U);
      }
  }
  void *qY;
  switch (buftype)
  {
    case Lib_Buffer_MUT:
      {
        qY = (void *)((uint64_t *)q + (uint32_t)4U);
        break;
      }
    case Lib_Buffer_IMMUT:
      {
        qY = (void *)((uint64_t *)q + (uint32_t)4U);
        break;
      }
    case Lib_Buffer_CONST:
      {
        qY = (void *)((const uint64_t *)q + (uint32_t)4U);
        break;
      }
    default:
      {
        KRML_HOST_EPRINTF("KreMLin incomplete match at %s:%d\n", __FILE__, __LINE__);
        KRML_HOST_EXIT(253U);
      }
  }
  uint64_t *qX1 = const_to_lbuffer__uint64_t((const uint64_t *)qX);
  uint64_t *qY1 = const_to_lbuffer__uint64_t((const uint64_t *)qY);
  copy_conditional(xOut1, qX1, mask);
  copy_conditional(yOut1, qY1, mask);
  uint64_t out_0 = zOut1[0U];
  uint64_t out_1 = zOut1[1U];
  uint64_t out_2 = zOut1[2U];
  uint64_t out_3 = zOut1[3U];
  uint64_t x_0 = (uint64_t)1U;
  uint64_t x_1 = (uint64_t)0U;
  uint64_t x_2 = (uint64_t)0U;
  uint64_t x_3 = (uint64_t)0U;
  uint64_t r_0 = out_0 ^ (mask & (out_0 ^ x_0));
  uint64_t r_1 = out_1 ^ (mask & (out_1 ^ x_1));
  uint64_t r_2 = out_2 ^ (mask & (out_2 ^ x_2));
  uint64_t r_3 = out_3 ^ (mask & (out_3 ^ x_3));
  zOut1[0U] = r_0;
  zOut1[1U] = r_1;
  zOut1[2U] = r_2;
  zOut1[3U] = r_3;
  void *x;
  switch (buftype)
  {
    case Lib_Buffer_MUT:
      {
        x = (void *)(uint64_t *)q;
        break;
      }
    case Lib_Buffer_IMMUT:
      {
        x = (void *)(uint64_t *)q;
        break;
      }
    case Lib_Buffer_CONST:
      {
        x = (void *)(const uint64_t *)q;
        break;
      }
    default:
      {
        KRML_HOST_EPRINTF("KreMLin incomplete match at %s:%d\n", __FILE__, __LINE__);
        KRML_HOST_EXIT(253U);
      }
  }
  void *y;
  switch (buftype)
  {
    case Lib_Buffer_MUT:
      {
        y = (void *)((uint64_t *)q + (uint32_t)4U);
        break;
      }
    case Lib_Buffer_IMMUT:
      {
        y = (void *)((uint64_t *)q + (uint32_t)4U);
        break;
      }
    case Lib_Buffer_CONST:
      {
        y = (void *)((const uint64_t *)q + (uint32_t)4U);
        break;
      }
    default:
      {
        KRML_HOST_EPRINTF("KreMLin incomplete match at %s:%d\n", __FILE__, __LINE__);
        KRML_HOST_EXIT(253U);
      }
  }
  uint64_t *uu____0 = const_to_ilbuffer__uint64_t((const uint64_t *)x);
  uint64_t a05 = uu____0[0U];
  uint64_t a15 = uu____0[1U];
  uint64_t a21 = uu____0[2U];
  uint64_t a31 = uu____0[3U];
  uint64_t r012 = FStar_UInt64_eq_mask(a05, (uint64_t)0U);
  uint64_t r110 = FStar_UInt64_eq_mask(a15, (uint64_t)0U);
  uint64_t r211 = FStar_UInt64_eq_mask(a21, (uint64_t)0U);
  uint64_t r310 = FStar_UInt64_eq_mask(a31, (uint64_t)0U);
  uint64_t r013 = r012 & r110;
  uint64_t r231 = r211 & r310;
  uint64_t xZero = r013 & r231;
  uint64_t *uu____1 = const_to_ilbuffer__uint64_t((const uint64_t *)y);
  uint64_t a0 = uu____1[0U];
  uint64_t a1 = uu____1[1U];
  uint64_t a2 = uu____1[2U];
  uint64_t a3 = uu____1[3U];
  uint64_t r0 = FStar_UInt64_eq_mask(a0, (uint64_t)0U);
  uint64_t r1 = FStar_UInt64_eq_mask(a1, (uint64_t)0U);
  uint64_t r2 = FStar_UInt64_eq_mask(a2, (uint64_t)0U);
  uint64_t r3 = FStar_UInt64_eq_mask(a3, (uint64_t)0U);
  uint64_t r01 = r0 & r1;
  uint64_t r23 = r2 & r3;
  uint64_t yZero = r01 & r23;
  uint64_t mask0 = xZero & yZero;
  uint64_t *p_x = p;
  uint64_t *p_y = p + (uint32_t)4U;
  uint64_t *p_z = p + (uint32_t)8U;
  uint64_t *x_x = pointOut;
  uint64_t *x_y = pointOut + (uint32_t)4U;
  uint64_t *x_z = pointOut + (uint32_t)8U;
  uint64_t *result_x = result;
  uint64_t *result_y = result + (uint32_t)4U;
  uint64_t *result_z = result + (uint32_t)8U;
  cmovznz4(result_x, x_x, p_x, mask0);
  cmovznz4(result_y, x_y, p_y, mask0);
  cmovznz4(result_z, x_z, p_z, mask0);
}

static uint64_t scalar_bit(uint8_t *s, uint32_t n)
{
  return (uint64_t)(s[(uint32_t)31U - n / (uint32_t)8U] >> n % (uint32_t)8U & (uint8_t)1U);
}

static void scalar_rwnaf(uint64_t *out, uint8_t *scalar)
{
  uint8_t in0 = scalar[31U];
  uint64_t windowStartValue = (uint64_t)1U | ((uint64_t)in0 & (uint64_t)63U);
  uint64_t window = windowStartValue;
  uint64_t r = (uint64_t)0U;
  uint64_t r1 = (uint64_t)0U;
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)50U; i++)
  {
    uint64_t wVar = window;
    uint64_t w = wVar & (uint64_t)63U;
    uint64_t d = (wVar & (uint64_t)63U) - (uint64_t)32U;
    uint64_t c = Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U, w, (uint64_t)32U, &r);
    Lib_IntTypes_Intrinsics_sub_borrow_u64_void((uint64_t)0U, (uint64_t)0U, r, &r1);
    uint64_t cAsFlag = (uint64_t)0xffffffffU + c;
    uint64_t r3 = ((r & cAsFlag) | (r1 & ~cAsFlag)) & (uint64_t)0xffU;
    out[(uint32_t)2U * i] = r3;
    out[(uint32_t)2U * i + (uint32_t)(krml_checked_int_t)1] = c;
    uint64_t wStart = (wVar - d) >> (uint32_t)(uint64_t)5U;
    uint64_t
    w0 =
      wStart
      +
        (scalar_bit(scalar,
          ((uint32_t)1U + i) * (uint32_t)(uint64_t)5U + (uint32_t)1U)
        << (uint32_t)1U);
    uint64_t
    w01 =
      w0
      +
        (scalar_bit(scalar,
          ((uint32_t)1U + i) * (uint32_t)(uint64_t)5U + (uint32_t)2U)
        << (uint32_t)2U);
    uint64_t
    w02 =
      w01
      +
        (scalar_bit(scalar,
          ((uint32_t)1U + i) * (uint32_t)(uint64_t)5U + (uint32_t)3U)
        << (uint32_t)3U);
    uint64_t
    w03 =
      w02
      +
        (scalar_bit(scalar,
          ((uint32_t)1U + i) * (uint32_t)(uint64_t)5U + (uint32_t)4U)
        << (uint32_t)4U);
    uint64_t
    w04 =
      w03
      +
        (scalar_bit(scalar,
          ((uint32_t)1U + i) * (uint32_t)(uint64_t)5U + (uint32_t)5U)
        << (uint32_t)5U);
    window = w04;
  }
  uint32_t i = (uint32_t)50U;
  uint64_t wVar = window;
  uint64_t w = wVar & (uint64_t)63U;
  uint64_t d = (wVar & (uint64_t)63U) - (uint64_t)32U;
  uint64_t c = Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U, w, (uint64_t)32U, &r);
  Lib_IntTypes_Intrinsics_sub_borrow_u64_void((uint64_t)0U, (uint64_t)0U, r, &r1);
  uint64_t cAsFlag = (uint64_t)0xffffffffU + c;
  uint64_t r3 = ((r & cAsFlag) | (r1 & ~cAsFlag)) & (uint64_t)0xffU;
  out[(uint32_t)2U * i] = r3;
  out[(uint32_t)2U * i + (uint32_t)(krml_checked_int_t)1] = c;
  uint64_t wStart = (wVar - d) >> (uint32_t)(uint64_t)5U;
  out[102U] = wStart;
}

static void loopK(uint64_t d, uint64_t *point, uint32_t j)
{
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)16U; i++)
  {
    uint64_t mask = FStar_UInt64_eq_mask(d, (uint64_t)i);
    uint64_t
    *lut_cmb_x =
      Hacl_Impl_ScalarMultiplication_RWNAF_Table_Ext_getUInt64((j * (uint32_t)16U + i)
        * (uint32_t)(krml_checked_int_t)8);
    uint64_t
    *lut_cmb_y =
      Hacl_Impl_ScalarMultiplication_RWNAF_Table_Ext_getUInt64((j * (uint32_t)16U + i)
        * (uint32_t)(krml_checked_int_t)8
        + (uint32_t)4U);
    copy_conditional(point, lut_cmb_x, mask);
    copy_conditional(point + (uint32_t)4U, lut_cmb_y, mask);
  }
}

static void
conditional_substraction(uint64_t *result, uint64_t *p, uint8_t *scalar, uint64_t *tempBuffer)
{
  uint64_t tempPoint[12U] = { 0U };
  uint64_t bpMinus[8U] = { 0U };
  uint64_t *bpMinusX = bpMinus;
  uint64_t *bpMinusY = bpMinus + (uint32_t)4U;
  uint8_t i0 = scalar[31U];
  uint64_t mask = ~((uint64_t)0U - (uint64_t)(i0 & (uint8_t)1U));
  uint64_t *bpX = Hacl_Impl_ScalarMultiplication_RWNAF_Table_Ext_getUInt64((uint32_t)0U);
  uint64_t *bpY = Hacl_Impl_ScalarMultiplication_RWNAF_Table_Ext_getUInt64((uint32_t)4U);
  memcpy(bpMinusX, bpX, (uint32_t)4U * sizeof (uint64_t));
  p256_neg(bpY, bpMinusY);
  point_add_mixed(Lib_Buffer_MUT, p, (void *)bpMinus, tempPoint, tempBuffer);
  uint64_t *x_x = tempPoint;
  uint64_t *x_y = tempPoint + (uint32_t)4U;
  uint64_t *x_z = tempPoint + (uint32_t)8U;
  uint64_t *result_x = result;
  uint64_t *result_y = result + (uint32_t)4U;
  uint64_t *result_z = result + (uint32_t)8U;
  copy_conditional(result_x, x_x, mask);
  copy_conditional(result_y, x_y, mask);
  copy_conditional(result_z, x_z, mask);
}

static void scalar_multiplication_cmb(uint64_t *result, void *scalar, uint64_t *tempBuffer)
{
  uint64_t rnaf2[104U] = { 0U };
  uint64_t lut[8U] = { 0U };
  uint64_t *temp4 = tempBuffer;
  scalar_rwnaf(rnaf2, (uint8_t *)scalar);
  uint32_t i0 = (uint32_t)1U;
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)26U; i++)
  {
    uint64_t d = rnaf2[(uint32_t)2U * (i * (uint32_t)2U + i0)];
    uint64_t is_neg = (uint64_t)0U - rnaf2[(uint32_t)2U * (i * (uint32_t)2U + i0) + (uint32_t)1U];
    uint64_t d1 = (d - (uint64_t)(uint32_t)1U) >> (uint32_t)1U;
    loopK(d1, lut, i);
    uint64_t *yLut = lut + (uint32_t)4U;
    p256_neg(yLut, temp4);
    copy_conditional(yLut, temp4, is_neg);
    point_add_mixed(Lib_Buffer_MUT, result, (void *)lut, result, tempBuffer);
  }
  uint32_t i1 = (uint32_t)0U;
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)(uint64_t)5U; i++)
  {
    point_double(result, result, tempBuffer);
  }
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)26U; i++)
  {
    uint64_t d = rnaf2[(uint32_t)2U * (i * (uint32_t)2U + i1)];
    uint64_t is_neg = (uint64_t)0U - rnaf2[(uint32_t)2U * (i * (uint32_t)2U + i1) + (uint32_t)1U];
    uint64_t d1 = (d - (uint64_t)(uint32_t)1U) >> (uint32_t)1U;
    loopK(d1, lut, i);
    uint64_t *yLut = lut + (uint32_t)4U;
    p256_neg(yLut, temp4);
    copy_conditional(yLut, temp4, is_neg);
    point_add_mixed(Lib_Buffer_MUT, result, (void *)lut, result, tempBuffer);
  }
  conditional_substraction(result, result, (uint8_t *)scalar, tempBuffer);
}

static const
uint64_t
points_radix_16[128U] =
  {
    (uint64_t)0x0U, (uint64_t)0x0U, (uint64_t)0x0U, (uint64_t)0x0U, (uint64_t)0x0U, (uint64_t)0x0U,
    (uint64_t)0x0U, (uint64_t)0x0U, (uint64_t)0x1fb38ab1388ad777U, (uint64_t)0x1dfee06615fa309dU,
    (uint64_t)0xfcac986c3afea4a7U, (uint64_t)0xdf65c2da29fb821aU, (uint64_t)0xeff44e23f63f8f6dU,
    (uint64_t)0xaa02cd3ed4b681a4U, (uint64_t)0xdd5fda3363818af8U, (uint64_t)0xfc53bc2629fbf0b3U,
    (uint64_t)0x12631d721b91beeaU, (uint64_t)0x5f73f2d3a11a09f8U, (uint64_t)0xac41f54484d5fcd8U,
    (uint64_t)0x86578e5c56025df4U, (uint64_t)0x577c956b15ed6b5aU, (uint64_t)0xb59c5f77982d848U,
    (uint64_t)0xb7c5e2c190fcdcc2U, (uint64_t)0x7d64d13ef1c91ffdU, (uint64_t)0xd40c2d6273f9d9f1U,
    (uint64_t)0x4dc6f628063ef17cU, (uint64_t)0x498e81df7ab17aa5U, (uint64_t)0xabb2a5026f17173cU,
    (uint64_t)0x4a3d7527f6739ef3U, (uint64_t)0xd941003268184c91U, (uint64_t)0xd2d458b8d401508bU,
    (uint64_t)0xb7437ab810ac5451U, (uint64_t)0x5256d9bdab491252U, (uint64_t)0x972d326eb1084c12U,
    (uint64_t)0xc3e96455e2ec3bfaU, (uint64_t)0xb75c723b549a10ffU, (uint64_t)0x9d9185f9f8a18961U,
    (uint64_t)0x2200a07b8589ba82U, (uint64_t)0x637b9d96fd4e9f5eU, (uint64_t)0xce75bfb2575e6cfaU,
    (uint64_t)0x7dd4477db8b77c7dU, (uint64_t)0x80818a776e5503b0U, (uint64_t)0x6fc7d58fb59581dU,
    (uint64_t)0xd899fb87efe43022U, (uint64_t)0x23b9912111694135U, (uint64_t)0x7e5de7bac33fa1c8U,
    (uint64_t)0xb3b83722a70e7d43U, (uint64_t)0xf06cfecbfb9bb38fU, (uint64_t)0xaa39277dfa93656U,
    (uint64_t)0x3dabb6cce67c5201U, (uint64_t)0x473ffb8bf1f94677U, (uint64_t)0xb9f0b93637453e56U,
    (uint64_t)0x8fce12ec20958fb2U, (uint64_t)0xcc16d74ff7786061U, (uint64_t)0x3678438a8235d096U,
    (uint64_t)0xe39ea044f06b43f6U, (uint64_t)0xbb40bdb5775c9950U, (uint64_t)0xd244a74cdc703cddU,
    (uint64_t)0x83dc1b8a6105dd53U, (uint64_t)0x38d9d50d49ef0437U, (uint64_t)0x58be44eba6096472U,
    (uint64_t)0x960afaec386fa5c5U, (uint64_t)0x1440032e000134b9U, (uint64_t)0x601e721454d6ba96U,
    (uint64_t)0x79ec42228671b9b6U, (uint64_t)0xfdc00dc48df9e25cU, (uint64_t)0x44500833d71d2e77U,
    (uint64_t)0x2bda4c3c0bc103d5U, (uint64_t)0x51528408aa925d53U, (uint64_t)0xefcb55b9c2f3a37dU,
    (uint64_t)0x9f28f6bb9846c915U, (uint64_t)0xe1547ce1d8340e55U, (uint64_t)0x97e310c1995b3ed2U,
    (uint64_t)0xed861937196256e6U, (uint64_t)0x1c6762abff2c65f2U, (uint64_t)0x268345e0978fceddU,
    (uint64_t)0x35ca2e572b784881U, (uint64_t)0x28ac888da0acd1b7U, (uint64_t)0x305640dc06a41bafU,
    (uint64_t)0x997c6fd2cb671bfbU, (uint64_t)0xf40d9eaf4a31e15aU, (uint64_t)0x8991dd7d54cfe03aU,
    (uint64_t)0x4889a3463a8deb0cU, (uint64_t)0x4cbf48092cd0a1faU, (uint64_t)0xc6965c4fbe18fb8cU,
    (uint64_t)0x1d499d0cb216fa84U, (uint64_t)0x8d5fe52c705dd3ebU, (uint64_t)0x812b268f84313b34U,
    (uint64_t)0x313b58808261591aU, (uint64_t)0xc2c322508f53d933U, (uint64_t)0xa49ef3f95094ed1bU,
    (uint64_t)0x13e326786e98c63U, (uint64_t)0x34be8167cd460429U, (uint64_t)0x698a328099a6b31U,
    (uint64_t)0xb9be3ba51b0c922dU, (uint64_t)0xe59cca03f7674edU, (uint64_t)0x4fbf7e505d3aca7cU,
    (uint64_t)0x2f4f8ba62020715U, (uint64_t)0x840502262ac1ec42U, (uint64_t)0xb8e0532775197de7U,
    (uint64_t)0x9142a358cf4e9b4bU, (uint64_t)0xc86a3c567e5d8626U, (uint64_t)0xd4051282b4a7992aU,
    (uint64_t)0xe7573c5999e3974eU, (uint64_t)0xd814a606da7bd76bU, (uint64_t)0x15604730f38cb788U,
    (uint64_t)0xbd195f868fbdd6c4U, (uint64_t)0xdb96f5b00a51d3f7U, (uint64_t)0xe1385c8a9b507feaU,
    (uint64_t)0x878e27813ee7310U, (uint64_t)0x6d7d8b12aea7e096U, (uint64_t)0x54978ad11e2f5ccaU,
    (uint64_t)0x49fffd6c3c4d07d4U, (uint64_t)0x703638f71fab7a5dU, (uint64_t)0xbed6e367fcc73960U,
    (uint64_t)0x215e161835a61d75U, (uint64_t)0xe52288a5e87a660bU, (uint64_t)0xf1d127ee3c802cb5U,
    (uint64_t)0xccde3c6aafc46044U, (uint64_t)0xdc11c08ef14cff32U, (uint64_t)0x29216f9ceca46668U,
    (uint64_t)0x22e584a3b2891c5eU, (uint64_t)0xe6deecd7810f6d87U, (uint64_t)0x6aff4b94a55659a3U,
    (uint64_t)0x12b59bb6d2e9f876U, (uint64_t)0x27ed01943aa02eabU, (uint64_t)0x8d6d420841f57075U,
    (uint64_t)0xe7b47285ef60a461U
  };

static inline void
generatePrecomputedTable(uint64_t *b, uint64_t *publicKey, uint64_t *tempBuffer)
{
  uint64_t *point0 = b;
  uint64_t *point1 = b + (uint32_t)12U;
  uint64_t *point2 = b + (uint32_t)24U;
  uint64_t *point3 = b + (uint32_t)36U;
  uint64_t *point4 = b + (uint32_t)48U;
  uint64_t *point5 = b + (uint32_t)60U;
  uint64_t *point6 = b + (uint32_t)72U;
  uint64_t *point7 = b + (uint32_t)84U;
  uint64_t *point8 = b + (uint32_t)96U;
  uint64_t *point9 = b + (uint32_t)108U;
  uint64_t *point10 = b + (uint32_t)120U;
  uint64_t *point11 = b + (uint32_t)132U;
  uint64_t *point12 = b + (uint32_t)144U;
  uint64_t *point13 = b + (uint32_t)156U;
  uint64_t *point14 = b + (uint32_t)168U;
  uint64_t *point15 = b + (uint32_t)180U;
  uint64_t *uu____0 = point0;
  uint32_t len0 = (uint32_t)4U;
  for (uint32_t i = (uint32_t)0U; i < len0; i++)
  {
    uu____0[i] = (uint64_t)0U;
  }
  uint64_t *uu____1 = point0 + (uint32_t)4U;
  uint32_t len1 = (uint32_t)4U;
  for (uint32_t i = (uint32_t)0U; i < len1; i++)
  {
    uu____1[i] = (uint64_t)0U;
  }
  uint64_t *uu____2 = point0 + (uint32_t)8U;
  uint32_t len = (uint32_t)4U;
  for (uint32_t i = (uint32_t)0U; i < len; i++)
  {
    uu____2[i] = (uint64_t)0U;
  }
  memcpy(point1, publicKey, (uint32_t)12U * sizeof (uint64_t));
  point_double(publicKey, point2, tempBuffer);
  point_add(point2, point1, point3, tempBuffer);
  point_double(point2, point4, tempBuffer);
  point_add(point4, point1, point5, tempBuffer);
  point_double(point3, point6, tempBuffer);
  point_add(point6, point1, point7, tempBuffer);
  point_double(point4, point8, tempBuffer);
  point_add(point8, point1, point9, tempBuffer);
  point_double(point5, point10, tempBuffer);
  point_add(point10, point1, point11, tempBuffer);
  point_double(point6, point12, tempBuffer);
  point_add(point12, point1, point13, tempBuffer);
  point_double(point7, point14, tempBuffer);
  point_add(point14, point1, point15, tempBuffer);
}

static inline void cswap(uint64_t bit, uint64_t *p1, uint64_t *p2)
{
  uint64_t mask = (uint64_t)0U - bit;
  {
    uint64_t dummy = mask & (p1[0U] ^ p2[0U]);
    p1[0U] = p1[0U] ^ dummy;
    p2[0U] = p2[0U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[1U] ^ p2[1U]);
    p1[1U] = p1[1U] ^ dummy;
    p2[1U] = p2[1U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[2U] ^ p2[2U]);
    p1[2U] = p1[2U] ^ dummy;
    p2[2U] = p2[2U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[3U] ^ p2[3U]);
    p1[3U] = p1[3U] ^ dummy;
    p2[3U] = p2[3U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[4U] ^ p2[4U]);
    p1[4U] = p1[4U] ^ dummy;
    p2[4U] = p2[4U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[5U] ^ p2[5U]);
    p1[5U] = p1[5U] ^ dummy;
    p2[5U] = p2[5U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[6U] ^ p2[6U]);
    p1[6U] = p1[6U] ^ dummy;
    p2[6U] = p2[6U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[7U] ^ p2[7U]);
    p1[7U] = p1[7U] ^ dummy;
    p2[7U] = p2[7U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[8U] ^ p2[8U]);
    p1[8U] = p1[8U] ^ dummy;
    p2[8U] = p2[8U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[9U] ^ p2[9U]);
    p1[9U] = p1[9U] ^ dummy;
    p2[9U] = p2[9U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[10U] ^ p2[10U]);
    p1[10U] = p1[10U] ^ dummy;
    p2[10U] = p2[10U] ^ dummy;
  }
  {
    uint64_t dummy = mask & (p1[11U] ^ p2[11U]);
    p1[11U] = p1[11U] ^ dummy;
    p2[11U] = p2[11U] ^ dummy;
  }
}

static inline uint64_t isPointAtInfinityPrivate(uint64_t *p)
{
  uint64_t z0 = p[8U];
  uint64_t z1 = p[9U];
  uint64_t z2 = p[10U];
  uint64_t z3 = p[11U];
  uint64_t z0_zero = FStar_UInt64_eq_mask(z0, (uint64_t)0U);
  uint64_t z1_zero = FStar_UInt64_eq_mask(z1, (uint64_t)0U);
  uint64_t z2_zero = FStar_UInt64_eq_mask(z2, (uint64_t)0U);
  uint64_t z3_zero = FStar_UInt64_eq_mask(z3, (uint64_t)0U);
  return (z0_zero & z1_zero) & (z2_zero & z3_zero);
}

static inline void norm(uint64_t *p, uint64_t *resultPoint, uint64_t *tempBuffer)
{
  uint64_t *xf = p;
  uint64_t *yf = p + (uint32_t)4U;
  uint64_t *zf = p + (uint32_t)8U;
  uint64_t *z2f = tempBuffer + (uint32_t)4U;
  uint64_t *z3f = tempBuffer + (uint32_t)8U;
  uint64_t *tempBuffer20 = tempBuffer + (uint32_t)12U;
  montgomery_square_buffer(zf, z2f);
  montgomery_multiplication_buffer(z2f, zf, z3f);
  exponent(z2f, z2f, tempBuffer20);
  exponent(z3f, z3f, tempBuffer20);
  montgomery_multiplication_buffer(xf, z2f, z2f);
  montgomery_multiplication_buffer(yf, z3f, z3f);
  uint64_t zeroBuffer[4U] = { 0U };
  uint64_t *resultX = resultPoint;
  uint64_t *resultY = resultPoint + (uint32_t)4U;
  uint64_t *resultZ = resultPoint + (uint32_t)8U;
  uint64_t bit = isPointAtInfinityPrivate(p);
  uint64_t t[8U] = { 0U };
  uint64_t *t_low0 = t;
  uint64_t round0[8U] = { 0U };
  uint64_t t10[8U] = { 0U };
  uint64_t t20[8U] = { 0U };
  memcpy(t_low0, z2f, (uint32_t)4U * sizeof (uint64_t));
  uint64_t t110 = t[0U];
  uint64_t temp1 = (uint64_t)0U;
  uint64_t f00 = (uint64_t)0xffffffffffffffffU;
  uint64_t f10 = (uint64_t)0xffffffffU;
  uint64_t f30 = (uint64_t)0xffffffff00000001U;
  uint64_t *o00 = t20;
  uint64_t *o10 = t20 + (uint32_t)1U;
  uint64_t *o20 = t20 + (uint32_t)2U;
  uint64_t *o30 = t20 + (uint32_t)3U;
  mul64(f00, t110, o00, &temp1);
  uint64_t h0 = temp1;
  mul64(f10, t110, o10, &temp1);
  uint64_t l0 = o10[0U];
  uint64_t c10 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l0, h0, o10);
  uint64_t h1 = temp1;
  temp1 = (uint64_t)0U;
  o20[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c10, (uint64_t)0U, h1, o20);
  uint64_t h5 = temp1;
  mul64(f30, t110, o30, &temp1);
  uint64_t l1 = o30[0U];
  uint64_t c3 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l1, h5, o30);
  uint64_t temp00 = temp1;
  t20[4U] = c3 + temp00;
  uint64_t *a0 = t;
  uint64_t *a10 = t + (uint32_t)4U;
  uint64_t *b00 = t20;
  uint64_t *b10 = t20 + (uint32_t)4U;
  uint64_t *c00 = t10;
  uint64_t *c11 = t10 + (uint32_t)4U;
  uint64_t *r00 = c00;
  uint64_t *r10 = c00 + (uint32_t)1U;
  uint64_t *r20 = c00 + (uint32_t)2U;
  uint64_t *r30 = c00 + (uint32_t)3U;
  uint64_t cc00 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a0[0U], b00[0U], r00);
  uint64_t cc1 = Lib_IntTypes_Intrinsics_add_carry_u64(cc00, a0[1U], b00[1U], r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_add_carry_u64(cc1, a0[2U], b00[2U], r20);
  uint64_t cc3 = Lib_IntTypes_Intrinsics_add_carry_u64(cc2, a0[3U], b00[3U], r30);
  uint64_t carry0 = cc3;
  uint64_t *r01 = c11;
  uint64_t *r11 = c11 + (uint32_t)1U;
  uint64_t *r21 = c11 + (uint32_t)2U;
  uint64_t *r31 = c11 + (uint32_t)3U;
  uint64_t cc01 = Lib_IntTypes_Intrinsics_add_carry_u64(carry0, a10[0U], b10[0U], r01);
  uint64_t cc10 = Lib_IntTypes_Intrinsics_add_carry_u64(cc01, a10[1U], b10[1U], r11);
  uint64_t cc20 = Lib_IntTypes_Intrinsics_add_carry_u64(cc10, a10[2U], b10[2U], r21);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc20, a10[3U], b10[3U], r31);
  uint64_t t12 = t10[1U];
  uint64_t t210 = t10[2U];
  uint64_t t30 = t10[3U];
  uint64_t t40 = t10[4U];
  uint64_t t50 = t10[5U];
  uint64_t t60 = t10[6U];
  uint64_t t70 = t10[7U];
  t10[0U] = t12;
  t10[1U] = t210;
  t10[2U] = t30;
  t10[3U] = t40;
  t10[4U] = t50;
  t10[5U] = t60;
  t10[6U] = t70;
  t10[7U] = (uint64_t)0U;
  uint64_t t111 = t10[0U];
  uint64_t temp2 = (uint64_t)0U;
  uint64_t f01 = (uint64_t)0xffffffffffffffffU;
  uint64_t f11 = (uint64_t)0xffffffffU;
  uint64_t f31 = (uint64_t)0xffffffff00000001U;
  uint64_t *o01 = t20;
  uint64_t *o11 = t20 + (uint32_t)1U;
  uint64_t *o21 = t20 + (uint32_t)2U;
  uint64_t *o31 = t20 + (uint32_t)3U;
  mul64(f01, t111, o01, &temp2);
  uint64_t h2 = temp2;
  mul64(f11, t111, o11, &temp2);
  uint64_t l2 = o11[0U];
  uint64_t c12 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l2, h2, o11);
  uint64_t h3 = temp2;
  temp2 = (uint64_t)0U;
  o21[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c12, (uint64_t)0U, h3, o21);
  uint64_t h50 = temp2;
  mul64(f31, t111, o31, &temp2);
  uint64_t l3 = o31[0U];
  uint64_t c30 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l3, h50, o31);
  uint64_t temp01 = temp2;
  t20[4U] = c30 + temp01;
  uint64_t *a00 = t10;
  uint64_t *a11 = t10 + (uint32_t)4U;
  uint64_t *b01 = t20;
  uint64_t *b11 = t20 + (uint32_t)4U;
  uint64_t *c01 = round0;
  uint64_t *c13 = round0 + (uint32_t)4U;
  uint64_t *r02 = c01;
  uint64_t *r12 = c01 + (uint32_t)1U;
  uint64_t *r22 = c01 + (uint32_t)2U;
  uint64_t *r32 = c01 + (uint32_t)3U;
  uint64_t cc02 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a00[0U], b01[0U], r02);
  uint64_t cc11 = Lib_IntTypes_Intrinsics_add_carry_u64(cc02, a00[1U], b01[1U], r12);
  uint64_t cc21 = Lib_IntTypes_Intrinsics_add_carry_u64(cc11, a00[2U], b01[2U], r22);
  uint64_t cc30 = Lib_IntTypes_Intrinsics_add_carry_u64(cc21, a00[3U], b01[3U], r32);
  uint64_t carry00 = cc30;
  uint64_t *r03 = c13;
  uint64_t *r13 = c13 + (uint32_t)1U;
  uint64_t *r23 = c13 + (uint32_t)2U;
  uint64_t *r33 = c13 + (uint32_t)3U;
  uint64_t cc03 = Lib_IntTypes_Intrinsics_add_carry_u64(carry00, a11[0U], b11[0U], r03);
  uint64_t cc12 = Lib_IntTypes_Intrinsics_add_carry_u64(cc03, a11[1U], b11[1U], r13);
  uint64_t cc22 = Lib_IntTypes_Intrinsics_add_carry_u64(cc12, a11[2U], b11[2U], r23);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc22, a11[3U], b11[3U], r33);
  uint64_t t120 = round0[1U];
  uint64_t t211 = round0[2U];
  uint64_t t31 = round0[3U];
  uint64_t t41 = round0[4U];
  uint64_t t51 = round0[5U];
  uint64_t t61 = round0[6U];
  uint64_t t71 = round0[7U];
  round0[0U] = t120;
  round0[1U] = t211;
  round0[2U] = t31;
  round0[3U] = t41;
  round0[4U] = t51;
  round0[5U] = t61;
  round0[6U] = t71;
  round0[7U] = (uint64_t)0U;
  uint64_t t112 = round0[0U];
  uint64_t temp3 = (uint64_t)0U;
  uint64_t f02 = (uint64_t)0xffffffffffffffffU;
  uint64_t f12 = (uint64_t)0xffffffffU;
  uint64_t f32 = (uint64_t)0xffffffff00000001U;
  uint64_t *o02 = t20;
  uint64_t *o12 = t20 + (uint32_t)1U;
  uint64_t *o22 = t20 + (uint32_t)2U;
  uint64_t *o32 = t20 + (uint32_t)3U;
  mul64(f02, t112, o02, &temp3);
  uint64_t h4 = temp3;
  mul64(f12, t112, o12, &temp3);
  uint64_t l4 = o12[0U];
  uint64_t c14 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l4, h4, o12);
  uint64_t h6 = temp3;
  temp3 = (uint64_t)0U;
  o22[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c14, (uint64_t)0U, h6, o22);
  uint64_t h51 = temp3;
  mul64(f32, t112, o32, &temp3);
  uint64_t l5 = o32[0U];
  uint64_t c31 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l5, h51, o32);
  uint64_t temp02 = temp3;
  t20[4U] = c31 + temp02;
  uint64_t *a01 = round0;
  uint64_t *a12 = round0 + (uint32_t)4U;
  uint64_t *b02 = t20;
  uint64_t *b12 = t20 + (uint32_t)4U;
  uint64_t *c02 = t10;
  uint64_t *c15 = t10 + (uint32_t)4U;
  uint64_t *r04 = c02;
  uint64_t *r14 = c02 + (uint32_t)1U;
  uint64_t *r24 = c02 + (uint32_t)2U;
  uint64_t *r34 = c02 + (uint32_t)3U;
  uint64_t cc04 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a01[0U], b02[0U], r04);
  uint64_t cc13 = Lib_IntTypes_Intrinsics_add_carry_u64(cc04, a01[1U], b02[1U], r14);
  uint64_t cc23 = Lib_IntTypes_Intrinsics_add_carry_u64(cc13, a01[2U], b02[2U], r24);
  uint64_t cc31 = Lib_IntTypes_Intrinsics_add_carry_u64(cc23, a01[3U], b02[3U], r34);
  uint64_t carry01 = cc31;
  uint64_t *r05 = c15;
  uint64_t *r15 = c15 + (uint32_t)1U;
  uint64_t *r25 = c15 + (uint32_t)2U;
  uint64_t *r35 = c15 + (uint32_t)3U;
  uint64_t cc05 = Lib_IntTypes_Intrinsics_add_carry_u64(carry01, a12[0U], b12[0U], r05);
  uint64_t cc14 = Lib_IntTypes_Intrinsics_add_carry_u64(cc05, a12[1U], b12[1U], r15);
  uint64_t cc24 = Lib_IntTypes_Intrinsics_add_carry_u64(cc14, a12[2U], b12[2U], r25);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc24, a12[3U], b12[3U], r35);
  uint64_t t121 = t10[1U];
  uint64_t t212 = t10[2U];
  uint64_t t32 = t10[3U];
  uint64_t t42 = t10[4U];
  uint64_t t52 = t10[5U];
  uint64_t t62 = t10[6U];
  uint64_t t72 = t10[7U];
  t10[0U] = t121;
  t10[1U] = t212;
  t10[2U] = t32;
  t10[3U] = t42;
  t10[4U] = t52;
  t10[5U] = t62;
  t10[6U] = t72;
  t10[7U] = (uint64_t)0U;
  uint64_t t113 = t10[0U];
  uint64_t temp4 = (uint64_t)0U;
  uint64_t f03 = (uint64_t)0xffffffffffffffffU;
  uint64_t f13 = (uint64_t)0xffffffffU;
  uint64_t f33 = (uint64_t)0xffffffff00000001U;
  uint64_t *o03 = t20;
  uint64_t *o13 = t20 + (uint32_t)1U;
  uint64_t *o23 = t20 + (uint32_t)2U;
  uint64_t *o33 = t20 + (uint32_t)3U;
  mul64(f03, t113, o03, &temp4);
  uint64_t h7 = temp4;
  mul64(f13, t113, o13, &temp4);
  uint64_t l6 = o13[0U];
  uint64_t c16 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l6, h7, o13);
  uint64_t h8 = temp4;
  temp4 = (uint64_t)0U;
  o23[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c16, (uint64_t)0U, h8, o23);
  uint64_t h52 = temp4;
  mul64(f33, t113, o33, &temp4);
  uint64_t l7 = o33[0U];
  uint64_t c32 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l7, h52, o33);
  uint64_t temp03 = temp4;
  t20[4U] = c32 + temp03;
  uint64_t *a02 = t10;
  uint64_t *a13 = t10 + (uint32_t)4U;
  uint64_t *b03 = t20;
  uint64_t *b13 = t20 + (uint32_t)4U;
  uint64_t *c03 = round0;
  uint64_t *c17 = round0 + (uint32_t)4U;
  uint64_t *r06 = c03;
  uint64_t *r16 = c03 + (uint32_t)1U;
  uint64_t *r26 = c03 + (uint32_t)2U;
  uint64_t *r36 = c03 + (uint32_t)3U;
  uint64_t cc06 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a02[0U], b03[0U], r06);
  uint64_t cc15 = Lib_IntTypes_Intrinsics_add_carry_u64(cc06, a02[1U], b03[1U], r16);
  uint64_t cc25 = Lib_IntTypes_Intrinsics_add_carry_u64(cc15, a02[2U], b03[2U], r26);
  uint64_t cc32 = Lib_IntTypes_Intrinsics_add_carry_u64(cc25, a02[3U], b03[3U], r36);
  uint64_t carry02 = cc32;
  uint64_t *r07 = c17;
  uint64_t *r17 = c17 + (uint32_t)1U;
  uint64_t *r27 = c17 + (uint32_t)2U;
  uint64_t *r37 = c17 + (uint32_t)3U;
  uint64_t cc07 = Lib_IntTypes_Intrinsics_add_carry_u64(carry02, a13[0U], b13[0U], r07);
  uint64_t cc16 = Lib_IntTypes_Intrinsics_add_carry_u64(cc07, a13[1U], b13[1U], r17);
  uint64_t cc26 = Lib_IntTypes_Intrinsics_add_carry_u64(cc16, a13[2U], b13[2U], r27);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc26, a13[3U], b13[3U], r37);
  uint64_t t122 = round0[1U];
  uint64_t t213 = round0[2U];
  uint64_t t33 = round0[3U];
  uint64_t t43 = round0[4U];
  uint64_t t53 = round0[5U];
  uint64_t t63 = round0[6U];
  uint64_t t73 = round0[7U];
  round0[0U] = t122;
  round0[1U] = t213;
  round0[2U] = t33;
  round0[3U] = t43;
  round0[4U] = t53;
  round0[5U] = t63;
  round0[6U] = t73;
  round0[7U] = (uint64_t)0U;
  uint64_t tempBuffer1[4U] = { 0U };
  uint64_t tempBufferForSubborrow0 = (uint64_t)0U;
  uint64_t cin0 = round0[4U];
  uint64_t *x_0 = round0;
  uint64_t *r08 = tempBuffer1;
  uint64_t *r18 = tempBuffer1 + (uint32_t)1U;
  uint64_t *r28 = tempBuffer1 + (uint32_t)2U;
  uint64_t *r38 = tempBuffer1 + (uint32_t)3U;
  uint64_t
  cc4 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      x_0[0U],
      (uint64_t)0xffffffffffffffffU,
      r08);
  uint64_t
  cc17 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc4, x_0[1U], (uint64_t)0xffffffffU, r18);
  uint64_t cc27 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc17, x_0[2U], (uint64_t)0U, r28);
  uint64_t
  cc33 =
    Lib_IntTypes_Intrinsics_sub_borrow_u64(cc27,
      x_0[3U],
      (uint64_t)0xffffffff00000001U,
      r38);
  uint64_t c = cc33;
  uint64_t
  carry = Lib_IntTypes_Intrinsics_sub_borrow_u64(c, cin0, (uint64_t)0U, &tempBufferForSubborrow0);
  cmovznz4(resultX, tempBuffer1, x_0, carry);
  uint64_t t0[8U] = { 0U };
  uint64_t *t_low = t0;
  uint64_t round[8U] = { 0U };
  uint64_t t1[8U] = { 0U };
  uint64_t t2[8U] = { 0U };
  memcpy(t_low, z3f, (uint32_t)4U * sizeof (uint64_t));
  uint64_t t11 = t0[0U];
  uint64_t temp5 = (uint64_t)0U;
  uint64_t f04 = (uint64_t)0xffffffffffffffffU;
  uint64_t f14 = (uint64_t)0xffffffffU;
  uint64_t f34 = (uint64_t)0xffffffff00000001U;
  uint64_t *o04 = t2;
  uint64_t *o14 = t2 + (uint32_t)1U;
  uint64_t *o24 = t2 + (uint32_t)2U;
  uint64_t *o34 = t2 + (uint32_t)3U;
  mul64(f04, t11, o04, &temp5);
  uint64_t h9 = temp5;
  mul64(f14, t11, o14, &temp5);
  uint64_t l8 = o14[0U];
  uint64_t c18 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l8, h9, o14);
  uint64_t h10 = temp5;
  temp5 = (uint64_t)0U;
  o24[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c18, (uint64_t)0U, h10, o24);
  uint64_t h53 = temp5;
  mul64(f34, t11, o34, &temp5);
  uint64_t l9 = o34[0U];
  uint64_t c33 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l9, h53, o34);
  uint64_t temp04 = temp5;
  t2[4U] = c33 + temp04;
  uint64_t *a03 = t0;
  uint64_t *a14 = t0 + (uint32_t)4U;
  uint64_t *b04 = t2;
  uint64_t *b14 = t2 + (uint32_t)4U;
  uint64_t *c04 = t1;
  uint64_t *c19 = t1 + (uint32_t)4U;
  uint64_t *r09 = c04;
  uint64_t *r19 = c04 + (uint32_t)1U;
  uint64_t *r29 = c04 + (uint32_t)2U;
  uint64_t *r39 = c04 + (uint32_t)3U;
  uint64_t cc08 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a03[0U], b04[0U], r09);
  uint64_t cc18 = Lib_IntTypes_Intrinsics_add_carry_u64(cc08, a03[1U], b04[1U], r19);
  uint64_t cc28 = Lib_IntTypes_Intrinsics_add_carry_u64(cc18, a03[2U], b04[2U], r29);
  uint64_t cc34 = Lib_IntTypes_Intrinsics_add_carry_u64(cc28, a03[3U], b04[3U], r39);
  uint64_t carry03 = cc34;
  uint64_t *r010 = c19;
  uint64_t *r110 = c19 + (uint32_t)1U;
  uint64_t *r210 = c19 + (uint32_t)2U;
  uint64_t *r310 = c19 + (uint32_t)3U;
  uint64_t cc09 = Lib_IntTypes_Intrinsics_add_carry_u64(carry03, a14[0U], b14[0U], r010);
  uint64_t cc19 = Lib_IntTypes_Intrinsics_add_carry_u64(cc09, a14[1U], b14[1U], r110);
  uint64_t cc29 = Lib_IntTypes_Intrinsics_add_carry_u64(cc19, a14[2U], b14[2U], r210);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc29, a14[3U], b14[3U], r310);
  uint64_t t123 = t1[1U];
  uint64_t t214 = t1[2U];
  uint64_t t34 = t1[3U];
  uint64_t t44 = t1[4U];
  uint64_t t54 = t1[5U];
  uint64_t t64 = t1[6U];
  uint64_t t74 = t1[7U];
  t1[0U] = t123;
  t1[1U] = t214;
  t1[2U] = t34;
  t1[3U] = t44;
  t1[4U] = t54;
  t1[5U] = t64;
  t1[6U] = t74;
  t1[7U] = (uint64_t)0U;
  uint64_t t114 = t1[0U];
  uint64_t temp6 = (uint64_t)0U;
  uint64_t f05 = (uint64_t)0xffffffffffffffffU;
  uint64_t f15 = (uint64_t)0xffffffffU;
  uint64_t f35 = (uint64_t)0xffffffff00000001U;
  uint64_t *o05 = t2;
  uint64_t *o15 = t2 + (uint32_t)1U;
  uint64_t *o25 = t2 + (uint32_t)2U;
  uint64_t *o35 = t2 + (uint32_t)3U;
  mul64(f05, t114, o05, &temp6);
  uint64_t h11 = temp6;
  mul64(f15, t114, o15, &temp6);
  uint64_t l10 = o15[0U];
  uint64_t c110 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l10, h11, o15);
  uint64_t h12 = temp6;
  temp6 = (uint64_t)0U;
  o25[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c110, (uint64_t)0U, h12, o25);
  uint64_t h54 = temp6;
  mul64(f35, t114, o35, &temp6);
  uint64_t l11 = o35[0U];
  uint64_t c34 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l11, h54, o35);
  uint64_t temp05 = temp6;
  t2[4U] = c34 + temp05;
  uint64_t *a04 = t1;
  uint64_t *a15 = t1 + (uint32_t)4U;
  uint64_t *b05 = t2;
  uint64_t *b15 = t2 + (uint32_t)4U;
  uint64_t *c05 = round;
  uint64_t *c111 = round + (uint32_t)4U;
  uint64_t *r011 = c05;
  uint64_t *r111 = c05 + (uint32_t)1U;
  uint64_t *r211 = c05 + (uint32_t)2U;
  uint64_t *r311 = c05 + (uint32_t)3U;
  uint64_t cc010 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a04[0U], b05[0U], r011);
  uint64_t cc110 = Lib_IntTypes_Intrinsics_add_carry_u64(cc010, a04[1U], b05[1U], r111);
  uint64_t cc210 = Lib_IntTypes_Intrinsics_add_carry_u64(cc110, a04[2U], b05[2U], r211);
  uint64_t cc35 = Lib_IntTypes_Intrinsics_add_carry_u64(cc210, a04[3U], b05[3U], r311);
  uint64_t carry04 = cc35;
  uint64_t *r012 = c111;
  uint64_t *r112 = c111 + (uint32_t)1U;
  uint64_t *r212 = c111 + (uint32_t)2U;
  uint64_t *r312 = c111 + (uint32_t)3U;
  uint64_t cc011 = Lib_IntTypes_Intrinsics_add_carry_u64(carry04, a15[0U], b15[0U], r012);
  uint64_t cc111 = Lib_IntTypes_Intrinsics_add_carry_u64(cc011, a15[1U], b15[1U], r112);
  uint64_t cc211 = Lib_IntTypes_Intrinsics_add_carry_u64(cc111, a15[2U], b15[2U], r212);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc211, a15[3U], b15[3U], r312);
  uint64_t t124 = round[1U];
  uint64_t t215 = round[2U];
  uint64_t t35 = round[3U];
  uint64_t t45 = round[4U];
  uint64_t t55 = round[5U];
  uint64_t t65 = round[6U];
  uint64_t t75 = round[7U];
  round[0U] = t124;
  round[1U] = t215;
  round[2U] = t35;
  round[3U] = t45;
  round[4U] = t55;
  round[5U] = t65;
  round[6U] = t75;
  round[7U] = (uint64_t)0U;
  uint64_t t115 = round[0U];
  uint64_t temp7 = (uint64_t)0U;
  uint64_t f06 = (uint64_t)0xffffffffffffffffU;
  uint64_t f16 = (uint64_t)0xffffffffU;
  uint64_t f36 = (uint64_t)0xffffffff00000001U;
  uint64_t *o06 = t2;
  uint64_t *o16 = t2 + (uint32_t)1U;
  uint64_t *o26 = t2 + (uint32_t)2U;
  uint64_t *o36 = t2 + (uint32_t)3U;
  mul64(f06, t115, o06, &temp7);
  uint64_t h13 = temp7;
  mul64(f16, t115, o16, &temp7);
  uint64_t l12 = o16[0U];
  uint64_t c112 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l12, h13, o16);
  uint64_t h14 = temp7;
  temp7 = (uint64_t)0U;
  o26[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c112, (uint64_t)0U, h14, o26);
  uint64_t h55 = temp7;
  mul64(f36, t115, o36, &temp7);
  uint64_t l13 = o36[0U];
  uint64_t c35 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l13, h55, o36);
  uint64_t temp06 = temp7;
  t2[4U] = c35 + temp06;
  uint64_t *a05 = round;
  uint64_t *a16 = round + (uint32_t)4U;
  uint64_t *b06 = t2;
  uint64_t *b16 = t2 + (uint32_t)4U;
  uint64_t *c06 = t1;
  uint64_t *c113 = t1 + (uint32_t)4U;
  uint64_t *r013 = c06;
  uint64_t *r113 = c06 + (uint32_t)1U;
  uint64_t *r213 = c06 + (uint32_t)2U;
  uint64_t *r313 = c06 + (uint32_t)3U;
  uint64_t cc012 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a05[0U], b06[0U], r013);
  uint64_t cc112 = Lib_IntTypes_Intrinsics_add_carry_u64(cc012, a05[1U], b06[1U], r113);
  uint64_t cc212 = Lib_IntTypes_Intrinsics_add_carry_u64(cc112, a05[2U], b06[2U], r213);
  uint64_t cc36 = Lib_IntTypes_Intrinsics_add_carry_u64(cc212, a05[3U], b06[3U], r313);
  uint64_t carry05 = cc36;
  uint64_t *r014 = c113;
  uint64_t *r114 = c113 + (uint32_t)1U;
  uint64_t *r214 = c113 + (uint32_t)2U;
  uint64_t *r314 = c113 + (uint32_t)3U;
  uint64_t cc013 = Lib_IntTypes_Intrinsics_add_carry_u64(carry05, a16[0U], b16[0U], r014);
  uint64_t cc113 = Lib_IntTypes_Intrinsics_add_carry_u64(cc013, a16[1U], b16[1U], r114);
  uint64_t cc213 = Lib_IntTypes_Intrinsics_add_carry_u64(cc113, a16[2U], b16[2U], r214);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc213, a16[3U], b16[3U], r314);
  uint64_t t125 = t1[1U];
  uint64_t t216 = t1[2U];
  uint64_t t36 = t1[3U];
  uint64_t t46 = t1[4U];
  uint64_t t56 = t1[5U];
  uint64_t t66 = t1[6U];
  uint64_t t76 = t1[7U];
  t1[0U] = t125;
  t1[1U] = t216;
  t1[2U] = t36;
  t1[3U] = t46;
  t1[4U] = t56;
  t1[5U] = t66;
  t1[6U] = t76;
  t1[7U] = (uint64_t)0U;
  uint64_t t116 = t1[0U];
  uint64_t temp = (uint64_t)0U;
  uint64_t f0 = (uint64_t)0xffffffffffffffffU;
  uint64_t f1 = (uint64_t)0xffffffffU;
  uint64_t f3 = (uint64_t)0xffffffff00000001U;
  uint64_t *o0 = t2;
  uint64_t *o1 = t2 + (uint32_t)1U;
  uint64_t *o2 = t2 + (uint32_t)2U;
  uint64_t *o3 = t2 + (uint32_t)3U;
  mul64(f0, t116, o0, &temp);
  uint64_t h15 = temp;
  mul64(f1, t116, o1, &temp);
  uint64_t l14 = o1[0U];
  uint64_t c114 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l14, h15, o1);
  uint64_t h = temp;
  temp = (uint64_t)0U;
  o2[0U] = (uint64_t)0U;
  Lib_IntTypes_Intrinsics_add_carry_u64_void(c114, (uint64_t)0U, h, o2);
  uint64_t h56 = temp;
  mul64(f3, t116, o3, &temp);
  uint64_t l = o3[0U];
  uint64_t c36 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, l, h56, o3);
  uint64_t temp0 = temp;
  t2[4U] = c36 + temp0;
  uint64_t *a06 = t1;
  uint64_t *a1 = t1 + (uint32_t)4U;
  uint64_t *b0 = t2;
  uint64_t *b1 = t2 + (uint32_t)4U;
  uint64_t *c0 = round;
  uint64_t *c1 = round + (uint32_t)4U;
  uint64_t *r015 = c0;
  uint64_t *r115 = c0 + (uint32_t)1U;
  uint64_t *r215 = c0 + (uint32_t)2U;
  uint64_t *r315 = c0 + (uint32_t)3U;
  uint64_t cc014 = Lib_IntTypes_Intrinsics_add_carry_u64((uint64_t)0U, a06[0U], b0[0U], r015);
  uint64_t cc114 = Lib_IntTypes_Intrinsics_add_carry_u64(cc014, a06[1U], b0[1U], r115);
  uint64_t cc214 = Lib_IntTypes_Intrinsics_add_carry_u64(cc114, a06[2U], b0[2U], r215);
  uint64_t cc37 = Lib_IntTypes_Intrinsics_add_carry_u64(cc214, a06[3U], b0[3U], r315);
  uint64_t carry06 = cc37;
  uint64_t *r016 = c1;
  uint64_t *r116 = c1 + (uint32_t)1U;
  uint64_t *r216 = c1 + (uint32_t)2U;
  uint64_t *r316 = c1 + (uint32_t)3U;
  uint64_t cc0 = Lib_IntTypes_Intrinsics_add_carry_u64(carry06, a1[0U], b1[0U], r016);
  uint64_t cc115 = Lib_IntTypes_Intrinsics_add_carry_u64(cc0, a1[1U], b1[1U], r116);
  uint64_t cc215 = Lib_IntTypes_Intrinsics_add_carry_u64(cc115, a1[2U], b1[2U], r216);
  Lib_IntTypes_Intrinsics_add_carry_u64_void(cc215, a1[3U], b1[3U], r316);
  uint64_t t126 = round[1U];
  uint64_t t21 = round[2U];
  uint64_t t3 = round[3U];
  uint64_t t4 = round[4U];
  uint64_t t5 = round[5U];
  uint64_t t6 = round[6U];
  uint64_t t7 = round[7U];
  round[0U] = t126;
  round[1U] = t21;
  round[2U] = t3;
  round[3U] = t4;
  round[4U] = t5;
  round[5U] = t6;
  round[6U] = t7;
  round[7U] = (uint64_t)0U;
  uint64_t tempBuffer10[4U] = { 0U };
  uint64_t tempBufferForSubborrow = (uint64_t)0U;
  uint64_t cin = round[4U];
  uint64_t *x_ = round;
  uint64_t *r0 = tempBuffer10;
  uint64_t *r1 = tempBuffer10 + (uint32_t)1U;
  uint64_t *r2 = tempBuffer10 + (uint32_t)2U;
  uint64_t *r3 = tempBuffer10 + (uint32_t)3U;
  uint64_t
  cc =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      x_[0U],
      (uint64_t)0xffffffffffffffffU,
      r0);
  uint64_t cc116 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, x_[1U], (uint64_t)0xffffffffU, r1);
  uint64_t cc216 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc116, x_[2U], (uint64_t)0U, r2);
  uint64_t
  cc38 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc216, x_[3U], (uint64_t)0xffffffff00000001U, r3);
  uint64_t c2 = cc38;
  uint64_t
  carry1 = Lib_IntTypes_Intrinsics_sub_borrow_u64(c2, cin, (uint64_t)0U, &tempBufferForSubborrow);
  cmovznz4(resultY, tempBuffer10, x_, carry1);
  resultZ[0U] = (uint64_t)1U;
  resultZ[1U] = (uint64_t)0U;
  resultZ[2U] = (uint64_t)0U;
  resultZ[3U] = (uint64_t)0U;
  copy_conditional(resultZ, zeroBuffer, bit);
}

static void
scalarMultiplicationL(
  Spec_P256_montgomery_ladder_mode m,
  uint64_t *p,
  uint64_t *result,
  uint8_t *scalar,
  uint64_t *tempBuffer
)
{
  uint64_t *q = tempBuffer;
  uint64_t *buff = tempBuffer + (uint32_t)12U;
  uint64_t *p_x = p;
  uint64_t *p_y = p + (uint32_t)4U;
  uint64_t *r_x = result;
  uint64_t *r_y = result + (uint32_t)4U;
  uint64_t multBuffer[8U] = { 0U };
  multBuffer[0U] = (uint64_t)0U;
  multBuffer[1U] = (uint64_t)0U;
  multBuffer[2U] = (uint64_t)0U;
  multBuffer[3U] = (uint64_t)0U;
  multBuffer[4U] = p_x[0U];
  multBuffer[5U] = p_x[1U];
  multBuffer[6U] = p_x[2U];
  multBuffer[7U] = p_x[3U];
  solinas_reduction_impl(multBuffer, r_x);
  uint64_t multBuffer0[8U] = { 0U };
  multBuffer0[0U] = (uint64_t)0U;
  multBuffer0[1U] = (uint64_t)0U;
  multBuffer0[2U] = (uint64_t)0U;
  multBuffer0[3U] = (uint64_t)0U;
  multBuffer0[4U] = p_y[0U];
  multBuffer0[5U] = p_y[1U];
  multBuffer0[6U] = p_y[2U];
  multBuffer0[7U] = p_y[3U];
  solinas_reduction_impl(multBuffer0, r_y);
  result[8U] = (uint64_t)1U;
  result[9U] = (uint64_t)18446744069414584320U;
  result[10U] = (uint64_t)18446744073709551615U;
  result[11U] = (uint64_t)4294967294U;
  uint64_t bufferPrecomputed[192U];
  switch (m)
  {
    case Spec_P256_Ladder:
      {
        for (uint32_t i = (uint32_t)0U; i < (uint32_t)256U; i++)
        {
          uint32_t bit0 = (uint32_t)255U - i;
          uint64_t
          bit =
            (uint64_t)(scalar[(uint32_t)31U
            - bit0 / (uint32_t)8U]
            >> bit0 % (uint32_t)8U
            & (uint8_t)1U);
          cswap(bit, q, result);
          point_add(q, result, result, buff);
          point_double(q, q, buff);
          cswap(bit, q, result);
        }
        break;
      }
    default:
      {
        uint64_t init = (uint64_t)0U;
        for (uint32_t i = (uint32_t)0U; i < (uint32_t)192U; i++)
        {
          bufferPrecomputed[i] = init;
        }
        generatePrecomputedTable(bufferPrecomputed, result, buff);
        uint32_t half0 = (uint32_t)(krml_checked_int_t)0 >> (uint32_t)1U;
        uint32_t word = (uint32_t)scalar[half0];
        uint32_t bitShift0 = (uint32_t)(krml_checked_int_t)0 & (uint32_t)1U;
        uint32_t bitShiftAsPrivate0 = bitShift0;
        uint64_t mask10 = (uint64_t)0U - (uint64_t)bitShiftAsPrivate0;
        uint32_t mask0 = (uint32_t)0xf0U ^ (uint32_t)(mask10 & (uint64_t)(uint32_t)255U);
        uint64_t mask11 = (uint64_t)0U - (uint64_t)bitShift0;
        uint32_t shiftMask = (uint32_t)0x4U ^ (uint32_t)(mask11 & (uint64_t)(uint32_t)4U);
        uint32_t result10 = word & mask0;
        uint32_t bits = result10 >> shiftMask;
        uint64_t *pointToStart = bufferPrecomputed + bits * (uint32_t)12U;
        memcpy(q, pointToStart, (uint32_t)12U * sizeof (uint64_t));
        for (uint32_t i0 = (uint32_t)1U; i0 < (uint32_t)64U; i0++)
        {
          uint32_t half = i0 >> (uint32_t)1U;
          uint32_t word0 = (uint32_t)scalar[half];
          uint32_t bitShift = i0 & (uint32_t)1U;
          uint32_t bitShiftAsPrivate = bitShift;
          uint64_t mask12 = (uint64_t)0U - (uint64_t)bitShiftAsPrivate;
          uint32_t mask2 = (uint32_t)0xf0U ^ (uint32_t)(mask12 & (uint64_t)(uint32_t)255U);
          uint64_t mask1 = (uint64_t)0U - (uint64_t)bitShift;
          uint32_t shiftMask0 = (uint32_t)0x4U ^ (uint32_t)(mask1 & (uint64_t)(uint32_t)4U);
          uint32_t result1 = word0 & mask2;
          uint32_t bits1 = result1 >> shiftMask0;
          uint64_t pointToAdd[12U] = { 0U };
          for (uint32_t i = (uint32_t)0U; i < (uint32_t)16U; i++)
          {
            uint64_t mask = FStar_UInt64_eq_mask((uint64_t)bits1, (uint64_t)i);
            uint64_t *lut_cmb_x = bufferPrecomputed + i * (uint32_t)(krml_checked_int_t)12;
            uint64_t
            *lut_cmb_y = bufferPrecomputed + i * (uint32_t)(krml_checked_int_t)12 + (uint32_t)4U;
            uint64_t
            *lut_cmb_z = bufferPrecomputed + i * (uint32_t)(krml_checked_int_t)12 + (uint32_t)8U;
            copy_conditional(pointToAdd, lut_cmb_x, mask);
            copy_conditional(pointToAdd + (uint32_t)4U, lut_cmb_y, mask);
            copy_conditional(pointToAdd + (uint32_t)8U, lut_cmb_z, mask);
          }
          point_double(q, q, buff);
          point_double(q, q, buff);
          point_double(q, q, buff);
          point_double(q, q, buff);
          point_add(pointToAdd, q, q, buff);
        }
      }
  }
  norm(q, result, buff);
}

static inline void
scalarMultiplicationWithoutNorm(
  Spec_P256_montgomery_ladder_mode m,
  uint64_t *p,
  uint64_t *result,
  uint8_t *scalar,
  uint64_t *tempBuffer
)
{
  uint64_t *q = tempBuffer;
  uint64_t *buff = tempBuffer + (uint32_t)12U;
  uint64_t *p_x = p;
  uint64_t *p_y = p + (uint32_t)4U;
  uint64_t *r_x = result;
  uint64_t *r_y = result + (uint32_t)4U;
  uint64_t multBuffer[8U] = { 0U };
  multBuffer[0U] = (uint64_t)0U;
  multBuffer[1U] = (uint64_t)0U;
  multBuffer[2U] = (uint64_t)0U;
  multBuffer[3U] = (uint64_t)0U;
  multBuffer[4U] = p_x[0U];
  multBuffer[5U] = p_x[1U];
  multBuffer[6U] = p_x[2U];
  multBuffer[7U] = p_x[3U];
  solinas_reduction_impl(multBuffer, r_x);
  uint64_t multBuffer0[8U] = { 0U };
  multBuffer0[0U] = (uint64_t)0U;
  multBuffer0[1U] = (uint64_t)0U;
  multBuffer0[2U] = (uint64_t)0U;
  multBuffer0[3U] = (uint64_t)0U;
  multBuffer0[4U] = p_y[0U];
  multBuffer0[5U] = p_y[1U];
  multBuffer0[6U] = p_y[2U];
  multBuffer0[7U] = p_y[3U];
  solinas_reduction_impl(multBuffer0, r_y);
  result[8U] = (uint64_t)1U;
  result[9U] = (uint64_t)18446744069414584320U;
  result[10U] = (uint64_t)18446744073709551615U;
  result[11U] = (uint64_t)4294967294U;
  uint64_t bufferPrecomputed[192U];
  switch (m)
  {
    case Spec_P256_Ladder:
      {
        for (uint32_t i = (uint32_t)0U; i < (uint32_t)256U; i++)
        {
          uint32_t bit0 = (uint32_t)255U - i;
          uint64_t
          bit =
            (uint64_t)(scalar[(uint32_t)31U
            - bit0 / (uint32_t)8U]
            >> bit0 % (uint32_t)8U
            & (uint8_t)1U);
          cswap(bit, q, result);
          point_add(q, result, result, buff);
          point_double(q, q, buff);
          cswap(bit, q, result);
        }
        break;
      }
    default:
      {
        uint64_t init = (uint64_t)0U;
        for (uint32_t i = (uint32_t)0U; i < (uint32_t)192U; i++)
        {
          bufferPrecomputed[i] = init;
        }
        generatePrecomputedTable(bufferPrecomputed, result, buff);
        uint32_t half0 = (uint32_t)(krml_checked_int_t)0 >> (uint32_t)1U;
        uint32_t word = (uint32_t)scalar[half0];
        uint32_t bitShift0 = (uint32_t)(krml_checked_int_t)0 & (uint32_t)1U;
        uint32_t bitShiftAsPrivate0 = bitShift0;
        uint64_t mask10 = (uint64_t)0U - (uint64_t)bitShiftAsPrivate0;
        uint32_t mask0 = (uint32_t)0xf0U ^ (uint32_t)(mask10 & (uint64_t)(uint32_t)255U);
        uint64_t mask11 = (uint64_t)0U - (uint64_t)bitShift0;
        uint32_t shiftMask = (uint32_t)0x4U ^ (uint32_t)(mask11 & (uint64_t)(uint32_t)4U);
        uint32_t result10 = word & mask0;
        uint32_t bits = result10 >> shiftMask;
        uint64_t *pointToStart = bufferPrecomputed + bits * (uint32_t)12U;
        memcpy(q, pointToStart, (uint32_t)12U * sizeof (uint64_t));
        for (uint32_t i0 = (uint32_t)1U; i0 < (uint32_t)64U; i0++)
        {
          uint32_t half = i0 >> (uint32_t)1U;
          uint32_t word0 = (uint32_t)scalar[half];
          uint32_t bitShift = i0 & (uint32_t)1U;
          uint32_t bitShiftAsPrivate = bitShift;
          uint64_t mask12 = (uint64_t)0U - (uint64_t)bitShiftAsPrivate;
          uint32_t mask2 = (uint32_t)0xf0U ^ (uint32_t)(mask12 & (uint64_t)(uint32_t)255U);
          uint64_t mask1 = (uint64_t)0U - (uint64_t)bitShift;
          uint32_t shiftMask0 = (uint32_t)0x4U ^ (uint32_t)(mask1 & (uint64_t)(uint32_t)4U);
          uint32_t result1 = word0 & mask2;
          uint32_t bits1 = result1 >> shiftMask0;
          uint64_t pointToAdd[12U] = { 0U };
          for (uint32_t i = (uint32_t)0U; i < (uint32_t)16U; i++)
          {
            uint64_t mask = FStar_UInt64_eq_mask((uint64_t)bits1, (uint64_t)i);
            uint64_t *lut_cmb_x = bufferPrecomputed + i * (uint32_t)(krml_checked_int_t)12;
            uint64_t
            *lut_cmb_y = bufferPrecomputed + i * (uint32_t)(krml_checked_int_t)12 + (uint32_t)4U;
            uint64_t
            *lut_cmb_z = bufferPrecomputed + i * (uint32_t)(krml_checked_int_t)12 + (uint32_t)8U;
            copy_conditional(pointToAdd, lut_cmb_x, mask);
            copy_conditional(pointToAdd + (uint32_t)4U, lut_cmb_y, mask);
            copy_conditional(pointToAdd + (uint32_t)8U, lut_cmb_z, mask);
          }
          point_double(q, q, buff);
          point_double(q, q, buff);
          point_double(q, q, buff);
          point_double(q, q, buff);
          point_add(pointToAdd, q, q, buff);
        }
      }
  }
  memcpy(result, q, (uint32_t)12U * sizeof (uint64_t));
}

static void
secretToPublicWithoutNorm(
  Spec_P256_montgomery_ladder_mode m,
  uint64_t *result,
  uint8_t *scalar,
  uint64_t *tempBuffer
)
{
  uint64_t *buff = tempBuffer + (uint32_t)12U;
  switch (m)
  {
    case Spec_P256_Ladder:
      {
        uint64_t basePoint[12U] = { 0U };
        basePoint[0U] = (uint64_t)8784043285714375740U;
        basePoint[1U] = (uint64_t)8483257759279461889U;
        basePoint[2U] = (uint64_t)8789745728267363600U;
        basePoint[3U] = (uint64_t)1770019616739251654U;
        basePoint[4U] = (uint64_t)15992936863339206154U;
        basePoint[5U] = (uint64_t)10037038012062884956U;
        basePoint[6U] = (uint64_t)15197544864945402661U;
        basePoint[7U] = (uint64_t)9615747158586711429U;
        basePoint[8U] = (uint64_t)1U;
        basePoint[9U] = (uint64_t)18446744069414584320U;
        basePoint[10U] = (uint64_t)18446744073709551615U;
        basePoint[11U] = (uint64_t)4294967294U;
        for (uint32_t i = (uint32_t)0U; i < (uint32_t)256U; i++)
        {
          uint32_t bit0 = (uint32_t)255U - i;
          uint64_t
          bit =
            (uint64_t)(scalar[(uint32_t)31U
            - bit0 / (uint32_t)8U]
            >> bit0 % (uint32_t)8U
            & (uint8_t)1U);
          cswap(bit, result, basePoint);
          point_add(result, basePoint, basePoint, buff);
          point_double(result, result, buff);
          cswap(bit, result, basePoint);
        }
        break;
      }
    case Spec_P256_Radix4:
      {
        uint32_t half0 = (uint32_t)(krml_checked_int_t)0 >> (uint32_t)1U;
        uint32_t word = (uint32_t)scalar[half0];
        uint32_t bitShift0 = (uint32_t)(krml_checked_int_t)0 & (uint32_t)1U;
        uint32_t bitShiftAsPrivate0 = bitShift0;
        uint64_t mask10 = (uint64_t)0U - (uint64_t)bitShiftAsPrivate0;
        uint32_t mask0 = (uint32_t)0xf0U ^ (uint32_t)(mask10 & (uint64_t)(uint32_t)255U);
        uint64_t mask11 = (uint64_t)0U - (uint64_t)bitShift0;
        uint32_t shiftMask = (uint32_t)0x4U ^ (uint32_t)(mask11 & (uint64_t)(uint32_t)4U);
        uint32_t result10 = word & mask0;
        uint32_t bits = result10 >> shiftMask;
        const uint64_t *pointToStart = points_radix_16 + bits * (uint32_t)8U;
        uint64_t *pointToStart1 = const_to_lbuffer__uint64_t(pointToStart);
        memcpy(result, pointToStart1, (uint32_t)8U * sizeof (uint64_t));
        result[8U] = (uint64_t)1U;
        result[9U] = (uint64_t)0U;
        result[10U] = (uint64_t)0U;
        result[11U] = (uint64_t)0U;
        for (uint32_t i0 = (uint32_t)1U; i0 < (uint32_t)64U; i0++)
        {
          uint32_t half = i0 >> (uint32_t)1U;
          uint32_t word0 = (uint32_t)scalar[half];
          uint32_t bitShift = i0 & (uint32_t)1U;
          uint32_t bitShiftAsPrivate = bitShift;
          uint64_t mask12 = (uint64_t)0U - (uint64_t)bitShiftAsPrivate;
          uint32_t mask2 = (uint32_t)0xf0U ^ (uint32_t)(mask12 & (uint64_t)(uint32_t)255U);
          uint64_t mask1 = (uint64_t)0U - (uint64_t)bitShift;
          uint32_t shiftMask0 = (uint32_t)0x4U ^ (uint32_t)(mask1 & (uint64_t)(uint32_t)4U);
          uint32_t result1 = word0 & mask2;
          uint32_t bits1 = result1 >> shiftMask0;
          uint64_t pointToAdd[8U] = { 0U };
          for (uint32_t i = (uint32_t)0U; i < (uint32_t)16U; i++)
          {
            uint64_t mask = FStar_UInt64_eq_mask((uint64_t)bits1, (uint64_t)i);
            uint64_t
            *lut_cmb_x =
              const_to_ilbuffer__uint64_t(points_radix_16 + i * (uint32_t)(krml_checked_int_t)8);
            uint64_t
            *lut_cmb_y =
              const_to_ilbuffer__uint64_t(points_radix_16
                + i * (uint32_t)(krml_checked_int_t)8 + (uint32_t)4U);
            copy_conditional(pointToAdd, lut_cmb_x, mask);
            copy_conditional(pointToAdd + (uint32_t)4U, lut_cmb_y, mask);
          }
          point_double(result, result, buff);
          point_double(result, result, buff);
          point_double(result, result, buff);
          point_double(result, result, buff);
          point_add_mixed(Lib_Buffer_MUT, result, (void *)pointToAdd, result, buff);
        }
        break;
      }
    default:
      {
        scalar_multiplication_cmb(result, (void *)scalar, buff);
      }
  }
}

static const
uint64_t
prime256order_buffer[4U] =
  {
    (uint64_t)17562291160714782033U,
    (uint64_t)13611842547513532036U,
    (uint64_t)18446744073709551615U,
    (uint64_t)18446744069414584320U
  };

static void reduction_prime_2prime_order(uint64_t *x, uint64_t *result)
{
  uint64_t tempBuffer[4U] = { 0U };
  uint64_t *r0 = tempBuffer;
  uint64_t *r1 = tempBuffer + (uint32_t)1U;
  uint64_t *r2 = tempBuffer + (uint32_t)2U;
  uint64_t *r3 = tempBuffer + (uint32_t)3U;
  uint64_t
  cc = Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U, x[0U], prime256order_buffer[0U], r0);
  uint64_t cc1 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, x[1U], prime256order_buffer[1U], r1);
  uint64_t
  cc2 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc1, x[2U], prime256order_buffer[2U], r2);
  uint64_t
  cc3 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc2, x[3U], prime256order_buffer[3U], r3);
  uint64_t c = cc3;
  cmovznz4(result, tempBuffer, x, c);
}

/*
   The input of the function is considered to be public,
thus this code is not secret independent with respect to the operations done over the input.
*/
static bool isPointAtInfinityPublic(uint64_t *p)
{
  uint64_t z0 = p[8U];
  uint64_t z1 = p[9U];
  uint64_t z2 = p[10U];
  uint64_t z3 = p[11U];
  bool z0_zero = z0 == (uint64_t)0U;
  bool z1_zero = z1 == (uint64_t)0U;
  bool z2_zero = z2 == (uint64_t)0U;
  bool z3_zero = z3 == (uint64_t)0U;
  return z0_zero && z1_zero && z2_zero && z3_zero;
}

/*
   The input of the function is considered to be public,
thus this code is not secret independent with respect to the operations done over the input.
*/
static bool isPointOnCurvePublic(uint64_t *p)
{
  uint64_t y2Buffer[4U] = { 0U };
  uint64_t xBuffer[4U] = { 0U };
  uint64_t *x = p;
  uint64_t *y = p + (uint32_t)4U;
  uint64_t multBuffer0[8U] = { 0U };
  multBuffer0[0U] = (uint64_t)0U;
  multBuffer0[1U] = (uint64_t)0U;
  multBuffer0[2U] = (uint64_t)0U;
  multBuffer0[3U] = (uint64_t)0U;
  multBuffer0[4U] = y[0U];
  multBuffer0[5U] = y[1U];
  multBuffer0[6U] = y[2U];
  multBuffer0[7U] = y[3U];
  solinas_reduction_impl(multBuffer0, y2Buffer);
  montgomery_square_buffer(y2Buffer, y2Buffer);
  uint64_t xToDomainBuffer[4U] = { 0U };
  uint64_t minusThreeXBuffer[4U] = { 0U };
  uint64_t p256_constant[4U] = { 0U };
  uint64_t multBuffer[8U] = { 0U };
  multBuffer[0U] = (uint64_t)0U;
  multBuffer[1U] = (uint64_t)0U;
  multBuffer[2U] = (uint64_t)0U;
  multBuffer[3U] = (uint64_t)0U;
  multBuffer[4U] = x[0U];
  multBuffer[5U] = x[1U];
  multBuffer[6U] = x[2U];
  multBuffer[7U] = x[3U];
  solinas_reduction_impl(multBuffer, xToDomainBuffer);
  montgomery_square_buffer(xToDomainBuffer, xBuffer);
  montgomery_multiplication_buffer(xBuffer, xToDomainBuffer, xBuffer);
  p256_add(xToDomainBuffer, xToDomainBuffer, minusThreeXBuffer);
  p256_add(xToDomainBuffer, minusThreeXBuffer, minusThreeXBuffer);
  p256_sub(xBuffer, minusThreeXBuffer, xBuffer);
  p256_constant[0U] = (uint64_t)15608596021259845087U;
  p256_constant[1U] = (uint64_t)12461466548982526096U;
  p256_constant[2U] = (uint64_t)16546823903870267094U;
  p256_constant[3U] = (uint64_t)15866188208926050356U;
  p256_add(xBuffer, p256_constant, xBuffer);
  uint64_t a_0 = y2Buffer[0U];
  uint64_t a_1 = y2Buffer[1U];
  uint64_t a_2 = y2Buffer[2U];
  uint64_t a_3 = y2Buffer[3U];
  uint64_t b_0 = xBuffer[0U];
  uint64_t b_1 = xBuffer[1U];
  uint64_t b_2 = xBuffer[2U];
  uint64_t b_3 = xBuffer[3U];
  uint64_t r_0 = FStar_UInt64_eq_mask(a_0, b_0);
  uint64_t r_1 = FStar_UInt64_eq_mask(a_1, b_1);
  uint64_t r_2 = FStar_UInt64_eq_mask(a_2, b_2);
  uint64_t r_3 = FStar_UInt64_eq_mask(a_3, b_3);
  uint64_t r01 = r_0 & r_1;
  uint64_t r23 = r_2 & r_3;
  uint64_t r1 = r01 & r23;
  uint64_t f = ~r1;
  bool r = f == (uint64_t)0U;
  return r;
}

static bool isCoordinateValid(uint64_t *p)
{
  uint64_t tempBuffer[4U] = { 0U };
  uint64_t *x = p;
  uint64_t *y = p + (uint32_t)4U;
  uint64_t *r00 = tempBuffer;
  uint64_t *r10 = tempBuffer + (uint32_t)1U;
  uint64_t *r20 = tempBuffer + (uint32_t)2U;
  uint64_t *r30 = tempBuffer + (uint32_t)3U;
  uint64_t
  cc0 = Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U, x[0U], prime256_buffer[0U], r00);
  uint64_t cc1 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc0, x[1U], prime256_buffer[1U], r10);
  uint64_t cc2 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc1, x[2U], prime256_buffer[2U], r20);
  uint64_t cc3 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc2, x[3U], prime256_buffer[3U], r30);
  uint64_t carryX = cc3;
  uint64_t *r0 = tempBuffer;
  uint64_t *r1 = tempBuffer + (uint32_t)1U;
  uint64_t *r2 = tempBuffer + (uint32_t)2U;
  uint64_t *r3 = tempBuffer + (uint32_t)3U;
  uint64_t
  cc = Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U, y[0U], prime256_buffer[0U], r0);
  uint64_t cc10 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, y[1U], prime256_buffer[1U], r1);
  uint64_t cc20 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc10, y[2U], prime256_buffer[2U], r2);
  uint64_t cc30 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc20, y[3U], prime256_buffer[3U], r3);
  uint64_t carryY = cc30;
  bool lessX = carryX == (uint64_t)1U;
  bool lessY = carryY == (uint64_t)1U;
  return lessX && lessY;
}

/*
   The input of the function is considered to be public,
thus this code is not secret independent with respect to the operations done over the input.
*/
static bool verifyQValidCurvePoint(uint64_t *pubKeyAsPoint)
{
  bool coordinatesValid = isCoordinateValid(pubKeyAsPoint);
  bool belongsToCurve = isPointOnCurvePublic(pubKeyAsPoint);
  return coordinatesValid && belongsToCurve;
}

static bool isMoreThanZeroLessThanOrder(uint8_t *x)
{
  uint64_t xAsFelem[4U] = { 0U };
  {
    uint64_t *os = xAsFelem;
    uint8_t *bj = x + (uint32_t)0U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x1 = r;
    os[0U] = x1;
  }
  {
    uint64_t *os = xAsFelem;
    uint8_t *bj = x + (uint32_t)1U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x1 = r;
    os[1U] = x1;
  }
  {
    uint64_t *os = xAsFelem;
    uint8_t *bj = x + (uint32_t)2U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x1 = r;
    os[2U] = x1;
  }
  {
    uint64_t *os = xAsFelem;
    uint8_t *bj = x + (uint32_t)3U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x1 = r;
    os[3U] = x1;
  }
  uint64_t zero = xAsFelem[0U];
  uint64_t one = xAsFelem[1U];
  uint64_t two = xAsFelem[2U];
  uint64_t three = xAsFelem[3U];
  xAsFelem[0U] = three;
  xAsFelem[1U] = two;
  xAsFelem[2U] = one;
  xAsFelem[3U] = zero;
  uint64_t tempBuffer[4U] = { 0U };
  uint64_t *r00 = tempBuffer;
  uint64_t *r10 = tempBuffer + (uint32_t)1U;
  uint64_t *r20 = tempBuffer + (uint32_t)2U;
  uint64_t *r30 = tempBuffer + (uint32_t)3U;
  uint64_t
  cc =
    Lib_IntTypes_Intrinsics_sub_borrow_u64((uint64_t)0U,
      xAsFelem[0U],
      prime256order_buffer[0U],
      r00);
  uint64_t
  cc1 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc, xAsFelem[1U], prime256order_buffer[1U], r10);
  uint64_t
  cc2 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc1, xAsFelem[2U], prime256order_buffer[2U], r20);
  uint64_t
  cc3 = Lib_IntTypes_Intrinsics_sub_borrow_u64(cc2, xAsFelem[3U], prime256order_buffer[3U], r30);
  uint64_t carry = cc3;
  uint64_t less = FStar_UInt64_eq_mask(carry, (uint64_t)1U);
  uint64_t a0 = xAsFelem[0U];
  uint64_t a1 = xAsFelem[1U];
  uint64_t a2 = xAsFelem[2U];
  uint64_t a3 = xAsFelem[3U];
  uint64_t r0 = FStar_UInt64_eq_mask(a0, (uint64_t)0U);
  uint64_t r1 = FStar_UInt64_eq_mask(a1, (uint64_t)0U);
  uint64_t r2 = FStar_UInt64_eq_mask(a2, (uint64_t)0U);
  uint64_t r3 = FStar_UInt64_eq_mask(a3, (uint64_t)0U);
  uint64_t r01 = r0 & r1;
  uint64_t r23 = r2 & r3;
  uint64_t more = r01 & r23;
  uint64_t notMore = ~more;
  uint64_t result = less & notMore;
  return ~result == (uint64_t)0U;
}

static void toFormPoint(uint8_t *i, uint64_t *p)
{
  uint8_t *pointScalarX = i;
  uint8_t *pointScalarY = i + (uint32_t)32U;
  uint64_t *pointX = p;
  uint64_t *pointY = p + (uint32_t)4U;
  uint64_t *pointZ = p + (uint32_t)8U;
  {
    uint64_t *os = pointX;
    uint8_t *bj = pointScalarX + (uint32_t)0U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x = r;
    os[0U] = x;
  }
  {
    uint64_t *os = pointX;
    uint8_t *bj = pointScalarX + (uint32_t)1U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x = r;
    os[1U] = x;
  }
  {
    uint64_t *os = pointX;
    uint8_t *bj = pointScalarX + (uint32_t)2U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x = r;
    os[2U] = x;
  }
  {
    uint64_t *os = pointX;
    uint8_t *bj = pointScalarX + (uint32_t)3U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x = r;
    os[3U] = x;
  }
  uint64_t zero = pointX[0U];
  uint64_t one0 = pointX[1U];
  uint64_t two0 = pointX[2U];
  uint64_t three0 = pointX[3U];
  pointX[0U] = three0;
  pointX[1U] = two0;
  pointX[2U] = one0;
  pointX[3U] = zero;
  {
    uint64_t *os = pointY;
    uint8_t *bj = pointScalarY + (uint32_t)0U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x = r;
    os[0U] = x;
  }
  {
    uint64_t *os = pointY;
    uint8_t *bj = pointScalarY + (uint32_t)1U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x = r;
    os[1U] = x;
  }
  {
    uint64_t *os = pointY;
    uint8_t *bj = pointScalarY + (uint32_t)2U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x = r;
    os[2U] = x;
  }
  {
    uint64_t *os = pointY;
    uint8_t *bj = pointScalarY + (uint32_t)3U * (uint32_t)8U;
    uint64_t u = load64_be(bj);
    uint64_t r = u;
    uint64_t x = r;
    os[3U] = x;
  }
  uint64_t zero0 = pointY[0U];
  uint64_t one = pointY[1U];
  uint64_t two = pointY[2U];
  uint64_t three = pointY[3U];
  pointY[0U] = three;
  pointY[1U] = two;
  pointY[2U] = one;
  pointY[3U] = zero0;
  pointZ[0U] = (uint64_t)1U;
  pointZ[1U] = (uint64_t)0U;
  pointZ[2U] = (uint64_t)0U;
  pointZ[3U] = (uint64_t)0U;
}

static void fromFormPoint(uint64_t *i, uint8_t *o)
{
  uint64_t *pointX = i;
  uint64_t *pointY = i + (uint32_t)4U;
  uint8_t *pointScalarX = o;
  uint8_t *pointScalarY = o + (uint32_t)32U;
  uint64_t zero = pointX[0U];
  uint64_t one0 = pointX[1U];
  uint64_t two0 = pointX[2U];
  uint64_t three0 = pointX[3U];
  pointX[0U] = three0;
  pointX[1U] = two0;
  pointX[2U] = one0;
  pointX[3U] = zero;
  {
    store64_be(pointScalarX + (uint32_t)0U * (uint32_t)8U, pointX[0U]);
  }
  {
    store64_be(pointScalarX + (uint32_t)1U * (uint32_t)8U, pointX[1U]);
  }
  {
    store64_be(pointScalarX + (uint32_t)2U * (uint32_t)8U, pointX[2U]);
  }
  {
    store64_be(pointScalarX + (uint32_t)3U * (uint32_t)8U, pointX[3U]);
  }
  uint64_t zero0 = pointY[0U];
  uint64_t one = pointY[1U];
  uint64_t two = pointY[2U];
  uint64_t three = pointY[3U];
  pointY[0U] = three;
  pointY[1U] = two;
  pointY[2U] = one;
  pointY[3U] = zero0;
  {
    store64_be(pointScalarY + (uint32_t)0U * (uint32_t)8U, pointY[0U]);
  }
  {
    store64_be(pointScalarY + (uint32_t)1U * (uint32_t)8U, pointY[1U]);
  }
  {
    store64_be(pointScalarY + (uint32_t)2U * (uint32_t)8U, pointY[2U]);
  }
  {
    store64_be(pointScalarY + (uint32_t)3U * (uint32_t)8U, pointY[3U]);
  }
}

/*
 Input: result: uint8[64], 
 scalar: uint8[32].
  
 Output: bool, where True stands for the correct key generation. 
  
 False means that an error has occurred (possibly that the result respresents point at infinity). 
  
*/
bool Hacl_P256_ecp256dh_i_ladder(uint8_t *result, uint8_t *scalar)
{
  uint64_t tempBuffer[100U] = { 0U };
  uint64_t resultBuffer[12U] = { 0U };
  uint64_t *buff = tempBuffer + (uint32_t)12U;
  uint64_t *basePoint = tempBuffer;
  basePoint[0U] = (uint64_t)8784043285714375740U;
  basePoint[1U] = (uint64_t)8483257759279461889U;
  basePoint[2U] = (uint64_t)8789745728267363600U;
  basePoint[3U] = (uint64_t)1770019616739251654U;
  basePoint[4U] = (uint64_t)15992936863339206154U;
  basePoint[5U] = (uint64_t)10037038012062884956U;
  basePoint[6U] = (uint64_t)15197544864945402661U;
  basePoint[7U] = (uint64_t)9615747158586711429U;
  basePoint[8U] = (uint64_t)1U;
  basePoint[9U] = (uint64_t)18446744069414584320U;
  basePoint[10U] = (uint64_t)18446744073709551615U;
  basePoint[11U] = (uint64_t)4294967294U;
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)256U; i++)
  {
    uint32_t bit0 = (uint32_t)255U - i;
    uint64_t
    bit =
      (uint64_t)(scalar[(uint32_t)31U - bit0 / (uint32_t)8U] >> bit0 % (uint32_t)8U & (uint8_t)1U);
    cswap(bit, resultBuffer, basePoint);
    point_add(resultBuffer, basePoint, basePoint, buff);
    point_double(resultBuffer, resultBuffer, buff);
    cswap(bit, resultBuffer, basePoint);
  }
  norm(resultBuffer, resultBuffer, buff);
  uint64_t flag = isPointAtInfinityPrivate(resultBuffer);
  fromFormPoint(resultBuffer, result);
  return flag == (uint64_t)0U;
}

/*
 Input: result: uint8[64], 
 scalar: uint8[32].
  
 Output: bool, where True stands for the correct key generation. 
  
 False means that an error has occurred (possibly that the result respresents point at infinity). 
  
*/
bool Hacl_P256_ecp256dh_i_radix4(uint8_t *result, uint8_t *scalar)
{
  uint64_t tempBuffer[100U] = { 0U };
  uint64_t resultBuffer[12U] = { 0U };
  uint64_t *buff = tempBuffer + (uint32_t)12U;
  uint32_t half0 = (uint32_t)(krml_checked_int_t)0 >> (uint32_t)1U;
  uint32_t word = (uint32_t)scalar[half0];
  uint32_t bitShift0 = (uint32_t)(krml_checked_int_t)0 & (uint32_t)1U;
  uint32_t bitShiftAsPrivate0 = bitShift0;
  uint64_t mask10 = (uint64_t)0U - (uint64_t)bitShiftAsPrivate0;
  uint32_t mask0 = (uint32_t)0xf0U ^ (uint32_t)(mask10 & (uint64_t)(uint32_t)255U);
  uint64_t mask11 = (uint64_t)0U - (uint64_t)bitShift0;
  uint32_t shiftMask = (uint32_t)0x4U ^ (uint32_t)(mask11 & (uint64_t)(uint32_t)4U);
  uint32_t result10 = word & mask0;
  uint32_t bits = result10 >> shiftMask;
  const uint64_t *pointToStart = points_radix_16 + bits * (uint32_t)8U;
  uint64_t *pointToStart1 = const_to_lbuffer__uint64_t(pointToStart);
  memcpy(resultBuffer, pointToStart1, (uint32_t)8U * sizeof (uint64_t));
  resultBuffer[8U] = (uint64_t)1U;
  resultBuffer[9U] = (uint64_t)0U;
  resultBuffer[10U] = (uint64_t)0U;
  resultBuffer[11U] = (uint64_t)0U;
  for (uint32_t i0 = (uint32_t)1U; i0 < (uint32_t)64U; i0++)
  {
    uint32_t half = i0 >> (uint32_t)1U;
    uint32_t word0 = (uint32_t)scalar[half];
    uint32_t bitShift = i0 & (uint32_t)1U;
    uint32_t bitShiftAsPrivate = bitShift;
    uint64_t mask12 = (uint64_t)0U - (uint64_t)bitShiftAsPrivate;
    uint32_t mask2 = (uint32_t)0xf0U ^ (uint32_t)(mask12 & (uint64_t)(uint32_t)255U);
    uint64_t mask1 = (uint64_t)0U - (uint64_t)bitShift;
    uint32_t shiftMask0 = (uint32_t)0x4U ^ (uint32_t)(mask1 & (uint64_t)(uint32_t)4U);
    uint32_t result1 = word0 & mask2;
    uint32_t bits1 = result1 >> shiftMask0;
    uint64_t pointToAdd[8U] = { 0U };
    for (uint32_t i = (uint32_t)0U; i < (uint32_t)16U; i++)
    {
      uint64_t mask = FStar_UInt64_eq_mask((uint64_t)bits1, (uint64_t)i);
      uint64_t
      *lut_cmb_x =
        const_to_ilbuffer__uint64_t(points_radix_16 + i * (uint32_t)(krml_checked_int_t)8);
      uint64_t
      *lut_cmb_y =
        const_to_ilbuffer__uint64_t(points_radix_16
          + i * (uint32_t)(krml_checked_int_t)8 + (uint32_t)4U);
      copy_conditional(pointToAdd, lut_cmb_x, mask);
      copy_conditional(pointToAdd + (uint32_t)4U, lut_cmb_y, mask);
    }
    point_double(resultBuffer, resultBuffer, buff);
    point_double(resultBuffer, resultBuffer, buff);
    point_double(resultBuffer, resultBuffer, buff);
    point_double(resultBuffer, resultBuffer, buff);
    point_add_mixed(Lib_Buffer_MUT, resultBuffer, (void *)pointToAdd, resultBuffer, buff);
  }
  norm(resultBuffer, resultBuffer, buff);
  uint64_t flag = isPointAtInfinityPrivate(resultBuffer);
  fromFormPoint(resultBuffer, result);
  return flag == (uint64_t)0U;
}

/*
 Input: result: uint8[64], 
 scalar: uint8[32].
  
 Output: bool, where True stands for the correct key generation. 
  
 False means that an error has occurred (possibly that the result respresents point at infinity). 
  
*/
bool Hacl_P256_ecp256dh_i_cmb(uint8_t *result, uint8_t *scalar)
{
  uint64_t tempBuffer[100U] = { 0U };
  uint64_t resultBuffer[12U] = { 0U };
  uint64_t *buff = tempBuffer + (uint32_t)12U;
  scalar_multiplication_cmb(resultBuffer, (void *)scalar, buff);
  norm(resultBuffer, resultBuffer, buff);
  uint64_t flag = isPointAtInfinityPrivate(resultBuffer);
  fromFormPoint(resultBuffer, result);
  return flag == (uint64_t)0U;
}

/*
 
   The pub(lic)_key input of the function is considered to be public, 
  thus this code is not secret independent with respect to the operations done over this variable.
  
 Input: result: uint8[64], 
 pub(lic)Key: uint8[64], 
 scalar: uint8[32].
  
 Output: bool, where True stands for the correct key generation. False value means that an error has occurred (possibly the provided public key was incorrect or the result represents point at infinity). 
  
*/
bool Hacl_P256_ecp256dh_r_ladder(uint8_t *result, uint8_t *pubKey, uint8_t *scalar)
{
  uint64_t resultBufferFelem[12U] = { 0U };
  uint64_t publicKeyAsFelem[12U] = { 0U };
  uint64_t tempBuffer[100U] = { 0U };
  toFormPoint(pubKey, publicKeyAsFelem);
  bool publicKeyCorrect = verifyQValidCurvePoint(publicKeyAsFelem);
  uint64_t flag;
  if (publicKeyCorrect)
  {
    scalarMultiplicationL(Spec_P256_Ladder,
      publicKeyAsFelem,
      resultBufferFelem,
      scalar,
      tempBuffer);
    flag = isPointAtInfinityPrivate(resultBufferFelem);
  }
  else
  {
    flag = (uint64_t)18446744073709551615U;
  }
  fromFormPoint(resultBufferFelem, result);
  return flag == (uint64_t)0U;
}

/*
 
   The pub(lic)_key input of the function is considered to be public, 
  thus this code is not secret independent with respect to the operations done over this variable.
  
 Input: result: uint8[64], 
 pub(lic)Key: uint8[64], 
 scalar: uint8[32].
  
 Output: bool, where True stands for the correct key generation. False value means that an error has occurred (possibly the provided public key was incorrect or the result represents point at infinity). 
  
*/
bool Hacl_P256_ecp256dh_r_radix4(uint8_t *result, uint8_t *pubKey, uint8_t *scalar)
{
  uint64_t resultBufferFelem[12U] = { 0U };
  uint64_t publicKeyAsFelem[12U] = { 0U };
  uint64_t tempBuffer[100U] = { 0U };
  toFormPoint(pubKey, publicKeyAsFelem);
  bool publicKeyCorrect = verifyQValidCurvePoint(publicKeyAsFelem);
  uint64_t flag;
  if (publicKeyCorrect)
  {
    scalarMultiplicationL(Spec_P256_Radix4,
      publicKeyAsFelem,
      resultBufferFelem,
      scalar,
      tempBuffer);
    flag = isPointAtInfinityPrivate(resultBufferFelem);
  }
  else
  {
    flag = (uint64_t)18446744073709551615U;
  }
  fromFormPoint(resultBufferFelem, result);
  return flag == (uint64_t)0U;
}

/*
 
   The pub(lic)_key input of the function is considered to be public, 
  thus this code is not secret independent with respect to the operations done over this variable.
  
 Input: result: uint8[64], 
 pub(lic)Key: uint8[64], 
 scalar: uint8[32].
  
 Output: bool, where True stands for the correct key generation. False value means that an error has occurred (possibly the provided public key was incorrect or the result represents point at infinity). 
  
*/
bool Hacl_P256_ecp256dh_r_comb(uint8_t *result, uint8_t *pubKey, uint8_t *scalar)
{
  uint64_t resultBufferFelem[12U] = { 0U };
  uint64_t publicKeyAsFelem[12U] = { 0U };
  uint64_t tempBuffer[100U] = { 0U };
  toFormPoint(pubKey, publicKeyAsFelem);
  bool publicKeyCorrect = verifyQValidCurvePoint(publicKeyAsFelem);
  uint64_t flag;
  if (publicKeyCorrect)
  {
    scalarMultiplicationL(Spec_P256_Comb, publicKeyAsFelem, resultBufferFelem, scalar, tempBuffer);
    flag = isPointAtInfinityPrivate(resultBufferFelem);
  }
  else
  {
    flag = (uint64_t)18446744073709551615U;
  }
  fromFormPoint(resultBufferFelem, result);
  return flag == (uint64_t)0U;
}

bool Hacl_P256_points_op(uint8_t *result, uint8_t *publicKey, uint8_t *u1, uint8_t *u2)
{
  uint64_t tempBuffer[100U] = { 0U };
  uint64_t publicKeyAsFelem[12U] = { 0U };
  uint64_t resultFelem[4U] = { 0U };
  toFormPoint(publicKey, publicKeyAsFelem);
  uint64_t pointSum[12U] = { 0U };
  uint64_t points[24U] = { 0U };
  uint64_t *buff = tempBuffer + (uint32_t)12U;
  uint64_t *pointU1G = points;
  uint64_t *pointU2Q0 = points + (uint32_t)12U;
  secretToPublicWithoutNorm(Spec_P256_Comb, pointU1G, u1, tempBuffer);
  scalarMultiplicationWithoutNorm(Spec_P256_Radix4, publicKeyAsFelem, pointU2Q0, u2, tempBuffer);
  uint64_t *pointU1G0 = points;
  uint64_t *pointU2Q = points + (uint32_t)12U;
  uint64_t tmp[112U] = { 0U };
  uint64_t *tmpForNorm = tmp;
  uint64_t *result0Norm = tmp + (uint32_t)88U;
  uint64_t *result1Norm = tmp + (uint32_t)100U;
  uint64_t *pointU1G1 = points;
  uint64_t *pointU2Q1 = points + (uint32_t)12U;
  norm(pointU1G1, result0Norm, tmpForNorm);
  norm(pointU2Q1, result1Norm, tmpForNorm);
  uint64_t *x0 = result0Norm;
  uint64_t *y0 = result0Norm + (uint32_t)4U;
  uint64_t *z0 = result0Norm + (uint32_t)8U;
  uint64_t *x1 = result1Norm;
  uint64_t *y1 = result1Norm + (uint32_t)4U;
  uint64_t *z1 = result1Norm + (uint32_t)8U;
  uint64_t a_00 = x0[0U];
  uint64_t a_10 = x0[1U];
  uint64_t a_20 = x0[2U];
  uint64_t a_30 = x0[3U];
  uint64_t b_00 = x1[0U];
  uint64_t b_10 = x1[1U];
  uint64_t b_20 = x1[2U];
  uint64_t b_30 = x1[3U];
  uint64_t r_00 = FStar_UInt64_eq_mask(a_00, b_00);
  uint64_t r_10 = FStar_UInt64_eq_mask(a_10, b_10);
  uint64_t r_20 = FStar_UInt64_eq_mask(a_20, b_20);
  uint64_t r_30 = FStar_UInt64_eq_mask(a_30, b_30);
  uint64_t r010 = r_00 & r_10;
  uint64_t r230 = r_20 & r_30;
  uint64_t r1 = r010 & r230;
  uint64_t f = ~r1;
  bool xEqual = f == (uint64_t)0U;
  uint64_t a_01 = y0[0U];
  uint64_t a_11 = y0[1U];
  uint64_t a_21 = y0[2U];
  uint64_t a_31 = y0[3U];
  uint64_t b_01 = y1[0U];
  uint64_t b_11 = y1[1U];
  uint64_t b_21 = y1[2U];
  uint64_t b_31 = y1[3U];
  uint64_t r_01 = FStar_UInt64_eq_mask(a_01, b_01);
  uint64_t r_11 = FStar_UInt64_eq_mask(a_11, b_11);
  uint64_t r_21 = FStar_UInt64_eq_mask(a_21, b_21);
  uint64_t r_31 = FStar_UInt64_eq_mask(a_31, b_31);
  uint64_t r011 = r_01 & r_11;
  uint64_t r231 = r_21 & r_31;
  uint64_t r10 = r011 & r231;
  uint64_t f0 = ~r10;
  bool yEqual = f0 == (uint64_t)0U;
  uint64_t a_0 = z0[0U];
  uint64_t a_1 = z0[1U];
  uint64_t a_2 = z0[2U];
  uint64_t a_3 = z0[3U];
  uint64_t b_0 = z1[0U];
  uint64_t b_1 = z1[1U];
  uint64_t b_2 = z1[2U];
  uint64_t b_3 = z1[3U];
  uint64_t r_0 = FStar_UInt64_eq_mask(a_0, b_0);
  uint64_t r_1 = FStar_UInt64_eq_mask(a_1, b_1);
  uint64_t r_2 = FStar_UInt64_eq_mask(a_2, b_2);
  uint64_t r_3 = FStar_UInt64_eq_mask(a_3, b_3);
  uint64_t r01 = r_0 & r_1;
  uint64_t r23 = r_2 & r_3;
  uint64_t r11 = r01 & r23;
  uint64_t f1 = ~r11;
  bool zEqual = f1 == (uint64_t)0U;
  bool equalX1 = xEqual && yEqual && zEqual;
  bool equalX = equalX1;
  if (equalX)
  {
    point_double(pointU1G0, pointSum, buff);
  }
  else
  {
    point_add(pointU1G0, pointU2Q, pointSum, buff);
  }
  norm(pointSum, pointSum, buff);
  bool resultIsPAI = isPointAtInfinityPublic(pointSum);
  uint64_t *xCoordinateSum = pointSum;
  memcpy(resultFelem, xCoordinateSum, (uint32_t)4U * sizeof (uint64_t));
  reduction_prime_2prime_order(resultFelem, resultFelem);
  bool r = !resultIsPAI;
  uint64_t zero = resultFelem[0U];
  uint64_t one = resultFelem[1U];
  uint64_t two = resultFelem[2U];
  uint64_t three = resultFelem[3U];
  resultFelem[0U] = three;
  resultFelem[1U] = two;
  resultFelem[2U] = one;
  resultFelem[3U] = zero;
  {
    store64_be(result + (uint32_t)0U * (uint32_t)8U, resultFelem[0U]);
  }
  {
    store64_be(result + (uint32_t)1U * (uint32_t)8U, resultFelem[1U]);
  }
  {
    store64_be(result + (uint32_t)2U * (uint32_t)8U, resultFelem[2U]);
  }
  {
    store64_be(result + (uint32_t)3U * (uint32_t)8U, resultFelem[3U]);
  }
  return r;
}

/*
 Input: scalar: uint8[32].
  
 Output: bool, where true stands for the scalar to be more than 0 and less than order.
*/
bool Hacl_P256_is_more_than_zero_less_than_order(uint8_t *x)
{
  return isMoreThanZeroLessThanOrder(x);
}
