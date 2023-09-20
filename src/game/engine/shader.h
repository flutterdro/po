#ifndef SHADER_H__
#define SHADER_H__

#include <string>
#include <fstream>
#include <sstream>
#include <type_traits>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../logger/logger.h"

class Shader {
  public:
    std::optional<Shader> load(char const * vs_path, char const * fs_path);

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

    void cleanUp();

    operator GLuint();
  private:
    std::string loadShaderSrc(char const * path) const;
    int compileShader(GLuint& shader, char const * path) const;
  public:
    GLuint m_id;
};

#endif