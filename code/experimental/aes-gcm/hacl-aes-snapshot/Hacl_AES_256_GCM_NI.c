/* 
  This file was generated by KreMLin <https://github.com/FStarLang/kremlin>
  KreMLin invocation: /Users/bhargava/Desktop/repositories/kremlin//krml -funroll-loops 8 -warn-error +9 -I /Users/bhargava/Desktop/repositories/hacl-star-dev//lib -I /Users/bhargava/Desktop/repositories/kremlin//kremlib -I /Users/bhargava/Desktop/repositories/hacl-star-dev//specs -I . -I /Users/bhargava/Desktop/repositories/hacl-star-dev//code/experimental/aes -I /Users/bhargava/Desktop/repositories/hacl-star-dev//code/experimental/gf128 -ccopt -march=native -fbuiltin-uint128 -drop FStar.UInt128 -fnocompound-literals -fparentheses -fcurly-braces -tmpdir aesgcm-aes256-ni-c aesgcm-aes256-ni-c/out.krml -skip-compilation -minimal -add-include "kremlib.h" -add-include "vec128.h" -drop Lib.Vec128 -bundle Hacl.AES_256_GCM.NI=*
  F* version: 7c70b890
  KreMLin version: b511d90c
 */

#include "Hacl_AES_256_GCM_NI.h"

static void Hacl_Impl_Gf128_FieldNI_fadd(Lib_Vec128_vec128 *x, Lib_Vec128_vec128 *y)
{
  x[0U] = Lib_Vec128_vec128_xor(x[0U], y[0U]);
}

static void Hacl_Impl_Gf128_FieldNI_fmul(Lib_Vec128_vec128 *x, Lib_Vec128_vec128 *y)
{
  Lib_Vec128_vec128 xe = x[0U];
  Lib_Vec128_vec128 ye = y[0U];
  Lib_Vec128_vec128 lo0 = Lib_Vec128_ni_clmul(xe, ye, (uint8_t)0x00U);
  Lib_Vec128_vec128 m1 = Lib_Vec128_ni_clmul(xe, ye, (uint8_t)0x10U);
  Lib_Vec128_vec128 m2 = Lib_Vec128_ni_clmul(xe, ye, (uint8_t)0x01U);
  Lib_Vec128_vec128 hi = Lib_Vec128_ni_clmul(xe, ye, (uint8_t)0x11U);
  Lib_Vec128_vec128 m11 = Lib_Vec128_vec128_xor(m1, m2);
  Lib_Vec128_vec128 m21 = Lib_Vec128_vec128_shift_left(m11, (uint32_t)64U);
  Lib_Vec128_vec128 m12 = Lib_Vec128_vec128_shift_right(m11, (uint32_t)64U);
  Lib_Vec128_vec128 lo10 = Lib_Vec128_vec128_xor(lo0, m21);
  Lib_Vec128_vec128 hi10 = Lib_Vec128_vec128_xor(hi, m12);
  Lib_Vec128_vec128 hi0 = hi10;
  Lib_Vec128_vec128 lo = lo10;
  Lib_Vec128_vec128 lo1 = Lib_Vec128_vec128_shift_right64(lo, (uint32_t)63U);
  Lib_Vec128_vec128 lo2 = Lib_Vec128_vec128_shift_left(lo1, (uint32_t)64U);
  Lib_Vec128_vec128 lo3 = Lib_Vec128_vec128_shift_left64(lo, (uint32_t)1U);
  Lib_Vec128_vec128 lo31 = Lib_Vec128_vec128_xor(lo3, lo2);
  Lib_Vec128_vec128 hi1 = Lib_Vec128_vec128_shift_right64(hi0, (uint32_t)63U);
  Lib_Vec128_vec128 hi11 = Lib_Vec128_vec128_shift_left(hi1, (uint32_t)64U);
  Lib_Vec128_vec128 hi2 = Lib_Vec128_vec128_shift_left64(hi0, (uint32_t)1U);
  Lib_Vec128_vec128 hi21 = Lib_Vec128_vec128_xor(hi2, hi11);
  Lib_Vec128_vec128 lo11 = Lib_Vec128_vec128_shift_right64(lo, (uint32_t)63U);
  Lib_Vec128_vec128 lo12 = Lib_Vec128_vec128_shift_right(lo11, (uint32_t)64U);
  Lib_Vec128_vec128 hi22 = Lib_Vec128_vec128_xor(hi21, lo12);
  Lib_Vec128_vec128 lo4 = lo31;
  Lib_Vec128_vec128 hi3 = hi22;
  Lib_Vec128_vec128 lo13 = Lib_Vec128_vec128_shift_left64(lo4, (uint32_t)63U);
  Lib_Vec128_vec128 lo21 = Lib_Vec128_vec128_shift_left64(lo4, (uint32_t)62U);
  Lib_Vec128_vec128 lo32 = Lib_Vec128_vec128_shift_left64(lo4, (uint32_t)57U);
  Lib_Vec128_vec128 lo14 = Lib_Vec128_vec128_xor(lo13, lo21);
  Lib_Vec128_vec128 lo15 = Lib_Vec128_vec128_xor(lo14, lo32);
  Lib_Vec128_vec128 lo22 = Lib_Vec128_vec128_shift_right(lo15, (uint32_t)64U);
  Lib_Vec128_vec128 lo33 = Lib_Vec128_vec128_shift_left(lo15, (uint32_t)64U);
  Lib_Vec128_vec128 lo5 = Lib_Vec128_vec128_xor(lo4, lo33);
  Lib_Vec128_vec128 lo_ = lo22;
  Lib_Vec128_vec128 lo16 = Lib_Vec128_vec128_shift_right64(lo5, (uint32_t)1U);
  Lib_Vec128_vec128 lo23 = Lib_Vec128_vec128_shift_right64(lo5, (uint32_t)2U);
  Lib_Vec128_vec128 lo34 = Lib_Vec128_vec128_shift_right64(lo5, (uint32_t)7U);
  Lib_Vec128_vec128 lo17 = Lib_Vec128_vec128_xor(lo16, lo23);
  Lib_Vec128_vec128 lo18 = Lib_Vec128_vec128_xor(lo17, lo34);
  Lib_Vec128_vec128 lo19 = Lib_Vec128_vec128_xor(lo18, lo_);
  Lib_Vec128_vec128 lo6 = Lib_Vec128_vec128_xor(lo5, lo19);
  Lib_Vec128_vec128 lo7 = Lib_Vec128_vec128_xor(lo6, hi3);
  Lib_Vec128_vec128 lo110 = lo7;
  x[0U] = lo110;
}

