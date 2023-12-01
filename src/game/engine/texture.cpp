#include "texture.h"

namespace engine {

void texture::free() noexcept { glDeleteTextures(1, &m_id); }

texture::texture(char const * path) {
    load_image(path);
}
texture::texture(unsigned char (&data)[64]) {
    load(data);
}

auto texture::create_fallback() noexcept
    -> texture {
    GLuint id;
    unsigned char data[64];
        for (int i = 0; i < 64; ++i) {
            data[i] = static_cast<unsigned char>((i + i / 8) % 2 * 0xFF);
        }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);
    texture texture_;
    texture_.m_id = id;
    return texture_;
}

std::optional<texture> texture::load_image(char const* path) {
    stbi_set_flip_vertically_on_load(!true);
    glGenTextures(1, &m_id);
    int width, height, nr_channels;
    unsigned char* data = stbi_load(path, &width, &height, &nr_channels, 0);
    GLenum color_mode;
    switch (nr_channels) {
    case 1:
        color_mode = GL_RED;
        break;
    case 4:
        color_mode = GL_RGBA;
        break;
    default:
        color_mode = GL_RGB;
    }
    if (data) {
        glBindTexture(GL_TEXTURE_2D, m_id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        log(LogLvl::WARNING, "Failed to load a texture:{}\n", path);
        stbi_image_free(data);
        return {};
    }
    stbi_image_free(data);
    return *this;
}

std::optional<texture> texture::load(unsigned char (&data)[64]) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);
    return *this;
}

void texture::bind() { glBindTexture(GL_TEXTURE_2D, m_id); }

texture::operator GLuint() { return m_id; }

}