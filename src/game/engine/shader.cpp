#include "shader.h"

namespace engine {

shader::shader(fs::path const& dir) {
    std::vector<GLuint> shaders{};
    shaders.reserve(24);
    m_id = glCreateProgram();
    auto fail_cleanup = [&]() {
        for (auto& shader : shaders) {
            glDeleteShader(shader);
        }
        glDeleteProgram(m_id);
    };
     for (auto const& dir_entry : fs::directory_iterator{dir}) {
        if (dir_entry.is_regular_file() and
            dir_entry.path().filename().c_str()[0] != '.') {
            if (dir_entry.path().extension() == ".vs") {
                GLuint vs;
                vs = glCreateShader(GL_VERTEX_SHADER);
                int errc = compile_shader(vs, dir_entry.path());
                shaders.push_back(vs);
                if (errc) {
                    fail_cleanup();
                    throw std::runtime_error("Shader compilation error, read logs");
                }
            } else if (dir_entry.path().extension() == ".fs") {
                GLuint fs;
                fs = glCreateShader(GL_FRAGMENT_SHADER);
                int errc = compile_shader(fs, dir_entry.path());
                shaders.push_back(fs);
                if (errc) {
                    fail_cleanup();
                    throw std::runtime_error("Shader compilation error, read logs");
                }
            } else if (dir_entry.path().extension() == ".gs") {
                GLuint gs;
                gs = glCreateShader(GL_GEOMETRY_SHADER);
                int errc = compile_shader(gs, dir_entry.path());
                shaders.push_back(gs);
                if (errc) {
                    fail_cleanup();
                    throw std::runtime_error("Shader compilation error, read logs");
                }
            } else {
                // fmt::print("{}\n", dir_entry.path().extension().c_str());
                log(LogLvl::ERROR, "Encountered invalid file format while processing {} shader\n", dir.c_str());
                throw std::runtime_error("Read logs");
            }
        }
    }

    for (auto shader : shaders) {
        glAttachShader(m_id, shader);
    }
    glLinkProgram(m_id);
    int success;
    char info_log[512];
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_id, 512, nullptr, info_log);
        log(LogLvl::ERROR, "Failed to link a shader program {}\n{}", dir.stem().c_str(), info_log);
        fail_cleanup();
        throw std::runtime_error("Shader linking error, read logs");
    } 
    log(LogLvl::STATUS, "Succesfully linked shader program {}\n", dir.stem().c_str());
    for (auto shader : shaders) {
        glDeleteShader(shader);
    }
}

auto shader::create_fallback() noexcept
    -> shader {
    char const * vs_src =
        "#version 330 core\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 0) in vec2 a_pos;\n"
        "out vec2 texCoord;\n"
        "out vec2 fragPos;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "void main() {\n"
        "    fragPos = a_pos;\n"
        "    texCoord = aTexCoord;\n"
        "    gl_Position = projection * model * vec4(a_pos, 0.0, 1.0);\n"
        "}\n";
    char const * fs_src =
        "#version 330 core\n"
        "out vec4 fragColor;\n"
        "in vec2 texCoord;\n"
        "in vec2 fragPos;\n"
        "uniform sampler2D tex;\n"
        "void main() {\n"
        "vec3 chessColor;\n"
        "vec2 res = vec2(1.0, 1.0);\n"
        "if (texture(tex, texCoord).r == 1) {\n"
        "    chessColor = vec3(0.922, 0.247, 0.953);\n"
        "} else {\n"
        "    chessColor = vec3(0.0, 0.0, 0.0);\n"
        "}\n"
        "fragColor = vec4(chessColor, 1.0);}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    int success;
    char info_log[512];
    glShaderSource(vs, 1, &vs_src, nullptr);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs, 512, nullptr, info_log);
        log(LogLvl::ERROR, "Failed to compile a shader:\n{}", info_log);
    }
    glShaderSource(fs, 1, &fs_src, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fs, 512, nullptr, info_log);
        log(LogLvl::ERROR, "Failed to compile a shader:\n{}", info_log);
    }

    GLuint id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    glDeleteShader(vs);
    glDeleteShader(fs);
    shader shader_;
    shader_.m_id = id;
    return shader_;
}

void shader::use() const noexcept { glUseProgram(m_id); }

void shader::free() { glDeleteProgram(m_id); }

int shader::compile_shader(GLuint shader, fs::path const& file) const {
    int success;
    char info_log[512];
    std::string shader_str  = load_shader_src(file);
    char const * shader_src = shader_str.c_str();
    glShaderSource(shader, 1, &shader_src, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        log(LogLvl::ERROR, "Failed to compile a shader {}\n{}", file.c_str(), info_log);
        return -1;
    }
    log(LogLvl::STATUS, "Succesfully compiled a shader {}\n", file.c_str());
    return 0;
}

auto shader::load_shader_src(fs::path const& path) const
    -> std::string {
    std::string ret;
    std::fstream fin;
    fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fin.open(path);
    std::stringstream sstream;
    sstream << fin.rdbuf();
    ret = sstream.str();
    return ret;
}

void shader::set(std::string const& name, int value) noexcept {
    use();
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}
void shader::set(std::string const& name, float value) noexcept {
    use();
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}
void shader::set(std::string const& name, float value_1,
                 float value_2) noexcept {
    use();
    glUniform2f(glGetUniformLocation(m_id, name.c_str()), value_1, value_2);
}
void shader::set(std::string const& name, glm::vec2 value) noexcept {
    set(name, value.x, value.y);
}
void shader::set(std::string const& name, float value_1, float value_2,
                 float value_3) noexcept {
    use();
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), value_1, value_2,
                value_3);
}
void shader::set(std::string const& name, glm::vec3 value) noexcept {
    set(name, value.x, value.y, value.z);
}
void shader::set(std::string const& name, glm::mat3 value) noexcept {
    use();
    glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(value));
}
void shader::set(std::string const& name, glm::mat4 value) noexcept {
    use();
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(value));
}

}