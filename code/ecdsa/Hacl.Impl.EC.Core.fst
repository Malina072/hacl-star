module Hacl.Impl.EC.Core 

open FStar.HyperStack.All
open FStar.HyperStack
module ST = FStar.HyperStack.ST

open Lib.IntTypes
open Lib.Buffer

open Hacl.Spec.EC.Definition
open Spec.ECC
open Hacl.Impl.EC.Reduction
open Hacl.Impl.EC.LowLevel 
open Hacl.Impl.EC.Exponent

open Hacl.Impl.EC.MontgomeryMultiplication

open Hacl.Impl.P256.Math 

open Hacl.Impl.EC.PointAdd
open Hacl.Impl.P.PointAdd.Aux
open Hacl.Impl.EC.PointDouble

open Hacl.Impl.EC.Masking

open FStar.Tactics 
open FStar.Tactics.Canon
open Lib.Loops

open FStar.Math.Lemmas
open Hacl.Lemmas.P256

open FStar.Mul


val swap: #c: curve ->  p: point_seq c -> q: point_seq c -> 
  Tot (r: tuple2 (point_seq c) (point_seq c) {let pNew, qNew = r in pNew == q /\ qNew == p})

let swap p q = (q, p)


val conditional_swap: #c: curve -> i: uint64 -> p: point_seq c -> q: point_seq c -> 
  Tot (r: tuple2 (point_seq c) (point_seq c) {
    let pNew, qNew = r in 
    if uint_v i = 0 then pNew == p /\ qNew == q
    else
      let p1, q1 = swap p q in 
      p1 == pNew /\ q1 == qNew}
  )

let conditional_swap i p q = 
  if uint_v i = 0 then (p, q) else (q, p)


#set-options "--z3rlimit 300 --max_fuel 0 --max_ifuel 0" 

let toDomain #c value result = 
  push_frame();
    let len = getCoordinateLenU64 c in 
    let multBuffer = create (size 2 *! len) (u64 0) in 
    shiftLeftWord value multBuffer;
    reduction multBuffer result;
  pop_frame()  


let fromDomain f result = 
  montgomery_multiplication_buffer_by_one f result  


let pointToDomain #c p result = 
  let len = getCoordinateLenU64 c in 
  
  let p_x = sub p (size 0) len in 
  let p_y = sub p len len in 
  let p_z = sub p (size 2 *! len) len in 
    
  let r_x = sub result (size 0) len in 
  let r_y = sub result len len in 
  let r_z = sub result (size 2 *! len) len in 

  toDomain #c p_x r_x;
  toDomain #c p_y r_y;
  toDomain #c p_z r_z


let pointFromDomain #c p result = 
  let len = getCoordinateLenU64 c in 

  let p_x = sub p (size 0) len in 
  let p_y = sub p len len in 
  let p_z = sub p (size 2 *! len) len in 

  let r_x = sub result (size 0) len in 
  let r_y = sub result len len in 
  let r_z = sub result (size 2 *! len) len in 

  fromDomain #c p_x r_x;
  fromDomain #c p_y r_y;
  fromDomain #c p_z r_z


val copy_point: #c: curve ->  p: point c -> result: point c -> Stack unit 
  (requires fun h -> live h p /\ live h result /\ disjoint p result) 
  (ensures fun h0 _ h1 -> modifies (loc result) h0 h1 /\ as_seq h1 result == as_seq h0 p)

let copy_point p result = copy result p
 
let getPower2 c = pow2 (getPower c)

