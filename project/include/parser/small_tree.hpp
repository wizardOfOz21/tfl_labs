#include "parser/tset.hpp"

using std::string;

enum class node_type {
    ALTER,
    CONCAT,
    ITER,
    SYMBOL,
};

int count = 1;
struct m_node {
    node_type type;
    char s;
    m_node* left = nullptr;
    m_node* right = nullptr;
    
    TSet* dfs() {
        switch (type) {
            case node_type::ALTER:
            {
                assert(left);
                assert(right);
                TSet* A = left->dfs();
                TSet* B = right->dfs();
                return TSet::plus(A,B);
            }
            case node_type::CONCAT:
            {
                assert(left);
                assert(right);
                TSet* A = left->dfs();
                TSet* B = right->dfs();
                return TSet::concat(A,B);
            }
            case node_type::ITER:
            {
                assert(left);
                assert(!right);
                TSet* A = left->dfs();
                return TSet::iter(A);
            }
            case node_type::SYMBOL:
            {
                if (s == 0) return new TSet();
                return new TSet(cchar{s,count++});
            }
        }
    }
};
