#include "shader.h"

std::optional<Shader> Shader::load(const char* vs_path, const char* fs_path) {
    GLuint vs, fs;
    vs = glCreateShader(GL_VERTEX_SHADER);
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    bool are_compiled = !compileShader(vs, vs_path) && !compileShader(fs, fs_path);
    if (!are_compiled)  {
        glDeleteShader(vs);
        glDeleteShader(fs);
        return {};
    }

    int success;
    char info_log[512];
    m_id = glCreateProgram();
    glAttachShader(m_id, vs);
    glAttachShader(m_id, fs);
    glLinkProgram(m_id);
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_id, 512, nullptr, info_log);
        LOG(LoggerLvl::ERROR, "Failed to link a program:\n{}", info_log);
        glDeleteProgram(m_id);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return {};
    } else {
        LOG(LoggerLvl::STATUS, "Succesfully linked a program\n");
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return *this;
}

void Shader::use() const noexcept {
    glUseProgram(m_id);
}

void Shader::cleanUp() {
    glDeleteProgram(m_id);
}

int Shader::compileShader(GLuint& shader, const char* path) const {
    int success;
    char info_log[512];
    std::string shader_str = loadShaderSrc(path);
    const char* shader_src = shader_str.c_str();
    glShaderSource(shader, 1, &shader_src, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        LOG(LoggerLvl::ERROR, "Failed to compile a shader:\n{}", info_log);
        return -1;
    }
    LOG(LoggerLvl::STATUS, "Succesfully compiled a shader\n");
    return 0;
}

std::string Shader::loadShaderSrc(const char* path) const {
    std::string ret;
    if (std::fstream fin{path}) {
        std::stringstream sstream;
        sstream << fin.rdbuf();
        ret = sstream.str();
    } else {
        LOG(LoggerLvl::WARNING, "Failed to open a file: {}\n", path);
    }
    return ret;
}

void Shader::set(const std::string& name, int value) noexcept {
    use();
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}
void Shader::set(const std::string& name, float value) noexcept {
    use();
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}
void Shader::set(const std::string& name, float value_1, float value_2) noexcept {
    use();
    glUniform2f(glGetUniformLocation(m_id, name.c_str()), value_1, value_2);
}
void Shader::set(const std::string& name, glm::vec2 value) noexcept {
    set(name, value.x, value.y);
}
void Shader::set(const std::string& name, float value_1, float value_2, float value_3) noexcept {
    use();
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), value_1, value_2, value_3);
}
void Shader::set(const std::string& name, glm::vec3 value) noexcept {
    set(name, value.x, value.y, value.z);
}
void Shader::set(const std::string& name, glm::mat3 value) noexcept {
    use();
    glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::set(const std::string& name, glm::mat4 value) noexcept {
    use();
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

Shader::operator GLuint() {
    return m_id;
}