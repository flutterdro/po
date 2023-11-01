#ifndef TEXTURE_H__
#define TEXTURE_H__

#include <array>

#include <glad/gl.h>
#include <stb_image.h>

#include "../../logger/logger.h"

class Texture {
  public:
    Texture() = default;

    std::optional<Texture> load_image(char const * path);
    std::optional<Texture> load(unsigned char (&data)[64]);
    void cleanUp() noexcept;

    operator GLuint();
    void bind();
  private:
    GLuint m_id;
};

#endif