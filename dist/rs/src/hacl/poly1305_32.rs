pub fn poly1305_init(ctx: &mut [u64], key: &mut [u8]) -> ()
{
    let acc: (&mut [u64], &mut [u64]) = ctx.split_at_mut(0usize);
    let pre: (&mut [u64], &mut [u64]) = acc.1.split_at_mut(5usize);
    let kr: (&mut [u8], &mut [u8]) = key.split_at_mut(0usize);
    pre.0[0u32 as usize] = 0u64;
    pre.0[1u32 as usize] = 0u64;
    pre.0[2u32 as usize] = 0u64;
    pre.0[3u32 as usize] = 0u64;
    pre.0[4u32 as usize] = 0u64;
    let u: u64 = crate::lowstar::endianness::load64_le(&mut kr.1[0u32 as usize..]);
    let lo: u64 = u;
    let u0: u64 = crate::lowstar::endianness::load64_le(&mut kr.1[8u32 as usize..]);
    let hi: u64 = u0;
    let mask0: u64 = 0x0ffffffc0fffffffu64;
    let mask1: u64 = 0x0ffffffc0ffffffcu64;
    let lo1: u64 = lo & mask0;
    let hi1: u64 = hi & mask1;
    let r: (&mut [u64], &mut [u64]) = pre.1.split_at_mut(0usize);
    let r5: (&mut [u64], &mut [u64]) = r.1.split_at_mut(5usize);
    let rn: (&mut [u64], &mut [u64]) = r5.1.split_at_mut(5usize);
    let rn_5: (&mut [u64], &mut [u64]) = rn.1.split_at_mut(5usize);
    let r_vec0: u64 = lo1;
    let r_vec1: u64 = hi1;
    let f0: u64 = r_vec0 & 0x3ffffffu64;
    let f1: u64 = r_vec0.wrapping_shr(26u32) & 0x3ffffffu64;
    let f2: u64 = r_vec0.wrapping_shr(52u32) | (r_vec1 & 0x3fffu64).wrapping_shl(12u32);
    let f3: u64 = r_vec1.wrapping_shr(14u32) & 0x3ffffffu64;
    let f4: u64 = r_vec1.wrapping_shr(40u32);
    let f00: u64 = f0;
    let f10: u64 = f1;
    let f20: u64 = f2;
    let f30: u64 = f3;
    let f40: u64 = f4;
    r5.0[0u32 as usize] = f00;
    r5.0[1u32 as usize] = f10;
    r5.0[2u32 as usize] = f20;
    r5.0[3u32 as usize] = f30;
    r5.0[4u32 as usize] = f40;
    let f200: u64 = r5.0[0u32 as usize];
    let f21: u64 = r5.0[1u32 as usize];
    let f22: u64 = r5.0[2u32 as usize];
    let f23: u64 = r5.0[3u32 as usize];
    let f24: u64 = r5.0[4u32 as usize];
    rn.0[0u32 as usize] = f200.wrapping_mul(5u64);
    rn.0[1u32 as usize] = f21.wrapping_mul(5u64);
    rn.0[2u32 as usize] = f22.wrapping_mul(5u64);
    rn.0[3u32 as usize] = f23.wrapping_mul(5u64);
    rn.0[4u32 as usize] = f24.wrapping_mul(5u64);
    rn_5.0[0u32 as usize] = r5.0[0u32 as usize];
    rn_5.0[1u32 as usize] = r5.0[1u32 as usize];
    rn_5.0[2u32 as usize] = r5.0[2u32 as usize];
    rn_5.0[3u32 as usize] = r5.0[3u32 as usize];
    rn_5.0[4u32 as usize] = r5.0[4u32 as usize];
    rn_5.1[0u32 as usize] = rn.0[0u32 as usize];
    rn_5.1[1u32 as usize] = rn.0[1u32 as usize];
    rn_5.1[2u32 as usize] = rn.0[2u32 as usize];
    rn_5.1[3u32 as usize] = rn.0[3u32 as usize];
    rn_5.1[4u32 as usize] = rn.0[4u32 as usize]
}

