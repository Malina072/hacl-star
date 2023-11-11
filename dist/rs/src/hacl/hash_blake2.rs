pub fn blake2b_init(hash: &mut [u64], kk: u32, nn: u32) -> ()
{
    let r0: (&mut [u64], &mut [u64]) = hash.split_at_mut(0usize);
    let r1: (&mut [u64], &mut [u64]) = r0.1.split_at_mut(4usize);
    let r2: (&mut [u64], &mut [u64]) = r1.1.split_at_mut(4usize);
    let r3: (&mut [u64], &mut [u64]) = r2.1.split_at_mut(4usize);
    let iv0: u64 = (&crate::hacl::impl_blake2_constants::ivTable_B)[0usize];
    let iv1: u64 = (&crate::hacl::impl_blake2_constants::ivTable_B)[1usize];
    let iv2: u64 = (&crate::hacl::impl_blake2_constants::ivTable_B)[2usize];
    let iv3: u64 = (&crate::hacl::impl_blake2_constants::ivTable_B)[3usize];
    let iv4: u64 = (&crate::hacl::impl_blake2_constants::ivTable_B)[4usize];
    let iv5: u64 = (&crate::hacl::impl_blake2_constants::ivTable_B)[5usize];
    let iv6: u64 = (&crate::hacl::impl_blake2_constants::ivTable_B)[6usize];
    let iv7: u64 = (&crate::hacl::impl_blake2_constants::ivTable_B)[7usize];
    r3.0[0usize] = iv0;
    r3.0[1usize] = iv1;
    r3.0[2usize] = iv2;
    r3.0[3usize] = iv3;
    r3.1[0usize] = iv4;
    r3.1[1usize] = iv5;
    r3.1[2usize] = iv6;
    r3.1[3usize] = iv7;
    let kk_shift_8: u64 = (kk as u64).wrapping_shl(8u32);
    let iv0_: u64 = iv0 ^ (0x01010000u64 ^ (kk_shift_8 ^ nn as u64));
    r1.0[0usize] = iv0_;
    r1.0[1usize] = iv1;
    r1.0[2usize] = iv2;
    r1.0[3usize] = iv3;
    r2.0[0usize] = iv4;
    r2.0[1usize] = iv5;
    r2.0[2usize] = iv6;
    r2.0[3usize] = iv7
}

pub fn blake2b_update_multi(
    len: u32,
    wv: &mut [u64],
    hash: &mut [u64],
    prev: crate::fstar::uint128::uint128,
    blocks: &mut [u8],
    nb: u32
) ->
    ()
{
    crate::lowstar::ignore::ignore::<u32>(len);
    for i in 0u32..nb
    {
        let totlen: crate::fstar::uint128::uint128 =
            crate::fstar::uint128::add_mod(
                prev,
                crate::fstar::uint128::uint64_to_uint128(
                    i.wrapping_add(1u32).wrapping_mul(128u32) as u64
                )
            );
        let b: (&mut [u8], &mut [u8]) =
            blocks.split_at_mut((i.wrapping_mul(128u32) as usize).wrapping_add(0usize));
        crate::hacl::blake2b_32::blake2b_update_block(wv, hash, falsebool, totlen, b.1)
    }
}

#[inline] fn blake2b_update(
    wv: &mut [u64],
    hash: &mut [u64],
    kk: u32,
    k: &mut [u8],
    ll: u32,
    d: &mut [u8]
) ->
    ()
{
    let lb: crate::fstar::uint128::uint128 =
        crate::fstar::uint128::uint64_to_uint128(128u32 as u64);
    if kk > 0u32
    {
        crate::hacl::blake2b_32::blake2b_update_key(wv, hash, kk, k, ll);
        if ! ll == 0u32 { crate::hacl::blake2b_32::blake2b_update_blocks(ll, wv, hash, lb, d) }
    }
    else
    {
        crate::hacl::blake2b_32::blake2b_update_blocks(
            ll,
            wv,
            hash,
            crate::fstar::uint128::uint64_to_uint128(0u32 as u64),
            d
        )
    }
}

