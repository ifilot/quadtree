#version 330 core

in vec2 position;
in vec2 texcoord;

out vec2 tc;

uniform mat4 mvp;

void main() {
    tc = texcoord;
    gl_Position = mvp * vec4(position, 0.0, 1.0);
}
