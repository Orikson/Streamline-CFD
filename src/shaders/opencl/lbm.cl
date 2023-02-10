__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_REPEAT | CLK_FILTER_LINEAR;

__kernel void lbm_2d_stream_and_collide(
        __read_only image2d_t boundaries,
        __read_only image2d_t df_0,
        __read_only image2d_t df_14,
        __read_only image2d_t df_58,
        __read_only image2d_t v_d,
        __read_only image2d_t dfeq_0,
        __read_only image2d_t dfeq_14,
        __read_only image2d_t dfeq_58,
        __private float dt, 
        __write_only image2d_t of_0,
        __write_only image2d_t of_14,
        __write_only image2d_t of_58
        ) {
    unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);
    int2 uv = (int2)(x, y);
    
    unsigned int bounds = read_imageui(boundaries, sampler, uv).x;
    float f_0 = read_imagef(df_0, sampler, uv).x;
    float4 f_14 = read_imagef(df_14, sampler, uv);
    float4 f_58 = read_imagef(df_58, sampler, uv);

    float feq_0 = read_imagef(dfeq_0, sampler, uv).x;
    float4 feq_14 = read_imagef(dfeq_14, sampler, uv);
    float4 feq_58 = read_imagef(dfeq_58, sampler, uv);


    // 


    write_imagef(of_0, (int2)(x, y), (float4)(bounds, bounds, bounds, 1.));
}