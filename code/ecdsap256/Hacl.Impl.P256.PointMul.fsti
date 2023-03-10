module Hacl.Impl.P256.PointMul

open FStar.Mul
open FStar.HyperStack.All
open FStar.HyperStack
module ST = FStar.HyperStack.ST

open Lib.IntTypes
open Lib.Buffer

open Hacl.Spec.P256.Bignum
open Hacl.Impl.P256.Point

module S = Spec.P256
module SM = Hacl.Spec.P256.MontgomeryMultiplication

#set-options "--z3rlimit 30 --fuel 0 --ifuel 0"

inline_for_extraction noextract
val scalarMultiplication: p:point -> res:point -> scalar:lbuffer uint8 32ul -> Stack unit
  (requires fun h ->
    live h p /\ live h res /\ live h scalar /\
    disjoint p res /\ disjoint scalar res /\ disjoint p scalar /\
    point_inv h p)
  (ensures fun h0 _ h1 -> modifies (loc p |+| loc res) h0 h1 /\
    point_inv h1 res /\
    as_point_nat h1 res == S.scalar_multiplication (as_seq h0 scalar) (as_point_nat h0 p))


val scalarMultiplicationWithoutNorm:
    p:point
  -> res:point
  -> scalar:lbuffer uint8 32ul ->
  Stack unit
  (requires fun h ->
    live h p /\ live h res /\ live h scalar /\
    disjoint p res /\ disjoint scalar res /\ disjoint p scalar /\
    point_inv h p)
  (ensures fun h0 _ h1 -> modifies (loc p |+| loc res) h0 h1 /\
    point_inv h1 res /\
    SM.fromDomainPoint (as_point_nat h1 res) ==
      fst (S.montgomery_ladder_spec (as_seq h0 scalar) (S.point_at_inf, as_point_nat h0 p)))


val secretToPublic: res:point -> scalar:lbuffer uint8 32ul -> Stack unit
  (requires fun h ->
    live h res /\ live h scalar /\ disjoint res scalar)
  (ensures fun h0 _ h1 -> modifies (loc res) h0 h1 /\
    point_inv h1 res /\ as_point_nat h1 res == S.secret_to_public (as_seq h0 scalar))


val secretToPublicWithoutNorm: res:point -> scalar:lbuffer uint8 32ul -> Stack unit
  (requires fun h ->
    live h res /\ live h scalar /\ disjoint res scalar)
  (ensures fun h0 _ h1 -> modifies (loc res) h0 h1 /\
    point_inv h1 res /\
    SM.fromDomainPoint (as_point_nat h1 res) ==
      fst (S.montgomery_ladder_spec (as_seq h0 scalar) (S.point_at_inf, S.base_point)))
