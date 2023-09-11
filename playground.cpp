#include <iostream>
#include <system_error>

enum class initerrc {
    OpenGL_load_failure = 1,
    GLFW_init_failure = 2,
};

namespace std {
    template <>
    struct is_error_code_enum<initerrc> : std::true_type {};
}

[[nodiscard]]std::error_code load(bool magic) {
    if (magic) {
        return std::error_code(initerrc::GLFW_init_failure);
    } else {
        return std::error_code(initerrc::OpenGL_load_failure);
    }
}

int main() {
    
}