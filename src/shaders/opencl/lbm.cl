__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

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

