#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec2 fragPos;

uniform sampler2D tex;

const float PAL_LINES = 3000.0; // 576.;
const float PAL_WIDTH = 4000.0; // 720.;
uniform float iTime;
uniform bool white;

vec4 scanlines(in vec2 pos, in vec4 col) {
    float scanline = clamp(
        0.94 + 0.08 * cos(3.141593 * (pos.y + 0.0034 * iTime) * PAL_LINES), 0.0,
        1.0);
    float grid = 0.8 + clamp(1.5 * cos(3.141593 * pos.x * PAL_WIDTH), 0.0, 1.0);
    return vec4(col.rgb * scanline * grid * 1.1, 1.0);
}
vec3 Gamma(vec3 value, float param) {
    return vec3(pow(abs(value.r), param),pow(abs(value.g), param),pow(abs(value.b), param));
}

vec3 brightnessContrast(vec3 value, float brightness, float contrast) {
    return (value - 0.5) * contrast + 0.5 + brightness;
}

void main() {
    vec3 darken = vec3(1.0);
    vec4 color = texture(tex, texCoord);
    if (white) {
        // if (color == vec4(0.757, 0.733, 0.702, 1.0)) {
        //     color = vec4(0.439, 0.424, 0.408, 1.0);
        // }
        // darken = vec3(0.98, 0.98, 0.98);
        // color = vec4(Gamma(vec3(color), 2.2), 1.0);
        color = vec4(brightnessContrast(vec3(color), -0.4, 1.9), 1.0);
    }
    fragColor = vec4(vec3(scanlines(fragPos, color)) * darken,
                     texture(tex, texCoord).a);
}