static void Hacl_Impl_Gf128_FieldNI_load_precompute_r(Lib_Vec128_vec128 *pre, uint8_t *key)
{
  Lib_Vec128_vec128 *r_4 = pre;
  Lib_Vec128_vec128 *r_3 = pre + (uint32_t)1U;
  Lib_Vec128_vec128 *r_2 = pre + (uint32_t)2U;
  Lib_Vec128_vec128 *r = pre + (uint32_t)3U;
  r[0U] = Lib_Vec128_vec128_load_be(key);
  pre[0U] = r[0U];
  pre[1U] = r[0U];
  pre[2U] = r[0U];
  Hacl_Impl_Gf128_FieldNI_fmul(r_2, r);
  Hacl_Impl_Gf128_FieldNI_fmul(r_3, r_2);
  Hacl_Impl_Gf128_FieldNI_fmul(r_4, r_3);
}

static void
Hacl_Impl_Gf128_FieldNI_fadd_mul4(
  Lib_Vec128_vec128 *acc,
  Lib_Vec128_vec128 *x,
  Lib_Vec128_vec128 *pre
)
{
  Lib_Vec128_vec128 x1 = x[0U];
  Lib_Vec128_vec128 x2 = x[1U];
  Lib_Vec128_vec128 x3 = x[2U];
  Lib_Vec128_vec128 x4 = x[3U];
  Lib_Vec128_vec128 y1 = pre[0U];
  Lib_Vec128_vec128 y2 = pre[1U];
  Lib_Vec128_vec128 y3 = pre[2U];
  Lib_Vec128_vec128 y4 = pre[3U];
  Lib_Vec128_vec128 acc0 = acc[0U];
  Lib_Vec128_vec128 acc01 = Lib_Vec128_vec128_xor(acc0, x1);
  Lib_Vec128_vec128 lo10 = Lib_Vec128_ni_clmul(acc01, y1, (uint8_t)0x00U);
  Lib_Vec128_vec128 lo2 = Lib_Vec128_ni_clmul(x2, y2, (uint8_t)0x00U);
  Lib_Vec128_vec128 lo30 = Lib_Vec128_ni_clmul(x3, y3, (uint8_t)0x00U);
  Lib_Vec128_vec128 lo40 = Lib_Vec128_ni_clmul(x4, y4, (uint8_t)0x00U);
  Lib_Vec128_vec128 lo0 = Lib_Vec128_vec128_xor(lo10, lo2);
  Lib_Vec128_vec128 lo5 = Lib_Vec128_vec128_xor(lo0, lo30);
  Lib_Vec128_vec128 lo6 = Lib_Vec128_vec128_xor(lo5, lo40);
  Lib_Vec128_vec128 m1 = Lib_Vec128_ni_clmul(acc01, y1, (uint8_t)0x10U);
  Lib_Vec128_vec128 m2 = Lib_Vec128_ni_clmul(x2, y2, (uint8_t)0x10U);
  Lib_Vec128_vec128 m3 = Lib_Vec128_ni_clmul(x3, y3, (uint8_t)0x10U);
  Lib_Vec128_vec128 m4 = Lib_Vec128_ni_clmul(x4, y4, (uint8_t)0x10U);
  Lib_Vec128_vec128 m = Lib_Vec128_vec128_xor(m1, m2);
  Lib_Vec128_vec128 m5 = Lib_Vec128_vec128_xor(m, m3);
  Lib_Vec128_vec128 m6 = Lib_Vec128_vec128_xor(m5, m4);
  Lib_Vec128_vec128 m11 = Lib_Vec128_ni_clmul(acc01, y1, (uint8_t)0x01U);
  Lib_Vec128_vec128 m21 = Lib_Vec128_ni_clmul(x2, y2, (uint8_t)0x01U);
  Lib_Vec128_vec128 m31 = Lib_Vec128_ni_clmul(x3, y3, (uint8_t)0x01U);
  Lib_Vec128_vec128 m41 = Lib_Vec128_ni_clmul(x4, y4, (uint8_t)0x01U);
  Lib_Vec128_vec128 m7 = Lib_Vec128_vec128_xor(m6, m11);
  Lib_Vec128_vec128 m8 = Lib_Vec128_vec128_xor(m7, m21);
  Lib_Vec128_vec128 m9 = Lib_Vec128_vec128_xor(m8, m31);
  Lib_Vec128_vec128 m10 = Lib_Vec128_vec128_xor(m9, m41);
  Lib_Vec128_vec128 hi10 = Lib_Vec128_ni_clmul(acc01, y1, (uint8_t)0x11U);
  Lib_Vec128_vec128 hi20 = Lib_Vec128_ni_clmul(x2, y2, (uint8_t)0x11U);
  Lib_Vec128_vec128 hi30 = Lib_Vec128_ni_clmul(x3, y3, (uint8_t)0x11U);
  Lib_Vec128_vec128 hi4 = Lib_Vec128_ni_clmul(x4, y4, (uint8_t)0x11U);
  Lib_Vec128_vec128 hi = Lib_Vec128_vec128_xor(hi10, hi20);
  Lib_Vec128_vec128 hi5 = Lib_Vec128_vec128_xor(hi, hi30);
  Lib_Vec128_vec128 hi6 = Lib_Vec128_vec128_xor(hi5, hi4);
  Lib_Vec128_vec128 m12 = Lib_Vec128_vec128_shift_left(m10, (uint32_t)64U);
  Lib_Vec128_vec128 m22 = Lib_Vec128_vec128_shift_right(m10, (uint32_t)64U);
  Lib_Vec128_vec128 lo7 = Lib_Vec128_vec128_xor(lo6, m12);
  Lib_Vec128_vec128 hi7 = Lib_Vec128_vec128_xor(hi6, m22);
  Lib_Vec128_vec128 hi0 = hi7;
  Lib_Vec128_vec128 lo = lo7;
  Lib_Vec128_vec128 lo1 = Lib_Vec128_vec128_shift_right64(lo, (uint32_t)63U);
  Lib_Vec128_vec128 lo20 = Lib_Vec128_vec128_shift_left(lo1, (uint32_t)64U);
  Lib_Vec128_vec128 lo3 = Lib_Vec128_vec128_shift_left64(lo, (uint32_t)1U);
  Lib_Vec128_vec128 lo31 = Lib_Vec128_vec128_xor(lo3, lo20);
  Lib_Vec128_vec128 hi1 = Lib_Vec128_vec128_shift_right64(hi0, (uint32_t)63U);
  Lib_Vec128_vec128 hi11 = Lib_Vec128_vec128_shift_left(hi1, (uint32_t)64U);
  Lib_Vec128_vec128 hi2 = Lib_Vec128_vec128_shift_left64(hi0, (uint32_t)1U);
  Lib_Vec128_vec128 hi21 = Lib_Vec128_vec128_xor(hi2, hi11);
  Lib_Vec128_vec128 lo11 = Lib_Vec128_vec128_shift_right64(lo, (uint32_t)63U);
  Lib_Vec128_vec128 lo12 = Lib_Vec128_vec128_shift_right(lo11, (uint32_t)64U);
  Lib_Vec128_vec128 hi22 = Lib_Vec128_vec128_xor(hi21, lo12);
  Lib_Vec128_vec128 lo4 = lo31;
  Lib_Vec128_vec128 hi3 = hi22;
  Lib_Vec128_vec128 lo13 = Lib_Vec128_vec128_shift_left64(lo4, (uint32_t)63U);
  Lib_Vec128_vec128 lo21 = Lib_Vec128_vec128_shift_left64(lo4, (uint32_t)62U);
  Lib_Vec128_vec128 lo32 = Lib_Vec128_vec128_shift_left64(lo4, (uint32_t)57U);
  Lib_Vec128_vec128 lo14 = Lib_Vec128_vec128_xor(lo13, lo21);
  Lib_Vec128_vec128 lo15 = Lib_Vec128_vec128_xor(lo14, lo32);
  Lib_Vec128_vec128 lo22 = Lib_Vec128_vec128_shift_right(lo15, (uint32_t)64U);
  Lib_Vec128_vec128 lo33 = Lib_Vec128_vec128_shift_left(lo15, (uint32_t)64U);
  Lib_Vec128_vec128 lo50 = Lib_Vec128_vec128_xor(lo4, lo33);
  Lib_Vec128_vec128 lo_ = lo22;
  Lib_Vec128_vec128 lo16 = Lib_Vec128_vec128_shift_right64(lo50, (uint32_t)1U);
  Lib_Vec128_vec128 lo23 = Lib_Vec128_vec128_shift_right64(lo50, (uint32_t)2U);
  Lib_Vec128_vec128 lo34 = Lib_Vec128_vec128_shift_right64(lo50, (uint32_t)7U);
  Lib_Vec128_vec128 lo17 = Lib_Vec128_vec128_xor(lo16, lo23);
  Lib_Vec128_vec128 lo18 = Lib_Vec128_vec128_xor(lo17, lo34);
  Lib_Vec128_vec128 lo19 = Lib_Vec128_vec128_xor(lo18, lo_);
  Lib_Vec128_vec128 lo60 = Lib_Vec128_vec128_xor(lo50, lo19);
  Lib_Vec128_vec128 lo70 = Lib_Vec128_vec128_xor(lo60, hi3);
  Lib_Vec128_vec128 lo110 = lo70;
  acc[0U] = lo110;
}

