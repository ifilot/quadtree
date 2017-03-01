#version 330 core

uniform sampler2D text;

in vec2 texcoord;

out vec4 color;

void main() {
    color = vec4(vec3(1,1,1) - texture(text, texcoord).rgb, texture(text, texcoord).a);
}
