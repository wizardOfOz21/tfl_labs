#include "parser/tset.hpp"

using std::string;

enum class node_type {
    ALTER,
    CONCAT,
    ITER,
};

struct m_node {
    node_type type;
    char s;
    node_type* left;
    node_type* right;
};
