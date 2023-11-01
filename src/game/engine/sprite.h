#ifndef SPRITE_H__
#define SPRITE_H__

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <array>

#include "shader.h"
#include "texture.h"
#include "../utilities/rectangle.h"

/**
 * @brief simple VAO wrapper
 */
class VAO {
  public:
	VAO();

	void clean_up();

	void draw();
  private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;
};

class Sprite {
public:
	Sprite() = default;
	Sprite(VAO vao, Shader shader, Texture texture);

	auto draw(ItemFrame const&)
		-> void;
	auto get_shader() const noexcept
		-> Shader;
private:
	VAO m_vao;
	Shader m_shader;
	Texture m_texture;
};

#endif