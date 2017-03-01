#version 330 core

uniform sampler2D tex;

in vec2 tc;
out vec4 outcol;

void main() {
    vec4 col = texture(tex, tc);
    outcol = col;
}
