#include "resource-manager.h"

RM::RM() {}

RM& RM::getInstance() {
    static RM * instance = new RM;
    return *instance;
}

std::optional<Shader> RM::getShader(std::string const& handle) const noexcept {
    if (auto shader_it = m_shaders.find(handle); shader_it != m_shaders.end()) {
        return shader_it->second;
    } else {
        return {};
    }
}

std::optional<Texture>
RM::getTexture(std::string const& handle) const noexcept {
    if (auto texture_it = m_textures.find(handle);
        texture_it != m_textures.end()) {
        return texture_it->second;
    } else {
        return {};
    }
}

std::optional<Sprite> RM::getSprite(std::string const& handle) const noexcept {
    if (auto sprite_it = m_sprites.find(handle); sprite_it != m_sprites.end()) {
        return sprite_it->second;
    } else {
        return {};
    }
}

void RM::cleanUp() {
    for (auto [handle, shader] : m_shaders) {
        shader.cleanUp();
    }
    for (auto [handle, texture] : m_textures) {
        texture.cleanUp();
    }
    for (auto [handle, sprite] : m_textures) {
        sprite.cleanUp();
    }
}

void RM::loadShader(char const * vs_path, char const * fs_path,
                    std::string const& handle) {
    auto shader_opt = Shader().load(vs_path, fs_path);
    if (shader_opt.has_value()) {
        auto [it, success] = m_shaders.insert({handle, shader_opt.value()});
        if (!success) {
            LOG(LoggerLvl::WARNING,
                "Shader with a handle \"{}\" already exists", handle);
        }
    } else {
        LOG(LoggerLvl::ERROR,
            "Failed to load a shader into the resource manager\n");
    }
}

void RM::loadTexture(char const * path, std::string const& handle) {
    auto texture_opt = Texture().load(path);
    if (texture_opt.has_value()) {
        auto [it, success] = m_textures.insert({handle, texture_opt.value()});
        if (!success) {
            LOG(LoggerLvl::WARNING,
                "Texture with a handle \"{}\" already exists", handle);
        }
    }
}

void RM::loadTexture(unsigned char (&data)[64], std::string const& handle) {
    auto texture_opt = Texture().load(data);
    if (texture_opt.has_value()) {
        auto [it, success] = m_textures.insert({handle, texture_opt.value()});
        if (!success) {
            LOG(LoggerLvl::WARNING,
                "Texture with a handle \"{}\" already exists", handle);
        }
    } else {
        LOG(LoggerLvl::WARNING, "Failed to load a texture\n");
    }
}

void RM::loadSprite(Shader shader, std::string const& handle) {
    Sprite sprite      = Sprite().load(shader);
    auto [it, success] = m_sprites.insert({handle, sprite});
    if (!success) {
        LOG(LoggerLvl::WARNING, "Texture with a handle \"{}\" already exists",
            handle);
    }
}