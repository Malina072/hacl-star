module Spec.Kyber.NTT

open Lib.IntTypes
open Lib.Sequence
open Lib.ByteSequence

open FStar.Mul
open FStar.Math.Lemmas
open FStar.Classical

open Spec.Kyber.Params
open Spec.Kyber.Lemmas
open Spec.Kyber.Poly
open Spec.Kyber.Arithmetic

module Seq = Lib.Sequence
module Loops = Lib.LoopCombinators


type reversed (#m:nat) (a:poly m) = bool
type normal (#m:nat) (a:poly m) = bool

unfold inline_for_extraction 
type nttpoly (m:nat) = p:(poly m){reversed p}

unfold inline_for_extraction
type poly (m:nat) =  p:(poly m){normal p}

#reset-options "--z3rlimit 100 --max_fuel 1 --max_ifuel 1 --using_facts_from '* -FStar.Seq'"

val powers: (#m:nat{m>1}) -> (o:field m) -> p:poly m{forall (k:nat{k<params_n}). p.[k] = o ^% k}

let powers #m o = 
  createi params_n (fun i -> o ^% i)


val sum_nth_root_unity_lemma_: #q:nat{q>1} -> o:field q{o ^% params_n = 1 /\ o<>1} -> Lemma (modular_sum_n (powers o) = o *% modular_sum_n (powers o))

let sum_nth_root_unity_lemma_ #q o =
  let p = powers o in
  let s = modular_sum_n p in
  let p' = map (fun x -> o *% x) p in
  let s' = modular_sum_n p' in
  modular_sum_n_mult_distrib_l_lemma p o;
  assert (s'== o *% s);
  assert (forall(k:nat{k<params_n-1}). p'.[k] = p.[k+1]);
  assert (forall(k:nat{k<params_n-1}). (sub p' 0 (params_n-1)).[k] = (sub p 1 (params_n-1)).[k]);
  eq_intro (sub p' 0 (params_n-1)) (sub p 1 (params_n-1));
  eq_elim (sub p' 0 (params_n-1)) (sub p 1 (params_n-1));
  modular_sum_n_simple_lemma p';
  assert(p'.[params_n-1] = o ^% params_n);
  assert(p'.[params_n-1] = p.[0]);
  calc (==) {
    s';
      = {modular_sum_n_simple_lemma p'}
    modular_sum_n (sub p' 0 (params_n-1)) +% p'.[params_n-1];
      = {assert (sub p' 0 (params_n-1) == sub p 1 (params_n-1)); ()}
    modular_sum_n (sub p 1 (params_n-1)) +% p'.[params_n-1];
      = {assert(p'.[params_n-1] = p.[0]); ()}
    p.[0] +% modular_sum_n (sub p 1 (params_n-1));
      = {}
    modular_sum_n p;
      = {}
    s;
    }


#reset-options "--z3rlimit 200 --max_fuel 0 --max_ifuel 0 --using_facts_from '* -FStar.Seq'"

val sum_nth_root_unity_lemma: #q:nat{q>1} -> o:field q{o ^% params_n = 1 /\ o<>1 /\ is_invertible (o-%1)} -> Lemma (modular_sum_n (powers o) = 0)

let sum_nth_root_unity_lemma #q o =
  let inv = invert_mod (o-%1) in
  let p = powers o in
  let s = modular_sum_n p in
  let p' = map (fun x -> o *% x) p in
  let s' = modular_sum_n p' in

  sum_nth_root_unity_lemma_ o;
  assert (s = modular_mul #q o s);
  modular_mul_one_lemma #q s;
  assert (modular_add #q (modular_mul #q (q-1) s) (modular_mul #q 1 s) = (q-1) *% s +% o *% s);
  modular_mul_add_distrib_lemma #q (q-1) 1 s;
  modular_mul_add_distrib_lemma #q (q-1) o s;
  assert (modular_add #q (q-1) 1 = q % q);
  assert (modular_add #q (q-1) 1 = 0);
  assert (((q-1) +% o ) *% s = 0);
  assert((o -% 1) *% s = 0);
  calc (==) {
    s;
      = {modular_mul_one_lemma #q s}
    1 *% s;
      = {}
    (inv *% (o-%1)) *% s;
      = {modular_mul_associativity_lemma inv (o-%1) s}
    inv *% ((o-%1) *% s);
      = {}
    inv *% 0;
      = {}
    0 % q;
      = {modulo_lemma 0 q}
    0;
  }


#reset-options "--z3rlimit 200 --max_fuel 0 --max_ifuel 0 --using_facts_from '* -FStar.Seq'"

  
val ntt: (#m:nat{m>1}) -> (omega:field m) -> (psi:field m(*{forall (i:nat{i<params_n}). is_invertible (psi ^% i)}*)) -> (p:poly m) -> Tot (p':nttpoly m{forall (k:nat{k<params_n}). p'.[k] = modular_sum_n (mapi (fun j g -> (psi^%j) *% g *% (omega ^% (k*j))) p)})

let ntt #m omega psi p = 
  let (p':nttpoly m) = createi params_n (fun k -> modular_sum_n (mapi (fun j g -> (psi^%j) *% g *% (omega ^% (k*j))) p)) in
  p'
  

val nttinv: (#m:nat{m>params_n /\ is_invertible #m params_n}) -> omegainv:field m -> psiinv:field m -> (p:nttpoly m) -> Tot (p':poly m{forall (k:nat{k<params_n}). p'.[k] = (invert_mod #m params_n) *% (psiinv^%k) *% modular_sum_n (mapi (fun j g -> g *% (omegainv ^% (k*j))) p)})

let nttinv #m omegainv psiinv p =
  let (p':poly m) = createi params_n (fun k -> (invert_mod #m params_n) *% (psiinv^%k) *% modular_sum_n (mapi (fun j g -> g *% (omegainv ^% (k*j))) p)) in p'

#reset-options "--z3rlimit 1000 --max_fuel 0 --max_ifuel 0 --using_facts_from '* -FStar.Seq'"


val ntt_inversion_sublemma_kj: (#m:nat{m>params_n /\ is_invertible #m params_n}) -> omega:field m{omega ^% params_n = 1 /\ omega <>1 /\ is_invertible omega} -> psi:field m{is_invertible psi} -> p:poly m -> k:nat{k<params_n} -> j:nat{j<params_n} -> Lemma(let pntt = ntt omega psi p in let omegainv = invert_mod omega in let sk = mapi (fun l g -> g *% (omegainv ^% (k*l))) pntt in let l = createi params_n (fun x -> x) in let f = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in sk.[j] = modular_sum_n (map (fun y -> f j y) l))


let ntt_inversion_sublemma_kj #m omega psi p k j =
  let omegainv = invert_mod omega in
  let pntt = ntt omega psi p in
  let sk = mapi (fun l g -> g *% (omegainv ^% (k*l))) pntt in
  let l = createi params_n (fun x -> x) in
  let f = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in
  let s' = mapi (fun i g -> (psi^%i) *% g *% (omega ^% (j*i))) p in
  let s = map (fun y -> y *% (omegainv ^% (k*j))) s' in
  assert(sk.[j] = pntt.[j] *% (omegainv ^% (k*j)));
  assert(pntt.[j] = modular_sum_n (mapi (fun i g -> (psi^%i) *% g *% (omega ^% (j*i))) p));
  assert(sk.[j] = (modular_sum_n (mapi (fun i g -> (psi^%i) *% g *% (omega ^% (j*i))) p)) *% (omegainv ^% (k*j)));
  modular_sum_n_mult_distrib_r_lemma (mapi (fun i g -> (psi^%i) *% g *% (omega ^% (j*i))) p) (omegainv ^% (k*j));
  assert(sk.[j] = modular_sum_n s);
  assert(forall (d:nat{d<params_n}). s.[d] = ((psi^%d) *% p.[d] *% (omega ^% (j*d))) *% (omegainv ^% (k*j)));
  let customprop (d:nat{d<params_n}) : GTot Type0 =
    s.[d] = f j d in
  let customlemma (d:nat{d<params_n}) : Lemma (customprop d) =
    modular_mul_associativity_lemma ((psi^%d) *% p.[d]) (omega ^% (j*d)) (omegainv ^% (k*j))
  in
  forall_intro customlemma;
  eq_intro s (Seq.map (fun y -> f j y) l);
  eq_elim s (Seq.map (fun y -> f j y) l)

#reset-options "--z3rlimit 300 --max_fuel 0 --max_ifuel 0 --using_facts_from '* -FStar.Seq'"
 
open FStar.Tactics
(*
val ntt_inversion_sublemma_kjd': (#m:nat{m>params_n /\ is_invertible #m params_n}) -> omega:field m{omega ^% params_n = 1 /\ omega <>1 /\ is_invertible omega} -> psi:field m{is_invertible psi} -> p:poly m -> k:nat{k<params_n} -> j:nat{j<params_n} -> d:nat{d<params_n} -> Lemma(let omegainv = invert_mod omega in let l = createi params_n (fun x -> x) in let f = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in let s = Seq.map (fun x -> f x j) l in s.[d] = (psi^%j) *% p.[j] *% (((omega ^% (j)) *% (omegainv^%(k))) ^% d))
*)

val ntt_inversion_sublemma_kjd': #m:nat{m>1} -> omega:field m{omega ^% params_n = 1} -> omegainv: field m{omega *% omegainv = 1} -> psi:field m -> p:poly m -> k:nat{k<params_n} -> j:nat{j<params_n} -> d:nat{d<params_n} -> Lemma(let l = createi params_n (fun x -> x) in let f = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in let s = Seq.map (fun x -> f x j) l in s.[d] = (psi^%j) *% p.[j] *% (((omega ^% ((j-k)%params_n))(* *% (omegainv^%(k))*)) ^% d))

let ntt_inversion_sublemma_kjd' #m omega omegainv psi p k j d =
  let l = createi params_n (fun x -> x) in
  let f = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in
  let s = Seq.map (fun x -> f x j) l in
  assert(s.[d] = f d j);
  assert(s.[d] = (psi^%j) *% p.[j] *% ((omega ^% (d*j)) *% (omegainv^%(k*d))));
  modular_exp_exp_lemma omega j d;
  modular_exp_exp_lemma omegainv k d;
  modular_exp_lemma_expand (omega ^%j) (omegainv ^% k) d;
  assert(s.[d] = (psi^%j) *% p.[j] *% (((omega ^% (j)) *% (omegainv^%(k))) ^% d));
  if j<k then begin
    modular_mul_one_lemma (omegainv^%k);
    assert (omegainv^%k = (omegainv ^% k) *% (omega ^% params_n));
    modular_exp_morphism_lemma omega k (params_n - k);
    modular_mul_associativity_lemma (omegainv ^% k) (omega ^%k) (omega ^% (params_n-k));
    modular_exp_lemma_inv omegainv omega k;
    modular_mul_one_lemma (omega^%(params_n-k));
    modular_exp_morphism_lemma omega j (params_n-k);
    assert(params_n-k+j = (j-k) % params_n);
    assert ((omega^%j) *% (omegainv^%k) = omega ^% ((j-k) % params_n))
    end
  else if j=k then begin
    modular_exp_lemma_inv omega omegainv k;
    modular_exp_lemma_zero omega
  end
  else begin
    modular_exp_morphism_lemma omega (j-k) k;
    modular_mul_associativity_lemma (omega ^% (j-k)) (omega ^% k) (omegainv ^% k);
    modular_exp_lemma_inv omega omegainv k;
    modular_mul_one_lemma (omega ^% (j-k));
    assert(j-k = (j-k) % params_n)
    end


#reset-options "--z3rlimit 200 --max_fuel 1 --max_ifuel 1 --using_facts_from '* -FStar.Seq'"

val modular_sum_n_one_non_zero_coeff:#n:size_nat{n>=0} -> #q:nat{q>0} -> k:nat{k<n}->l:set n q{forall (d:nat{d<n}). d <> k ==> l.[d] = 0} -> Lemma (ensures modular_sum_n l = l.[k]) (decreases k)

let rec modular_sum_n_one_non_zero_coeff #n #q k l =
  if k = 0 then begin
    modular_sum_n_all_zero (sub l 1 (n-1));
    assert (modular_sum_n l = l.[0] +% modular_sum_n (sub l 1 (n-1)));
    assert (modular_sum_n l = l.[0]) end
  else begin
    assert(modular_sum_n l = l.[0] +% modular_sum_n (sub l 1 (n-1)));
    assert(l.[0]=0);
    modular_sum_n_one_non_zero_coeff (k-1) (sub l 1 (n-1));
    assert(modular_sum_n (sub l 1 (n-1)) = l.[k]);
    assert(modular_sum_n l = 0 +% l.[k])
  end
  
val modular_sum_n_all_1_is_n_mod_q: #n:size_nat{n>=0} -> #q:nat{q > 1} -> l:set n q{forall(d:nat{d<n}). l.[d] = 1} -> Lemma (ensures modular_sum_n l = (n % q)) (decreases n)


let rec modular_sum_n_all_1_is_n_mod_q #n #q l =
  let _lemma (a:pos) : Lemma ((1+(a-1)) % q = a % q) =
    ()
  in
  if n = 0 then () 
  else begin
    assert(modular_sum_n #n #q l = l.[0] +% (modular_sum_n #(n-1) #q (sub l 1 (n-1))));
    modular_sum_n_all_1_is_n_mod_q #(n-1) #q (sub l 1 (n-1));
    assert(1%q = 1);
    assert(modular_sum_n l = (1%q) +% ((n-1) % q));
    modulo_distributivity 1 (n-1) q;
    assert(modular_sum_n l = (1 + (n-1)) %q);
    _lemma n;
    assert(modular_sum_n l = n % q)
  end

#reset-options "--z3rlimit 2000 --max_fuel 0 --max_ifuel 0 --using_facts_from '* -FStar.Seq'"

val ntt_inversion_sublemma_kj'_: (#m:nat{m>1}) -> omega:field m{omega ^% params_n = 1}(* /\ (forall (nn:field params_n). (omega ^% nn = 1 ==> nn = 0) /\ (~(is_invertible((omega ^% nn) -%1)) ==> nn = 0))}*) -> omegainv:field m{omega *% omegainv = 1} -> psi:field m -> p:poly m -> k:nat{k<params_n} -> j:nat{j<params_n} -> Lemma(let l = createi params_n (fun x -> x) in let f : field params_n -> field params_n -> field m = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in let sk' = Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l in let pows = powers (omega ^% ((j-k)%params_n)) in sk'.[j] = ((psi ^%j) *% p.[j]) *% modular_sum_n pows)

let ntt_inversion_sublemma_kj'_ #m omega omegainv psi p k j =
  let l = createi params_n (fun x -> x) in
  let f : field params_n -> field params_n -> field m = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in
  let sk' = Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l in
  assert (sk'.[j] = modular_sum_n (Seq.map (fun x -> f x j) l));
  let s = Seq.map (fun x -> f x j) l in
  let pows = powers (omega ^% (j-k) % params_n) in
  assert (sk'.[j] = modular_sum_n s);
  let customprop (d:nat{d<params_n}) : Type0 =
    s.[d] = (psi ^%j) *% p.[j] *% pows.[d] in
  let customlemma (d:nat{d<params_n}) : Lemma (customprop d) =
    ntt_inversion_sublemma_kjd' omega omegainv psi p k j d in
  FStar.Classical.forall_intro customlemma;
  eq_intro s (Seq.map (fun y -> ((psi ^%j) *% p.[j]) *%y) pows);
  eq_elim s (Seq.map (fun y -> ((psi ^%j) *% p.[j]) *%y) pows);
  modular_sum_n_mult_distrib_l_lemma pows ((psi ^%j) *% p.[j]);  
  assert( modular_sum_n s = ((psi ^%j) *% p.[j]) *% modular_sum_n pows);
  assert(sk'.[j] = ((psi ^%j) *% p.[j]) *% modular_sum_n pows)

#reset-options "--z3rlimit 2000 --max_fuel 0 --max_ifuel 0 --using_facts_from '* -FStar.Seq'"

val ntt_inversion_sublemma_kj': (#m:nat{m>1}) -> omega:field m{omega ^% params_n = 1 /\ (forall (nn:field params_n). (omega ^% nn = 1 ==> nn = 0) /\ (~(is_invertible((omega ^% nn) -%1)) ==> nn = 0))} -> omegainv:field m{omega *% omegainv = 1} -> psi:field m -> p:poly m -> k:nat{k<params_n} -> j:nat{j<params_n /\ j <>k} -> Lemma(let l = createi params_n (fun x -> x) in let f : field params_n -> field params_n -> field m = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in let sk' = Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l in let pows = powers (omega ^% ((j-k)%params_n)) in sk'.[j] = 0)

let ntt_inversion_sublemma_kj' #m omega omegainv psi p k j =
    let l = createi params_n (fun x -> x) in
    let f : field params_n -> field params_n -> field m = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in
    let sk' = Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l in
    let h = ((j-k)%params_n) in
    let pows = powers (omega ^% (j-k) % params_n) in
    ntt_inversion_sublemma_kj'_ omega omegainv psi p k j;
    assert (h <> 0);
    modular_exp_exp_lemma omega h params_n;
    modular_exp_exp_lemma omega params_n h;
    assert ((omega ^% h) ^% params_n = (omega ^% params_n) ^% h);
    modular_exp_lemma_one2 #m h; 
    let o = (omega ^% h) in    
    sum_nth_root_unity_lemma o;
    assert (modular_sum_n pows = 0);
    assert (sk'.[j] = (((psi ^%j) *% p.[j]) * 0) % m);
    assert(sk'.[j] = 0 % m);
    assert(sk'.[j] = 0)
  
#reset-options "--z3rlimit 2000 --max_fuel 0 --max_ifuel 0 --using_facts_from '* -FStar.Seq'"

val ntt_inversion_sublemma_kk': (#m:nat{m>params_n}) -> omega:field m{omega ^% params_n = 1} (* /\ (forall (nn:field params_n). (omega ^% nn = 1 ==> nn = 0) /\ (~(is_invertible((omega ^% nn) -%1)) ==> nn = 0))}*) -> omegainv:field m{omega *% omegainv = 1} -> psi:field m -> p:poly m -> k:nat{k<params_n} -> Lemma(let l = createi params_n (fun x -> x) in let f : field params_n -> field params_n -> field m = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in let sk' = Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l in sk'.[k] = ((psi ^% k) *% p.[k]) *% params_n)

let ntt_inversion_sublemma_kk' #m omega omegainv psi p k = 
  let l = createi params_n (fun x -> x) in
    let f : field params_n -> field params_n -> field m = (fun (x:nat{x<params_n}) (y:nat{y<params_n}) -> (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x)))) in
    let sk' = Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l in
    let h = ((k-k)%params_n) in
   let pows = powers (omega ^% (k-k) % params_n) in
   ntt_inversion_sublemma_kj'_ omega omegainv psi p k k;
   let customprop' (d:field params_n) : Type0 = (pows.[d] = 1) in
  let customlemma' (d:field params_n) : Lemma (customprop' d) =
    assert ((k-k)%params_n = 0);
    modular_exp_lemma_zero omega;
    modular_exp_lemma_one2 #m d
  in  
  FStar.Classical.forall_intro customlemma';
  modular_sum_n_all_1_is_n_mod_q pows;
  admit();
  assert(params_n % m = params_n);
  assert(sk'.[k] = ((psi ^%k) *% p.[k]) *% params_n)


#reset-options "--z3rlimit 1000 --max_fuel 3 --max_ifuel 3 --using_facts_from '* -FStar.Seq'"

val ntt_inversion_lemma_k: (#m:nat{m>params_n /\ is_invertible #m params_n}) -> omega:field m{omega ^% params_n = 1 /\ (forall (nn:field params_n). (omega ^% nn = 1 ==> nn = 0) /\ (~(is_invertible((omega ^% nn) -%1)) ==> nn = 0))} -> omegainv:field m{omega *% omegainv = 1} -> psi:field m ->psiinv:field m{psi *% psiinv = 1} -> p:poly m -> k:nat{k<params_n} -> Lemma(let p' = nttinv (invert_mod omega) (invert_mod psi) (ntt omega psi p) in p'.[k] = p.[k])

val fubini: #m:pos -> f:(field params_n -> field params_n -> field m) -> l:set params_n params_n -> Lemma (modular_sum_n (Seq.map (fun x -> modular_sum_n (Seq.map (fun y -> f x y) l)) l) = modular_sum_n (Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l))

let fubini #m f l =
  modular_fubini_lemma f l l (Seq.map (fun x -> modular_sum_n (Seq.map (fun y -> f x y) l)) l) (Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l)

#reset-options "--z3rlimit 1000 --max_fuel 0 --max_ifuel 0 --using_facts_from '* -FStar.Seq'"

open FStar.Tactics

let ntt_inversion_lemma_k #m omega omegainv psi psiinv p k =
  let ninv = invert_mod params_n in
  let pntt = ntt omega psi p in
  let p' = nttinv omegainv psiinv pntt in
  assert(p'.[k] = ninv *% (psiinv^%k) *% modular_sum_n (mapi (fun j g -> g *% (omegainv ^% (k*j))) pntt));
  let l = createi params_n (fun x -> x) in
  let f (x:nat{x<params_n}) (y:nat{y<params_n}) : field m = (psi^%y) *% p.[y] *% ((omega ^% (x*y)) *% (omegainv^%(k*x))) in
  let sk = mapi (fun j g -> g *% (omegainv ^% (k*j))) pntt in
  let sk' = Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l in
  assert(p'.[k] = ninv *% (psiinv^%k) *% modular_sum_n sk);
  let customprop (j:nat{j<params_n}) : GTot Type0 =
    sk.[j] = modular_sum_n (Seq.map (fun y -> f j y) l) in
  let customlemma (j:nat{j<params_n}) : Lemma (customprop j) =
    ntt_inversion_sublemma_kj omega psi p k j in
  FStar.Classical.forall_intro customlemma;
  eq_intro sk (Seq.map (fun x -> modular_sum_n (Seq.map (fun y -> f x y) l)) l);
  eq_elim sk (Seq.map (fun x -> modular_sum_n (Seq.map (fun y -> f x y) l)) l);
  assert(p'.[k] = ninv *% (psiinv^%k) *% modular_sum_n (Seq.map (fun x -> modular_sum_n (Seq.map (fun y -> f x y) l)) l));
  //fubini #m f l;
  assert(modular_sum_n (Seq.map (fun x -> modular_sum_n (Seq.map (fun y -> f x y) l)) l) = modular_sum_n (Seq.map (fun y -> modular_sum_n (Seq.map (fun x -> f x y) l)) l)) by
   (apply_lemma (`fubini));
  admit();
  assert (p'.[k] = ninv *% (psiinv^%k) *% modular_sum_n sk');
  let customprop' (j:nat{j<params_n /\ j<>k}) : Type0 =
    sk'.[j] = 0 in
  let customlemma' (j:nat{j<params_n}) : Lemma (customprop' j) =
    ntt_inversion_sublemma_kj' omega omegainv psi p k j
  in
  FStar.Classical.forall_intro customlemma';
  modular_sum_n_one_non_zero_coeff k sk';
  ntt_inversion_sublemma_kk' omega omegainv psi p k;
  assert(modular_sum_n sk' = ((psi ^%k) *% p.[k]) *% params_n);
  admit()



(*
  assert(p'.[k] = (invert_mod #m params_n) *% (psiinv^%k) *% modular_sum_n (Seq.map (fun y -> modular_sum_n (map (fun x -> f x y) l)) l));
   admit()


val ntt_inversion_lemma: (#m:nat{m>params_n /\ is_invertible #m params_n}) -> omega:field m{omega ^% params_n = 1 /\ omega <>1 /\ is_invertible omega} -> psi:field m{is_invertible psi} -> p:poly m -> k:nat{k<params_n} -> Lemma(nttinv (invert_mod omega) (invert_mod psi) (ntt omega psi p) == p)

let ntt_inversion_lemma #m omega psi p k =
  let omegainv = invert_mod omega in
  let psiinv = invert_mod psi in
  let pntt = ntt omega psi p in
  let p' = nttinv omegainv psiinv pntt in

  assert(forall (k:nat{k<params_n}). p'.[k] = (invert_mod #m params_n) *% (psiinv^%k) *% modular_sum_n (mapi (fun j g -> g *% (omegainv ^% (k*j))) pntt));
  
  admit()
        
  
*)
