#version 330 core

//"in" attributes from our vertex shader
in vec2 texcoord;

// out color
out vec4 color;

//declare uniforms
uniform sampler2D text;
uniform float resolution;
uniform float radius;
uniform vec2 dir;

// code taken from: https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5

void main() {
    //this will be our RGBA sum
    vec4 sum = vec4(0.0);

    //our original texcoord for this fragment
    vec2 tc = texcoord;

    //the amount to blur, i.e. how far off center to sample from
    //1.0 -> blur by one pixel
    //2.0 -> blur by two pixels, etc.
    float blur = radius/resolution;

    //the direction of our blur
    //(1.0, 0.0) -> x-axis blur
    //(0.0, 1.0) -> y-axis blur
    float hstep = dir.x;
    float vstep = dir.y;

    //apply blurring, using a 9-tap filter with predefined Gaussian weights

    sum += texture(text, vec2(tc.x - 4.0 * blur * hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
    sum += texture(text, vec2(tc.x - 3.0 * blur * hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
    sum += texture(text, vec2(tc.x - 2.0 * blur * hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
    sum += texture(text, vec2(tc.x - 1.0 * blur * hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;

    sum += texture(text, vec2(tc.x, tc.y)) * 0.2270270270;

    sum += texture(text, vec2(tc.x + 1.0 * blur * hstep, tc.y + 1.0*blur*vstep)) * 0.1945945946;
    sum += texture(text, vec2(tc.x + 2.0 * blur * hstep, tc.y + 2.0*blur*vstep)) * 0.1216216216;
    sum += texture(text, vec2(tc.x + 3.0 * blur * hstep, tc.y + 3.0*blur*vstep)) * 0.0540540541;
    sum += texture(text, vec2(tc.x + 4.0 * blur * hstep, tc.y + 4.0*blur*vstep)) * 0.0162162162;

    color = sum;
}
