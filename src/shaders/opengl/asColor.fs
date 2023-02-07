#version 330 core

out vec4 fragColor;

varying vec2 uv;

void main() {
    fragColor = vec4(uv, 0., 1.);
}