pub fn blake2b_malloc(r: ()) -> &mut [u64]
{
    let mut buf: Vec<u64> = vec![0u64; 16usize];
    &mut buf
}

pub fn blake2s_init(hash: &mut [u32], kk: u32, nn: u32) -> ()
{
    let r0: (&mut [u32], &mut [u32]) = hash.split_at_mut(0usize);
    let r1: (&mut [u32], &mut [u32]) = r0.1.split_at_mut(4usize);
    let r2: (&mut [u32], &mut [u32]) = r1.1.split_at_mut(4usize);
    let r3: (&mut [u32], &mut [u32]) = r2.1.split_at_mut(4usize);
    let iv0: u32 = (&crate::hacl::impl_blake2_constants::ivTable_S)[0usize];
    let iv1: u32 = (&crate::hacl::impl_blake2_constants::ivTable_S)[1usize];
    let iv2: u32 = (&crate::hacl::impl_blake2_constants::ivTable_S)[2usize];
    let iv3: u32 = (&crate::hacl::impl_blake2_constants::ivTable_S)[3usize];
    let iv4: u32 = (&crate::hacl::impl_blake2_constants::ivTable_S)[4usize];
    let iv5: u32 = (&crate::hacl::impl_blake2_constants::ivTable_S)[5usize];
    let iv6: u32 = (&crate::hacl::impl_blake2_constants::ivTable_S)[6usize];
    let iv7: u32 = (&crate::hacl::impl_blake2_constants::ivTable_S)[7usize];
    r3.0[0usize] = iv0;
    r3.0[1usize] = iv1;
    r3.0[2usize] = iv2;
    r3.0[3usize] = iv3;
    r3.1[0usize] = iv4;
    r3.1[1usize] = iv5;
    r3.1[2usize] = iv6;
    r3.1[3usize] = iv7;
    let kk_shift_8: u32 = kk.wrapping_shl(8u32);
    let iv0_: u32 = iv0 ^ (0x01010000u32 ^ (kk_shift_8 ^ nn));
    r1.0[0usize] = iv0_;
    r1.0[1usize] = iv1;
    r1.0[2usize] = iv2;
    r1.0[3usize] = iv3;
    r2.0[0usize] = iv4;
    r2.0[1usize] = iv5;
    r2.0[2usize] = iv6;
    r2.0[3usize] = iv7
}

pub fn blake2s_update_multi(
    len: u32,
    wv: &mut [u32],
    hash: &mut [u32],
    prev: u64,
    blocks: &mut [u8],
    nb: u32
) ->
    ()
{
    crate::lowstar::ignore::ignore::<u32>(len);
    for i in 0u32..nb
    {
        let totlen: u64 = prev.wrapping_add(i.wrapping_add(1u32).wrapping_mul(64u32) as u64);
        let b: (&mut [u8], &mut [u8]) =
            blocks.split_at_mut((i.wrapping_mul(64u32) as usize).wrapping_add(0usize));
        crate::hacl::blake2s_32::blake2s_update_block(wv, hash, falsebool, totlen, b.1)
    }
}

#[inline] fn blake2s_update(
    wv: &mut [u32],
    hash: &mut [u32],
    kk: u32,
    k: &mut [u8],
    ll: u32,
    d: &mut [u8]
) ->
    ()
{
    let lb: u64 = 64u32 as u64;
    if kk > 0u32
    {
        crate::hacl::blake2s_32::blake2s_update_key(wv, hash, kk, k, ll);
        if ! ll == 0u32 { crate::hacl::blake2s_32::blake2s_update_blocks(ll, wv, hash, lb, d) }
    }
    else
    { crate::hacl::blake2s_32::blake2s_update_blocks(ll, wv, hash, 0u32 as u64, d) }
}

pub fn blake2s_malloc(r: ()) -> &mut [u32]
{
    let mut buf: Vec<u32> = vec![0u32; 16usize];
    &mut buf
}
