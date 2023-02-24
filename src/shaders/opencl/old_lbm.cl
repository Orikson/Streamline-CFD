__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
__constant unsigned int inversei[9] = {
    0u,
    2u,
    1u,
    4u,
    3u,
    6u,
    5u,
    8u,
    7u
};

__constant int2 e[9] = {
    (int2)(0, 0),
    (int2)(1, 0),
    (int2)(-1, 0),
    (int2)(0, 1),
    (int2)(0, -1),
    (int2)(1, 1),
    (int2)(-1, -1),
    (int2)(-1, 1),
    (int2)(1, -1)
};
__constant float w[9] = {
    0.44444f,
    0.11111f,
    0.11111f,
    0.11111f,
    0.11111f,
    0.02778f,
    0.02778f,
    0.02778f,
    0.02778f,
};
__constant float omega = 0.0f;

float feqi(float2 v, float p, unsigned int i) {
    float2 u = v + (float2)(0.02, 0.0);
    float wi = w[i];
    float2 ei = convert_float2(e[i]);
    float ei_u = dot(ei, u);
    return wi * p * (1.0f + 3.0f * ei_u - 1.5f * dot(u, u) + 4.5f * ei_u * ei_u);
}

unsigned int invi(unsigned int i) {
    if (i == 0u) { return 0u; }
    return i + 2u * (i % 2u) - 1u;
}

__kernel void lbm_2d_stream_and_collide(
        __read_only image2d_t boundaries,
        __read_only image2d_t df_0,
        __read_only image2d_t df_14,
        __read_only image2d_t df_58,
        __private float dt, 
        __write_only image2d_t of_0,
        __write_only image2d_t of_14,
        __write_only image2d_t of_58,
        __write_only image2d_t v_d
        ) {
    unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);
    int2 uv = (int2)(x, y);
    
    float4 f_0 = read_imagef(df_0, sampler, uv);
    float4 f_14 = read_imagef(df_14, sampler, uv);
    float4 f_58 = read_imagef(df_58, sampler, uv);
    float f[9] = { f_0.x, f_14.x, f_14.y, f_14.z, f_14.w, f_58.x, f_58.y, f_58.z, f_58.w };

    float4 nf_0 = f_0;
    float4 nf_1 = read_imagef(df_14, sampler,  uv - e[1]);
    float4 nf_2 = read_imagef(df_14, sampler,  uv - e[2]);
    float4 nf_3 = read_imagef(df_14, sampler,  uv - e[3]);
    float4 nf_4 = read_imagef(df_14, sampler,  uv - e[4]);
    float4 nf_5 = read_imagef(df_58, sampler,  uv - e[5]);
    float4 nf_6 = read_imagef(df_58, sampler,  uv - e[6]);
    float4 nf_7 = read_imagef(df_58, sampler,  uv - e[7]);
    float4 nf_8 = read_imagef(df_58, sampler,  uv - e[8]);
    float nf[9] = { nf_0.x, nf_1.x, nf_2.y, nf_3.z, nf_4.w, nf_5.x, nf_6.y, nf_7.z, nf_8.w };

    float f_in[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // stream
    for (int i = 0; i < 9; i ++) {
        int inv = inversei[i];
        
        f_in[i] = nf[i];

        unsigned int bounds = read_imageui(boundaries, sampler, uv - e[i]).x;
        if (bounds == 1u) {
            f_in[i] = f[inv];
        }
    }

    // collide
    float p = f_in[0] + f_in[1] + f_in[2] + f_in[3] + f_in[4] + f_in[5] + f_in[6] + f_in[7] + f_in[8];
    float2 v = 
        f_in[0] * convert_float2(e[0]) +
        f_in[1] * convert_float2(e[1]) +
        f_in[2] * convert_float2(e[2]) +
        f_in[3] * convert_float2(e[3]) +
        f_in[4] * convert_float2(e[4]) +
        f_in[5] * convert_float2(e[5]) +
        f_in[6] * convert_float2(e[6]) +
        f_in[7] * convert_float2(e[7]) +
        f_in[8] * convert_float2(e[8]);
    v = v / p;

    float feq[9] = { feqi(v, p, 0), feqi(v, p, 1), feqi(v, p, 2), feqi(v, p, 3), feqi(v, p, 4), feqi(v, p, 5), feqi(v, p, 6), feqi(v, p, 7), feqi(v, p, 8) };
    float f_fin[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    for (int i = 0; i < 9; i ++) {
        f_fin[i] = (1.0f - omega) * f_in[i] + omega * feq[i];
    }

    /*if (x == 0) {
        for (int i = 0; i < 9; i ++) {
            f_fin[i] = 0.0f;
        }
        f_fin[1] = 0.02f;
    }*/

    unsigned int bounds0 = read_imageui(boundaries, sampler, uv).x;
    if (bounds0 != 1u) {
        write_imagef(of_0, uv, (float4)(f_fin[0], 0.0f, 0.0f, 0.0f));
        write_imagef(of_14, uv, (float4)(f_fin[1], f_fin[2], f_fin[3], f_fin[4]));
        write_imagef(of_58, uv, (float4)(f_fin[5], f_fin[6], f_fin[7], f_fin[8]));
        write_imagef(v_d, uv, (float4)(20.0f*v*v, 0.0f, 0.0f));
    } else {
        write_imagef(of_0, uv, (float4)(0.0f, 0.0f, 0.0f, 0.0f));
        write_imagef(of_14, uv, (float4)(0.0f, 0.0f, 0.0f, 0.0f));
        write_imagef(of_58, uv, (float4)(0.0f, 0.0f, 0.0f, 0.0f));
        write_imagef(v_d, uv, (float4)(1.0f, 1.0f, 0.0f, 0.0f));
    }
}
