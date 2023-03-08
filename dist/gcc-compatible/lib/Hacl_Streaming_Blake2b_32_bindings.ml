open Ctypes
module Bindings(F:Cstubs.FOREIGN) =
  struct
    open F
    type hacl_Streaming_Blake2b_32_block_state =
      [ `hacl_Streaming_Blake2b_32_block_state ] structure
    let (hacl_Streaming_Blake2b_32_block_state :
      [ `hacl_Streaming_Blake2b_32_block_state ] structure typ) =
      structure "Hacl_Streaming_Blake2b_32_block_state_s"
    let hacl_Streaming_Blake2b_32_block_state_fst =
      field hacl_Streaming_Blake2b_32_block_state "fst" (ptr uint64_t)
    let hacl_Streaming_Blake2b_32_block_state_snd =
      field hacl_Streaming_Blake2b_32_block_state "snd" (ptr uint64_t)
    let _ = seal hacl_Streaming_Blake2b_32_block_state
    type hacl_Streaming_Blake2b_32_state =
      [ `hacl_Streaming_Blake2b_32_state ] structure
    let (hacl_Streaming_Blake2b_32_state :
      [ `hacl_Streaming_Blake2b_32_state ] structure typ) =
      structure "Hacl_Streaming_Blake2b_32_state_s"
    let hacl_Streaming_Blake2b_32_state_block_state =
      field hacl_Streaming_Blake2b_32_state "block_state"
        hacl_Streaming_Blake2b_32_block_state
    let hacl_Streaming_Blake2b_32_state_buf =
      field hacl_Streaming_Blake2b_32_state "buf" (ptr uint8_t)
    let hacl_Streaming_Blake2b_32_state_total_len =
      field hacl_Streaming_Blake2b_32_state "total_len" uint64_t
    let _ = seal hacl_Streaming_Blake2b_32_state
    let hacl_Streaming_Blake2b_32_malloc =
      foreign "Hacl_Streaming_Blake2b_32_malloc"
        (void @-> (returning (ptr hacl_Streaming_Blake2b_32_state)))
    let hacl_Streaming_Blake2b_32_reset =
      foreign "Hacl_Streaming_Blake2b_32_reset"
        ((ptr hacl_Streaming_Blake2b_32_state) @-> (returning void))
    let hacl_Streaming_Blake2b_32_update =
      foreign "Hacl_Streaming_Blake2b_32_update"
        ((ptr hacl_Streaming_Blake2b_32_state) @->
           (ocaml_bytes @-> (uint32_t @-> (returning uint32_t))))
    let hacl_Streaming_Blake2b_32_digest =
      foreign "Hacl_Streaming_Blake2b_32_digest"
        ((ptr hacl_Streaming_Blake2b_32_state) @->
           (ocaml_bytes @-> (returning void)))
    let hacl_Streaming_Blake2b_32_free =
      foreign "Hacl_Streaming_Blake2b_32_free"
        ((ptr hacl_Streaming_Blake2b_32_state) @-> (returning void))
  end