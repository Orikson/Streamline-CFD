#pragma OPENCL EXTENSION cl_intel_printf : enable

// Example kernel 
__kernel void mandelbrot(
        __private float time, 
        __write_only image2d_t texture
        ) {
    unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);
    float2 uv = (float2)((float)(x) / (float)(get_global_size(0)), (float)(y) / (float)(get_global_size(1))) * 2.0f - 1.0f;

    float rTime = time * 0.01f;
    float dTime = 0.27f;

    // Julia set with f(z) = z^n + c
    float R = 2.0f;
    float n = 3.0f;
    float2 c = 0.7885f * (float2)(cos(rTime), sin(rTime));
    //float2 c = 0.7885 * (float2)(cos(dTime), sin(dTime));
    
    float2 z = R * uv;
    
    int iter = 0;
    int max_iter = 100;
    
    while (dot(z, z) < R * R && iter < max_iter) {
        float tmp = n * atan2(z.y, z.x);
        z = pow(dot(z, z), n * 0.5f) * (float2)(cos(tmp), sin(tmp)) + c;
        iter = iter + 1;
    }
    float res = (float)(iter) / (float)(max_iter);
    res = sqrt(res);

    //write_imagef(texture, (int2)(get_global_size(0)-x-1, get_global_size(1)-y-1), (float4)(0.0f, res, 1.0f, 1.0f));
    write_imagef(texture, (int2)(x, y), (float4)(0.0f, 0.2f*sin(res), 2.0f*sin(res), 1.0f));
}