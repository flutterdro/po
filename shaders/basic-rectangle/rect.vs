#version 410 core
layout (location = 0) in vec2 a_pos;

uniform mat4 projection;
uniform mat4 model;

out vec2 frag_pos;

void main() {
	frag_pos = a_pos;
	gl_Position = projection * model * vec4(a_pos, 0.0, 1.0);
}