static void Hacl_Gf128_NI_gcm_init(Lib_Vec128_vec128 *ctx, uint8_t *key)
{
  Lib_Vec128_vec128 *acc = ctx;
  Lib_Vec128_vec128 *pre = ctx + (uint32_t)1U;
  acc[0U] = Lib_Vec128_vec128_zero;
  Hacl_Impl_Gf128_FieldNI_load_precompute_r(pre, key);
}

static void
Hacl_Gf128_NI_gcm_update_blocks(Lib_Vec128_vec128 *ctx, uint32_t len, uint8_t *text)
{
  Lib_Vec128_vec128 *acc = ctx;
  Lib_Vec128_vec128 *pre = ctx + (uint32_t)1U;
  KRML_CHECK_SIZE(sizeof (Lib_Vec128_vec128), (uint32_t)4U);
  Lib_Vec128_vec128 b4[4U];
  for (uint32_t _i = 0U; _i < (uint32_t)4U; ++_i)
    b4[_i] = Lib_Vec128_vec128_zero;
  uint32_t blocks = len / (uint32_t)64U;
  for (uint32_t i = (uint32_t)0U; i < blocks; i = i + (uint32_t)1U)
  {
    uint8_t *tb = text + i * (uint32_t)64U;
    b4[0U] = Lib_Vec128_vec128_load_be(tb);
    b4[1U] = Lib_Vec128_vec128_load_be(tb + (uint32_t)16U);
    b4[2U] = Lib_Vec128_vec128_load_be(tb + (uint32_t)32U);
    b4[3U] = Lib_Vec128_vec128_load_be(tb + (uint32_t)48U);
    Hacl_Impl_Gf128_FieldNI_fadd_mul4(acc, b4, pre);
  }
  uint32_t rem1 = len % (uint32_t)64U;
  uint8_t *last1 = text + blocks * (uint32_t)64U;
  Lib_Vec128_vec128 *acc1 = ctx;
  Lib_Vec128_vec128 *r = ctx + (uint32_t)4U;
  uint32_t blocks1 = rem1 / (uint32_t)16U;
  for (uint32_t i = (uint32_t)0U; i < blocks1; i = i + (uint32_t)1U)
  {
    uint8_t *tb = last1 + i * (uint32_t)16U;
    Lib_Vec128_vec128 elem = Lib_Vec128_vec128_zero;
    elem = Lib_Vec128_vec128_load_be(tb);
    Hacl_Impl_Gf128_FieldNI_fadd(acc1, &elem);
    Hacl_Impl_Gf128_FieldNI_fmul(acc1, r);
  }
  uint32_t rem2 = rem1 % (uint32_t)16U;
  if (rem2 > (uint32_t)0U)
  {
    uint8_t *last2 = last1 + blocks1 * (uint32_t)16U;
    Lib_Vec128_vec128 elem = Lib_Vec128_vec128_zero;
    uint8_t b[16U] = { 0U };
    memcpy(b, last2, rem2 * sizeof last2[0U]);
    elem = Lib_Vec128_vec128_load_be(b);
    Hacl_Impl_Gf128_FieldNI_fadd(acc1, &elem);
    Hacl_Impl_Gf128_FieldNI_fmul(acc1, r);
  }
}

static void
Hacl_Gf128_NI_gcm_update_padded(Lib_Vec128_vec128 *ctx, uint32_t len, uint8_t *text)
{
  Hacl_Gf128_NI_gcm_update_blocks(ctx, len, text);
}

static void Hacl_Gf128_NI_gcm_emit(uint8_t *tag, Lib_Vec128_vec128 *ctx)
{
  Lib_Vec128_vec128 *acc = ctx;
  Lib_Vec128_vec128_store_be(tag, acc[0U]);
}

