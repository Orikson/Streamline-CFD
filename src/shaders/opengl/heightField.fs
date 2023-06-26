#version 430 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;

void main() {
    // compute normal
    vec3 dx = dFdx(FragPos);
    vec3 dy = dFdy(FragPos);
    vec3 N = normalize(cross(dx, dy));
    //N = Normal;

    FragColor = vec4(normalize(N.xz * N.xz), 0., 1.);
} 