#include "resource-manager.h"

RM::RM() {}

RM& RM::getInstance() {
    static RM* instance = new RM;
    return *instance;
}

std::optional<Shader> RM::getShader(const std::string& handle) const noexcept {
    if (auto shader_it = m_shaders.find(handle); shader_it != m_shaders.end()) {
        return shader_it->second;
    } else {
        return {};
    }
}

std::optional<Texture> RM::getTexture(const std::string& handle) const noexcept {
    if (auto texture_it = m_textures.find(handle); texture_it != m_textures.end()) {
        return texture_it->second;
    } else {
        return {};
    }
}

void RM::cleanUp() {
    for (auto [handle, shader] : m_shaders) {
        shader.cleanUp();
    }
    for (auto [handle, texture]: m_textures) {
        texture.cleanUp();
    }
}

void RM::loadShader(const char* vs_path, const char* fs_path, const std::string& handle) {
    auto shader_opt = Shader().load(vs_path, fs_path);
    if (shader_opt.has_value()) {
        auto [it, success] = m_shaders.insert({handle, shader_opt.value()});
        if (!success) {
            LOG(LoggerLvl::WARNING, "Shader with a handle \"{}\" already exists", handle);
        }
    } else {
        LOG(LoggerLvl::ERROR, "Failed to load a shader into the resource manager\n");
    }
}

void RM::loadTexture(const char* path, const std::string& handle) {
    auto texture_opt = Texture().load(path);
    if (texture_opt.has_value()) {
        auto [it, success] = m_textures.insert({handle, texture_opt.value()});
        if (!success) {
            LOG(LoggerLvl::WARNING, "Texture with a handle \"{}\" already exists", handle);
        }
    }
}

void RM::loadTexture(unsigned char (&data)[64], const std::string& handle) {
    auto texture_opt = Texture().load(data);
    if (texture_opt.has_value()) {
        auto [it, success] = m_textures.insert({handle, texture_opt.value()});
        if (!success) {
            LOG(LoggerLvl::WARNING, "Texture with a handle \"{}\" already exists", handle);
        }
    } else {
        LOG(LoggerLvl::WARNING, "Failed to load a texture\n");
    }
}