inline static void
Hacl_AES_256_NI_aes256_init(Lib_Vec128_vec128 *ctx, uint8_t *key, uint8_t *nonce)
{
  Lib_Vec128_vec128 *kex = ctx + (uint32_t)1U;
  Lib_Vec128_vec128 *n1 = ctx;
  uint32_t klen1 = (uint32_t)1U;
  Lib_Vec128_vec128 *next0 = kex;
  Lib_Vec128_vec128 *next1 = kex + klen1;
  next0[0U] = Lib_Vec128_vec128_load_le(key);
  next1[0U] = Lib_Vec128_vec128_load_le(key + (uint32_t)16U);
  Lib_Vec128_vec128 *prev0 = next0;
  Lib_Vec128_vec128 *prev1 = next1;
  Lib_Vec128_vec128 *next01 = kex + klen1 * (uint32_t)2U;
  Lib_Vec128_vec128 *next11 = kex + klen1 * (uint32_t)3U;
  Lib_Vec128_vec128 v10 = Lib_Vec128_ni_aes_keygen_assist(prev1[0U], (uint8_t)0x01U);
  next01[0U] =
    Lib_Vec128_vec128_shuffle32(v10,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)3U, (uint8_t)3U, (uint8_t)3U, (uint8_t)3U));
  Lib_Vec128_vec128 key1 = prev0[0U];
  Lib_Vec128_vec128
  key2 = Lib_Vec128_vec128_xor(key1, Lib_Vec128_vec128_shift_left(key1, (uint32_t)32U));
  Lib_Vec128_vec128
  key3 = Lib_Vec128_vec128_xor(key2, Lib_Vec128_vec128_shift_left(key2, (uint32_t)32U));
  Lib_Vec128_vec128
  key4 = Lib_Vec128_vec128_xor(key3, Lib_Vec128_vec128_shift_left(key3, (uint32_t)32U));
  next01[0U] = Lib_Vec128_vec128_xor(next01[0U], key4);
  Lib_Vec128_vec128 v11 = Lib_Vec128_ni_aes_keygen_assist(next01[0U], (uint8_t)0U);
  next11[0U] =
    Lib_Vec128_vec128_shuffle32(v11,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)2U, (uint8_t)2U, (uint8_t)2U, (uint8_t)2U));
  Lib_Vec128_vec128 key10 = prev1[0U];
  Lib_Vec128_vec128
  key20 = Lib_Vec128_vec128_xor(key10, Lib_Vec128_vec128_shift_left(key10, (uint32_t)32U));
  Lib_Vec128_vec128
  key30 = Lib_Vec128_vec128_xor(key20, Lib_Vec128_vec128_shift_left(key20, (uint32_t)32U));
  Lib_Vec128_vec128
  key40 = Lib_Vec128_vec128_xor(key30, Lib_Vec128_vec128_shift_left(key30, (uint32_t)32U));
  next11[0U] = Lib_Vec128_vec128_xor(next11[0U], key40);
  Lib_Vec128_vec128 *prev01 = next01;
  Lib_Vec128_vec128 *prev11 = next11;
  Lib_Vec128_vec128 *next02 = kex + klen1 * (uint32_t)4U;
  Lib_Vec128_vec128 *next12 = kex + klen1 * (uint32_t)5U;
  Lib_Vec128_vec128 v12 = Lib_Vec128_ni_aes_keygen_assist(prev11[0U], (uint8_t)0x02U);
  next02[0U] =
    Lib_Vec128_vec128_shuffle32(v12,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)3U, (uint8_t)3U, (uint8_t)3U, (uint8_t)3U));
  Lib_Vec128_vec128 key11 = prev01[0U];
  Lib_Vec128_vec128
  key21 = Lib_Vec128_vec128_xor(key11, Lib_Vec128_vec128_shift_left(key11, (uint32_t)32U));
  Lib_Vec128_vec128
  key31 = Lib_Vec128_vec128_xor(key21, Lib_Vec128_vec128_shift_left(key21, (uint32_t)32U));
  Lib_Vec128_vec128
  key41 = Lib_Vec128_vec128_xor(key31, Lib_Vec128_vec128_shift_left(key31, (uint32_t)32U));
  next02[0U] = Lib_Vec128_vec128_xor(next02[0U], key41);
  Lib_Vec128_vec128 v13 = Lib_Vec128_ni_aes_keygen_assist(next02[0U], (uint8_t)0U);
  next12[0U] =
    Lib_Vec128_vec128_shuffle32(v13,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)2U, (uint8_t)2U, (uint8_t)2U, (uint8_t)2U));
  Lib_Vec128_vec128 key12 = prev11[0U];
  Lib_Vec128_vec128
  key22 = Lib_Vec128_vec128_xor(key12, Lib_Vec128_vec128_shift_left(key12, (uint32_t)32U));
  Lib_Vec128_vec128
  key32 = Lib_Vec128_vec128_xor(key22, Lib_Vec128_vec128_shift_left(key22, (uint32_t)32U));
  Lib_Vec128_vec128
  key42 = Lib_Vec128_vec128_xor(key32, Lib_Vec128_vec128_shift_left(key32, (uint32_t)32U));
  next12[0U] = Lib_Vec128_vec128_xor(next12[0U], key42);
  Lib_Vec128_vec128 *prev02 = next02;
  Lib_Vec128_vec128 *prev12 = next12;
  Lib_Vec128_vec128 *next03 = kex + klen1 * (uint32_t)6U;
  Lib_Vec128_vec128 *next13 = kex + klen1 * (uint32_t)7U;
  Lib_Vec128_vec128 v14 = Lib_Vec128_ni_aes_keygen_assist(prev12[0U], (uint8_t)0x04U);
  next03[0U] =
    Lib_Vec128_vec128_shuffle32(v14,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)3U, (uint8_t)3U, (uint8_t)3U, (uint8_t)3U));
  Lib_Vec128_vec128 key13 = prev02[0U];
  Lib_Vec128_vec128
  key23 = Lib_Vec128_vec128_xor(key13, Lib_Vec128_vec128_shift_left(key13, (uint32_t)32U));
  Lib_Vec128_vec128
  key33 = Lib_Vec128_vec128_xor(key23, Lib_Vec128_vec128_shift_left(key23, (uint32_t)32U));
  Lib_Vec128_vec128
  key43 = Lib_Vec128_vec128_xor(key33, Lib_Vec128_vec128_shift_left(key33, (uint32_t)32U));
  next03[0U] = Lib_Vec128_vec128_xor(next03[0U], key43);
  Lib_Vec128_vec128 v15 = Lib_Vec128_ni_aes_keygen_assist(next03[0U], (uint8_t)0U);
  next13[0U] =
    Lib_Vec128_vec128_shuffle32(v15,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)2U, (uint8_t)2U, (uint8_t)2U, (uint8_t)2U));
  Lib_Vec128_vec128 key14 = prev12[0U];
  Lib_Vec128_vec128
  key24 = Lib_Vec128_vec128_xor(key14, Lib_Vec128_vec128_shift_left(key14, (uint32_t)32U));
  Lib_Vec128_vec128
  key34 = Lib_Vec128_vec128_xor(key24, Lib_Vec128_vec128_shift_left(key24, (uint32_t)32U));
  Lib_Vec128_vec128
  key44 = Lib_Vec128_vec128_xor(key34, Lib_Vec128_vec128_shift_left(key34, (uint32_t)32U));
  next13[0U] = Lib_Vec128_vec128_xor(next13[0U], key44);
  Lib_Vec128_vec128 *prev03 = next03;
  Lib_Vec128_vec128 *prev13 = next13;
  Lib_Vec128_vec128 *next04 = kex + klen1 * (uint32_t)8U;
  Lib_Vec128_vec128 *next14 = kex + klen1 * (uint32_t)9U;
  Lib_Vec128_vec128 v16 = Lib_Vec128_ni_aes_keygen_assist(prev13[0U], (uint8_t)0x08U);
  next04[0U] =
    Lib_Vec128_vec128_shuffle32(v16,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)3U, (uint8_t)3U, (uint8_t)3U, (uint8_t)3U));
  Lib_Vec128_vec128 key15 = prev03[0U];
  Lib_Vec128_vec128
  key25 = Lib_Vec128_vec128_xor(key15, Lib_Vec128_vec128_shift_left(key15, (uint32_t)32U));
  Lib_Vec128_vec128
  key35 = Lib_Vec128_vec128_xor(key25, Lib_Vec128_vec128_shift_left(key25, (uint32_t)32U));
  Lib_Vec128_vec128
  key45 = Lib_Vec128_vec128_xor(key35, Lib_Vec128_vec128_shift_left(key35, (uint32_t)32U));
  next04[0U] = Lib_Vec128_vec128_xor(next04[0U], key45);
  Lib_Vec128_vec128 v17 = Lib_Vec128_ni_aes_keygen_assist(next04[0U], (uint8_t)0U);
  next14[0U] =
    Lib_Vec128_vec128_shuffle32(v17,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)2U, (uint8_t)2U, (uint8_t)2U, (uint8_t)2U));
  Lib_Vec128_vec128 key16 = prev13[0U];
  Lib_Vec128_vec128
  key26 = Lib_Vec128_vec128_xor(key16, Lib_Vec128_vec128_shift_left(key16, (uint32_t)32U));
  Lib_Vec128_vec128
  key36 = Lib_Vec128_vec128_xor(key26, Lib_Vec128_vec128_shift_left(key26, (uint32_t)32U));
  Lib_Vec128_vec128
  key46 = Lib_Vec128_vec128_xor(key36, Lib_Vec128_vec128_shift_left(key36, (uint32_t)32U));
  next14[0U] = Lib_Vec128_vec128_xor(next14[0U], key46);
  Lib_Vec128_vec128 *prev04 = next04;
  Lib_Vec128_vec128 *prev14 = next14;
  Lib_Vec128_vec128 *next05 = kex + klen1 * (uint32_t)10U;
  Lib_Vec128_vec128 *next15 = kex + klen1 * (uint32_t)11U;
  Lib_Vec128_vec128 v18 = Lib_Vec128_ni_aes_keygen_assist(prev14[0U], (uint8_t)0x10U);
  next05[0U] =
    Lib_Vec128_vec128_shuffle32(v18,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)3U, (uint8_t)3U, (uint8_t)3U, (uint8_t)3U));
  Lib_Vec128_vec128 key17 = prev04[0U];
  Lib_Vec128_vec128
  key27 = Lib_Vec128_vec128_xor(key17, Lib_Vec128_vec128_shift_left(key17, (uint32_t)32U));
  Lib_Vec128_vec128
  key37 = Lib_Vec128_vec128_xor(key27, Lib_Vec128_vec128_shift_left(key27, (uint32_t)32U));
  Lib_Vec128_vec128
  key47 = Lib_Vec128_vec128_xor(key37, Lib_Vec128_vec128_shift_left(key37, (uint32_t)32U));
  next05[0U] = Lib_Vec128_vec128_xor(next05[0U], key47);
  Lib_Vec128_vec128 v19 = Lib_Vec128_ni_aes_keygen_assist(next05[0U], (uint8_t)0U);
  next15[0U] =
    Lib_Vec128_vec128_shuffle32(v19,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)2U, (uint8_t)2U, (uint8_t)2U, (uint8_t)2U));
  Lib_Vec128_vec128 key18 = prev14[0U];
  Lib_Vec128_vec128
  key28 = Lib_Vec128_vec128_xor(key18, Lib_Vec128_vec128_shift_left(key18, (uint32_t)32U));
  Lib_Vec128_vec128
  key38 = Lib_Vec128_vec128_xor(key28, Lib_Vec128_vec128_shift_left(key28, (uint32_t)32U));
  Lib_Vec128_vec128
  key48 = Lib_Vec128_vec128_xor(key38, Lib_Vec128_vec128_shift_left(key38, (uint32_t)32U));
  next15[0U] = Lib_Vec128_vec128_xor(next15[0U], key48);
  Lib_Vec128_vec128 *prev05 = next05;
  Lib_Vec128_vec128 *prev15 = next15;
  Lib_Vec128_vec128 *next06 = kex + klen1 * (uint32_t)12U;
  Lib_Vec128_vec128 *next16 = kex + klen1 * (uint32_t)13U;
  Lib_Vec128_vec128 v110 = Lib_Vec128_ni_aes_keygen_assist(prev15[0U], (uint8_t)0x20U);
  next06[0U] =
    Lib_Vec128_vec128_shuffle32(v110,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)3U, (uint8_t)3U, (uint8_t)3U, (uint8_t)3U));
  Lib_Vec128_vec128 key19 = prev05[0U];
  Lib_Vec128_vec128
  key29 = Lib_Vec128_vec128_xor(key19, Lib_Vec128_vec128_shift_left(key19, (uint32_t)32U));
  Lib_Vec128_vec128
  key39 = Lib_Vec128_vec128_xor(key29, Lib_Vec128_vec128_shift_left(key29, (uint32_t)32U));
  Lib_Vec128_vec128
  key49 = Lib_Vec128_vec128_xor(key39, Lib_Vec128_vec128_shift_left(key39, (uint32_t)32U));
  next06[0U] = Lib_Vec128_vec128_xor(next06[0U], key49);
  Lib_Vec128_vec128 v111 = Lib_Vec128_ni_aes_keygen_assist(next06[0U], (uint8_t)0U);
  next16[0U] =
    Lib_Vec128_vec128_shuffle32(v111,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)2U, (uint8_t)2U, (uint8_t)2U, (uint8_t)2U));
  Lib_Vec128_vec128 key110 = prev15[0U];
  Lib_Vec128_vec128
  key210 = Lib_Vec128_vec128_xor(key110, Lib_Vec128_vec128_shift_left(key110, (uint32_t)32U));
  Lib_Vec128_vec128
  key310 = Lib_Vec128_vec128_xor(key210, Lib_Vec128_vec128_shift_left(key210, (uint32_t)32U));
  Lib_Vec128_vec128
  key410 = Lib_Vec128_vec128_xor(key310, Lib_Vec128_vec128_shift_left(key310, (uint32_t)32U));
  next16[0U] = Lib_Vec128_vec128_xor(next16[0U], key410);
  Lib_Vec128_vec128 *prev06 = next06;
  Lib_Vec128_vec128 *prev16 = next16;
  Lib_Vec128_vec128 *next07 = kex + klen1 * (uint32_t)14U;
  Lib_Vec128_vec128 v1 = Lib_Vec128_ni_aes_keygen_assist(prev16[0U], (uint8_t)0x40U);
  next07[0U] =
    Lib_Vec128_vec128_shuffle32(v1,
      Lib_Vec128_vec128_shuffle32_spec((uint8_t)3U, (uint8_t)3U, (uint8_t)3U, (uint8_t)3U));
  Lib_Vec128_vec128 key111 = prev06[0U];
  Lib_Vec128_vec128
  key211 = Lib_Vec128_vec128_xor(key111, Lib_Vec128_vec128_shift_left(key111, (uint32_t)32U));
  Lib_Vec128_vec128
  key311 = Lib_Vec128_vec128_xor(key211, Lib_Vec128_vec128_shift_left(key211, (uint32_t)32U));
  Lib_Vec128_vec128
  key411 = Lib_Vec128_vec128_xor(key311, Lib_Vec128_vec128_shift_left(key311, (uint32_t)32U));
  next07[0U] = Lib_Vec128_vec128_xor(next07[0U], key411);
  uint8_t nb[16U] = { 0U };
  memcpy(nb, nonce, (uint32_t)12U * sizeof nonce[0U]);
  n1[0U] = Lib_Vec128_vec128_load_le(nb);
}

