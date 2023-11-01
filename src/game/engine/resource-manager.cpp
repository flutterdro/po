#include "resource-manager.h"


auto compile_shaders()
    -> void {
    ResourceManager& rm = ResourceManager::get_instance();
    for (auto const& dir_entry : fs::directory_iterator{fs::path{"shaders"}}) {
        if (dir_entry.is_directory()) {
            rm.submit_shader(dir_entry.path().stem().string(), Shader{dir_entry.path()});
        }
    }
}

auto ResourceManager::submit_shader(std::string const& handle, Shader shader)
    -> void {
    auto [it, success] = m_shaders.insert({handle, shader});
    if (not success) {
        log(LogLvl::WARNING, "Shader with a handle \"{}\" already exists \n", handle);
    }
}

ResourceManager::ResourceManager() : def_vao() {
    def_shader = Shader().create_default();
    unsigned char data[64];
    for (int i = 0; i < 64; ++i) {
        data[i] = static_cast<unsigned char>((i + i / 8) % 2 * 0xFF);
    }
    def_texture = *Texture().load(data);
    def_sprite = Sprite(def_vao, def_shader, def_texture);
}

ResourceManager& ResourceManager::get_instance() {
    static ResourceManager * instance = new ResourceManager;
    return *instance;
}

auto ResourceManager::request_shader(std::string const& handle, source_location loc) const noexcept 
    -> Shader {
    if (auto shader_it = m_shaders.find(handle); shader_it != m_shaders.end()) {
        return shader_it->second;
    } else {
        log({LogLvl::WARNING, loc}, "Failed to retrieve shader: {}\n", handle);
        return def_shader;
    }
}

auto ResourceManager::request_texture(std::string const& handle, source_location loc) const noexcept //
    -> Texture {
    if (auto texture_it = m_textures.find(handle);
        texture_it != m_textures.end()) {
        return texture_it->second;
    } else {
        log({LogLvl::WARNING, loc}, "Failed to retrieve texture: {}\n", handle);
        return def_texture;
    }
}
auto ResourceManager::request_sprite(std::string const& handle, source_location loc) const noexcept //
    -> Sprite {
    if (auto sprite_it = m_sprites.find(handle); sprite_it != m_sprites.end()) {
        return sprite_it->second;
    }
    log({LogLvl::WARNING, loc}, "Failed to retrieve sprite: {}\n", handle);
    return def_sprite;
}

void ResourceManager::free_all() {
    for (auto [handle, shader] : m_shaders) {
        shader.clean_up();
    }
    for (auto [handle, texture] : m_textures) {
        texture.cleanUp();
    }
    def_vao.clean_up();
    def_texture.cleanUp();
    def_shader.clean_up();
}

auto ResourceManager::load_texture_image(char const * path, std::string const& handle) 
    -> Texture {
    auto texture_opt = Texture().load_image(path);
    if (texture_opt.has_value()) {
        auto [it, success] = m_textures.insert({handle, texture_opt.value()});
        if (!success) {
            log(LogLvl::WARNING,
                "Texture with a handle \"{}\" already exists\n", handle);
            return def_texture;
        }
        return *texture_opt;
    }
    return def_texture;
}

auto ResourceManager::loadTexture(unsigned char (&data)[64], std::string const& handle) 
    -> Texture {
    auto texture_opt = Texture().load(data);
    if (texture_opt.has_value()) {
        auto [it, success] = m_textures.insert({handle, texture_opt.value()});
        if (!success) {
            log(LogLvl::WARNING,
                "Texture with a handle \"{}\" already exists\n", handle);
        }
    } else {
        log(LogLvl::WARNING, "Failed to load a texture\n");
    }
    return def_texture;
}

auto ResourceManager::emplaceSprite(std::string const& handle, Shader shader, Texture texture, VAO vao) 
    -> Sprite {
    auto [it, success] = m_sprites.insert({handle, Sprite(vao, shader, texture)});
    if (!success) {
        log(LogLvl::WARNING,
            "Sprite with a handle \"{}\" already exists\n", handle);
            return def_sprite;
    }
    return it->second;
}

auto ResourceManager::emplaceSprite(std::string const& handle, Shader shader, Texture texture) 
    -> Sprite {
    return emplaceSprite(handle, shader, texture, def_vao);
}

ResourceManager::~ResourceManager() {
    free_all();
}