pub fn poly1305_update1(ctx: &mut [u64], text: &mut [u8]) -> ()
{
    let pre: (&mut [u64], &mut [u64]) = ctx.split_at_mut(5usize);
    let acc: (&mut [u64], &mut [u64]) = pre.0.split_at_mut(0usize);
    let mut e: [u64; 5] = [0u64; 5u32 as usize];
    let u: u64 = crate::lowstar::endianness::load64_le(&mut text[0u32 as usize..]);
    let lo: u64 = u;
    let u0: u64 = crate::lowstar::endianness::load64_le(&mut text[8u32 as usize..]);
    let hi: u64 = u0;
    let f0: u64 = lo;
    let f1: u64 = hi;
    let f01: u64 = f0 & 0x3ffffffu64;
    let f11: u64 = f0.wrapping_shr(26u32) & 0x3ffffffu64;
    let f2: u64 = f0.wrapping_shr(52u32) | (f1 & 0x3fffu64).wrapping_shl(12u32);
    let f3: u64 = f1.wrapping_shr(14u32) & 0x3ffffffu64;
    let f4: u64 = f1.wrapping_shr(40u32);
    let f010: u64 = f01;
    let f110: u64 = f11;
    let f20: u64 = f2;
    let f30: u64 = f3;
    let f40: u64 = f4;
    (&mut e)[0u32 as usize] = f010;
    (&mut e)[1u32 as usize] = f110;
    (&mut e)[2u32 as usize] = f20;
    (&mut e)[3u32 as usize] = f30;
    (&mut e)[4u32 as usize] = f40;
    let b: u64 = 0x1000000u64;
    let mask: u64 = b;
    let f41: u64 = (&mut e)[4u32 as usize];
    (&mut e)[4u32 as usize] = f41 | mask;
    let r: (&mut [u64], &mut [u64]) = pre.1.split_at_mut(0usize);
    let r5: (&mut [u64], &mut [u64]) = r.1.split_at_mut(5usize);
    let r0: u64 = r5.0[0u32 as usize];
    let r1: u64 = r5.0[1u32 as usize];
    let r2: u64 = r5.0[2u32 as usize];
    let r3: u64 = r5.0[3u32 as usize];
    let r4: u64 = r5.0[4u32 as usize];
    let r51: u64 = r5.1[1u32 as usize];
    let r52: u64 = r5.1[2u32 as usize];
    let r53: u64 = r5.1[3u32 as usize];
    let r54: u64 = r5.1[4u32 as usize];
    let f10: u64 = (&mut e)[0u32 as usize];
    let f111: u64 = (&mut e)[1u32 as usize];
    let f12: u64 = (&mut e)[2u32 as usize];
    let f13: u64 = (&mut e)[3u32 as usize];
    let f14: u64 = (&mut e)[4u32 as usize];
    let a0: u64 = acc.1[0u32 as usize];
    let a1: u64 = acc.1[1u32 as usize];
    let a2: u64 = acc.1[2u32 as usize];
    let a3: u64 = acc.1[3u32 as usize];
    let a4: u64 = acc.1[4u32 as usize];
    let a01: u64 = a0.wrapping_add(f10);
    let a11: u64 = a1.wrapping_add(f111);
    let a21: u64 = a2.wrapping_add(f12);
    let a31: u64 = a3.wrapping_add(f13);
    let a41: u64 = a4.wrapping_add(f14);
    let a02: u64 = r0.wrapping_mul(a01);
    let a12: u64 = r1.wrapping_mul(a01);
    let a22: u64 = r2.wrapping_mul(a01);
    let a32: u64 = r3.wrapping_mul(a01);
    let a42: u64 = r4.wrapping_mul(a01);
    let a03: u64 = a02.wrapping_add(r54.wrapping_mul(a11));
    let a13: u64 = a12.wrapping_add(r0.wrapping_mul(a11));
    let a23: u64 = a22.wrapping_add(r1.wrapping_mul(a11));
    let a33: u64 = a32.wrapping_add(r2.wrapping_mul(a11));
    let a43: u64 = a42.wrapping_add(r3.wrapping_mul(a11));
    let a04: u64 = a03.wrapping_add(r53.wrapping_mul(a21));
    let a14: u64 = a13.wrapping_add(r54.wrapping_mul(a21));
    let a24: u64 = a23.wrapping_add(r0.wrapping_mul(a21));
    let a34: u64 = a33.wrapping_add(r1.wrapping_mul(a21));
    let a44: u64 = a43.wrapping_add(r2.wrapping_mul(a21));
    let a05: u64 = a04.wrapping_add(r52.wrapping_mul(a31));
    let a15: u64 = a14.wrapping_add(r53.wrapping_mul(a31));
    let a25: u64 = a24.wrapping_add(r54.wrapping_mul(a31));
    let a35: u64 = a34.wrapping_add(r0.wrapping_mul(a31));
    let a45: u64 = a44.wrapping_add(r1.wrapping_mul(a31));
    let a06: u64 = a05.wrapping_add(r51.wrapping_mul(a41));
    let a16: u64 = a15.wrapping_add(r52.wrapping_mul(a41));
    let a26: u64 = a25.wrapping_add(r53.wrapping_mul(a41));
    let a36: u64 = a35.wrapping_add(r54.wrapping_mul(a41));
    let a46: u64 = a45.wrapping_add(r0.wrapping_mul(a41));
    let t0: u64 = a06;
    let t1: u64 = a16;
    let t2: u64 = a26;
    let t3: u64 = a36;
    let t4: u64 = a46;
    let mask26: u64 = 0x3ffffffu64;
    let z0: u64 = t0.wrapping_shr(26u32);
    let z1: u64 = t3.wrapping_shr(26u32);
    let x0: u64 = t0 & mask26;
    let x3: u64 = t3 & mask26;
    let x1: u64 = t1.wrapping_add(z0);
    let x4: u64 = t4.wrapping_add(z1);
    let z01: u64 = x1.wrapping_shr(26u32);
    let z11: u64 = x4.wrapping_shr(26u32);
    let t: u64 = z11.wrapping_shl(2u32);
    let z12: u64 = z11.wrapping_add(t);
    let x11: u64 = x1 & mask26;
    let x41: u64 = x4 & mask26;
    let x2: u64 = t2.wrapping_add(z01);
    let x01: u64 = x0.wrapping_add(z12);
    let z02: u64 = x2.wrapping_shr(26u32);
    let z13: u64 = x01.wrapping_shr(26u32);
    let x21: u64 = x2 & mask26;
    let x02: u64 = x01 & mask26;
    let x31: u64 = x3.wrapping_add(z02);
    let x12: u64 = x11.wrapping_add(z13);
    let z03: u64 = x31.wrapping_shr(26u32);
    let x32: u64 = x31 & mask26;
    let x42: u64 = x41.wrapping_add(z03);
    let o0: u64 = x02;
    let o1: u64 = x12;
    let o2: u64 = x21;
    let o3: u64 = x32;
    let o4: u64 = x42;
    acc.1[0u32 as usize] = o0;
    acc.1[1u32 as usize] = o1;
    acc.1[2u32 as usize] = o2;
    acc.1[3u32 as usize] = o3;
    acc.1[4u32 as usize] = o4
}