inline static void Hacl_AES_256_NI_aes256_set_nonce(Lib_Vec128_vec128 *ctx, uint8_t *nonce)
{
  Lib_Vec128_vec128 *n1 = ctx;
  uint8_t nb[16U] = { 0U };
  memcpy(nb, nonce, (uint32_t)12U * sizeof nonce[0U]);
  n1[0U] = Lib_Vec128_vec128_load_le(nb);
}

inline static void
Hacl_AES_256_NI_aes256_key_block(uint8_t *kb, Lib_Vec128_vec128 *ctx, uint32_t counter)
{
  Lib_Vec128_vec128 *kex = ctx + (uint32_t)1U;
  Lib_Vec128_vec128 *n1 = ctx;
  KRML_CHECK_SIZE(sizeof (Lib_Vec128_vec128), (uint32_t)4U);
  Lib_Vec128_vec128 st[4U];
  for (uint32_t _i = 0U; _i < (uint32_t)4U; ++_i)
    st[_i] = Lib_Vec128_vec128_zero;
  uint32_t counter1 = counter;
  uint32_t counter0 = htobe32(counter1);
  uint32_t counter11 = htobe32(counter1 + (uint32_t)1U);
  uint32_t counter2 = htobe32(counter1 + (uint32_t)2U);
  uint32_t counter3 = htobe32(counter1 + (uint32_t)3U);
  st[0U] = Lib_Vec128_vec128_insert32(n1[0U], counter0, (uint8_t)3U);
  st[1U] = Lib_Vec128_vec128_insert32(n1[0U], counter11, (uint8_t)3U);
  st[2U] = Lib_Vec128_vec128_insert32(n1[0U], counter2, (uint8_t)3U);
  st[3U] = Lib_Vec128_vec128_insert32(n1[0U], counter3, (uint8_t)3U);
  uint32_t klen1 = (uint32_t)1U;
  Lib_Vec128_vec128 *k0 = kex;
  Lib_Vec128_vec128 *kr = kex + klen1;
  Lib_Vec128_vec128 *kn = kex + (uint32_t)14U * klen1;
  st[0U] = Lib_Vec128_vec128_xor(st[0U], k0[0U]);
  st[1U] = Lib_Vec128_vec128_xor(st[1U], k0[0U]);
  st[2U] = Lib_Vec128_vec128_xor(st[2U], k0[0U]);
  st[3U] = Lib_Vec128_vec128_xor(st[3U], k0[0U]);
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)13U; i = i + (uint32_t)1U)
  {
    Lib_Vec128_vec128 *sub_key = kr + i * (uint32_t)1U;
    st[0U] = Lib_Vec128_ni_aes_enc(st[0U], sub_key[0U]);
    st[1U] = Lib_Vec128_ni_aes_enc(st[1U], sub_key[0U]);
    st[2U] = Lib_Vec128_ni_aes_enc(st[2U], sub_key[0U]);
    st[3U] = Lib_Vec128_ni_aes_enc(st[3U], sub_key[0U]);
  }
  st[0U] = Lib_Vec128_ni_aes_enc_last(st[0U], kn[0U]);
  st[1U] = Lib_Vec128_ni_aes_enc_last(st[1U], kn[0U]);
  st[2U] = Lib_Vec128_ni_aes_enc_last(st[2U], kn[0U]);
  st[3U] = Lib_Vec128_ni_aes_enc_last(st[3U], kn[0U]);
  Lib_Vec128_vec128_store_le(kb, st[0U]);
}

