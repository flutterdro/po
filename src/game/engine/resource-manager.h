#ifndef RESOURCE_MANAGER_H__
#define RESOURCE_MANAGER_H__

#include <unordered_map>
#include <string>
#include <string_view>
#include <optional>

#include "shader.h"
#include "texture.h"
#include "sprite.h"

//singleton pattern 
//potential issue with destruction order fiasco
class RM {
private:
    RM();
public:
    RM(const RM&) = delete;
    RM operator=(const RM&) = delete;
    [[nodiscard]]static RM& getInstance();
    void loadShader(const char* vs_path, const char* fs_path, const std::string& handle);
    void loadTexture(const char* path, const std::string& handle);
    void loadTexture(unsigned char (&data)[64], const std::string& handle);
    void loadSprite(Shader shader, const std::string& handle);
    [[nodiscard]]std::optional<Shader> getShader(const std::string& handle) const noexcept;
    [[nodiscard]]std::optional<Texture> getTexture(const std::string& handle) const noexcept;
    [[nodiscard]]std::optional<Sprite> getSprite(const std::string& handle) const noexcept;
    void cleanUp();
private:
    std::unordered_map<std::string, Shader> m_shaders;
    std::unordered_map<std::string, Texture> m_textures;
    std::unordered_map<std::string, Sprite> m_sprites;
};

#endif