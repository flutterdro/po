#ifndef RECTANGLE_H__DIJCN928bduc0w
#define RECTANGLE_H__DIJCN928bduc0w

#include <cmath>
#include <limits>

#include <glm/glm.hpp>

template<std::floating_point T>
auto fcomp(T a, T b) noexcept
    -> bool {
    return std::fabs(a - b) < std::numeric_limits<T>::epsilon() * 
        std::fmax(std::fmax(std::fabs(b), std::fabs(a)), 1.0f);
}


struct Rectangle {
    /**
     * @brief tlc == top-left corner
     */
    glm::vec2 tlc_pos{0.0f, 0.0f};
    /**
     * @brief {.x == width, .y == height}
    */
    glm::vec2 size{0.0f, 0.0f};
    /**
     * @brief specifies an angle of rotation of the rectangle
     * @warning rotation is specified as a rotation around the centre NOT
     * a top left corner
    */
    float angle{0.0f};
    /**
     * @brief returns true if... shouldn't it be obvious?
    */
    [[nodiscard]]
    auto contains(glm::vec2 point) const noexcept
        ->bool { 
        glm::vec2 diff = point - tlc_pos;
        return diff.x < size.x && 
               diff.x > 0.0f   &&
               diff.y < size.y &&
               diff.y > 0.0f; 
    }
};

#endif