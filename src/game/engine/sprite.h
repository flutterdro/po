#ifndef SPRITE_H__
#define SPRITE_H__

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <array>

#include "shader.h"
#include "texture.h"
#include "../utilities/rectangle.h"

class Sprite {
  public:
    Sprite() = default;

    Sprite load(Shader shader);
    void cleanUp();

    void draw(Texture texture, Rectangle const& rect);
  private:
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    Shader m_shader;
};

#endif