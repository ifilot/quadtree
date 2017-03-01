#version 330 core

in vec2 position;

uniform mat4 window;

void main() {
    gl_Position = window * vec4(position, 0.0, 1.0);
}
