#ifndef PAINTER_H__
#define PAINTER_H__

#include "shader.h"
#include "texture.h"
#include "../utilities/rectangle.h"
#include "sprite.h"

template<typename T>
class painter {
public:

private:
    Texture m_texture;
    Sprite m_sprite;
    Shader m_shader;
    ItemFrame m_rect;
};

#endif