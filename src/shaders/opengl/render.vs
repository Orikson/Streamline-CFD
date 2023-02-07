#version 330 core
layout (location = 0) in vec3 pos;

varying vec2 uv;

void main() {
    gl_Position = vec4(pos, 1.);
    uv = pos.xy * 0.5 + 0.5;
}