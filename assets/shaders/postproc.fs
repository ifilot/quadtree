#version 330 core

uniform sampler2D text;

in vec2 texcoord;

out vec4 glColor;

/*
 * @brief increase brightness and contrast of image
 *
 * @param[in] brightness brightness value between 0-1
 * @param[in] contrast   contrast value between 0-2
 */
vec3 adjust_brightness_and_contrast(vec3 value, float brightness, float contrast) {
    return (value - 0.5) * contrast + 0.5 + brightness;
}

vec3 adjust_gamma(vec3 value, float param) {
    return vec3(pow(abs(value.r), param), pow(abs(value.g), param), pow(abs(value.b), param));
}

void main() {

    vec3 color = texture(text, texcoord).rgb;
    color = adjust_gamma(color, 1.2);

    glColor = vec4(color, texture(text, texcoord).a);
}
