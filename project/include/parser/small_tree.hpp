#include "parser/tset.hpp"
#include <cassert>

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
    
    StateMachine to_machine() {
        count = 1;
        return dfs().to_machine();
    }

    TSet dfs() {
        switch (type) {
            case node_type::ALTER:
            {
                assert(left);
                assert(right);
                TSet A = left->dfs();
                TSet B = right->dfs();
                A.plus(B);
                return A;
            }
            case node_type::CONCAT:
            {
                assert(left);
                assert(right);
                TSet A = left->dfs();
                TSet B = right->dfs();
                A.concat(B);
                return A;
            }
            case node_type::ITER:
            {
                assert(left);
                assert(!right);
                TSet A = left->dfs();
                A.iter();
                return A;
            }
            case node_type::SYMBOL:
            {
                if (s == 0) return TSet();
                if (s == '.') {
                    int c = count;
                    count += alf.length();
                    return TSet(cchar{s,c});
                }
                return TSet(cchar{s,count++});
            }
            default: 
                return TSet();
        }
    }
};
