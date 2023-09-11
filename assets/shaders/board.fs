#version 330 core

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D tex;

void main() {
    vec4 darken = vec4(0.6, 0.6, 0.6, 1.0);
    vec4 lighten = vec4(1.3, 1.3, 1.3, 1.0);
    if (texture(tex, texCoord).r == 1) {
        fragColor = vec4(0.98, 0.922, 0.843, 1.0)*lighten;
    } else {
        fragColor = vec4(0.357, 0.431, 0.882, 1.0)*darken;
    }
    //frag_color = vec4(vec3(texture(tex, tex_coord).r), 1.0);
}