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
class RM {
  private:
    RM();
  public:
    RM(const RM&)           = delete;
    RM operator=(const RM&) = delete;
    [[nodiscard]] static auto getInstance() //
        -> RM&;
    void loadShader(char const * vs_path, char const * fs_path,
                    std::string const& handle);
    void loadTexture(char const * path, std::string const& handle);
    void loadTexture(unsigned char (&data)[64], std::string const& handle);
    void loadSprite(Shader shader, std::string const& handle);
    [[nodiscard]] auto getShader(std::string const& handle) const noexcept //
        -> std::optional<Shader>;
    [[nodiscard]] auto getTexture(std::string const& handle) const noexcept //
        -> std::optional<Texture>;
    [[nodiscard]] auto getSprite(std::string const& handle) const noexcept //
        -> std::optional<Sprite>;
    void cleanUp();
  private:
    std::unordered_map<std::string, Shader> m_shaders;
    std::unordered_map<std::string, Texture> m_textures;
    std::unordered_map<std::string, Sprite> m_sprites;
};

#endif