static void
Hacl_AES_256_NI_aes256_ctr(
  uint32_t len,
  uint8_t *out,
  uint8_t *inp,
  Lib_Vec128_vec128 *ctx,
  uint32_t c
)
{
  uint32_t blocks64 = len / (uint32_t)64U;
  for (uint32_t i = (uint32_t)0U; i < blocks64; i = i + (uint32_t)1U)
  {
    uint32_t ctr = c + i * (uint32_t)4U;
    uint8_t *ib = inp + i * (uint32_t)64U;
    uint8_t *ob = out + i * (uint32_t)64U;
    KRML_CHECK_SIZE(sizeof (Lib_Vec128_vec128), (uint32_t)4U);
    Lib_Vec128_vec128 st[4U];
    for (uint32_t _i = 0U; _i < (uint32_t)4U; ++_i)
      st[_i] = Lib_Vec128_vec128_zero;
    Lib_Vec128_vec128 *kex = ctx + (uint32_t)1U;
    Lib_Vec128_vec128 *n1 = ctx;
    uint32_t counter = ctr;
    uint32_t counter0 = htobe32(counter);
    uint32_t counter1 = htobe32(counter + (uint32_t)1U);
    uint32_t counter2 = htobe32(counter + (uint32_t)2U);
    uint32_t counter3 = htobe32(counter + (uint32_t)3U);
    st[0U] = Lib_Vec128_vec128_insert32(n1[0U], counter0, (uint8_t)3U);
    st[1U] = Lib_Vec128_vec128_insert32(n1[0U], counter1, (uint8_t)3U);
    st[2U] = Lib_Vec128_vec128_insert32(n1[0U], counter2, (uint8_t)3U);
    st[3U] = Lib_Vec128_vec128_insert32(n1[0U], counter3, (uint8_t)3U);
    uint32_t klen1 = (uint32_t)1U;
    Lib_Vec128_vec128 *k0 = kex;
    Lib_Vec128_vec128 *kr = kex + klen1;
    Lib_Vec128_vec128 *kn = kex + (uint32_t)14U * klen1;
    st[0U] = Lib_Vec128_vec128_xor(st[0U], k0[0U]);
    st[1U] = Lib_Vec128_vec128_xor(st[1U], k0[0U]);
    st[2U] = Lib_Vec128_vec128_xor(st[2U], k0[0U]);
    st[3U] = Lib_Vec128_vec128_xor(st[3U], k0[0U]);
    for (uint32_t i = (uint32_t)0U; i < (uint32_t)13U; i = i + (uint32_t)1U)
    {
      Lib_Vec128_vec128 *sub_key = kr + i * (uint32_t)1U;
      st[0U] = Lib_Vec128_ni_aes_enc(st[0U], sub_key[0U]);
      st[1U] = Lib_Vec128_ni_aes_enc(st[1U], sub_key[0U]);
      st[2U] = Lib_Vec128_ni_aes_enc(st[2U], sub_key[0U]);
      st[3U] = Lib_Vec128_ni_aes_enc(st[3U], sub_key[0U]);
    }
    st[0U] = Lib_Vec128_ni_aes_enc_last(st[0U], kn[0U]);
    st[1U] = Lib_Vec128_ni_aes_enc_last(st[1U], kn[0U]);
    st[2U] = Lib_Vec128_ni_aes_enc_last(st[2U], kn[0U]);
    st[3U] = Lib_Vec128_ni_aes_enc_last(st[3U], kn[0U]);
    Lib_Vec128_vec128 v0 = Lib_Vec128_vec128_load_le(ib);
    Lib_Vec128_vec128 v1 = Lib_Vec128_vec128_load_le(ib + (uint32_t)16U);
    Lib_Vec128_vec128 v2 = Lib_Vec128_vec128_load_le(ib + (uint32_t)32U);
    Lib_Vec128_vec128 v3 = Lib_Vec128_vec128_load_le(ib + (uint32_t)48U);
    Lib_Vec128_vec128 v01 = Lib_Vec128_vec128_xor(v0, st[0U]);
    Lib_Vec128_vec128 v11 = Lib_Vec128_vec128_xor(v1, st[1U]);
    Lib_Vec128_vec128 v21 = Lib_Vec128_vec128_xor(v2, st[2U]);
    Lib_Vec128_vec128 v31 = Lib_Vec128_vec128_xor(v3, st[3U]);
    Lib_Vec128_vec128_store_le(ob, v01);
    Lib_Vec128_vec128_store_le(ob + (uint32_t)16U, v11);
    Lib_Vec128_vec128_store_le(ob + (uint32_t)32U, v21);
    Lib_Vec128_vec128_store_le(ob + (uint32_t)48U, v31);
  }
  uint32_t rem1 = len % (uint32_t)64U;
  uint8_t last1[64U];
  if (rem1 > (uint32_t)0U)
  {
    uint32_t ctr = c + blocks64 * (uint32_t)4U;
    uint8_t *ib = inp + blocks64 * (uint32_t)64U;
    uint8_t *ob = out + blocks64 * (uint32_t)64U;
    uint8_t init = (uint8_t)0U;
    for (uint32_t i = (uint32_t)0U; i < (uint32_t)64U; i = i + (uint32_t)1U)
    {
      last1[i] = init;
    }
    memcpy(last1, ib, rem1 * sizeof ib[0U]);
    KRML_CHECK_SIZE(sizeof (Lib_Vec128_vec128), (uint32_t)4U);
    Lib_Vec128_vec128 st[4U];
    for (uint32_t _i = 0U; _i < (uint32_t)4U; ++_i)
      st[_i] = Lib_Vec128_vec128_zero;
    Lib_Vec128_vec128 *kex = ctx + (uint32_t)1U;
    Lib_Vec128_vec128 *n1 = ctx;
    uint32_t counter = ctr;
    uint32_t counter0 = htobe32(counter);
    uint32_t counter1 = htobe32(counter + (uint32_t)1U);
    uint32_t counter2 = htobe32(counter + (uint32_t)2U);
    uint32_t counter3 = htobe32(counter + (uint32_t)3U);
    st[0U] = Lib_Vec128_vec128_insert32(n1[0U], counter0, (uint8_t)3U);
    st[1U] = Lib_Vec128_vec128_insert32(n1[0U], counter1, (uint8_t)3U);
    st[2U] = Lib_Vec128_vec128_insert32(n1[0U], counter2, (uint8_t)3U);
    st[3U] = Lib_Vec128_vec128_insert32(n1[0U], counter3, (uint8_t)3U);
    uint32_t klen1 = (uint32_t)1U;
    Lib_Vec128_vec128 *k0 = kex;
    Lib_Vec128_vec128 *kr = kex + klen1;
    Lib_Vec128_vec128 *kn = kex + (uint32_t)14U * klen1;
    st[0U] = Lib_Vec128_vec128_xor(st[0U], k0[0U]);
    st[1U] = Lib_Vec128_vec128_xor(st[1U], k0[0U]);
    st[2U] = Lib_Vec128_vec128_xor(st[2U], k0[0U]);
    st[3U] = Lib_Vec128_vec128_xor(st[3U], k0[0U]);
    for (uint32_t i = (uint32_t)0U; i < (uint32_t)13U; i = i + (uint32_t)1U)
    {
      Lib_Vec128_vec128 *sub_key = kr + i * (uint32_t)1U;
      st[0U] = Lib_Vec128_ni_aes_enc(st[0U], sub_key[0U]);
      st[1U] = Lib_Vec128_ni_aes_enc(st[1U], sub_key[0U]);
      st[2U] = Lib_Vec128_ni_aes_enc(st[2U], sub_key[0U]);
      st[3U] = Lib_Vec128_ni_aes_enc(st[3U], sub_key[0U]);
    }
    st[0U] = Lib_Vec128_ni_aes_enc_last(st[0U], kn[0U]);
    st[1U] = Lib_Vec128_ni_aes_enc_last(st[1U], kn[0U]);
    st[2U] = Lib_Vec128_ni_aes_enc_last(st[2U], kn[0U]);
    st[3U] = Lib_Vec128_ni_aes_enc_last(st[3U], kn[0U]);
    Lib_Vec128_vec128 v0 = Lib_Vec128_vec128_load_le(last1);
    Lib_Vec128_vec128 v1 = Lib_Vec128_vec128_load_le(last1 + (uint32_t)16U);
    Lib_Vec128_vec128 v2 = Lib_Vec128_vec128_load_le(last1 + (uint32_t)32U);
    Lib_Vec128_vec128 v3 = Lib_Vec128_vec128_load_le(last1 + (uint32_t)48U);
    Lib_Vec128_vec128 v01 = Lib_Vec128_vec128_xor(v0, st[0U]);
    Lib_Vec128_vec128 v11 = Lib_Vec128_vec128_xor(v1, st[1U]);
    Lib_Vec128_vec128 v21 = Lib_Vec128_vec128_xor(v2, st[2U]);
    Lib_Vec128_vec128 v31 = Lib_Vec128_vec128_xor(v3, st[3U]);
    Lib_Vec128_vec128_store_le(last1, v01);
    Lib_Vec128_vec128_store_le(last1 + (uint32_t)16U, v11);
    Lib_Vec128_vec128_store_le(last1 + (uint32_t)32U, v21);
    Lib_Vec128_vec128_store_le(last1 + (uint32_t)48U, v31);
    memcpy(ob, last1, rem1 * sizeof last1[0U]);
  }
}

