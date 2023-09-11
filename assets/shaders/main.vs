#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;
uniform mat4 projection;
uniform mat4 model;

void main() {
    texCoord = aTexCoord;
    gl_Position = projection * model * vec4(a_pos, 0.0, 1.0);
}