module Spec.ECDSA

open FStar.Mul

open Lib.ByteSequence
open Lib.IntTypes
open Lib.Sequence

open Spec.Hash
open Spec.ECC
open Spec.ECC.Curves
open Spec.ECDSA.Lemmas

module Def = Spec.Hash.Definitions

open FStar.Math.Lemmas
open FStar.Math.Lib


#set-options "--z3rlimit 200"

val ith_bit_power: #c: curve -> k: scalar_bytes #c -> i:nat{i < v (getScalarLen c)}
  -> t:uint64 {(v t == 0 \/ v t == 1) /\ v t == nat_from_intseq_le k / pow2 i % 2}

let ith_bit_power #c k i =
  let q = i / 8 in
  let r = i % 8 in
  let tmp1 = k.[q] >>. (size r) in
  let tmp2 = tmp1 &. u8 1 in
  let res = to_u64 tmp2 in
  logand_le tmp1 (u8 1);
  logand_mask tmp1 (u8 1) 1;
  lemma_scalar_ith (getCoordinateLen c) k q;
  let k = nat_from_intseq_le k in
  pow2_modulo_division_lemma_1 (k / pow2 (8 * (i / 8))) (i % 8) 8;
  division_multiplication_lemma k (pow2 (8 * (i / 8))) (pow2 (i % 8));
  lemma_euclidian_for_ithbit k i;
  pow2_modulo_modulo_lemma_1 (k / pow2 i) 1 (8 - (i % 8));
  res


