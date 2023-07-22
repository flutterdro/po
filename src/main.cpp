#include <fmt/core.h>
#include "rtw/matop.h"

using namespace rtw;

int main() {
    Matrix mat3;
    {float a[]{
        1, 0, 0, 8,
        0, 1, 3, 7
    };
    float b[]{
        2, 1,
        4, 0,
        2, 5
    };
    float c[]{
        1, 1, 1, 1,
        1, 1, 1, 1
    };
    Matrix mat1(a, 4, 2); Matrix mat2(b, 2, 3); Matrix mat4(c, 4, 2);
    mat3 = 2.0f * mat2 * (mat1 + mat4);}
    mat3.print();
    fmt::print("Hello world\n");
}