void Hacl_AES_256_GCM_NI_aes256_gcm_init(Lib_Vec128_vec128 *ctx, uint8_t *key, uint8_t *nonce)
{
  uint8_t gcm_key[16U] = { 0U };
  uint8_t tag_mix[16U] = { 0U };
  uint8_t nonce0[12U] = { 0U };
  Lib_Vec128_vec128 *aes_ctx = ctx;
  Lib_Vec128_vec128 *gcm_ctx = ctx + (uint32_t)16U;
  Hacl_AES_256_NI_aes256_init(aes_ctx, key, nonce0);
  Hacl_AES_256_NI_aes256_key_block(gcm_key, aes_ctx, (uint32_t)0U);
  Hacl_AES_256_NI_aes256_set_nonce(aes_ctx, nonce);
  Hacl_AES_256_NI_aes256_key_block(tag_mix, aes_ctx, (uint32_t)1U);
  Hacl_Gf128_NI_gcm_init(gcm_ctx, gcm_key);
  ctx[21U] = Lib_Vec128_vec128_load_le(tag_mix);
}

void
Hacl_AES_256_GCM_NI_aes256_gcm_encrypt(
  Lib_Vec128_vec128 *ctx,
  uint32_t len,
  uint8_t *out,
  uint8_t *text,
  uint32_t aad_len,
  uint8_t *aad
)
{
  uint8_t *cip = out;
  Lib_Vec128_vec128 *aes_ctx = ctx;
  Hacl_AES_256_NI_aes256_ctr(len, cip, text, aes_ctx, (uint32_t)2U);
  Lib_Vec128_vec128 *gcm_ctx = ctx + (uint32_t)16U;
  Lib_Vec128_vec128 tag_mix = ctx[21U];
  Hacl_Gf128_NI_gcm_update_padded(gcm_ctx, aad_len, aad);
  Hacl_Gf128_NI_gcm_update_padded(gcm_ctx, len, cip);
  uint8_t tmp[16U] = { 0U };
  store64_be(tmp, (uint64_t)(aad_len * (uint32_t)8U));
  store64_be(tmp + (uint32_t)8U, (uint64_t)(len * (uint32_t)8U));
  Hacl_Gf128_NI_gcm_update_blocks(gcm_ctx, (uint32_t)16U, tmp);
  Hacl_Gf128_NI_gcm_emit(tmp, gcm_ctx);
  Lib_Vec128_vec128 tmp_vec = Lib_Vec128_vec128_load_le(tmp);
  Lib_Vec128_vec128 tmp_vec1 = Lib_Vec128_vec128_xor(tmp_vec, tag_mix);
  Lib_Vec128_vec128_store_le(out + len, tmp_vec1);
}