pub fn poly1305_update(ctx: &mut [u64], len: u32, text: &mut [u8]) -> ()
{
    let pre: (&mut [u64], &mut [u64]) = ctx.split_at_mut(5usize);
    let acc: (&mut [u64], &mut [u64]) = pre.0.split_at_mut(0usize);
    let nb: u32 = len.wrapping_div(16u32);
    let rem: u32 = len.wrapping_rem(16u32);
    for i in 0u32..nb
    {
        let block: (&mut [u8], &mut [u8]) =
            text.split_at_mut((i.wrapping_mul(16u32) as usize).wrapping_add(0usize));
        let mut e: [u64; 5] = [0u64; 5u32 as usize];
        let u: u64 = crate::lowstar::endianness::load64_le(&mut block.1[0u32 as usize..]);
        let lo: u64 = u;
        let u0: u64 = crate::lowstar::endianness::load64_le(&mut block.1[8u32 as usize..]);
        let hi: u64 = u0;
        let f0: u64 = lo;
        let f1: u64 = hi;
        let f01: u64 = f0 & 0x3ffffffu64;
        let f11: u64 = f0.wrapping_shr(26u32) & 0x3ffffffu64;
        let f2: u64 = f0.wrapping_shr(52u32) | (f1 & 0x3fffu64).wrapping_shl(12u32);
        let f3: u64 = f1.wrapping_shr(14u32) & 0x3ffffffu64;
        let f4: u64 = f1.wrapping_shr(40u32);
        let f010: u64 = f01;
        let f110: u64 = f11;
        let f20: u64 = f2;
        let f30: u64 = f3;
        let f40: u64 = f4;
        (&mut e)[0u32 as usize] = f010;
        (&mut e)[1u32 as usize] = f110;
        (&mut e)[2u32 as usize] = f20;
        (&mut e)[3u32 as usize] = f30;
        (&mut e)[4u32 as usize] = f40;
        let b: u64 = 0x1000000u64;
        let mask: u64 = b;
        let f41: u64 = (&mut e)[4u32 as usize];
        (&mut e)[4u32 as usize] = f41 | mask;
        let r: (&mut [u64], &mut [u64]) = pre.1.split_at_mut(0usize);
        let r5: (&mut [u64], &mut [u64]) = r.1.split_at_mut(5usize);
        let r0: u64 = r5.0[0u32 as usize];
        let r1: u64 = r5.0[1u32 as usize];
        let r2: u64 = r5.0[2u32 as usize];
        let r3: u64 = r5.0[3u32 as usize];
        let r4: u64 = r5.0[4u32 as usize];
        let r51: u64 = r5.1[1u32 as usize];
        let r52: u64 = r5.1[2u32 as usize];
        let r53: u64 = r5.1[3u32 as usize];
        let r54: u64 = r5.1[4u32 as usize];
        let f10: u64 = (&mut e)[0u32 as usize];
        let f111: u64 = (&mut e)[1u32 as usize];
        let f12: u64 = (&mut e)[2u32 as usize];
        let f13: u64 = (&mut e)[3u32 as usize];
        let f14: u64 = (&mut e)[4u32 as usize];
        let a0: u64 = acc.1[0u32 as usize];
        let a1: u64 = acc.1[1u32 as usize];
        let a2: u64 = acc.1[2u32 as usize];
        let a3: u64 = acc.1[3u32 as usize];
        let a4: u64 = acc.1[4u32 as usize];
        let a01: u64 = a0.wrapping_add(f10);
        let a11: u64 = a1.wrapping_add(f111);
        let a21: u64 = a2.wrapping_add(f12);
        let a31: u64 = a3.wrapping_add(f13);
        let a41: u64 = a4.wrapping_add(f14);
        let a02: u64 = r0.wrapping_mul(a01);
        let a12: u64 = r1.wrapping_mul(a01);
        let a22: u64 = r2.wrapping_mul(a01);
        let a32: u64 = r3.wrapping_mul(a01);
        let a42: u64 = r4.wrapping_mul(a01);
        let a03: u64 = a02.wrapping_add(r54.wrapping_mul(a11));
        let a13: u64 = a12.wrapping_add(r0.wrapping_mul(a11));
        let a23: u64 = a22.wrapping_add(r1.wrapping_mul(a11));
        let a33: u64 = a32.wrapping_add(r2.wrapping_mul(a11));
        let a43: u64 = a42.wrapping_add(r3.wrapping_mul(a11));
        let a04: u64 = a03.wrapping_add(r53.wrapping_mul(a21));
        let a14: u64 = a13.wrapping_add(r54.wrapping_mul(a21));
        let a24: u64 = a23.wrapping_add(r0.wrapping_mul(a21));
        let a34: u64 = a33.wrapping_add(r1.wrapping_mul(a21));
        let a44: u64 = a43.wrapping_add(r2.wrapping_mul(a21));
        let a05: u64 = a04.wrapping_add(r52.wrapping_mul(a31));
        let a15: u64 = a14.wrapping_add(r53.wrapping_mul(a31));
        let a25: u64 = a24.wrapping_add(r54.wrapping_mul(a31));
        let a35: u64 = a34.wrapping_add(r0.wrapping_mul(a31));
        let a45: u64 = a44.wrapping_add(r1.wrapping_mul(a31));
        let a06: u64 = a05.wrapping_add(r51.wrapping_mul(a41));
        let a16: u64 = a15.wrapping_add(r52.wrapping_mul(a41));
        let a26: u64 = a25.wrapping_add(r53.wrapping_mul(a41));
        let a36: u64 = a35.wrapping_add(r54.wrapping_mul(a41));
        let a46: u64 = a45.wrapping_add(r0.wrapping_mul(a41));
        let t0: u64 = a06;
        let t1: u64 = a16;
        let t2: u64 = a26;
        let t3: u64 = a36;
        let t4: u64 = a46;
        let mask26: u64 = 0x3ffffffu64;
        let z0: u64 = t0.wrapping_shr(26u32);
        let z1: u64 = t3.wrapping_shr(26u32);
        let x0: u64 = t0 & mask26;
        let x3: u64 = t3 & mask26;
        let x1: u64 = t1.wrapping_add(z0);
        let x4: u64 = t4.wrapping_add(z1);
        let z01: u64 = x1.wrapping_shr(26u32);
        let z11: u64 = x4.wrapping_shr(26u32);
        let t: u64 = z11.wrapping_shl(2u32);
        let z12: u64 = z11.wrapping_add(t);
        let x11: u64 = x1 & mask26;
        let x41: u64 = x4 & mask26;
        let x2: u64 = t2.wrapping_add(z01);
        let x01: u64 = x0.wrapping_add(z12);
        let z02: u64 = x2.wrapping_shr(26u32);
        let z13: u64 = x01.wrapping_shr(26u32);
        let x21: u64 = x2 & mask26;
        let x02: u64 = x01 & mask26;
        let x31: u64 = x3.wrapping_add(z02);
        let x12: u64 = x11.wrapping_add(z13);
        let z03: u64 = x31.wrapping_shr(26u32);
        let x32: u64 = x31 & mask26;
        let x42: u64 = x41.wrapping_add(z03);
        let o0: u64 = x02;
        let o1: u64 = x12;
        let o2: u64 = x21;
        let o3: u64 = x32;
        let o4: u64 = x42;
        acc.1[0u32 as usize] = o0;
        acc.1[1u32 as usize] = o1;
        acc.1[2u32 as usize] = o2;
        acc.1[3u32 as usize] = o3;
        acc.1[4u32 as usize] = o4
    };
    if rem > 0u32
    {
        let last: (&mut [u8], &mut [u8]) =
            text.split_at_mut((nb.wrapping_mul(16u32) as usize).wrapping_add(0usize));
        let mut e: [u64; 5] = [0u64; 5u32 as usize];
        let mut tmp: [u8; 16] = [0u8; 16u32 as usize];
        ((&mut tmp)[0u32 as usize..0u32 as usize + rem as usize]).copy_from_slice(
            &last.1[0u32 as usize..0u32 as usize + rem as usize]
        );
        let u: u64 = crate::lowstar::endianness::load64_le(&mut (&mut tmp)[0u32 as usize..]);
        let lo: u64 = u;
        let u0: u64 = crate::lowstar::endianness::load64_le(&mut (&mut tmp)[8u32 as usize..]);
        let hi: u64 = u0;
        let f0: u64 = lo;
        let f1: u64 = hi;
        let f01: u64 = f0 & 0x3ffffffu64;
        let f11: u64 = f0.wrapping_shr(26u32) & 0x3ffffffu64;
        let f2: u64 = f0.wrapping_shr(52u32) | (f1 & 0x3fffu64).wrapping_shl(12u32);
        let f3: u64 = f1.wrapping_shr(14u32) & 0x3ffffffu64;
        let f4: u64 = f1.wrapping_shr(40u32);
        let f010: u64 = f01;
        let f110: u64 = f11;
        let f20: u64 = f2;
        let f30: u64 = f3;
        let f40: u64 = f4;
        (&mut e)[0u32 as usize] = f010;
        (&mut e)[1u32 as usize] = f110;
        (&mut e)[2u32 as usize] = f20;
        (&mut e)[3u32 as usize] = f30;
        (&mut e)[4u32 as usize] = f40;
        let b: u64 = 1u64.wrapping_shl(rem.wrapping_mul(8u32).wrapping_rem(26u32));
        let mask: u64 = b;
        let fi: u64 = (&mut e)[rem.wrapping_mul(8u32).wrapping_div(26u32) as usize];
        (&mut e)[rem.wrapping_mul(8u32).wrapping_div(26u32) as usize] = fi | mask;
        let r: (&mut [u64], &mut [u64]) = pre.1.split_at_mut(0usize);
        let r5: (&mut [u64], &mut [u64]) = r.1.split_at_mut(5usize);
        let r0: u64 = r5.0[0u32 as usize];
        let r1: u64 = r5.0[1u32 as usize];
        let r2: u64 = r5.0[2u32 as usize];
        let r3: u64 = r5.0[3u32 as usize];
        let r4: u64 = r5.0[4u32 as usize];
        let r51: u64 = r5.1[1u32 as usize];
        let r52: u64 = r5.1[2u32 as usize];
        let r53: u64 = r5.1[3u32 as usize];
        let r54: u64 = r5.1[4u32 as usize];
        let f10: u64 = (&mut e)[0u32 as usize];
        let f111: u64 = (&mut e)[1u32 as usize];
        let f12: u64 = (&mut e)[2u32 as usize];
        let f13: u64 = (&mut e)[3u32 as usize];
        let f14: u64 = (&mut e)[4u32 as usize];
        let a0: u64 = acc.1[0u32 as usize];
        let a1: u64 = acc.1[1u32 as usize];
        let a2: u64 = acc.1[2u32 as usize];
        let a3: u64 = acc.1[3u32 as usize];
        let a4: u64 = acc.1[4u32 as usize];
        let a01: u64 = a0.wrapping_add(f10);
        let a11: u64 = a1.wrapping_add(f111);
        let a21: u64 = a2.wrapping_add(f12);
        let a31: u64 = a3.wrapping_add(f13);
        let a41: u64 = a4.wrapping_add(f14);
        let a02: u64 = r0.wrapping_mul(a01);
        let a12: u64 = r1.wrapping_mul(a01);
        let a22: u64 = r2.wrapping_mul(a01);
        let a32: u64 = r3.wrapping_mul(a01);
        let a42: u64 = r4.wrapping_mul(a01);
        let a03: u64 = a02.wrapping_add(r54.wrapping_mul(a11));
        let a13: u64 = a12.wrapping_add(r0.wrapping_mul(a11));
        let a23: u64 = a22.wrapping_add(r1.wrapping_mul(a11));
        let a33: u64 = a32.wrapping_add(r2.wrapping_mul(a11));
        let a43: u64 = a42.wrapping_add(r3.wrapping_mul(a11));
        let a04: u64 = a03.wrapping_add(r53.wrapping_mul(a21));
        let a14: u64 = a13.wrapping_add(r54.wrapping_mul(a21));
        let a24: u64 = a23.wrapping_add(r0.wrapping_mul(a21));
        let a34: u64 = a33.wrapping_add(r1.wrapping_mul(a21));
        let a44: u64 = a43.wrapping_add(r2.wrapping_mul(a21));
        let a05: u64 = a04.wrapping_add(r52.wrapping_mul(a31));
        let a15: u64 = a14.wrapping_add(r53.wrapping_mul(a31));
        let a25: u64 = a24.wrapping_add(r54.wrapping_mul(a31));
        let a35: u64 = a34.wrapping_add(r0.wrapping_mul(a31));
        let a45: u64 = a44.wrapping_add(r1.wrapping_mul(a31));
        let a06: u64 = a05.wrapping_add(r51.wrapping_mul(a41));
        let a16: u64 = a15.wrapping_add(r52.wrapping_mul(a41));
        let a26: u64 = a25.wrapping_add(r53.wrapping_mul(a41));
        let a36: u64 = a35.wrapping_add(r54.wrapping_mul(a41));
        let a46: u64 = a45.wrapping_add(r0.wrapping_mul(a41));
        let t0: u64 = a06;
        let t1: u64 = a16;
        let t2: u64 = a26;
        let t3: u64 = a36;
        let t4: u64 = a46;
        let mask26: u64 = 0x3ffffffu64;
        let z0: u64 = t0.wrapping_shr(26u32);
        let z1: u64 = t3.wrapping_shr(26u32);
        let x0: u64 = t0 & mask26;
        let x3: u64 = t3 & mask26;
        let x1: u64 = t1.wrapping_add(z0);
        let x4: u64 = t4.wrapping_add(z1);
        let z01: u64 = x1.wrapping_shr(26u32);
        let z11: u64 = x4.wrapping_shr(26u32);
        let t: u64 = z11.wrapping_shl(2u32);
        let z12: u64 = z11.wrapping_add(t);
        let x11: u64 = x1 & mask26;
        let x41: u64 = x4 & mask26;
        let x2: u64 = t2.wrapping_add(z01);
        let x01: u64 = x0.wrapping_add(z12);
        let z02: u64 = x2.wrapping_shr(26u32);
        let z13: u64 = x01.wrapping_shr(26u32);
        let x21: u64 = x2 & mask26;
        let x02: u64 = x01 & mask26;
        let x31: u64 = x3.wrapping_add(z02);
        let x12: u64 = x11.wrapping_add(z13);
        let z03: u64 = x31.wrapping_shr(26u32);
        let x32: u64 = x31 & mask26;
        let x42: u64 = x41.wrapping_add(z03);
        let o0: u64 = x02;
        let o1: u64 = x12;
        let o2: u64 = x21;
        let o3: u64 = x32;
        let o4: u64 = x42;
        acc.1[0u32 as usize] = o0;
        acc.1[1u32 as usize] = o1;
        acc.1[2u32 as usize] = o2;
        acc.1[3u32 as usize] = o3;
        acc.1[4u32 as usize] = o4
    }
}

