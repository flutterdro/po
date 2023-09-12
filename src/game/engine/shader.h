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
    std::optional<Shader> load(const char* vs_path, const char* fs_path);

    void set(const std::string& name, int value) noexcept;
    void set(const std::string& name, float value) noexcept;
    void set(const std::string& name, float value_1, float value_2) noexcept;
    void set(const std::string& name, float value_1, float value_2, float value_3) noexcept;
    void set(const std::string& name, glm::vec2 value) noexcept;
    void set(const std::string& name, glm::vec3 value) noexcept;
    void set(const std::string& name, glm::mat3 value) noexcept;
    void set(const std::string& name, glm::mat4 value) noexcept;

    void use() const noexcept;

    void cleanUp();

    operator GLuint();
private:
    std::string loadShaderSrc(const char* path) const;
    int compileShader(GLuint& shader, const char* path) const;
public:
    GLuint m_id;
};




#endif