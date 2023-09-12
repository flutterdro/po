#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec2 fragPos;

uniform sampler2D tex;

const float PAL_LINES = 576.0;//576.;
const float PAL_WIDTH = 720.0;//720.;

// float Bayer2(vec2 a) {
//     a = floor(a);
//     return fract(a.x / 2. + a.y * a.y * .75);
// }

// #define Bayer4(a)   (Bayer2 (.5 *(a)) * .25 + Bayer2(a))
// #define Bayer8(a)   (Bayer4 (.5 *(a)) * .25 + Bayer2(a))
// #define Bayer16(a)  (Bayer8 (.5 *(a)) * .25 + Bayer2(a))
// #define Bayer32(a)  (Bayer16(.5 *(a)) * .25 + Bayer2(a))
// #define Bayer64(a)  (Bayer32(.5 *(a)) * .25 + Bayer2(a))

// vec4 vignette(in vec2 pos, in vec4 col) {
//     float len = length(pos - vec2(.5));
//     float v = smoothstep(VIGNETTE_RADIUS, VIGNETTE_RADIUS - VIGNETTE_SOFTNESS, len);
//     v = clamp(pow(2. * v, .3), 0., 1.);
//     return vec4(col.rgb * v, 1.);
// }
uniform float iTime;

vec4 scanlines(in vec2 pos, in vec4 col) {
    float scanline = clamp(0.94 + 0.08 * cos(3.141593 * (pos.y + 0.0034 * iTime) * PAL_LINES), 0.0, 1.0);
	float grid = 0.8 + clamp(1.5 * cos(3.141593 * pos.x * PAL_WIDTH), 0.0, 1.0);
    return vec4(col.rgb * scanline * grid *1.2, 1.0);
}

void main() {
    vec3 chessColor;
    vec2 res = vec2(1.0, 1.0);
    if (texture(tex, texCoord).r == 1) {
        chessColor = vec3(0.98, 0.922, 0.843);
    } else {
        chessColor = vec3(0.357, 0.431, 0.882);
    }
    vec2 uv = fragPos / res;
    // float dithering = (Bayer64(fragPos * 0.25) * 2.0 - 1.0) * 0.5;
    // vec2 pixel = fragPos / res;
    // pixel.x += dithering;
    fragColor = scanlines(fragPos, vec4(chessColor, 1.0));
    //fragColor = vec4(mix(vec3(pixel.x < 0.5), chessColor, 0.5), 1.0);

}