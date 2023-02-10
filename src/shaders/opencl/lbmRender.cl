__constant sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_REPEAT | CLK_FILTER_LINEAR;

__kernel void render_lbm(
        __read_only image2d_t boundaries,
        __private float time, 
        __write_only image2d_t texture
        ) {
    unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);
    float2 uv = (float2)((float)(x) / (float)(get_global_size(0)), (float)(y) / (float)(get_global_size(1)));

    unsigned int bounds = read_imageui(boundaries, sampler, uv).x;

    write_imagef(texture, (int2)(x, y), (float4)(bounds, bounds, bounds, 1.));
}