module Hacl.Impl.EC.PointDouble

open FStar.HyperStack.All
open FStar.HyperStack
module ST = FStar.HyperStack.ST

open Lib.IntTypes
open Lib.Buffer

open Hacl.Impl.EC.LowLevel
open Hacl.Spec.EC.Definition
open Spec.ECC
open Spec.ECC.Curves

val point_double: #c: curve -> p: point c -> result: point c 
  -> tempBuffer: lbuffer uint64  (getCoordinateLenU64 c *! 17ul) 
  -> Stack unit
  (requires fun h -> live h p /\ live h tempBuffer /\ live h result /\
    disjoint p tempBuffer /\ disjoint result tempBuffer /\
    eq_or_disjoint p result /\ point_eval c h p)
  (ensures fun h0 _ h1 -> modifies (loc tempBuffer |+| loc result) h0 h1 /\
    point_eval c h1 result /\ (
    let x, y, z = point_x_as_nat c h0 p, point_y_as_nat c h0 p, point_z_as_nat c h0 p in 
    let x3, y3, z3 = point_x_as_nat c h1 result, point_y_as_nat c h1 result, point_z_as_nat c h1 result in       
    let xD, yD, zD = fromDomain #c x, fromDomain #c y, fromDomain #c z in 
    let x3D, y3D, z3D = fromDomain #c x3, fromDomain #c y3, fromDomain #c z3 in
    let xN, yN, zN = _point_double #c (xD, yD, zD) in 
    x3D == xN /\ y3D == yN /\ z3D == zN))
