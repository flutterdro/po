#ifndef TEXTURE_H__
#define TEXTURE_H__

#include <array>

#include <glad/gl.h>
#include <stb_image.h>
#include <optional>

#include "../../logger/logger.h"

namespace engine {
class texture {
  public:
    texture() = default;
    texture(char const * path);
    texture(unsigned char (&data)[64]);
    static auto create_fallback() noexcept
        -> texture;
    auto load_image(char const * path)
        -> std::optional<texture>;
    auto load(unsigned char (&data)[64])
        -> std::optional<texture>;
    void free() noexcept;

    operator GLuint();
    void bind();
  private:
    GLuint m_id;
};
}

#endif