#version 330 core

in vec2 position;
in vec2 texture_coordinate;

out vec2 texcoord;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position.xy, 0.0, 1.0);
    texcoord = texture_coordinate;
}
