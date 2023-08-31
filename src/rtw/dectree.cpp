#include "dectree.h"



namespace rtw {

float gini(const std::vector<std::span<float>>& data) {
    float ret = 0;
    std::unordered_map<float, int> count;
    for (int i = 0; i < data.size(); ++i) {
        ++count[data[i][data[0].size() - 1]];
    }
    std::vector<float> frequencies;
    for (const auto& [key, val] : count) {
        float tmp = static_cast<float>(val)/static_cast<float>(data.size());
        tmp *= tmp;
        frequencies.push_back(tmp);
    }
    ret = 1.0f - std::accumulate(frequencies.begin(), frequencies.end(), 0.0f);
    return ret;
}

[[nodiscard]]float DecisionTreeClasifier::classify(const std::vector<float>& feature_set) const noexcept {
    float ret{0.0f};
    classifyImpl(feature_set, m_root, ret);
    return ret;
}

void DecisionTreeClasifier::classifyImpl(const std::vector<float>& feature_set, Node* root, float& ret) const noexcept {
    if (root->parametr.second == -1) {
        //fmt::print("arrived to feature\n");
        ret = root->parametr.first;
        return;
    }
    //fmt::print("comparing feature: {} of {}, with {} -> ", root->parametr.second, feature_set[root->parametr.second], root->parametr.first);
    if (feature_set[root->parametr.second] < root->parametr.first) {
        //fmt::print("choosing left wing -> ");
        classifyImpl(feature_set, root->left, ret);
    } else {
        //fmt::print("choosing right wing ->");
        classifyImpl(feature_set, root->right, ret);
    }
    return;
}

DecisionTreeClasifier::DecisionTreeClasifier(std::vector<std::vector<float>>& data,
                          int max_depth, int splitter, int min_smaple_split) : max_depth(max_depth){
        std::vector<std::span<float>> train_data;
        for (int i = 0; i < data.size(); ++i) {
            std::span<float> buf(data[i].begin(), data[i].end());
            train_data.push_back(buf);
        }
        m_root = buildTree(train_data, 0);
}

DecisionTreeClasifier::~DecisionTreeClasifier() {
    destroyTree(m_root);
}

void DecisionTreeClasifier::destroyTree(Node* root) {
    if (!root) return;
    destroyTree(root->left);
    destroyTree(root->right);
    delete root;
}

[[nodiscard]]Node* DecisionTreeClasifier::buildTree(std::vector<std::span<float>>& data, int depth) {
    if (data.empty()) return new Node(NAN);
    std::optional<std::pair<float, int>> parameter{bestSplit(data)};
    if (!parameter || depth == max_depth) {
        
        float label {
            [](const std::vector<std::span<float>>& data) {
                float ret{0.0f};
                std::unordered_map<float, int> labels{};
                for (const auto& feature_set : data) {
                    ++labels[feature_set.back()];
                }
                ret = std::max_element(labels.begin(), labels.end(), 
                    [](const auto& a, const auto& b) {
                        return a.second < b.second;
                    }) -> first;
                return ret;
            }(data)
        };
        return new Node(label);
    } 
    auto [split, feature_index] = parameter.value();
    std::vector<std::span<float>> left_child_data{};
    std::vector<std::span<float>> right_child_data{};
    for (int value = 0; value < data.size(); ++value) {
        if (split > data[value][feature_index]) {
            left_child_data.push_back(data[value]);
        } else {
            right_child_data.push_back(data[value]);
        }
    }
    std::vector<std::span<float>>().swap(data);
    Node* ret = new Node(buildTree(left_child_data, depth + 1), 
                         buildTree(right_child_data, depth + 1),
                         split, feature_index);
    return ret;
}

[[nodiscard]] std::optional<std::pair<float, int>> 
DecisionTreeClasifier::bestSplit(const std::vector<std::span<float>>& data) const noexcept {
    std::optional<std::pair<float, int>> ret{};
    const float parent_gini{gini(data)};
    float max_information_gain{0.0f};
    if (parent_gini == 0.0f) return ret;
    for (int feature = 0; feature < data[0].size() - 1; ++feature) {
        for (int split = 0; split < data.size(); ++split) {
            std::vector<std::span<float>> left_child{};
            std::vector<std::span<float>> right_child{};
            for (int value = 0; value < data.size(); ++value) {
                if (data[split][feature] > data[value][feature]) {
                    left_child.push_back(data[value]);
                } else {
                    right_child.push_back(data[value]);
                }
            }
            const float weight_left{static_cast<float>(left_child.size())/data.size()};
            const float weight_right{static_cast<float>(right_child.size())/data.size()};
            const float information_gain{
                parent_gini - weight_left*gini(left_child) - weight_right*gini(right_child)
            };
            if (information_gain > max_information_gain) {
                max_information_gain = information_gain;
                ret = std::make_pair(data[split][feature], feature);
            }
        }
    }
    return ret;
}

}