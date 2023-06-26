int p(int x, int y, int n) {
    return 3 * (max(min(x, n-1), 0) + max(min(y, n-1), 0) * n);
}
int v(int x, int y, int n) {
    return max(min(x, n-1), 0) + max(min(y, n-1), 0) * n;
}

__kernel void wave2d(
             __global float* positions, 
             __global float* velocities,
             __private int n,
             __private float dt
        ) {
    unsigned int x = get_global_id(0);
    unsigned int y = get_global_id(1);

    float pos = positions[p(x, y, n)+1];
    float vel = velocities[v(x, y, n)];

    vel += dt * ((positions[p(x-1, y, n)+1] + positions[p(x+1, y, n)+1] + positions[p(x, y-1, n)+1] + positions[p(x, y+1, n)+1]) * 0.25f - pos);
    vel *= 0.95f;
    pos += dt * vel;

    positions[p(x, y, n)+1] = pos;
    velocities[v(x, y, n)] = vel;
}