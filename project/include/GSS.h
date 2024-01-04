#include <vector>
#include <cassert>
#include <sstream>
#include <unordered_set>

struct gss_node {
    std::unordered_set<gss_node*> childs;
    int state;
    
    static gss_node* get_node(std::unordered_set<gss_node*>& childs, int state) {
        return new gss_node{childs, state};
    }

    static gss_node* get_node(int state) {
        return new gss_node{{}, state};
    }

    std::string to_graph_vertex() {
        std::stringstream ss;
        ss << state << "(" << this << ")"; 
        return ss.str();
    }

    gss_node* push(int value) {
        gss_node* node = new gss_node{{this}, value};
        return node;
    }

    void push(gss_node* node) {
        node->childs.insert(this);
    }

    void look_dfs(int n, std::unordered_set<gss_node*>& result) {
        if (n == 0) {
            result.insert(this);
            return;
        }
        for (gss_node* parent : childs) {
            parent->look_dfs(n-1, result);
        }
    }

    std::unordered_set<gss_node*> look(int n) {
        std::unordered_set<gss_node*> result;
        this->look_dfs(n, result);
        return result;
    }
};
