#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <string>

// #include <TGUI.hpp>
// #include <Backend/GLFW-OpenGL3.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "rtw/matop.h"
#include "rtw/dectree.h"


using namespace rtw;

enum class Iris {
    Iris_Setosa = 1,
    Iris_Versicolor = 2,
    Iris_Virginica = 3,
};



int main() {
    std::vector<std::vector<float>> data;
    if (std::ifstream fin{"../iris/train.data"}) {
        std::string line;
        while (getline(fin, line)) {
            data.push_back(std::vector<float>{});
            std::istringstream ssin(line);
            float buff = 0;
            while (ssin >> buff) {
                data.back().push_back(buff);
            }
        }
    } else {
        fmt::print("failed to open file");
    }

    DecisionTreeClasifier dtree(data, 4, 0, 0);

    std::vector<std::vector<float>> test;
    if (std::ifstream fin{"../iris/test.data"}) {
        std::string line;
        while (getline(fin, line)) {
            test.push_back(std::vector<float>{});
            std::istringstream ssin(line);
            float buff = 0;
            while (ssin >> buff) {
                test.back().push_back(buff);
            }
        }
    } else {
        fmt::print("failed to open file");
    }
    float count_right{0};
    float count{0};
    for (const auto& tes : test) {
        float expected = tes.back();
        float predicted = dtree.classify(tes);
        fmt::print("expected: {}, predicted: {} | ", expected, predicted);
        if (expected == predicted) {
            ++count_right;
            fmt::print("correct\n");
        } else {
            fmt::print("wrong\n");
        }
        ++count;
    }
    fmt::print("------------");
    fmt::print("total accuracy: {}%\n", count_right/count * 100);
    
    // for (auto dat : data) {
    //     for (auto da : dat) {
    //         fmt::print("{} ", da);
    //     }
    //     fmt::print("\n");
    // }
    //fmt::print("{}\n", rtw::gini(data));
    fmt::print("Hello world\n");
}