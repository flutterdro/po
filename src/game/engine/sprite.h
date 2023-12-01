#ifndef SPRITE_H__
#define SPRITE_H__

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <array>

#include "shader.h"
#include "texture.h"
#include "../utilities/rectangle.h"
namespace engine {
/**
 * @brief simple VAO wrapper
 */
class vao {
  public:
	vao() noexcept;
	static auto create_fallback() noexcept
		-> vao;
	auto free()
		-> void;
	auto draw()
		-> void;
  private:
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;
};


class sprite {
public:

	auto draw(ItemFrame const&)
		-> void;
	auto get_shader() const noexcept
		-> shader;

	vao vao;
	shader shader;
	texture texture;
};
}

#endif