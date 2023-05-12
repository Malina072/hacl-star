module Spec.P256

open FStar.Mul
open Lib.IntTypes
open Lib.Sequence
open Lib.ByteSequence

module EC = Spec.ECC
module EP = Spec.ECC.Projective

#set-options "--z3rlimit 50 --fuel 0 --ifuel 0"

(** https://eprint.iacr.org/2013/816.pdf *)

// 0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff
let prime: (a:pos{a = 0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff /\ a < pow2 256}) =
  let p = pow2 256 - pow2 224 + pow2 192 + pow2 96 - 1 in
  assert_norm (0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff = p);
  assert_norm (p < pow2 256); p

let order: (a:pos{a < pow2 256}) =
  let o = 0xffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551 in
  assert_norm (o < pow2 256); o

let felem = x:nat{x < prime}
let a_coeff : felem = (-3) % prime
let b_coeff : felem =
  let b = 0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b in
  assert_norm (b < prime); b

// Base point
let g_x : felem =
  let x = 0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296 in
  assert_norm (x < prime); x
let g_y : felem =
  let y = 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5 in
  assert_norm (y < prime); y

let base_point : x:EC.tuple2_lt_n prime{EC.tuple2_on_curve prime a_coeff b_coeff x} =
  let ( +% ) = Lib.NatMod.add_mod #prime in
  let ( *% ) = Lib.NatMod.mul_mod #prime in
  assert_norm (g_y *% g_y = g_x *% g_x *% g_x +% a_coeff *% g_x +% b_coeff);
  (g_x, g_y)

assume
val prime_lemma: unit -> Lemma (FStar.Math.Euclid.is_prime prime)

assume
val order_lemma: unit -> Lemma (FStar.Math.Euclid.is_prime order)

val weierstrass_curve: unit ->
  Lemma ((4 * a_coeff * a_coeff * a_coeff + 27 * b_coeff * b_coeff) % prime <> 0)

let weierstrass_curve () =
  assert_norm ((4 * a_coeff * a_coeff * a_coeff + 27 * b_coeff * b_coeff) % prime <> 0)


let p256: EC.curve = {
  EC.prime;
  EC.coeff_a = a_coeff;
  EC.coeff_b = b_coeff;

  EC.order;
  EC.base_point;

  EC.prime_len_bytes = 32;
  EC.order_len_bytes = 32;

  EC.weierstrass_curve;
  EC.prime_lemma;
  EC.order_lemma;
}

let fadd a b = EC.fadd p256 a b
let fsub a b = EC.fsub p256 a b
let fmul a b = EC.fmul p256 a b
let finv a = EC.finv p256 a
let fsqrt a = EC.fsqrt p256 a

let qelem = x:nat{x < order}
let qadd a b = EC.qadd p256 a b
let qmul a b = EC.qmul p256 a b
let qinv a = EC.qinv p256 a

let p256_concrete_ops : EC.ec_concrete_ops =
  EP.ec_concrete_ops_proj p256

let aff_point_c = EC.aff_point_c p256
let proj_point = EP.proj_point p256
let proj_point_c = EP.proj_point_c p256
let point_at_inf : proj_point_c = EP.point_at_inf_c p256 ()
let is_point_at_inf = EP.is_point_at_inf p256
let to_proj_point = EP.to_proj_point_c p256
let to_aff_point = EP.to_aff_point p256
let aff_point_inv = EC.aff_point_inv p256
let point_inv = EP.point_inv p256
let is_on_curve = EC.is_on_curve p256
let aff_point_store = EC.aff_point_store p256
let point_store (p:proj_point_c) = EC.point_store p256_concrete_ops p
let aff_point_load = EC.aff_point_load p256
let load_point = EC.load_point p256_concrete_ops
let aff_point_decompress = EC.aff_point_decompress p256
let recover_y = EC.recover_y p256
let point_add = EP.point_add_a3 p256
let point_double = EP.point_double_a3 p256
let point_mul = EP.point_mul p256
let point_mul_g = ECC.point_mul_g p256_concrete_ops
let point_mul_double_g = EP.point_mul_double_g p256
let ecdsa_sign_msg_as_qelem = EC.ecdsa_sign_msg_as_qelem p256_concrete_ops
let ecdsa_verify_msg_as_qelem = ECC.ecdsa_verify_msg_as_qelem p256_concrete_ops


///  ECDSA over the P256 elliptic curve

open Spec.Hash.Definitions

type hash_alg_ecdsa =
  | NoHash
  | Hash of (a:hash_alg{a == SHA2_256 \/ a == SHA2_384 \/ a == SHA2_512})

let _: squash (inversion hash_alg_ecdsa) = allow_inversion hash_alg_ecdsa

let _: squash (pow2 32 < pow2 61 /\ pow2 32 < pow2 125) =
  Math.Lemmas.pow2_lt_compat 61 32;
  Math.Lemmas.pow2_lt_compat 125 32


let min_input_length (a:hash_alg_ecdsa) : nat =
  match a with | NoHash -> 32 | Hash a -> 0


val hash_ecdsa:
    a:hash_alg_ecdsa
  -> msg_len:size_nat{msg_len >= min_input_length a}
  -> msg:lseq uint8 msg_len ->
  Tot (r:lbytes
    (if Hash? a then hash_length (match a with Hash a -> a) else msg_len){length r >= 32})

let hash_ecdsa a msg_len msg =
  match a with | NoHash -> msg | Hash a -> Spec.Agile.Hash.hash a msg


val ecdsa_signature_agile:
    alg:hash_alg_ecdsa
  -> msg_len:size_nat{msg_len >= min_input_length alg}
  -> msg:lbytes msg_len
  -> private_key:lbytes 32
  -> nonce:lbytes 32 ->
  option (lbytes 64)

let ecdsa_signature_agile alg msg_len msg private_key nonce =
  let hashM = hash_ecdsa alg msg_len msg in
  let m_q = nat_from_bytes_be (sub hashM 0 32) % order in
  EC.ecdsa_sign_msg_as_qelem p256_concrete_ops m_q private_key nonce


val ecdsa_verification_agile:
    alg:hash_alg_ecdsa
  -> msg_len:size_nat{msg_len >= min_input_length alg}
  -> msg:lbytes msg_len
  -> public_key:lbytes 64
  -> signature_r:lbytes 32
  -> signature_s:lbytes 32 ->
  bool

let ecdsa_verification_agile alg msg_len msg public_key signature_r signature_s =
  let hashM = hash_ecdsa alg msg_len msg in
  let m_q = nat_from_bytes_be (sub hashM 0 32) % order in
  EC.ecdsa_verify_msg_as_qelem p256_concrete_ops m_q public_key signature_r signature_s


(** The following functions can be removed *)

///  ECDH over the P256 elliptic curve

let secret_to_public (private_key:lbytes 32) : option (lbytes 64) =
  EC.secret_to_public p256_concrete_ops private_key

let ecdh (their_public_key:lbytes 64) (private_key:lbytes 32) : option (lbytes 64) =
  EC.ecdh p256_concrete_ops their_public_key private_key


///  Parsing and Serializing public keys

// raw          = [ x; y ], 64 bytes
// uncompressed = [ 0x04; x; y ], 65 bytes
// compressed   = [ 0x02 for even `y` and 0x03 for odd `y`; x ], 33 bytes

let validate_public_key (pk:lbytes 64) : bool =
  EC.validate_public_key p256_concrete_ops pk

let pk_uncompressed_to_raw (pk:lbytes 65) : option (lbytes 64) =
  EC.pk_uncompressed_to_raw p256_concrete_ops pk

let pk_uncompressed_from_raw (pk:lbytes 64) : lbytes 65 =
  EC.pk_uncompressed_from_raw p256_concrete_ops pk

let pk_compressed_to_raw (pk:lbytes 33) : option (lbytes 64) =
  EC.pk_compressed_to_raw p256_concrete_ops pk

let pk_compressed_from_raw (pk:lbytes 64) : lbytes 33 =
  EC.pk_compressed_from_raw p256_concrete_ops pk
