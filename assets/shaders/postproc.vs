#version 330 core

in vec3 position;

out vec2 texcoord;

uniform sampler2D text;
uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position.xy, 0.0, 1.0);
    texcoord = (position.xy + 1.0) / 2.0;
}