bool
Hacl_AES_256_GCM_NI_aes256_gcm_decrypt(
  Lib_Vec128_vec128 *ctx,
  uint32_t len,
  uint8_t *out,
  uint8_t *cipher,
  uint32_t aad_len,
  uint8_t *aad
)
{
  uint8_t scratch[18U] = { 0U };
  uint8_t *text = scratch;
  uint8_t *result = scratch + (uint32_t)17U;
  uint8_t *ciphertext = cipher;
  uint8_t *tag = cipher + len;
  Lib_Vec128_vec128 *aes_ctx = ctx;
  Lib_Vec128_vec128 *gcm_ctx = ctx + (uint32_t)16U;
  Lib_Vec128_vec128 tag_mix = ctx[21U];
  Hacl_Gf128_NI_gcm_update_padded(gcm_ctx, aad_len, aad);
  Hacl_Gf128_NI_gcm_update_padded(gcm_ctx, len, ciphertext);
  store64_be(text, (uint64_t)(aad_len * (uint32_t)8U));
  store64_be(text + (uint32_t)8U, (uint64_t)(len * (uint32_t)8U));
  Hacl_Gf128_NI_gcm_update_blocks(gcm_ctx, (uint32_t)16U, text);
  Hacl_Gf128_NI_gcm_emit(text, gcm_ctx);
  Lib_Vec128_vec128 text_vec = Lib_Vec128_vec128_load_le(text);
  Lib_Vec128_vec128 text_vec1 = Lib_Vec128_vec128_xor(text_vec, tag_mix);
  Lib_Vec128_vec128_store_le(text, text_vec1);
  for (uint32_t i = (uint32_t)0U; i < (uint32_t)16U; i = i + (uint32_t)1U)
  {
    result[0U] = result[0U] | (text[i] ^ tag[i]);
  }
  uint8_t res8 = result[0U];
  bool r;
  if (res8 == (uint8_t)0U)
  {
    Hacl_AES_256_NI_aes256_ctr(len, out, ciphertext, aes_ctx, (uint32_t)2U);
    r = true;
  }
  else
  {
    r = false;
  }
  return r;
}