pub fn poly1305_finish(tag: &mut [u8], key: &mut [u8], ctx: &mut [u64]) -> ()
{
    let acc: (&mut [u64], &mut [u64]) = ctx.split_at_mut(0usize);
    let ks: (&mut [u8], &mut [u8]) = key.split_at_mut(16usize);
    let f0: u64 = acc.1[0u32 as usize];
    let f1: u64 = acc.1[1u32 as usize];
    let f2: u64 = acc.1[2u32 as usize];
    let f3: u64 = acc.1[3u32 as usize];
    let f4: u64 = acc.1[4u32 as usize];
    let l: u64 = f0.wrapping_add(0u64);
    let tmp0: u64 = l & 0x3ffffffu64;
    let c0: u64 = l.wrapping_shr(26u32);
    let l0: u64 = f1.wrapping_add(c0);
    let tmp1: u64 = l0 & 0x3ffffffu64;
    let c1: u64 = l0.wrapping_shr(26u32);
    let l1: u64 = f2.wrapping_add(c1);
    let tmp2: u64 = l1 & 0x3ffffffu64;
    let c2: u64 = l1.wrapping_shr(26u32);
    let l2: u64 = f3.wrapping_add(c2);
    let tmp3: u64 = l2 & 0x3ffffffu64;
    let c3: u64 = l2.wrapping_shr(26u32);
    let l3: u64 = f4.wrapping_add(c3);
    let tmp4: u64 = l3 & 0x3ffffffu64;
    let c4: u64 = l3.wrapping_shr(26u32);
    let f01: u64 = tmp0.wrapping_add(c4.wrapping_mul(5u64));
    let f11: u64 = tmp1;
    let f21: u64 = tmp2;
    let f31: u64 = tmp3;
    let f41: u64 = tmp4;
    let l4: u64 = f01.wrapping_add(0u64);
    let tmp00: u64 = l4 & 0x3ffffffu64;
    let c00: u64 = l4.wrapping_shr(26u32);
    let l5: u64 = f11.wrapping_add(c00);
    let tmp10: u64 = l5 & 0x3ffffffu64;
    let c10: u64 = l5.wrapping_shr(26u32);
    let l6: u64 = f21.wrapping_add(c10);
    let tmp20: u64 = l6 & 0x3ffffffu64;
    let c20: u64 = l6.wrapping_shr(26u32);
    let l7: u64 = f31.wrapping_add(c20);
    let tmp30: u64 = l7 & 0x3ffffffu64;
    let c30: u64 = l7.wrapping_shr(26u32);
    let l8: u64 = f41.wrapping_add(c30);
    let tmp40: u64 = l8 & 0x3ffffffu64;
    let c40: u64 = l8.wrapping_shr(26u32);
    let f02: u64 = tmp00.wrapping_add(c40.wrapping_mul(5u64));
    let f12: u64 = tmp10;
    let f22: u64 = tmp20;
    let f32: u64 = tmp30;
    let f42: u64 = tmp40;
    let mh: u64 = 0x3ffffffu64;
    let ml: u64 = 0x3fffffbu64;
    let mask: u64 = crate::fstar::uint64::eq_mask(f42, mh);
    let mask1: u64 = mask & crate::fstar::uint64::eq_mask(f32, mh);
    let mask2: u64 = mask1 & crate::fstar::uint64::eq_mask(f22, mh);
    let mask3: u64 = mask2 & crate::fstar::uint64::eq_mask(f12, mh);
    let mask4: u64 = mask3 & ! ! crate::fstar::uint64::gte_mask(f02, ml);
    let ph: u64 = mask4 & mh;
    let pl: u64 = mask4 & ml;
    let o0: u64 = f02.wrapping_sub(pl);
    let o1: u64 = f12.wrapping_sub(ph);
    let o2: u64 = f22.wrapping_sub(ph);
    let o3: u64 = f32.wrapping_sub(ph);
    let o4: u64 = f42.wrapping_sub(ph);
    let f010: u64 = o0;
    let f110: u64 = o1;
    let f210: u64 = o2;
    let f310: u64 = o3;
    let f410: u64 = o4;
    acc.1[0u32 as usize] = f010;
    acc.1[1u32 as usize] = f110;
    acc.1[2u32 as usize] = f210;
    acc.1[3u32 as usize] = f310;
    acc.1[4u32 as usize] = f410;
    let f00: u64 = acc.1[0u32 as usize];
    let f10: u64 = acc.1[1u32 as usize];
    let f20: u64 = acc.1[2u32 as usize];
    let f30: u64 = acc.1[3u32 as usize];
    let f40: u64 = acc.1[4u32 as usize];
    let f011: u64 = f00;
    let f111: u64 = f10;
    let f211: u64 = f20;
    let f311: u64 = f30;
    let f411: u64 = f40;
    let lo: u64 = f011 | f111.wrapping_shl(26u32) | f211.wrapping_shl(52u32);
    let hi: u64 = f211.wrapping_shr(12u32) | f311.wrapping_shl(14u32) | f411.wrapping_shl(40u32);
    let f100: u64 = lo;
    let f112: u64 = hi;
    let u: u64 = crate::lowstar::endianness::load64_le(&mut ks.1[0u32 as usize..]);
    let lo0: u64 = u;
    let u0: u64 = crate::lowstar::endianness::load64_le(&mut ks.1[8u32 as usize..]);
    let hi0: u64 = u0;
    let f200: u64 = lo0;
    let f212: u64 = hi0;
    let r0: u64 = f100.wrapping_add(f200);
    let r1: u64 = f112.wrapping_add(f212);
    let c: u64 = (r0 ^ (r0 ^ f200 | r0.wrapping_sub(f200) ^ f200)).wrapping_shr(63u32);
    let r11: u64 = r1.wrapping_add(c);
    let f300: u64 = r0;
    let f312: u64 = r11;
    crate::lowstar::endianness::store64_le(&mut tag[0u32 as usize..], f300);
    crate::lowstar::endianness::store64_le(&mut tag[8u32 as usize..], f312)
}

pub fn poly1305_mac(tag: &mut [u8], len: u32, text: &mut [u8], key: &mut [u8]) -> ()
{
    let mut ctx: [u64; 25] = [0u64; 25u32 as usize];
    poly1305_init(&mut ctx, key);
    poly1305_update(&mut ctx, len, text);
    poly1305_finish(tag, key, &mut ctx)
}