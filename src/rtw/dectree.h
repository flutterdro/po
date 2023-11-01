#ifndef _DECTREE_H_
#define _DECTREE_H_

#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <span>

#include <fmt/format.h>

// namespace rtw {
namespace {
class Node {
public:
    Node() : left(nullptr), right(nullptr), parametr(std::make_pair(NAN, -1)) {}
    Node(Node* left, Node* right, float split, int feature_index) 
        : left(left), right(right), parametr(std::make_pair(split, feature_index)) {}
    Node(float label) : left(nullptr), right(nullptr), parametr(std::make_pair(label, -1)) {}

    Node* left;
    Node* right;
    std::pair<float, int> parametr;
};
}

class DecisionTreeClasifier {
public:
    DecisionTreeClasifier(std::vector<std::vector<float>>&,
                          int max_depth, int splitter, int min_smaple_split);
    DecisionTreeClasifier()                                   = delete;
    DecisionTreeClasifier(const DecisionTreeClasifier& other) = delete;
    DecisionTreeClasifier(DecisionTreeClasifier&& other)      = delete;
    ~DecisionTreeClasifier();
    

    [[nodiscard]]float classify(const std::vector<float>&) const noexcept;

private:
    [[nodiscard]]std::optional<std::pair<float, int>> 
    bestSplit(const std::vector<std::span<float>>&) const noexcept;

    void classifyImpl(const std::vector<float>&, Node*, float&) const noexcept;

    void destroyTree(Node*);
    [[nodiscard]]Node* buildTree(std::vector<std::span<float>>&, int);
private:
    Node* m_root;
    const int max_depth;
};

float gini(const std::vector<std::span<float>>&);


// }

#endif