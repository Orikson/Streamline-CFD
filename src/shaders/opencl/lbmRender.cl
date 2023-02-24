__constant sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_REPEAT | CLK_FILTER_LINEAR;

__kernel void render_lbm(
        __read_only image2d_t boundaries,
        __read_only image2d_t v_d,
        __private float time, 
        __write_only image2d_t texture
        ) {
    unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);
    float2 uv = (float2)((float)(x) / (float)(get_global_size(0)), (float)(y) / (float)(get_global_size(1)));

    unsigned int bounds = read_imageui(boundaries, sampler, uv).x;
    float3 v = read_imagef(v_d, sampler, uv).xyz;

    float3 v0 = read_imagef(v_d, sampler, uv + (float2)(-1.0f / 400.0f, 0)).xyz;
    float3 v1 = read_imagef(v_d, sampler, uv + (float2)(1.0f / 400.0f, 0)).xyz;
    float3 v2 = read_imagef(v_d, sampler, uv + (float2)(0, -1.0f / 400.0f)).xyz;
    float3 v3 = read_imagef(v_d, sampler, uv + (float2)(0, 1.0f / 400.0f)).xyz;
        
    float3 res = 20.0f*((v1 - v0) + (v2 - v3));

    //write_imagef(texture, (int2)(x, y), (float4)(res.xy * res.xy, 0., 1.));
    write_imagef(texture, (int2)(x, y), (float4)(v.xy, bounds, 1.));
}