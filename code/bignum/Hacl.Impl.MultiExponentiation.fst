module Hacl.Impl.MultiExponentiation

open FStar.HyperStack
open FStar.HyperStack.ST
open FStar.Mul

open Lib.IntTypes
open Lib.Buffer

module ST = FStar.HyperStack.ST
module Loops = Lib.LoopCombinators

module SE = Spec.Exponentiation
module BD = Hacl.Bignum.Definitions
module PT = Hacl.Impl.PrecompTable

friend Hacl.Impl.Exponentiation

#reset-options "--z3rlimit 50 --fuel 0 --ifuel 0"

inline_for_extraction noextract
let lexp_double_fw_f_st
  (a_t:inttype_a)
  (len:size_t{v len > 0})
  (ctx_len:size_t)
  (k:concrete_ops a_t len ctx_len)
  (l:size_window_t a_t len)
  (table_len:table_len_t len)
  (table_inv1:table_inv_t a_t len table_len)
  (table_inv2:table_inv_t a_t len table_len) =
    ctx:lbuffer (uint_t a_t SEC) ctx_len
  -> a1:lbuffer (uint_t a_t SEC) len
  -> bLen:size_t
  -> bBits:size_t{(v bBits - 1) / bits a_t < v bLen}
  -> b1:lbuffer (uint_t a_t SEC) bLen
  -> a2:lbuffer (uint_t a_t SEC) len
  -> b2:lbuffer (uint_t a_t SEC) bLen
  -> table1:lbuffer (uint_t a_t SEC) (table_len *! len)
  -> table2:lbuffer (uint_t a_t SEC) (table_len *! len)
  -> i:size_t{v i < v bBits / v l}
  -> acc:lbuffer (uint_t a_t SEC) len ->
  Stack unit
  (requires fun h ->
    live h a1 /\ live h b1 /\ live h a2 /\ live h b2 /\
    live h acc /\ live h ctx /\ live h table1 /\ live h table2 /\

    eq_or_disjoint a1 a2 /\ disjoint a1 acc /\ disjoint a1 ctx /\
    disjoint a1 table1 /\ disjoint a1 table2 /\ disjoint b1 acc /\
    disjoint a2 acc /\ disjoint a2 ctx /\ disjoint a2 table1 /\ disjoint a2 table2 /\
    disjoint b2 acc /\ disjoint acc ctx /\ disjoint acc table1 /\ disjoint acc table2 /\
    disjoint ctx table1 /\ disjoint ctx table2 /\ eq_or_disjoint table1 table2 /\

    BD.bn_v h b1 < pow2 (v bBits) /\
    BD.bn_v h b2 < pow2 (v bBits) /\
    k.to.linv_ctx (as_seq h ctx) /\
    k.to.linv (as_seq h a1) /\ k.to.linv (as_seq h a2) /\
    k.to.linv (as_seq h acc) /\
    table_inv1 (as_seq h a1) (as_seq h table1) /\
    table_inv2 (as_seq h a2) (as_seq h table2))
  (ensures  fun h0 _ h1 -> modifies (loc acc) h0 h1 /\
    k.to.linv (as_seq h1 acc) /\
    k.to.refl (as_seq h1 acc) ==
    SE.exp_double_fw_f #k.to.t_spec k.to.concr_ops
      (k.to.refl (as_seq h0 a1)) (v bBits) (BD.bn_v h0 b1)
      (k.to.refl (as_seq h0 a2)) (BD.bn_v h0 b2) (v l) (v i) (k.to.refl (as_seq h0 acc)))


inline_for_extraction noextract
val lexp_double_fw_f:
    #a_t:inttype_a
  -> len:size_t{v len > 0}
  -> ctx_len:size_t
  -> k:concrete_ops a_t len ctx_len
  -> l:size_window_t a_t len
  -> table_len:table_len_t len
  -> table_inv1:table_inv_t a_t len table_len
  -> table_inv2:table_inv_t a_t len table_len
  -> pow_a_to_small_b1:pow_a_to_small_b_st a_t len ctx_len k l table_len table_inv1
  -> pow_a_to_small_b2:pow_a_to_small_b_st a_t len ctx_len k l table_len table_inv2 ->
  lexp_double_fw_f_st a_t len ctx_len k l table_len table_inv1 table_inv2

let lexp_double_fw_f #a_t len ctx_len k l table_len table_inv1 table_inv2
  pow_a_to_small_b1 pow_a_to_small_b2 ctx a1 bLen bBits b1 a2 b2 table1 table2 i acc =
  lexp_fw_f len ctx_len k l table_len table_inv1 pow_a_to_small_b1 ctx a1 bLen bBits b1 table1 i acc;
  lmul_acc_pow_a_bits_l len ctx_len k l table_len table_inv2 pow_a_to_small_b2 ctx a2 bLen bBits b2 table2 i acc


