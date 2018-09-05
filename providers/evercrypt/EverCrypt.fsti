module EverCrypt

open FStar.HyperStack.ST
open EverCrypt.Helpers
open EverCrypt.Specs

module B = LowStar.Buffer

/// Hash algorithms

include EverCrypt.Hash

/// Curve25519

val x25519: dst:uint8_p -> secret:uint8_p -> base:uint8_p ->
  Stack unit curve_x25519_pre curve_x25519_post

/// Random sampling

val random_init: unit ->
  ST uint32_t random_init_pre random_init_post

val random_sample: len:uint32_t -> out:uint8_p ->
  ST unit random_sample_pre random_sample_post

val random_cleanup: unit ->
  ST unit random_cleanup_pre random_cleanup_post

/// AES block function

[@CAbstractStruct]
val aes128_key_s: Type0

let aes128_key = B.pointer aes128_key_s

val aes128_create: key:uint8_p ->
  ST aes128_key aes128_create_pre aes128_create_post

val aes128_compute: key:aes128_key ->
  plain: uint8_p -> cipher:uint8_p ->
  ST unit aes128_compute_pre aes128_compute_post

val aes128_free: aes128_key ->
  ST unit aes128_free_pre aes128_free_post

[@CAbstractStruct]
val aes256_key_s : Type0

let aes256_key = B.pointer aes256_key_s

val aes256_create: key:uint8_p ->
  ST aes256_key aes256_create_pre aes256_create_post

val aes256_compute: key:aes256_key ->
  plain: uint8_p -> cipher:uint8_p ->
  ST unit aes256_compute_pre aes256_compute_post

val aes256_free: aes256_key ->
  ST unit aes256_free_pre aes256_free_post

/// ChaCha20

val chacha20: key:uint8_p -> iv:uint8_p -> ctr: uint32_t ->
  plain: uint8_p -> len: uint32_t -> cipher: uint8_p ->
  Stack unit chacha20_pre chacha20_post

/// AES-GCM

val aes128_gcm_encrypt: key:uint8_p -> iv:uint8_p ->
  ad:uint8_p -> adlen:uint32_t ->
  plain:uint8_p -> len:uint32_t ->
  cipher: uint8_p -> tag:uint8_p ->
  ST unit aes256_gcm_encrypt_pre aes256_gcm_encrypt_post

val aes128_gcm_decrypt: key:uint8_p -> iv:uint8_p ->
  ad:uint8_p -> adlen:uint32_t ->
  plain:uint8_p -> len:uint32_t ->
  cipher: uint8_p -> tag:uint8_p ->
  ST uint32_t aes128_gcm_decrypt_pre aes128_gcm_decrypt_post

val aes256_gcm_encrypt: key:uint8_p -> iv:uint8_p ->
  ad:uint8_p -> adlen:uint32_t ->
  plain:uint8_p -> len:uint32_t ->
  cipher: uint8_p -> tag:uint8_p ->
  ST unit aes256_gcm_encrypt_pre aes256_gcm_encrypt_post

val aes256_gcm_decrypt: key:uint8_p -> iv:uint8_p ->
  ad:uint8_p -> adlen:uint32_t ->
  plain:uint8_p -> len:uint32_t ->
  cipher: uint8_p -> tag:uint8_p ->
  ST uint32_t aes256_gcm_decrypt_pre aes256_gcm_decrypt_post

/// Chacha20-Poly1305

val chacha20_poly1305_encrypt: key:uint8_p -> iv:uint8_p ->
  ad:uint8_p -> adlen:uint32_t ->
  plain:uint8_p -> len:uint32_t ->
  cipher: uint8_p -> tag:uint8_p ->
  ST unit chacha20_poly1305_encrypt_pre chacha20_poly1305_encrypt_post

val chacha20_poly1305_decrypt: key:uint8_p -> iv:uint8_p ->
  ad:uint8_p -> adlen:uint32_t ->
  plain:uint8_p -> len:uint32_t ->
  cipher: uint8_p -> tag:uint8_p ->
  ST uint32_t chacha20_poly1305_decrypt_pre chacha20_poly1305_decrypt_post

/// AEAD

type aead_alg =
  | AES128_GCM
  | AES256_GCM
  | CHACHA20_POLY1305

[@CAbstractStruct]
val aead_state_s: Type0

let aead_state = B.pointer aead_state_s

val aead_create: alg:aead_alg -> key:uint8_p ->
  ST aead_state aead_create_pre aead_create_post

val aead_encrypt: key:aead_state -> iv:uint8_p ->
  ad:uint8_p -> adlen:uint32_t ->
  plain:uint8_p -> len:uint32_t ->
  cipher:uint8_p -> tag:uint8_p ->
  ST unit aead_encrypt_pre aead_encrypt_post

val aead_decrypt: key:aead_state -> iv:uint8_p ->
  ad:uint8_p -> adlen:uint32_t ->
  plain:uint8_p -> len:uint32_t ->
  cipher:uint8_p -> tag:uint8_p ->
  ST uint32_t aead_decrypt_pre aead_decrypt_post

val aead_free: aead_state ->
  ST unit aead_free_pre aead_free_post

/// DH

[@CAbstractStruct]
val dh_state_s: Type0

let dh_state = B.pointer dh_state_s

val dh_load_group:
  dh_p: uint8_p ->
  dh_p_len: uint32_t ->
  dh_g: uint8_p ->
  dh_g_len: uint32_t ->
  dh_q: uint8_p ->
  dh_q_len: uint32_t ->
  ST dh_state
  (requires fun h0 -> False)
  (ensures fun h0 _ h1 -> True)

val dh_free_group:
  st: dh_state ->
  ST unit
  (requires fun h0 -> False)
  (ensures fun h0 _ h1 -> True)

val dh_keygen:
  st: dh_state ->
  out: uint8_p ->
  ST uint32_t
  (requires fun h0 -> False)
  (ensures fun h0 _ h1 -> True)

val dh_compute:
  st: dh_state ->
  public: uint8_p ->
  public_len: uint32_t ->
  out: uint8_p ->
  ST uint32_t
  (requires fun h0 -> False)
  (ensures fun h0 _ h1 -> True)

/// ECDH

type ec_curve =
  | ECC_P256
  | ECC_P384
  | ECC_P521
  | ECC_X25519
  | ECC_X448

[@CAbstractStruct]
val ecdh_state_s: Type0

let ecdh_state = B.pointer ecdh_state_s

val ecdh_load_curve:
  g: ec_curve ->
  ST ecdh_state
  (requires fun h0 -> False)
  (ensures fun h0 _ h1 -> True)

val ecdh_free_curve:
  st: ecdh_state ->
  ST unit
  (requires fun h0 -> False)
  (ensures fun h0 _ h1 -> True)

val ecdh_keygen:
  st: ecdh_state ->
  outx: uint8_p ->
  outy: uint8_p ->
  ST unit
  (requires fun h0 -> False)
  (ensures fun h0 _ h1 -> True)

val ecdh_compute:
  st: ecdh_state ->
  inx: uint8_p ->
  iny: uint8_p ->
  out: uint8_p ->
  ST uint32_t
  (requires fun h0 -> False)
  (ensures fun h0 _ h1 -> True)
