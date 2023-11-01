#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec2 fragPos;

uniform sampler2D tex;

const float PAL_LINES = 576.0; // 576.;
const float PAL_WIDTH = 720.0; // 720.;

uniform float iTimee;
uniform int board[64];

vec4 scanlines(in vec2 pos, in vec4 col) {
    float scanline = clamp(
        0.94 + 0.08 * cos(3.141593 * (pos.y + 0.0034 * iTimee) * PAL_LINES),
        0.0, 1.0);
    float grid = 0.8 + clamp(1.5 * cos(3.141593 * pos.x * PAL_WIDTH), 0.0, 1.0);
    return vec4(col.rgb * scanline * grid * 1.2, 1.0);
}

vec4 drawCircle(in vec2 pos, in int crow, in int ccol, in vec4 color) {
    vec2 res    = vec2(800.0, 800.0);
    vec2 center = vec2(ccol + 0.5, crow + 0.5) / 8.0;
    vec3 colorc = vec3(1.0);
    if (distance(center, pos) < 1.0 / 27.0) { colorc = vec3(0.5); }
    return vec4(color.rgb * colorc, 1.0);
}

void main() {
    vec3 chessColor;
    vec2 res = vec2(1.0, 1.0);
    if (texture(tex, texCoord).r == 1) {
        chessColor = vec3(0.98, 0.922, 0.843);
    } else {
        chessColor = vec3(0.357, 0.431, 0.882);
    }
    vec2 uv  = fragPos;
    vec4 ret = scanlines(fragPos, vec4(chessColor, 1.0));
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i * 8 + j] == 1) { ret = drawCircle(fragPos, i, j, ret); }
        }
    }
    fragColor = ret;
}