inline_for_extraction noextract
let lexp_double_fw_acc0_st
  (a_t:inttype_a)
  (len:size_t{v len > 0})
  (ctx_len:size_t)
  (k:concrete_ops a_t len ctx_len)
  (l:size_window_t a_t len)
  (table_len:table_len_t len)
  (table_inv1:table_inv_t a_t len table_len)
  (table_inv2:table_inv_t a_t len table_len) =
    ctx:lbuffer (uint_t a_t SEC) ctx_len
  -> a1:lbuffer (uint_t a_t SEC) len
  -> bLen:size_t
  -> bBits:size_t{0 < v bBits /\ (v bBits - 1) / bits a_t < v bLen}
  -> b1:lbuffer (uint_t a_t SEC) bLen
  -> a2:lbuffer (uint_t a_t SEC) len
  -> b2:lbuffer (uint_t a_t SEC) bLen
  -> table1:lbuffer (uint_t a_t SEC) (table_len *! len)
  -> table2:lbuffer (uint_t a_t SEC) (table_len *! len)
  -> acc:lbuffer (uint_t a_t SEC) len ->
  Stack unit
  (requires fun h -> v bBits % v l <> 0 /\
    live h a1 /\ live h b1 /\ live h a2 /\ live h b2 /\
    live h acc /\ live h ctx /\ live h table1 /\ live h table2 /\

    eq_or_disjoint a1 a2 /\ disjoint a1 acc /\ disjoint a1 ctx /\
    disjoint a1 table1 /\ disjoint a1 table2 /\ disjoint b1 acc /\
    disjoint a2 acc /\ disjoint a2 ctx /\ disjoint a2 table1 /\ disjoint a2 table2 /\
    disjoint b2 acc /\ disjoint acc ctx /\ disjoint acc table1 /\ disjoint acc table2 /\
    disjoint ctx table1 /\ disjoint ctx table2 /\ eq_or_disjoint table1 table2 /\

    BD.bn_v h b1 < pow2 (v bBits) /\
    BD.bn_v h b2 < pow2 (v bBits) /\
    k.to.linv_ctx (as_seq h ctx) /\
    k.to.linv (as_seq h a1) /\ k.to.linv (as_seq h a2) /\
    table_inv1 (as_seq h a1) (as_seq h table1) /\
    table_inv2 (as_seq h a2) (as_seq h table2))
  (ensures  fun h0 _ h1 -> modifies (loc acc) h0 h1 /\
    k.to.linv (as_seq h1 acc) /\
    k.to.refl (as_seq h1 acc) ==
    SE.exp_double_fw_acc0 #k.to.t_spec k.to.concr_ops
      (k.to.refl (as_seq h0 a1)) (v bBits) (BD.bn_v h0 b1)
      (k.to.refl (as_seq h0 a2)) (BD.bn_v h0 b2) (v l))


inline_for_extraction noextract
val lexp_double_fw_acc0:
    #a_t:inttype_a
  -> len:size_t{v len > 0}
  -> ctx_len:size_t
  -> k:concrete_ops a_t len ctx_len
  -> l:size_window_t a_t len
  -> table_len:table_len_t len
  -> table_inv1:table_inv_t a_t len table_len
  -> table_inv2:table_inv_t a_t len table_len
  -> pow_a_to_small_b1:pow_a_to_small_b_st a_t len ctx_len k l table_len table_inv1
  -> pow_a_to_small_b2:pow_a_to_small_b_st a_t len ctx_len k l table_len table_inv2 ->
  lexp_double_fw_acc0_st a_t len ctx_len k l table_len table_inv1 table_inv2