let ( *% ) (#c: curve) a b = (a * b) % getOrder #c

let fmul (#c: curve) (a: elem (getOrder #c)) (b: elem (getOrder #c)) : elem (getOrder #c) = a * b % getOrder #c


val _exp_step0: #c: curve -> p: elem (getOrder #c) -> q: elem (getOrder #c) -> tuple2 (elem (getOrder #c)) (elem (getOrder #c))

let _exp_step0 #c r0 r1 =
  let r1 = fmul #c r0 r1 in
  let r0 = fmul #c r0  r0 in
  r0, r1


val _exp_step1: #c: curve -> p: elem (getOrder #c) -> q: elem (getOrder #c) -> tuple2 (elem (getOrder #c)) (elem (getOrder #c))

let _exp_step1 #c r0 r1 =
  let r0 = fmul #c r0 r1 in
  let r1 = fmul #c r1 r1 in
  (r0, r1)


let swap p q = q, p


val conditional_swap: #c: curve -> i:uint64 -> p: nat -> q: nat -> tuple2 nat nat

let conditional_swap i p q =
  if v i = 0 then (p, q) else (q, p)


val lemma_swaped_steps: #c: curve -> p: elem (getOrder #c) -> q: elem (getOrder #c) -> Lemma (
  let afterSwapP, afterSwapQ = swap p q in
  let p1, q1 = _exp_step0 afterSwapP afterSwapQ in
  let p2, q2 = swap p1 q1 in
  let r0, r1 = _exp_step1 p q in
  p2 == r0 /\ q2 == r1)

let lemma_swaped_steps p q = ()


val _exp_step: #c: curve 
  -> k: scalar_bytes #c
  -> i: nat{i < v (getScalarLen c)} 
  -> before: tuple2 (elem (getOrder #c)) (elem (getOrder #c))
  -> tuple2 (elem (getOrder #c)) (elem (getOrder #c))

let _exp_step #c k i (p, q) =
  let bit0 = v (getScalarLen c) - 1 - i in
  let bit = ith_bit_power #c k bit0 in
  let open Lib.RawIntTypes in
  if uint_to_nat bit = 0 then _exp_step0 #c p q else _exp_step1 #c p q


val _exponent_spec: #c: curve 
  -> k: scalar_bytes #c
  -> tuple2 (elem (getOrder #c)) (elem (getOrder #c))
  -> tuple2 (elem (getOrder #c)) (elem (getOrder #c))

let _exponent_spec #c k (p, q) =
  let open Lib.LoopCombinators in
  Lib.LoopCombinators.repeati (v (getScalarLen c)) (_exp_step k) (p, q)


val lemma_even: #c: curve 
  -> index: pos {index <= v (getScalarLen c)} 
  -> k: scalar_bytes #c {v (ith_bit_power #c k (v (getScalarLen c) - index)) == 0} ->
  Lemma (
    let number = nat_from_bytes_le k in
    let newIndex = v (getScalarLen c) - index in
    2 * arithmetic_shift_right number (newIndex + 1) == arithmetic_shift_right number newIndex)

let lemma_even #c index k =
  let number = nat_from_bytes_le k in
  let n = (getPower c)  - index in
  FStar.Math.Lemmas.pow2_double_mult n;
  lemma_div_def (number / pow2 n) 2;
  FStar.Math.Lemmas.division_multiplication_lemma number (pow2 n) 2


val lemma_odd: #c: curve -> index:pos{index <= getPower c} 
  -> k:lseq uint8 (getCoordinateLen c) {uint_v (ith_bit_power #c k (getPower c - index)) == 1} ->
  Lemma(
    let number = nat_from_intseq_le k in 
    let n = getPower c - index  in
    2 * arithmetic_shift_right number (n + 1) + 1 ==
    arithmetic_shift_right number n)

let lemma_odd #c index k =
  let number = nat_from_bytes_le k in
  let n = getPower c - index in
  let a0 = 2 * arithmetic_shift_right number (n + 1) + 1 in
  lemma_div_def (number / pow2 n) 2;
  division_multiplication_lemma number (pow2 n) 2;
  pow2_double_mult n;
  assert (arithmetic_shift_right number (n + 1) == number / pow2 (n + 1));
  assert (arithmetic_shift_right number n ==
          2 * arithmetic_shift_right number (n + 1) + 1)


val lemma_exponen_spec: #c: curve -> k:lseq uint8 (getCoordinateLen c)
  -> start:tuple2 (nat_prime #c) (nat_prime #c) {let st0, st1 = start in st0 == 1}
  -> index:nat{index <= getPower c} ->
  Lemma (
    let start0, start1 = start in
    let number = nat_from_bytes_le k in
    let newIndex = getPower c - index in
    let f0, f1 = Lib.LoopCombinators.repeati index (_exp_step #c k) start in
    f0 == pow start1 (arithmetic_shift_right number newIndex) % getOrder #c /\
    f1 == pow start1 (arithmetic_shift_right number newIndex + 1) % getOrder #c
  )

#push-options "--fuel 1"

val lemma_exponen_spec_0: #c: curve -> k:lseq uint8 (getCoordinateLen c)
  -> start:tuple2 (nat_prime #c) (nat_prime #c) {let st0, st1 = start in st0 == 1} ->
  Lemma (
    let start0, start1 = start in
    let number = nat_from_bytes_le k in
    let newIndex = getPower c in
    let f0, f1 = Lib.LoopCombinators.repeati 0 (_exp_step #c k) start in
    f0 == pow start1 (arithmetic_shift_right number newIndex) % getOrder #c /\
    f1 == pow start1 (arithmetic_shift_right number newIndex + 1) % getOrder #c
  )

let lemma_exponen_spec_0 #c k start =
  let st0, st1 = start in
  let number = nat_from_bytes_le k in
  assert (arithmetic_shift_right number (getPower c) == number / pow2 (getPower c));
  FStar.Math.Lemmas.lemma_div_lt_nat number (getPower c) (getPower c);
  assert (arithmetic_shift_right number (getPower c) == 0);
  Lib.LoopCombinators.eq_repeati0 (getPower c) (_exp_step #c k) start;
  admit()

#pop-options

let rec lemma_exponen_spec #c k start index =
  admit();
  let prime_order = getOrder #c in 
  let power = getPower c in 
  let f = _exp_step #c k in
  let st0, st1 = start in
  let number = nat_from_bytes_le k in
  let newIndex = getPower c - index in
  let open Lib.LoopCombinators in
  match index with
  | 0 -> lemma_exponen_spec_0 k start
  | _ ->
    begin
    unfold_repeati (getPower c) f start (index - 1);
    lemma_exponen_spec k start (index - 1);
    let bitMask = uint_v (ith_bit_power #c k ((getPower c) - index)) in
    match bitMask with
      | 0 ->
        let a0 = pow st1 (arithmetic_shift_right number (power - index + 1)) in
        let a1 = pow st1 (arithmetic_shift_right number (power - index + 1) + 1) in
        calc (==) {
          (a0 % prime_order) * (a0 % prime_order) % prime_order;
          == {modulo_distributivity_mult a0 a0 prime_order}
          (a0 * a0) % prime_order;
          == { }
          (pow st1 (arithmetic_shift_right number (power - index + 1)) * pow st1 (arithmetic_shift_right number (power - index + 1))) % prime_order;
          == {pow_plus st1 (arithmetic_shift_right number (power - index + 1)) (arithmetic_shift_right number (power - index + 1))}
          (pow st1 (arithmetic_shift_right number (power - index + 1) + arithmetic_shift_right number (power - index + 1))) % prime_order;
          == {}
          (pow st1 (2 * arithmetic_shift_right number (power - index + 1))) % prime_order;
          == {lemma_even #c index k}
          pow st1 (arithmetic_shift_right number newIndex) % prime_order;
        };
        calc (==) {
          (a0 % prime_order) * (a1 % prime_order) % prime_order;
          == {modulo_distributivity_mult a0 a1 prime_order}
          (a0 * a1) % prime_order;
          == { }
          (pow st1 (arithmetic_shift_right number (power - index + 1)) * pow st1 (arithmetic_shift_right number (power - index + 1) + 1)) % prime_order;
          == {pow_plus st1 (arithmetic_shift_right number (power - index + 1)) (arithmetic_shift_right number (power - index + 1) + 1)}
          (pow st1 (arithmetic_shift_right number (power - index + 1) + arithmetic_shift_right number (power - index + 1) + 1)) % prime_order;
          == {}
          (pow st1 (2 * arithmetic_shift_right number (power - index + 1) + 1)) % prime_order;
          == {lemma_even #c index k}
          (pow st1 (arithmetic_shift_right number (power - index) + 1)) % prime_order;
        }
      | 1 ->
        let a0 = pow st1 (arithmetic_shift_right number (power - index + 1)) in
        let a1 = pow st1 (arithmetic_shift_right number (power - index + 1) + 1) in
        calc (==) {
          (a1 % prime_order) * (a1 % prime_order) % prime_order;
          == {modulo_distributivity_mult a1 a1 prime_order}
          (a1 * a1) % prime_order;
          == { }
          (pow st1 (arithmetic_shift_right number (power - index + 1) + 1) * pow st1 (arithmetic_shift_right number (power - index + 1) + 1)) % prime_order;
          == {pow_plus st1 (arithmetic_shift_right number (power - index + 1) + 1) (arithmetic_shift_right number (power - index + 1) + 1)}
          (pow st1 (arithmetic_shift_right number (power - index + 1) + 1 + arithmetic_shift_right number (power - index + 1) + 1)) % prime_order;
          == {}
          (pow st1 (2 * arithmetic_shift_right number (power - index + 1) + 2)) % prime_order;
          == {lemma_odd index k}
          pow st1 (arithmetic_shift_right number newIndex + 1) % prime_order;
        };
        calc (==) {
          (a0 % prime_order) * (a1 % prime_order) % prime_order;
          == {modulo_distributivity_mult a0 a1 prime_order}
          (a0 * a1) % prime_order;
          == { }
          (pow st1 (arithmetic_shift_right number (power - index + 1)) * pow st1 (arithmetic_shift_right number (power - index + 1) + 1)) % prime_order;
          == {pow_plus st1 (arithmetic_shift_right number (power - index + 1)) (arithmetic_shift_right number (power - index + 1) + 1)}
          (pow st1 (arithmetic_shift_right number (power - index + 1) + arithmetic_shift_right number (power - index + 1) + 1)) % prime_order;
          == {}
          (pow st1 (2 * arithmetic_shift_right number (power - index + 1) + 1)) % prime_order;
          == {lemma_odd index k}
          (pow st1 (arithmetic_shift_right (nat_from_bytes_le k) (power - index)) % prime_order);
        }
    end


unfold let prime_order_inverse_list (#c: curve) : list uint8 = 
  match c with 
  |P256 -> 
    [
      u8 79;  u8 37;  u8 99;  u8 252; u8 194; u8 202; u8 185; u8 243;
      u8 132; u8 158; u8 23;  u8 167; u8 173; u8 250; u8 230; u8 188;
      u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255;
      u8 0;   u8 0;   u8 0;   u8 0;   u8 255; u8 255; u8 255; u8 255
    ]
  |P384 -> 
    [ 
      u8 113; u8 41;  u8 197; u8 204; u8 106; u8 25;  u8 236; u8 236;
      u8 122; u8 167; u8 176; u8 72;  u8 178; u8 13;  u8 26;  u8 88;
      u8 223; u8 45;  u8 55;  u8 244; u8 129; u8 77;  u8 99;  u8 199;
      u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255;
      u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255;
      u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255
   ]


let prime_order_inverse_seq (#c: curve) : (s:lseq uint8 (getCoordinateLen c) {nat_from_intseq_le s == getOrder #c - 2}) =  
  assert_norm (List.Tot.length (prime_order_inverse_list #P256) == getCoordinateLen P256);
  assert_norm (List.Tot.length (prime_order_inverse_list #P384) == getCoordinateLen P384);
  nat_from_intlist_seq_le (getCoordinateLen c) (prime_order_inverse_list #c); 
  assert_norm (nat_from_intlist_le (prime_order_inverse_list #P256) == getOrder #P256 - 2);
  assert_norm (nat_from_intlist_le (prime_order_inverse_list #P384) == getOrder #P384 - 2);
  of_list (prime_order_inverse_list #c)


unfold let prime_order_list (#c: curve) : list uint8 =
  match c with 
  |P256 -> 
    [
      u8 255; u8 255; u8 255; u8 255; u8 0;   u8 0;   u8 0;   u8 0;
      u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255;
      u8 188; u8 230; u8 250; u8 173; u8 167; u8 23;  u8 158; u8 132;
      u8 243; u8 185; u8 202; u8 194; u8 252; u8 99;  u8 37;  u8 81
    ]
  |P384 -> 
    [
      u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255;
      u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255;
      u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255; u8 255;
      u8 199; u8 99;  u8 77;  u8 129; u8 244; u8 55;  u8 45;  u8 223;
      u8 88;  u8 26;  u8 13;  u8 178; u8 72 ; u8 176; u8 167; u8 122;
      u8 236; u8 236; u8 25;  u8 106; u8 204; u8 197; u8 41;  u8 115
    ]


let prime_order_seq (#c: curve) : s:lseq uint8 (getCoordinateLen c) 
  {nat_from_intseq_be s == (getOrder #c)} =
  assert_norm (List.Tot.length (prime_order_list #P256) == getCoordinateLen P256);
  assert_norm (List.Tot.length (prime_order_list #P384) == getCoordinateLen P384);
  nat_from_intlist_seq_be (getCoordinateLen P256) (prime_order_list #P256);
  nat_from_intlist_seq_be (getCoordinateLen P384) (prime_order_list #P384); 
  assert_norm (nat_from_intlist_be (prime_order_list #P256) == getOrder #P256);
  assert_norm (nat_from_intlist_be (prime_order_list #P384) == getOrder #P384);
  of_list (prime_order_list #c)


val exponent_spec: #c: curve -> a: elem (getOrder #c) -> r: elem (getOrder #c) {r = pow a (getOrder #c - 2) % getOrder #c}

let exponent_spec #c a =
  let a0, _ = _exponent_spec (prime_order_inverse_seq #c) (1, a) in
  lemma_exponen_spec (prime_order_inverse_seq #c) (1, a) (getPower c) ;
  a0


(* The last check should be done only if the group is not prime *)
val verifyQValidCurvePointSpec: #c: curve 
  -> publicKey:tuple3 nat nat nat{~(isPointAtInfinity publicKey)} -> bool

let verifyQValidCurvePointSpec #c publicKey =
  let (x: nat), (y:nat), (z:nat) = publicKey in
  let prime = getPrime c in 
  x < prime &&
  y < prime &&
  z < prime &&
  isPointOnCurve #c (x, y, z) && (
  if (isPrimeGroup c) then true else 
    isPointAtInfinity (scalar_multiplication #c (prime_order_seq #c) publicKey))


(* Check that {\displaystyle Q_{A}}Q_{A} is not equal to the identity element O, and its coordinates are otherwise valid *)
val checkCoordinates: #c: curve -> r: nat -> s: nat -> bool

let checkCoordinates #c r s =
  let order = getOrder #c in 
  if r > 0 && r < order && s > 0 && s < order
  then true
  else false


open Spec.Hash.Definitions

type hash_alg_ecdsa = 
  |NoHash
  |Hash of (a: hash_alg {a == SHA2_256 \/ a == SHA2_384 \/ a == SHA2_512})


let invert_state_s (a: hash_alg_ecdsa): Lemma
  (requires True)
  (ensures (inversion hash_alg_ecdsa))
  [ SMTPat (hash_alg_ecdsa) ]
  = allow_inversion (hash_alg_ecdsa)


val min_input_length: #c: curve -> hash_alg_ecdsa -> Tot int

let min_input_length #c a =
  match a with
    |NoHash -> getCoordinateLen c
    |Hash a -> 0


(* Calculate {\displaystyle e={\textrm {HASH}}(m)}e={\textrm {HASH}}(m). (Here HASH is a cryptographic hash function, such as SHA-2, with the output converted to an integer.)
Let {\displaystyle z}z be the {\displaystyle L_{n}}L_{n} leftmost bits of {\displaystyle e}e, where {\displaystyle L_{n}}L_{n} is the bit length of the group order {\displaystyle n}n. (Note that {\displaystyle z}z can be greater than {\displaystyle n}n but not longer.[1]) *)

val hashSpec:
  c: curve 
  -> a: hash_alg_ecdsa 
  -> mLen: size_nat{mLen >= min_input_length #c a}
  -> m: lseq uint8 mLen ->
  Tot (Lib.ByteSequence.lbytes (
    if NoHash? a then getCoordinateLen c 
    else if hash_length (match a with Hash a -> a) > getCoordinateLen c
    then getCoordinateLen c else hash_length (match a with Hash a -> a)))

let hashSpec c a mLen m = 
  assert_norm (pow2 32 < pow2 61);
  assert_norm (pow2 32 < pow2 125);
  allow_inversion hash_alg_ecdsa;
  match a with 
  |NoHash ->  sub m 0 (getCoordinateLen c)
  |Hash a ->
    let hashed = Spec.Agile.Hash.hash a m in 
    if hash_length a > getCoordinateLen c then 
      sub hashed 0 (getCoordinateLen c)
    else
      hashed


val ecdsa_verification_agile:
  c: curve 
  -> alg: hash_alg_ecdsa
  -> publicKey:tuple2 nat nat 
  -> r: nat 
  -> s: nat
  -> mLen:size_nat {mLen >= min_input_length #c alg} 
  -> m:lseq uint8 mLen
  -> bool

let ecdsa_verification_agile c alg publicKey r s mLen m =
  allow_inversion hash_alg_ecdsa; 
  let order = getOrder #c in 
  let publicJacobian = toJacobianCoordinates publicKey in
  if not (verifyQValidCurvePointSpec #c publicJacobian) then false
  else
    begin
    if not ((checkCoordinates #c) r s) then false
    else
      begin
      let hashM = hashSpec c alg mLen m in
      let hashNat = nat_from_bytes_be hashM % order in 

      let u1 = nat_to_bytes_be (getCoordinateLen c) (pow s (order - 2) * hashNat % order) in
      let u2 = nat_to_bytes_be (getCoordinateLen c) (pow s (order - 2) * r % order) in 

      point_mult_0 (basePoint #c) 0;
      point_mult_0 #c publicJacobian 0;
      let u1D, _ = montgomery_ladder_spec_left #c u1 (pointAtInfinity, basePoint #c) in
      let u2D, _ = montgomery_ladder_spec_left #c u2 (pointAtInfinity, publicJacobian) in

      let sumPoints = _point_add #c u1D u2D in
      let pointNorm = _norm #c sumPoints in
      let x, y, z = pointNorm in
      let x = x % order in 
      if isPointAtInfinity pointNorm then false else x = r
    end
  end


val ecdsa_signature_agile:
  c: curve  
  -> alg: hash_alg_ecdsa
  -> mLen:size_nat{mLen >= min_input_length #c alg} 
  -> m:lseq uint8 mLen
  -> privateKey: scalar_bytes #c
  -> k: scalar_bytes #c
  -> tuple3 nat nat uint64

let ecdsa_signature_agile c alg mLen m privateKey k =
  assert_norm (pow2 32 < pow2 61);
  assert_norm (pow2 32 < pow2 125);
  let prime_order = getOrder #c in 
    point_mult_0 (basePoint #c) 0;
  let r, _ = montgomery_ladder_spec_left #c k (pointAtInfinity, (basePoint #c)) in
  let (xN, _, _) = _norm #c r in
  let hashM = hashSpec c alg mLen m in 
  let z = nat_from_bytes_be hashM % prime_order in
  let kFelem = nat_from_bytes_be k in
  let privateKeyFelem = nat_from_bytes_be privateKey in
  let resultR = xN % prime_order in
  let resultS = (z + resultR * privateKeyFelem) * pow kFelem (prime_order - 2) % prime_order in
    if resultR = 0 || resultS = 0 then
      resultR, resultS, u64 (pow2 64 - 1)
    else
      resultR, resultS, u64 0
