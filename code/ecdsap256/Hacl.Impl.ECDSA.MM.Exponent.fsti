module Hacl.Impl.ECDSA.MM.Exponent


open FStar.HyperStack.All
open FStar.HyperStack
module ST = FStar.HyperStack.ST

open Lib.IntTypes
open Lib.Buffer

open FStar.Math.Lemmas

open Spec.P256.Lemmas
open Spec.P256.Constants
open Hacl.Spec.P256.Felem
open Spec.ECDSA
open Hacl.Impl.P256.Bignum

open Spec.P256.Lemmas

open FStar.Mul

open Hacl.Impl.P256.Scalar
open Lib.Loops

#reset-options " --z3rlimit 200"

noextract
let prime = prime_p256_order

// qinv
val montgomery_ladder_exponent: a:felem -> Stack unit
  (requires fun h -> live h a /\ as_nat h a < prime)
  (ensures fun h0 _ h1 -> modifies (loc a) h0 h1 /\
    fromDomain_ (as_nat h1 a) == exponent_spec (fromDomain_ (as_nat h0 a)) /\
    as_nat h1 a < prime)

val multPowerPartial: s:felem -> a:felem -> b:felem -> result:felem -> Stack unit
  (requires fun h ->
    live h a /\ live h b /\ live h result /\
    as_nat h a < prime /\ as_nat h b < prime /\
    fromDomain_ (as_nat h a) == exponent_spec (fromDomain_ (fromDomain_ (as_nat h s))))
  (ensures fun h0 _ h1 -> modifies (loc result) h0 h1 /\
    as_nat h1 result = (pow (as_nat h0 s) (prime_p256_order - 2) * (as_nat h0 b)) % prime_p256_order)
