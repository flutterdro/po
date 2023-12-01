#include "sprite.h"

namespace engine {

vao::vao() noexcept {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    std::array<float, 16> vertices{
        0.0f,  0.0f, 0.0f, 0.0f, //
        0.0f,  1.0f, 0.0f, 1.0f,
        1.0f,  1.0f, 1.0f, 1.0f, //
        1.0f,  0.0f, 1.0f, 0.0f, //
    };
    std::array<unsigned int, 6> indices{
        0, 1, 3, 1, 2, 3,
    };

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void const*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void const*>(sizeof(float) * 2));
    glBindVertexArray(0);
}

auto vao::create_fallback() noexcept
    -> vao {
    return vao{};
}

void vao::free() {
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void vao::draw() {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


auto sprite::draw(ItemFrame const& rect)
    -> void {
    assert(glIsProgram((GLuint)shader));
    shader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(rect.tlc_pos, 0.0f));
    if (fcomp(rect.angle, 0.0f)) {
        model = glm::translate(model, glm::vec3(0.5f * rect.size, 0.0f)); 
        model = glm::rotate(model, glm::radians(rect.angle), glm::vec3(0.0f, 0.0f, 1.0f)); 
        model = glm::translate(model, glm::vec3(-0.5f * rect.size, 0.0f));
    }
    model = glm::scale(model, glm::vec3(rect.size, 0.0f));
    shader.set("model", model);
    assert(glIsTexture((GLuint)texture));
    glActiveTexture(GL_TEXTURE0);
    texture.bind();
    vao.draw();
}

auto sprite::get_shader() const noexcept
    -> class shader {
    return shader;
}

}