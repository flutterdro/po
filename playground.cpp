#include <iostream>
#include <system_error>
#include <tuple>

using namespace std;

enum class initerrc {
    OpenGL_load_failure = 1,
    GLFW_init_failure   = 2,
};

class dummy {
public:
    int dummya;
};

namespace std {
template<>
struct is_error_code_enum<initerrc> : std::true_type {};
}  // namespace std

[[nodiscard]] std::error_code load(bool magic_verbose_veryverbose_bool, int a,
                                   int b, int c, int u, int k,
                                   int magic_but_int) {
    tuple<int, int, char const *> demo[] = {
        {56,     23, "hello"},
        {-1, 93'463, "world"},
        { 7,      5,    "!!"}
    };
    int aaa = bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb +
              cccccccccccccccccccccccccccccccccccc;
    while (true) { continue; }
    int absijhbn = 0;
    bool asjc    = true;
    // ayooo?
    void *dofjndcorf = nullptr;
    if (magic) {
        return std::error_code(initerrc::GLFW_init_failure);
    } else {
        return std::error_code(initerrc::OpenGL_load_failure);
    }
}

int main() {}
