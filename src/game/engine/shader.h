#ifndef SHADER_H__
#define SHADER_H__

#include <string>
#include <fstream>
#include <sstream>
#include <type_traits>
#include <filesystem>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../logger/logger.h"

namespace fs = std::filesystem;

namespace engine {
class shader {
public:
    shader() = default;
    shader(fs::path const&);

    static auto create_fallback() noexcept
        -> shader;

    void set(std::string const& name, int value) noexcept;
    void set(std::string const& name, float value) noexcept;
    void set(std::string const& name, float value_1, float value_2) noexcept;
    void set(std::string const& name, float value_1, float value_2,
             float value_3) noexcept;
    template<GLsizei size>
    void set(std::string const& name, int const (&arr)[size]) {
        use();
        glUniform1iv(glGetUniformLocation(m_id, name.c_str()), size, arr);
    }
    void set(std::string const& name, glm::vec2 value) noexcept;
    void set(std::string const& name, glm::vec3 value) noexcept;
    void set(std::string const& name, glm::mat3 value) noexcept;
    void set(std::string const& name, glm::mat4 value) noexcept;

    void use() const noexcept;

    void free();

    constexpr explicit operator GLuint() const noexcept { return m_id; }
private:
    std::string load_shader_src(fs::path const&) const;
    int compile_shader(GLuint shader, fs::path const&) const;
private:
    GLuint m_id;
};

}

#endif