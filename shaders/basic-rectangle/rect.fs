#version 410 core

in vec2 frag_pos;
out vec4 frag_color;

uniform vec3 rect_color;

void main() {
	frag_color = vec4(rect_color, 1.0);
}