(* https://crypto.stackexchange.com/questions/43869/point-at-infinity-and-error-handling*)
val lemma_pointAtInfInDomain: #c: curve -> x: nat -> y: nat -> z: nat {z < getPrime c} -> 
  Lemma (
    isPointAtInfinity (x, y, z) == 
    isPointAtInfinity ((fromDomain_ #c x), (fromDomain_ #c y), (fromDomain_ #c z)))

let lemma_pointAtInfInDomain #c x y z =
    assert_norm (modp_inv2 #P256 (getPower2 P256) % (getPrime P256) <> 0);
    assert_norm (modp_inv2 #P384 (getPower2 P384) % (getPrime P384) <> 0);
  lemmaFromDomain #c z;
    assert_norm (0 * modp_inv2 #P256 (getPower2 P256) % (getPrime P256) == 0);
    assert_norm (0 * modp_inv2 #P384 (getPower2 P384) % (getPrime P384) == 0);
  begin
  match c with 
    |P256 -> lemma_multiplication_not_mod_prime_p256 z
    |P384 -> lemma_multiplication_not_mod_prime_p384 z
 end


(* The point is a point at infinity iff z == 0 *)
let isPointAtInfinityPrivate #c p =  
  let h0 = ST.get() in 
  let len = getCoordinateLenU64 c in 
    
  let start = len *! size 2 in 

  let zCoordinate = sub p start len in 
  let r = isZero_uint64_CT #c zCoordinate in 

 lemma_pointAtInfInDomain #c
    (as_nat c h0 (gsub p (size 0) len))
    (as_nat c h0 (gsub p len len))
    (as_nat c h0 (gsub p (2ul *! len) len)); 
  r

(*
val lemma_norm_as_specification: #c: curve 
  -> xD: nat {xD < getPrime c} 
  -> yD: nat {yD < getPrime c} 
  -> zD: nat {zD < getPrime c} -> 
  x3 : nat {x3 == xD * (pow (zD * zD) (getPrime c - 2) % getPrime c) % getPrime c}-> 
  y3 : nat {y3 == yD * (pow (zD * zD * zD) (getPrime c -2) % getPrime c) % getPrime c} -> 
  z3 : nat {if isPointAtInfinity(xD, yD, zD) then z3 == 0 else z3 == 1} -> 
  Lemma (
    let (xN, yN, zN) = _norm #c (xD, yD, zD) in 
    x3 == xN /\ y3 == yN /\ z3 == zN)


let lemma_norm_as_specification #c xD yD zD x3 y3 z3 = 
  let prime = getPrime c in 

  calc (==)
  {
    xD * (pow (zD * zD) (getPrime c - 2) % prime) % prime;
    (==) {}
    xD * (modp_inv2_pow #c (zD * zD)) % prime;
    (==) {_ by (canon())}
    modp_inv2_pow #c (zD * zD) * xD % prime;
  };

  calc (==)
  {
    yD * (pow (zD * zD * zD) (getPrime c -2) % prime) % prime;
    (==) {}
    yD * (modp_inv2_pow #c (zD * zD * zD)) % prime;
    (==) {_ by (canon())}
    modp_inv2_pow #c (zD * zD * zD) * yD % prime;
  }
*)

val lemma_point_eval_swapped: c: curve -> h0: mem -> h1: mem -> p: point c -> q: point c ->
  Lemma (requires (point_eval c h0 p /\ as_seq h0 p == as_seq h1 q))
  (ensures (point_eval c h1 q))

let lemma_point_eval_swapped c h0 h1 p q = 
  assert(
  as_nat c h0 (gsub p (size 2 *! getCoordinateLenU64 c) (getCoordinateLenU64 c)) == 
  as_nat c h1 (gsub q (size 2 *! getCoordinateLenU64 c) (getCoordinateLenU64 c)))


[@ CInline]
val cswap: #c: curve ->  bit:uint64{v bit <= 1} -> p:point c -> q:point c -> 
  Stack unit
  (requires fun h -> live h p /\ live h q /\ eq_or_disjoint p q /\ point_eval c h p /\ point_eval c h q)
  (ensures  fun h0 _ h1 -> modifies (loc p |+| loc q) h0 h1 /\
    point_eval c h1 p /\ point_eval c h1 q /\ (
    let pBefore = as_seq h0 p in let qBefore = as_seq h0 q in 
    let pAfter = as_seq h1 p in let qAfter = as_seq h1 q in 
    let condP0, condP1 = conditional_swap #c bit pBefore qBefore in 
    condP0 == pAfter /\ condP1 == qAfter) /\ (
    v bit == 1 ==> as_seq h1 p == as_seq h0 q /\ as_seq h1 q == as_seq h0 p) /\ (
    v bit == 0 ==> as_seq h1 p == as_seq h0 p /\ as_seq h1 q == as_seq h0 q)
  )


let cswap #c bit p1 p2 =
  let open Lib.Sequence in 
  let h0 = ST.get () in
  let mask = u64 0 -. bit in
  
  let len = getCoordinateLenU64 c *! size 3 in 

  [@ inline_let]
  let inv h1 (i:nat{i <= uint_v (getCoordinateLenU64 c) * 3}) =
    (forall (k:nat{k < i}).
      if v bit = 1
      then (as_seq h1 p1).[k] == (as_seq h0 p2).[k] /\ (as_seq h1 p2).[k] == (as_seq h0 p1).[k]
      else (as_seq h1 p1).[k] == (as_seq h0 p1).[k] /\ (as_seq h1 p2).[k] == (as_seq h0 p2).[k]) /\
    (forall (k:nat{i <= k /\ k < uint_v (getCoordinateLenU64 c) * 3}).
      (as_seq h1 p1).[k] == (as_seq h0 p1).[k] /\ (as_seq h1 p2).[k] == (as_seq h0 p2).[k]) /\
    modifies (loc p1 |+| loc p2) h0 h1 in
 
  Lib.Loops.for 0ul len inv
    (fun i ->
      let dummy = mask &. (p1.(i) ^. p2.(i)) in
      p1.(i) <- p1.(i) ^. dummy;
      p2.(i) <- p2.(i) ^. dummy;
      lemma_cswap2_step bit ((as_seq h0 p1).[v i]) ((as_seq h0 p2).[v i])
    );
  let h1 = ST.get () in
  
  Lib.Sequence.eq_intro (as_seq h1 p1) (if v bit = 1 then as_seq h0 p2 else as_seq h0 p1);
  Lib.Sequence.eq_intro (as_seq h1 p2) (if v bit = 1 then as_seq h0 p1 else as_seq h0 p2);

  if v bit = 1 then begin
    lemma_point_eval_swapped c h0 h1 p1 p2;
    lemma_point_eval_swapped c h0 h1 p2 p1 
    end
  else begin
    lemma_point_eval c h0 h1 p1;
    lemma_point_eval c h0 h1 p2 end


val normalisation_update: #c: curve -> z2x: felem c -> z3y: felem c -> p: point c -> resultPoint: point c -> 
  Stack unit 
  (requires fun h -> live h z2x /\ live h z3y /\ live h resultPoint /\ live h p /\ 
    felem_eval c h z2x /\ felem_eval c h z3y /\ felem_eval c h (getZ p) /\
    disjoint z2x z3y /\ disjoint z2x resultPoint /\ disjoint z3y resultPoint)
  (ensures fun h0 _ h1 -> modifies (loc resultPoint) h0 h1 /\ (
    let x0 = point_x_as_nat c h0 p in 
    let y0 = point_y_as_nat c h0 p in 
    let z0 = point_z_as_nat c h0 p in 

    let x1 = point_x_as_nat c h1 resultPoint in 
    let y1 = point_y_as_nat c h1 resultPoint in 
    let z1 = point_z_as_nat c h1 resultPoint in 

    x1 == fromDomain_ #c (as_nat c h0 z2x) /\ 
    y1 == fromDomain_ #c (as_nat c h0 z3y) /\ (
    if Spec.ECC.isPointAtInfinity (fromDomain_ #c x0, fromDomain_ #c y0, fromDomain_ #c z0) 
    then 
      z1 == 0 
    else 
      z1 == 1)))

let normalisation_update #c z2x z3y p resultPoint = 
  push_frame(); 
    let len = getCoordinateLenU64 c in 
    
  let zeroBuffer = create len (u64 0) in   
  let resultX = sub resultPoint (size 0) len in 
  let resultY = sub resultPoint len len in 
  let resultZ = sub resultPoint (size 2 *! len) len in 

  let bit = isPointAtInfinityPrivate p in
  fromDomain z2x resultX;
  fromDomain z3y resultY;
  uploadOneImpl #c resultZ;
  copy_conditional #c resultZ zeroBuffer bit;

  pop_frame()


val lemma_norm: #c: curve -> pD : point_nat_prime #c -> r: point_nat_prime #c ->
  Lemma 
  (requires (
    let prime = getPrime c in 
    let xD, yD, zD = pD in 
    let x3, y3, z3 = r in 
    x3 == xD * (pow (zD * zD % prime) (prime - 2) % prime) % prime /\
    y3 == yD * (pow ((zD * zD % prime) * zD % prime) (prime - 2) % prime) % prime/\
    (if Spec.ECC.isPointAtInfinity (xD, yD, zD) then z3 == 0 else z3 == 1)))
  (ensures (let xN, yN, zN = _norm #c pD in r == (xN, yN, zN))) 


let lemma_norm #c pD r = 
  let prime = getPrime c in 

  let xD, yD, zD = pD in 
  let x3, y3, z3 = r in 

  calc (==)
  {
    xD * (pow (zD * zD % prime) (prime - 2) % prime) % prime;
    (==) {power_distributivity (zD * zD) (prime - 2) prime}
    xD * (pow (zD * zD) (prime - 2) % prime) % prime;
    (==) {}
    xD * (modp_inv2_pow #c (zD * zD)) % prime;
    (==) {_ by (canon())}
    modp_inv2_pow #c (zD * zD) * xD % prime;
  };  

  calc (==) 
  {
    yD * (pow ((zD * zD % prime) * zD % prime) (prime - 2) % prime) % prime;
    (==) {lemma_mod_mul_distr_l (zD * zD) zD prime}
    yD * (pow (zD * zD * zD % prime) (prime - 2) % prime) % prime;
    (==) {power_distributivity (zD * zD * zD) (prime - 2) prime}
    yD * (pow (zD * zD * zD) (prime - 2) % prime) % prime;
    (==) {}
    yD * (modp_inv2_pow #c (zD * zD * zD)) % prime;
    (==) {_ by (canon())}
    modp_inv2_pow #c (zD * zD * zD) * yD % prime;
  }

(* to prove *)
let norm #c p resultPoint tempBuffer = 
  admit();
  [@inline_let]
  let len = getCoordinateLenU64 c in 

  let xf = sub p (size 0) len in 
  let yf = sub p len len in 
  let zf = sub p (size 2 *! len) len in 
  
  let z2f = sub tempBuffer len len in 
  let z3f = sub tempBuffer (size 2 *! len) len in

  let t8 = sub tempBuffer (3ul *! len) (8ul *! len) in 

    let h0 = ST.get() in 
  montgomery_square_buffer #c zf z2f; 
  montgomery_multiplication_buffer #c z2f zf z3f; 

  exponent #c z2f z2f t8;
  exponent #c z3f z3f t8; 
  montgomery_multiplication_buffer #c xf z2f z2f;
  montgomery_multiplication_buffer #c yf z3f z3f;
  normalisation_update z2f z3f p resultPoint; 

  admit();
    let h1 = ST.get() in 

  lemma_norm #c
    (fromDomainPoint #c (point_prime_to_coordinates c (as_seq h0 p))) 
    (point_prime_to_coordinates c (as_seq h1 resultPoint))



let normX #c p result tempBuffer = 
  [@inline_let]
  let len = getCoordinateLenU64 c in 
  
  let xf = sub p (size 0) len in 
  let yf = sub p len len in 
  let zf = sub p (size 2 *! len) len in 
  
  let z2f = sub tempBuffer len len in 
  let z3f = sub tempBuffer (size 2 *! len) len in


  let t8 = sub tempBuffer (3ul *. len) (8ul *. len) in 

    let h0 = ST.get() in 
  montgomery_square_buffer #c zf z2f; 
  exponent #c z2f z2f t8;
  montgomery_multiplication_buffer #c z2f xf z2f;
  fromDomain z2f result;

  admit();
    let prime = getPrime c in 
    power_distributivity (fromDomain_ #c (as_nat c h0 zf) * fromDomain_ #c (as_nat c h0 zf)) (prime - 2) prime


(* this piece of code is taken from Hacl.Curve25519 *)
(* changed Endian for Scalar Bit access *)

val scalar_bit: #c: curve 
  -> #buf_type: buftype 
  -> s:lbuffer_t buf_type uint8 (getScalarLen c) 
  -> n:size_t{v n < getPower c}
  -> Stack uint64
    (requires fun h0 -> live h0 s)
    (ensures  fun h0 r h1 -> h0 == h1 /\ r == ith_bit #c (as_seq h0 s) (v n) /\ v r <= 1)

let scalar_bit #c #buf_type s n =
  let h0 = ST.get () in
  (* mod_mask_lemma ((Lib.Sequence.index (as_seq h0 s) (v (getScalarLen c) -1 - v n / 8)) >>. (n %. 8ul)) 1ul; *)
  assert_norm (1 = pow2 1 - 1);
  assert (v (mod_mask #U8 #SEC 1ul) == v (u8 1)); 
  to_u64 ((s.(getScalarLenBytes c -. 1ul -. n /. 8ul) >>. (n %. 8ul)) &. u8 1)


val montgomery_ladder_step1: #c : curve ->  p: point c -> q: point c 
  -> tempBuffer: lbuffer uint64 (size 17 *! getCoordinateLenU64 c) -> Stack unit
  (requires fun h -> 
    live h p /\ live h q /\ live h tempBuffer /\ 
    LowStar.Monotonic.Buffer.all_disjoint [loc p; loc q; loc tempBuffer] /\
    point_eval c h p /\ point_eval c h q)
  (ensures fun h0 _ h1 -> 
    modifies (loc p |+| loc q |+| loc tempBuffer) h0 h1 /\ 
    point_eval c h1 p /\ point_eval c h1 q /\
    (
      let pX = point_x_as_nat c h0 p in
      let pY = point_y_as_nat c h0 p in
      let pZ = point_z_as_nat c h0 p in

      let qX = point_x_as_nat c h0 q in
      let qY = point_y_as_nat c h0 q in
      let qZ = point_z_as_nat c h0 q in

      let r0X = point_x_as_nat c h1 p in
      let r0Y = point_y_as_nat c h1 p in
      let r0Z = point_z_as_nat c h1 p in

      let r1X = point_x_as_nat c h1 q in
      let r1Y = point_y_as_nat c h1 q in
      let r1Z = point_z_as_nat c h1 q in

      let (rN0X, rN0Y, rN0Z), (rN1X, rN1Y, rN1Z) = _ml_step1 #c
	(fromDomain_ #c pX, fromDomain_ #c pY, fromDomain_ #c pZ) 
	(fromDomain_ #c qX, fromDomain_ #c qY, fromDomain_ #c qZ) in 
      
      fromDomain_ #c r0X == rN0X /\ fromDomain_ #c r0Y == rN0Y /\ fromDomain_ #c r0Z == rN0Z /\
      fromDomain_ #c r1X == rN1X /\ fromDomain_ #c r1Y == rN1Y /\ fromDomain_ #c r1Z == rN1Z
  ) 
)


let montgomery_ladder_step1 #c r0 r1 tempBuffer = 
    let h0 = ST.get() in 
  point_add r0 r1 r1 tempBuffer;
    let h1 = ST.get() in 
    lemma_point_eval c h0 h1 r0;
    lemma_coord_eval c h0 h1 r0;
  point_double r0 r0 tempBuffer;
    let h2 = ST.get() in 
    lemma_point_eval c h1 h2 r1;
    lemma_coord_eval c h1 h2 r1
      

val lemma_step: #c: curve -> i: size_t {uint_v i < getPower c} -> 
  Lemma (uint_v (getPowerU c -. 1ul -. i) == getPower c - 1 - (uint_v i))

let lemma_step i = ()


val montgomery_ladder_step: #c: curve -> #buf_type: buftype-> 
  p: point c -> q: point c -> tempBuffer: lbuffer uint64 (size 17 *! getCoordinateLenU64 c) -> 
  scalar: lbuffer_t buf_type uint8 (getScalarLen c) -> 
  i:size_t{v i < getPower c} -> 
  Stack unit
  (requires fun h -> live h p /\ live h q /\ live h tempBuffer /\ live h scalar /\
    LowStar.Monotonic.Buffer.all_disjoint [loc p; loc q; loc tempBuffer; loc scalar] /\
    point_eval c h p /\ point_eval c h q
  )
  (ensures fun h0 _ h1 -> modifies (loc p |+| loc q |+| loc tempBuffer) h0 h1 /\ 
    point_eval c h1 p /\ point_eval c h1 q /\
    (
      let len = getCoordinateLenU64 c in 

      let pX = point_x_as_nat c h0 p in
      let pY = point_y_as_nat c h0 p in
      let pZ = point_z_as_nat c h0 p in

      let qX = point_x_as_nat c h0 q in
      let qY = point_y_as_nat c h0 q in
      let qZ = point_z_as_nat c h0 q in

      let r0X = point_x_as_nat c h1 p in
      let r0Y = point_y_as_nat c h1 p in
      let r0Z = point_z_as_nat c h1 p in

      let r1X = point_x_as_nat c h1 q in
      let r1Y = point_y_as_nat c h1 q in
      let r1Z = point_z_as_nat c h1 q in

      let (rN0X, rN0Y, rN0Z), (rN1X, rN1Y, rN1Z) = _ml_step #c (as_seq h0 scalar) (uint_v i) (
	(fromDomain_ #c pX, fromDomain_ #c pY, fromDomain_ #c pZ), 
	(fromDomain_ #c qX, fromDomain_ #c qY, fromDomain_ #c qZ)) in 
      
      fromDomain_ #c r0X == rN0X /\ fromDomain_ #c r0Y == rN0Y /\ fromDomain_ #c r0Z == rN0Z /\
      fromDomain_ #c r1X == rN1X /\ fromDomain_ #c r1Y == rN1Y /\ fromDomain_ #c r1Z == rN1Z
    ) 
  )


let montgomery_ladder_step #c #buf_type r0 r1 tempBuffer scalar i = 
    let h0 = ST.get() in 
  let bit0 = getPowerU c -. i -. 1ul in 
  let bit = scalar_bit scalar bit0 in 
  cswap bit r0 r1; 
  montgomery_ladder_step1 r0 r1 tempBuffer; 
  cswap bit r0 r1; 
  lemma_step #c i;
    admit()

(* ___________________________________________________ *)
val montgomery_ladder: #c: curve -> #buf_type: buftype->  p: point c -> q: point c ->
  scalar: lbuffer_t buf_type uint8 (getScalarLen c) -> 
  tempBuffer:  lbuffer uint64 (size 17 *! getCoordinateLenU64 c)  -> 
  Stack unit
  (requires fun h -> live h p /\ live h q /\ live h scalar /\  live h tempBuffer /\
    LowStar.Monotonic.Buffer.all_disjoint [loc p; loc q; loc tempBuffer; loc scalar] /\
      (
	let prime = getPrime c in 
	let len = getCoordinateLenU64 c in 
	  
	as_nat c h (gsub p (size 0) len) < prime /\ 
	as_nat c h (gsub p len len) < prime /\
	as_nat c h (gsub p (size 2 *! len) len) < prime /\
	       
	as_nat c h (gsub q (size 0) len) < prime /\  
	as_nat c h (gsub q len len) < prime /\
	as_nat c h (gsub q (size 2 *! len) len) < prime )
  )
  (ensures fun h0 _ h1 -> modifies (loc p |+| loc q |+| loc tempBuffer) h0 h1 /\
    (
      let prime = getPrime c in 
      let len = getCoordinateLenU64 c in 
      
      as_nat c h1 (gsub p (size 0) len) < prime /\ 
      as_nat c h1 (gsub p len len) < prime /\
      as_nat c h1 (gsub p (size 2 *! len) len) < prime /\
	
      as_nat c h1 (gsub q (size 0) len) < prime /\  
      as_nat c h1 (gsub q len len) < prime /\
      as_nat c h1 (gsub q (size 2 *! len) len) < prime /\


      (
	let p1 = fromDomainPoint #c (point_prime_to_coordinates c (as_seq h1 p)) in 
	let q1 = fromDomainPoint #c (point_prime_to_coordinates c (as_seq h1 q)) in 
	let rN, qN = montgomery_ladder_spec_left #c(as_seq h0 scalar) 
	  (
	    fromDomainPoint #c (point_prime_to_coordinates c (as_seq h0 p)),  
	    fromDomainPoint #c (point_prime_to_coordinates c (as_seq h0 q))
	  ) in 
	rN == p1 /\ qN == q1
	)
      )
   )
 

let montgomery_ladder #c #a p q scalar tempBuffer =  
  let h0 = ST.get() in 

  [@inline_let]
  let len = getCoordinateLenU64 c in 
  let cycleLoop = getPowerU c in 

  [@inline_let]
  let spec_ml h0 = _ml_step #c (as_seq h0 scalar) in 

  [@inline_let] 
  let acc (h:mem) : GTot (tuple2 point_nat_prime point_nat_prime) = 
  (fromDomainPoint #c (point_prime_to_coordinates c (as_seq h p)),
  fromDomainPoint #c (point_prime_to_coordinates c (as_seq h q)))  in 
  
  Lib.LoopCombinators.eq_repeati0 (getPower c) (spec_ml h0) (acc h0);
  [@inline_let]
  let inv h (i: nat {i <= getPower c}) = 
    as_nat c h (gsub p (size 0) len) < getPrime c /\ 
    as_nat c h (gsub p len len) < getPrime c /\
    as_nat c h (gsub p (size 2 *! len) len) < getPrime c /\
	
    as_nat c h (gsub q (size 0) len) < getPrime c /\  
    as_nat c h (gsub q len len) < getPrime c /\
    as_nat c h (gsub q (size 2 *! len) len) < getPrime c /\
    modifies3 p q tempBuffer h0 h   /\
    acc h == Lib.LoopCombinators.repeati i (spec_ml h0) (acc h0)

    in 

  for 0ul cycleLoop inv 
    (fun i -> let h2 = ST.get() in
      montgomery_ladder_step p q tempBuffer scalar i; 
      Lib.LoopCombinators.unfold_repeati (getPower c) (spec_ml h0) (acc h0) (uint_v i)
    );
    admit()


val lemma_point_to_domain: #c: curve ->  h0: mem -> h1: mem 
  ->  p: point c 
  -> result: point c ->  Lemma
   (requires (
     point_x_as_nat c h0 p < getPrime c /\ 
     point_y_as_nat c h0 p < getPrime c /\
     point_z_as_nat c h0 p < getPrime c /\
       point_x_as_nat c h1 result == toDomain_ #c (point_x_as_nat c h0 p) /\
       point_y_as_nat c h1 result == toDomain_ #c (point_y_as_nat c h0 p) /\
       point_z_as_nat c h1 result == toDomain_ #c (point_z_as_nat c h0 p) 
     )
   )
   (ensures (fromDomainPoint #c (point_prime_to_coordinates c (as_seq h1 result)) == point_prime_to_coordinates c (as_seq h0 p)))

let lemma_point_to_domain #c h0 h1 p result = ()


val lemma_pif_to_domain: #c: curve -> h: mem ->  p: point c -> Lemma
  (requires (
    point_x_as_nat c h p == 0 /\ 
    point_y_as_nat c h p == 0 /\ 
    point_z_as_nat c h p == 0))
  (ensures (fromDomainPoint #c 
    (point_prime_to_coordinates c (as_seq h p)) == 
    point_prime_to_coordinates c (as_seq h p)))

let lemma_pif_to_domain #c h p = 
  let (x, y, z) = point_prime_to_coordinates c (as_seq h p) in 
  let (x3, y3, z3) = fromDomainPoint #c (x, y, z) in 
  lemmaFromDomain #c x;
  lemmaFromDomain #c y;
  lemmaFromDomain #c z;
  lemma_multiplication_not_mod_prime #c x; 
  lemma_multiplication_not_mod_prime #c y;
  lemma_multiplication_not_mod_prime #c z


val lemma_coord: #c: curve -> h3: mem -> q: point c -> Lemma (
   let (r0, r1, r2) = fromDomainPoint #c (point_prime_to_coordinates c (as_seq h3 q)) in 
	let xD = fromDomain_ #c (point_x_as_nat c h3 q) in 
	let yD = fromDomain_ #c (point_y_as_nat c h3 q) in 
	let zD = fromDomain_ #c (point_z_as_nat c h3 q) in 
    r0 == xD /\ r1 == yD /\ r2 == zD)	

let lemma_coord h3 q = ()


inline_for_extraction
val scalarMultiplication_t: #c: curve -> #t:buftype -> p: point c -> result: point c -> 
  scalar: lbuffer_t t uint8 (getScalarLen c) -> 
  tempBuffer: lbuffer uint64 (size 20 *! getCoordinateLenU64 c) ->
  Stack unit
    (requires fun h -> 
      let len = getCoordinateLenU64 c in 
      let prime = getPrime c in 
      live h p /\ live h result /\ live h scalar /\ live h tempBuffer /\
      LowStar.Monotonic.Buffer.all_disjoint [loc p; loc tempBuffer; loc scalar; loc result] /\
      as_nat c h (gsub p (size 0) len) < prime /\ 
      as_nat c h (gsub p len len) < prime /\
      as_nat c h (gsub p (size 2 *! len) len) < prime 
    )
  (ensures fun h0 _ h1 -> 
    let prime = getPrime c in 
    let len = getCoordinateLenU64 c in 
    modifies (loc p |+| loc result |+| loc tempBuffer) h0 h1  /\ 
    
    as_nat c h1 (gsub result (size 0) len) < prime /\ 
    as_nat c h1 (gsub result len len) < prime /\
    as_nat c h1 (gsub result (size 2 *! len) len) < prime /\
    
    modifies (loc p |+| loc result |+| loc tempBuffer) h0 h1 /\
    (
      let x3, y3, z3 = point_x_as_nat c h1 result, point_y_as_nat c h1 result, point_z_as_nat c h1 result in 
      let (xN, yN, zN) = scalar_multiplication #c (as_seq h0 scalar) (point_prime_to_coordinates c (as_seq h0 p)) in 
      x3 == xN /\ y3 == yN /\ z3 == zN 
    ) 
) 


let scalarMultiplication_t #c #t p result scalar tempBuffer  = 
    let h0 = ST.get() in 
  let len = getCoordinateLenU64 c in 
  let q = sub tempBuffer (size 0) (size 3 *! len) in 
  uploadZeroPoint #c q;
  let buff = sub tempBuffer (size 3 *! len) (size 22 *! len) in 
  pointToDomain p result;
    let h2 = ST.get() in 
  montgomery_ladder q result scalar buff;
    admit();
    let h3 = ST.get() in 
    lemma_point_to_domain h0 h2 p result;
    lemma_pif_to_domain #c h2 q;
  norm q result buff; 
    lemma_coord #c  h3 q;
    admit()


let scalarMultiplicationL #c = scalarMultiplication_t #c #MUT
let scalarMultiplicationI #c = scalarMultiplication_t #c #IMMUT
let scalarMultiplicationC #c = scalarMultiplication_t #c #CONST


let scalarMultiplication #c #buf_type p result scalar tempBuffer = 
  match buf_type with 
  |MUT -> scalarMultiplicationL #c p result scalar tempBuffer 
  |IMMUT -> scalarMultiplicationI #c p result scalar tempBuffer
  |CONST -> scalarMultiplicationC #c p result scalar tempBuffer


val uploadBasePoint: #c: curve -> p: point c -> Stack unit 
  (requires fun h -> live h p)
  (ensures fun h0 _ h1 -> 
    let len = getCoordinateLenU64 c in 
    let prime = getPrime c in 
  
    modifies (loc p) h0 h1 /\ 
    as_nat c h1 (gsub p (size 0) len) < prime /\ 
    as_nat c h1 (gsub p len len) < prime /\
    as_nat c h1 (gsub p (size 2 *! len) len) < prime /\
      (
	let x1 = as_nat c h1 (gsub p (size 0) len) in 
	let y1 = as_nat c h1 (gsub p len len) in 
	let z1 = as_nat c h1 (gsub p (size 2 *! len) len) in 

	let baseX, baseY, baseZ = basePoint #c in 
	fromDomain_ #c x1 == baseX /\ fromDomain_ #c y1 == baseY /\ fromDomain_ #c z1 == baseZ
    )
)


let uploadBasePoint #c p = 
  match c with
  |P384 -> 
    let h0 = ST.get() in
    upd p (size 0) (u64 0x3dd0756649c0b528);
    upd p (size 1) (u64 0x20e378e2a0d6ce38);
    upd p (size 2) (u64 0x879c3afc541b4d6e);
    upd p (size 3) (u64 0x6454868459a30eff);
    upd p (size 4) (u64 0x812ff723614ede2b);
    upd p (size 5) (u64 0x4d3aadc2299e1513);


    upd p (size 6) (u64 0x23043dad4b03a4fe);
    upd p (size 7) (u64 0xa1bfa8bf7bb4a9ac);
    upd p (size 8) (u64 0x8bade7562e83b050);
    upd p (size 9) (u64 0xc6c3521968f4ffd9);
    upd p (size 10) (u64 0xdd8002263969a840);
    upd p (size 11) (u64 0x2b78abc25a15c5e9);

    upd p (size 12) (u64 0xffffffff00000001);
    upd p (size 13) (u64 0xffffffff);
    upd p (size 14) (u64 0x1);
    upd p (size 15) (u64 0);
    upd p (size 16) (u64 0);
    upd p (size 17) (u64 0);
    admit()



  |P256 -> 
    let h0 = ST.get() in 
  upd p (size 0) (u64 0x79e730d418a9143c);
  upd p (size 1) (u64 0x75ba95fc5fedb601);
  upd p (size 2) (u64 0x79fb732b77622510);
  upd p (size 3) (u64 0x18905f76a53755c6);

(*
  assert_norm (8784043285714375740 + pow2 64 * 8483257759279461889 + pow2 64 * pow2 64 * 8789745728267363600 + pow2 64 * pow2 64 * pow2 64 * 1770019616739251654 < prime256); 
    assert_norm (8784043285714375740 + pow2 64 * 8483257759279461889 + pow2 64 * pow2 64 * 8789745728267363600 + pow2 64 * pow2 64 * pow2 64 * 1770019616739251654 = 11110593207902424140321080247206512405358633331993495164878354046817554469948); 
  assert_norm(0x6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296 == fromDomain_ #P256 11110593207902424140321080247206512405358633331993495164878354046817554469948);
*) 
  upd p (size 4) (u64 0xddf25357ce95560a);
  upd p (size 5) (u64 0x8b4ab8e4ba19e45c);
  upd p (size 6) (u64 0xd2e88688dd21f325);
  upd p (size 7) (u64 0x8571ff1825885d85);
(* 
  assert_norm(15992936863339206154 + pow2 64 * 10037038012062884956 + pow2 64 * pow2 64 * 15197544864945402661 + pow2 64 * pow2 64 * pow2 64 * 9615747158586711429 < prime256);
  assert_norm (15992936863339206154 + pow2 64 * 10037038012062884956 + pow2 64 * pow2 64 * 15197544864945402661 + pow2 64 * pow2 64 * pow2 64 * 9615747158586711429 = 60359023176204190920225817201443260813112970217682417638161152432929735267850);
  assert_norm (0x4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5 == fromDomain_ #P256 60359023176204190920225817201443260813112970217682417638161152432929735267850);
  *)
  
  upd p (size 8) (u64 0x1);
  upd p (size 9) (u64 0xffffffff00000000);
  upd p (size 10) (u64 0xffffffffffffffff);
  upd p (size 11) (u64 0xfffffffe);
  
  (* 
  assert_norm (1 + pow2 64 * 18446744069414584320 + pow2 64 * pow2 64 * 18446744073709551615 + pow2 64 * pow2 64 * pow2 64 * 4294967294 < prime256);
  assert_norm (1 = fromDomain_ #P256 26959946660873538059280334323183841250350249843923952699046031785985);
  assert_norm (1 + pow2 64 * 18446744069414584320 + pow2 64 * pow2 64 * 18446744073709551615 + pow2 64 * pow2 64 * pow2 64 * 4294967294 = 26959946660873538059280334323183841250350249843923952699046031785985)  *)
  admit()


let scalarMultiplicationWithoutNorm #c p result scalar tempBuffer = 
  let h0 = ST.get() in  
  let len = getCoordinateLenU64 c in 
  let q = sub tempBuffer (size 0) (size 3 *! len) in 
  uploadZeroPoint #c q;
  let buff = sub tempBuffer (size 3 *! len) (size 22 *! len) in 
  pointToDomain p result;
    let h1 = ST.get() in 
    admit();
  montgomery_ladder q result scalar buff;
  copy_point q result;  
    lemma_point_to_domain h0 h1 p result;
    lemma_pif_to_domain #c h1 q
    

let secretToPublic #c result scalar tempBuffer = 
  push_frame(); 
    let len = getCoordinateLenU64 c in 
    let basePoint = create (size 3 *! len) (u64 0) in 
  uploadBasePoint #c basePoint;
    let q = sub tempBuffer (size 0) (size 3 *! len) in 
    let buff = sub tempBuffer (size 3 *! len) (size 22 *! len) in 
  uploadZeroPoint #c q; 
  admit();
  let h1 = ST.get() in 
    lemma_pif_to_domain #c h1 q;
  montgomery_ladder #c q basePoint scalar buff; 
  norm q result buff;  
  pop_frame();
  admit()


let secretToPublicWithoutNorm #c result scalar tempBuffer = 
  push_frame(); 
    let len = getCoordinateLenU64 c in 
    let basePoint = create (size 3 *! len) (u64 0) in 
  uploadBasePoint #c basePoint;
      let q = sub tempBuffer (size 0) (size 3 *! len) in 
      let buff = sub tempBuffer (size 3 *! len) (size 22 *! len) in 
  uploadZeroPoint #c q; 
  admit();
      let h1 = ST.get() in 
      lemma_pif_to_domain #c h1 q; 
  montgomery_ladder #c q basePoint scalar buff; 
  copy_point q result;
  pop_frame()  



