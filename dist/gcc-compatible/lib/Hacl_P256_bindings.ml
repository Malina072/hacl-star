open Ctypes
module Bindings(F:Cstubs.FOREIGN) =
  struct
    open F
    module Hacl_Spec_applied = (Hacl_Spec_bindings.Bindings)(Hacl_Spec_stubs)
    open Hacl_Spec_applied
    let hacl_Impl_P256_LowLevel_sub4_il =
      foreign "Hacl_Impl_P256_LowLevel_sub4_il"
        ((ptr uint64_t) @->
           ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning uint64_t))))
      
    let hacl_Impl_P256_LowLevel_shift_256_impl =
      foreign "Hacl_Impl_P256_LowLevel_shift_256_impl"
        ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning void)))
      
    let hacl_Impl_P256_LowLevel_toUint8 =
      foreign "Hacl_Impl_P256_LowLevel_toUint8"
        ((ptr uint64_t) @-> (ocaml_bytes @-> (returning void)))
      
    let hacl_Impl_P256_LowLevel_changeEndian =
      foreign "Hacl_Impl_P256_LowLevel_changeEndian"
        ((ptr uint64_t) @-> (returning void))
      
    let hacl_Impl_P256_LowLevel_toUint64ChangeEndian =
      foreign "Hacl_Impl_P256_LowLevel_toUint64ChangeEndian"
        (ocaml_bytes @-> ((ptr uint64_t) @-> (returning void)))
      
    let hacl_Impl_ECDSA_MontgomeryMultiplication_reduction_prime_2prime_order
      =
      foreign
        "Hacl_Impl_ECDSA_MontgomeryMultiplication_reduction_prime_2prime_order"
        ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning void)))
      
    let hacl_Impl_SolinasReduction_solinas_reduction_impl =
      foreign "Hacl_Impl_SolinasReduction_solinas_reduction_impl"
        ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning void)))
      
    let hacl_Impl_P256_PointAdd_point_add =
      foreign "Hacl_Impl_P256_PointAdd_point_add"
        ((ptr uint64_t) @->
           ((ptr uint64_t) @->
              ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning void)))))
      
    let hacl_Impl_P256_Core_norm =
      foreign "Hacl_Impl_P256_Core_norm"
        ((ptr uint64_t) @->
           ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning void))))
      
    let hacl_Impl_P256_Core_scalarMultiplicationWithoutNorm =
      foreign "Hacl_Impl_P256_Core_scalarMultiplicationWithoutNorm"
        ((ptr uint64_t) @->
           ((ptr uint64_t) @->
              (ocaml_bytes @-> ((ptr uint64_t) @-> (returning void)))))
      
    let hacl_Impl_P256_Core_secretToPublicWithoutNorm =
      foreign "Hacl_Impl_P256_Core_secretToPublicWithoutNorm"
        ((ptr uint64_t) @->
           (ocaml_bytes @-> ((ptr uint64_t) @-> (returning void))))
      
    let hacl_Impl_P256_Signature_Common_bufferToJac =
      foreign "Hacl_Impl_P256_Signature_Common_bufferToJac"
        ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning void)))
      
    let hacl_Impl_P256_Signature_Common_isPointAtInfinityPublic =
      foreign "Hacl_Impl_P256_Signature_Common_isPointAtInfinityPublic"
        ((ptr uint64_t) @-> (returning bool))
      
    let hacl_Impl_P256_Signature_Common_verifyQValidCurvePoint =
      foreign "Hacl_Impl_P256_Signature_Common_verifyQValidCurvePoint"
        ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning bool)))
      
    let hacl_Impl_P256_DH_ecp256dh_i =
      foreign "Hacl_Impl_P256_DH_ecp256dh_i"
        (ocaml_bytes @-> (ocaml_bytes @-> (returning uint64_t)))
      
    let hacl_Impl_P256_DH_ecp256dh_r =
      foreign "Hacl_Impl_P256_DH_ecp256dh_r"
        (ocaml_bytes @->
           (ocaml_bytes @-> (ocaml_bytes @-> (returning uint64_t))))
      
    let hacl_Impl_P256_Compression_computeYFromX =
      foreign "Hacl_Impl_P256_Compression_computeYFromX"
        ((ptr uint64_t) @->
           ((ptr uint64_t) @-> (uint64_t @-> (returning void))))
      
    let hacl_Impl_ECDSA_MM_Exponent_montgomery_ladder_exponent =
      foreign "Hacl_Impl_ECDSA_MM_Exponent_montgomery_ladder_exponent"
        ((ptr uint64_t) @-> (returning void))
      
    let hacl_Impl_ECDSA_MM_Exponent_fromDomainImpl =
      foreign "Hacl_Impl_ECDSA_MM_Exponent_fromDomainImpl"
        ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning void)))
      
    let hacl_Impl_ECDSA_MM_Exponent_multPowerPartial =
      foreign "Hacl_Impl_ECDSA_MM_Exponent_multPowerPartial"
        ((ptr uint64_t) @->
           ((ptr uint64_t) @->
              ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning void)))))
      
    let hacl_Impl_ECDSA_P256_Verification_Agile_isMoreThanZeroLessThanOrderMinusOne
      =
      foreign
        "Hacl_Impl_ECDSA_P256_Verification_Agile_isMoreThanZeroLessThanOrderMinusOne"
        ((ptr uint64_t) @-> (returning bool))
      
    let hacl_Impl_ECDSA_P256_Verification_Agile_compare_felem_bool =
      foreign "Hacl_Impl_ECDSA_P256_Verification_Agile_compare_felem_bool"
        ((ptr uint64_t) @-> ((ptr uint64_t) @-> (returning bool)))
      
    let hacl_Impl_ECDSA_P256_Signature_Agile_ecdsa_signature_core =
      foreign "Hacl_Impl_ECDSA_P256_Signature_Agile_ecdsa_signature_core"
        (spec_ECDSA_hash_alg_ecdsa @->
           ((ptr uint64_t) @->
              ((ptr uint64_t) @->
                 (uint32_t @->
                    (ocaml_bytes @->
                       ((ptr uint64_t) @->
                          (ocaml_bytes @-> (returning uint64_t))))))))
      
    let hacl_Interface_P256_ecdsa_sign_p256_sha2 =
      foreign "Hacl_Interface_P256_ecdsa_sign_p256_sha2"
        (ocaml_bytes @->
           (uint32_t @->
              (ocaml_bytes @->
                 (ocaml_bytes @-> (ocaml_bytes @-> (returning uint64_t))))))
      
    let hacl_Interface_P256_ecdsa_sign_p256_sha384 =
      foreign "Hacl_Interface_P256_ecdsa_sign_p256_sha384"
        (ocaml_bytes @->
           (uint32_t @->
              (ocaml_bytes @->
                 (ocaml_bytes @-> (ocaml_bytes @-> (returning uint64_t))))))
      
    let hacl_Interface_P256_ecdsa_sign_p256_sha512 =
      foreign "Hacl_Interface_P256_ecdsa_sign_p256_sha512"
        (ocaml_bytes @->
           (uint32_t @->
              (ocaml_bytes @->
                 (ocaml_bytes @-> (ocaml_bytes @-> (returning uint64_t))))))
      
    let hacl_Interface_P256_ecdsa_sign_p256_without_hash =
      foreign "Hacl_Interface_P256_ecdsa_sign_p256_without_hash"
        (ocaml_bytes @->
           (uint32_t @->
              (ocaml_bytes @->
                 (ocaml_bytes @-> (ocaml_bytes @-> (returning uint64_t))))))
      
    let hacl_Interface_P256_ecdsa_verif_p256_sha2 =
      foreign "Hacl_Interface_P256_ecdsa_verif_p256_sha2"
        (uint32_t @->
           (ocaml_bytes @->
              (ocaml_bytes @->
                 (ocaml_bytes @-> (ocaml_bytes @-> (returning bool))))))
      
    let hacl_Interface_P256_ecdsa_verif_p256_sha384 =
      foreign "Hacl_Interface_P256_ecdsa_verif_p256_sha384"
        (uint32_t @->
           (ocaml_bytes @->
              (ocaml_bytes @->
                 (ocaml_bytes @-> (ocaml_bytes @-> (returning bool))))))
      
    let hacl_Interface_P256_ecdsa_verif_p256_sha512 =
      foreign "Hacl_Interface_P256_ecdsa_verif_p256_sha512"
        (uint32_t @->
           (ocaml_bytes @->
              (ocaml_bytes @->
                 (ocaml_bytes @-> (ocaml_bytes @-> (returning bool))))))
      
    let hacl_Interface_P256_ecdsa_verif_without_hash =
      foreign "Hacl_Interface_P256_ecdsa_verif_without_hash"
        (uint32_t @->
           (ocaml_bytes @->
              (ocaml_bytes @->
                 (ocaml_bytes @-> (ocaml_bytes @-> (returning bool))))))
      
    let hacl_Interface_P256_verify_q =
      foreign "Hacl_Interface_P256_verify_q"
        (ocaml_bytes @-> (returning bool))
      
    let hacl_Interface_P256_decompression_not_compressed_form =
      foreign "Hacl_Interface_P256_decompression_not_compressed_form"
        (ocaml_bytes @-> (ocaml_bytes @-> (returning bool)))
      
    let hacl_Interface_P256_decompression_compressed_form =
      foreign "Hacl_Interface_P256_decompression_compressed_form"
        (ocaml_bytes @-> (ocaml_bytes @-> (returning bool)))
      
    let hacl_Interface_P256_compression_not_compressed_form =
      foreign "Hacl_Interface_P256_compression_not_compressed_form"
        (ocaml_bytes @-> (ocaml_bytes @-> (returning void)))
      
    let hacl_Interface_P256_compression_compressed_form =
      foreign "Hacl_Interface_P256_compression_compressed_form"
        (ocaml_bytes @-> (ocaml_bytes @-> (returning void)))
      
    let hacl_Interface_P256_reduction_8_32 =
      foreign "Hacl_Interface_P256_reduction_8_32"
        (ocaml_bytes @-> (ocaml_bytes @-> (returning void)))
      
    let hacl_Interface_P256_ecp256dh_i =
      foreign "Hacl_Interface_P256_ecp256dh_i"
        (ocaml_bytes @-> (ocaml_bytes @-> (returning uint64_t)))
      
    let hacl_Interface_P256_ecp256dh_r =
      foreign "Hacl_Interface_P256_ecp256dh_r"
        (ocaml_bytes @->
           (ocaml_bytes @-> (ocaml_bytes @-> (returning uint64_t))))
      
  end