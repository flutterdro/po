#ifndef RESOURCE_MANAGER_H__
#define RESOURCE_MANAGER_H__

#include <unordered_map>
#include <string>
#include <string_view>
#include <optional>

#include "shader.h"
#include "texture.h"
#include "sprite.h"

// singleton pattern
// potential issue with destruction order fiasco

auto compile_shaders()
    -> void;
class ResourceManager {
private:
    ResourceManager();
public:
    ResourceManager(const ResourceManager&)           = delete;
    ResourceManager operator=(const ResourceManager&) = delete;
    ~ResourceManager();
    [[nodiscard]] static auto get_instance() //
        -> ResourceManager&;
    auto submit_shader(std::string const&, Shader)
        -> void;
    auto load_texture_image(char const * path, std::string const& handle)
        -> Texture;
    auto loadTexture(unsigned char (&data)[64], std::string const& handle)
        -> Texture;
    auto emplaceSprite(std::string const& handle, Shader shader, Texture texture)
        -> Sprite;
    auto emplaceSprite(std::string const& handle, Shader shader, Texture texture, VAO vao)
        -> Sprite;
    [[nodiscard]] 
    auto request_shader(std::string const& handle, source_location loc = source_location::current()) const noexcept //
        -> Shader;
    [[nodiscard]] 
    auto request_texture(std::string const& handle, source_location loc = source_location::current()) const noexcept //
        -> Texture;
    [[nodiscard]] 
    auto request_sprite(std::string const& handle, source_location loc = source_location::current()) const noexcept //
        -> Sprite;
    void free_all();
private:
    VAO def_vao;
    Shader def_shader;
    Texture def_texture;
    Sprite def_sprite;
private:
    std::unordered_map<std::string, Shader> m_shaders;
    std::unordered_map<std::string, Texture> m_textures;
    std::unordered_map<std::string, Sprite> m_sprites;
};

#endif