let lexp_double_fw_acc0 #a_t len ctx_len k l table_len table_inv1 table_inv2 pow_a_to_small_b1 pow_a_to_small_b2 ctx a1 bLen bBits b1 a2 b2 table1 table2 acc =
  push_frame ();
  let tmp = create len (uint #a_t #SEC 0) in
  lexp_fw_acc0 len ctx_len k l table_len table_inv1 pow_a_to_small_b1 ctx a1 bLen bBits b1 table1 acc;
  lexp_fw_acc0 len ctx_len k l table_len table_inv2 pow_a_to_small_b2 ctx a2 bLen bBits b2 table2 tmp;
  k.lmul ctx acc tmp acc;
  pop_frame ()


inline_for_extraction noextract
let lexp_double_fw_loop_st
  (a_t:inttype_a)
  (len:size_t{v len > 0})
  (ctx_len:size_t)
  (k:concrete_ops a_t len ctx_len)
  (l:size_window_t a_t len)
  (table_len:table_len_t len)
  (table_inv1:table_inv_t a_t len table_len)
  (table_inv2:table_inv_t a_t len table_len) =
    ctx:lbuffer (uint_t a_t SEC) ctx_len
  -> a1:lbuffer (uint_t a_t SEC) len
  -> bLen:size_t
  -> bBits:size_t{(v bBits - 1) / bits a_t < v bLen}
  -> b1:lbuffer (uint_t a_t SEC) bLen
  -> a2:lbuffer (uint_t a_t SEC) len
  -> b2:lbuffer (uint_t a_t SEC) bLen
  -> table1:lbuffer (uint_t a_t SEC) (table_len *! len)
  -> table2:lbuffer (uint_t a_t SEC) (table_len *! len)
  -> acc:lbuffer (uint_t a_t SEC) len ->
  Stack unit
  (requires fun h ->
    live h a1 /\ live h b1 /\ live h a2 /\ live h b2 /\
    live h acc /\ live h ctx /\ live h table1 /\ live h table2 /\

    eq_or_disjoint a1 a2 /\ disjoint a1 acc /\ disjoint a1 ctx /\
    disjoint a1 table1 /\ disjoint a1 table2 /\ disjoint b1 acc /\
    disjoint a2 acc /\ disjoint a2 ctx /\ disjoint a2 table1 /\ disjoint a2 table2 /\
    disjoint b2 acc /\ disjoint acc ctx /\ disjoint acc table1 /\ disjoint acc table2 /\
    disjoint ctx table1 /\ disjoint ctx table2 /\ eq_or_disjoint table1 table2 /\

    BD.bn_v h b1 < pow2 (v bBits) /\
    BD.bn_v h b2 < pow2 (v bBits) /\
    k.to.linv_ctx (as_seq h ctx) /\
    k.to.linv (as_seq h a1) /\ k.to.linv (as_seq h a2) /\
    k.to.linv (as_seq h acc) /\
    table_inv1 (as_seq h a1) (as_seq h table1) /\
    table_inv2 (as_seq h a2) (as_seq h table2))
  (ensures  fun h0 _ h1 -> modifies (loc acc) h0 h1 /\ k.to.linv (as_seq h1 acc) /\
    k.to.refl (as_seq h1 acc) ==
    Loops.repeati (v bBits / v l) (SE.exp_double_fw_f k.to.concr_ops (k.to.refl (as_seq h0 a1))
      (v bBits) (BD.bn_v h0 b1) (k.to.refl (as_seq h0 a2)) (BD.bn_v h0 b2) (v l)) (k.to.refl (as_seq h0 acc)))


inline_for_extraction noextract
val lexp_double_fw_loop:
    #a_t:inttype_a
  -> len:size_t{v len > 0}
  -> ctx_len:size_t
  -> k:concrete_ops a_t len ctx_len
  -> l:size_window_t a_t len
  -> table_len:table_len_t len
  -> table_inv1:table_inv_t a_t len table_len
  -> table_inv2:table_inv_t a_t len table_len
  -> pow_a_to_small_b1:pow_a_to_small_b_st a_t len ctx_len k l table_len table_inv1
  -> pow_a_to_small_b2:pow_a_to_small_b_st a_t len ctx_len k l table_len table_inv2 ->
  lexp_double_fw_loop_st a_t len ctx_len k l table_len table_inv1 table_inv2

let lexp_double_fw_loop #a_t len ctx_len k l table_len table_inv1 table_inv2 pow_a_to_small_b1 pow_a_to_small_b2 ctx a1 bLen bBits b1 a2 b2 table1 table2 acc =
  let h0 = ST.get () in

  [@ inline_let]
  let refl1 i : GTot k.to.t_spec = k.to.refl (as_seq h0 acc) in
  [@inline_let]
  let spec (h:mem) = SE.exp_double_fw_f k.to.concr_ops (k.to.refl (as_seq h0 a1))
    (v bBits) (BD.bn_v h0 b1) (k.to.refl (as_seq h0 a2)) (BD.bn_v h0 b2) (v l) in

  [@inline_let]
  let inv h (i:nat{i <= v bBits / v l}) =
    modifies (loc acc) h0 h /\
    k.to.linv (as_seq h acc) /\
    k.to.refl (as_seq h acc) == Loops.repeati i (spec h0) (refl1 0) /\
    table_inv1 (as_seq h a1) (as_seq h table1) /\
    table_inv2 (as_seq h a2) (as_seq h table2) in

  Loops.eq_repeati0 (v bBits / v l) (spec h0) (refl1 0);
  Lib.Loops.for 0ul (bBits /. l) inv
  (fun i ->
    Loops.unfold_repeati (v bBits / v l) (spec h0) (refl1 0) (v i);
    lexp_double_fw_f len ctx_len k l table_len table_inv1 table_inv2 pow_a_to_small_b1 pow_a_to_small_b2 ctx a1 bLen bBits b1 a2 b2 table1 table2 i acc
  )


let mk_lexp_double_fw_tables #a_t len ctx_len k l table_len table_inv1 table_inv2 pow_a_to_small_b1 pow_a_to_small_b2 ctx a1 bLen bBits b1 a2 b2 table1 table2 res =
  assert (v (bBits %. l) == v bBits % v l);
  if bBits %. l <> 0ul then
    lexp_double_fw_acc0 len ctx_len k l table_len table_inv1 table_inv2 pow_a_to_small_b1 pow_a_to_small_b2 ctx a1 bLen bBits b1 a2 b2 table1 table2 res
  else k.lone ctx res;

  lexp_double_fw_loop #a_t len ctx_len k l table_len table_inv1 table_inv2 pow_a_to_small_b1 pow_a_to_small_b2 ctx a1 bLen bBits b1 a2 b2 table1 table2 res

//-----------------------------------

inline_for_extraction noextract
val lexp_double_fw_gen:
    #a_t:inttype_a
  -> len:size_t{v len > 0}
  -> ctx_len:size_t
  -> k:concrete_ops a_t len ctx_len
  -> l:size_window_t a_t len
  -> table_len:table_len_t len{1 < v table_len /\ v table_len == pow2 (v l)}
  -> lprecomp_get:pow_a_to_small_b_st a_t len ctx_len k l table_len
                   (table_inv_precomp a_t len ctx_len k l table_len) ->
  lexp_double_fw_st a_t len ctx_len k l

#push-options "--z3rlimit 150"
let lexp_double_fw_gen #a_t len ctx_len k l table_len lprecomp_get ctx a1 bLen bBits b1 a2 b2 acc =
  push_frame ();
  let table1 = create (table_len *! len) (uint #a_t #SEC 0) in
  PT.lprecomp_table #a_t len ctx_len k ctx a1 table_len table1;
  let h0 = ST.get () in
  assert (table_inv_precomp a_t len ctx_len k l table_len (as_seq h0 a1) (as_seq h0 table1));

  let table2 = create (table_len *! len) (uint #a_t #SEC 0) in
  PT.lprecomp_table #a_t len ctx_len k ctx a2 table_len table2;
  let h1 = ST.get () in
  assert (table_inv_precomp a_t len ctx_len k l table_len (as_seq h1 a1) (as_seq h1 table1));
  assert (table_inv_precomp a_t len ctx_len k l table_len (as_seq h1 a2) (as_seq h1 table2));

  mk_lexp_double_fw_tables len ctx_len k l table_len
    (table_inv_precomp a_t len ctx_len k l table_len)
    (table_inv_precomp a_t len ctx_len k l table_len)
    lprecomp_get lprecomp_get ctx a1 bLen bBits b1 a2 b2 table1 table2 acc;
  pop_frame ()
#pop-options


let lexp_double_fw_vartime #a_t len ctx_len k l ctx a1 bLen bBits b1 a2 b2 acc =
  [@inline_let]
  let table_len = 1ul <<. l in
  assert (v table_len == pow2 (v l));
  Math.Lemmas.pow2_le_compat (v l) 1;
  assert (1 < v table_len /\ v table_len * v len <= max_size_t);

  lexp_double_fw_gen len ctx_len k l table_len
    (lprecomp_get_vartime len ctx_len k l table_len)
    ctx a1 bLen bBits b1 a2 b2 acc


let lexp_double_fw_consttime #a_t len ctx_len k l ctx a1 bLen bBits b1 a2 b2 acc =
  [@inline_let]
  let table_len = 1ul <<. l in
  assert (v table_len == pow2 (v l));
  Math.Lemmas.pow2_le_compat (v l) 1;
  assert (1 < v table_len /\ v table_len * v len <= max_size_t);

  lexp_double_fw_gen len ctx_len k l table_len
    (lprecomp_get_consttime len ctx_len k l table_len)
    ctx a1 bLen bBits b1 a2 b2 acc
