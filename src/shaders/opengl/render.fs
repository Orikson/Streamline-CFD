#version 330 core

out vec4 fragColor;

uniform sampler2D tex;
varying vec2 uv;

void main() {
    fragColor = texture(tex, uv);
}
