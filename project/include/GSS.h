#include <vector>
#include <cassert>
#include <sstream>
#include <unordered_set>

struct gss_node {
    std::unordered_set<gss_node*> childs;
    int state;
    int parents = 0;
    
    static gss_node* get_node(std::unordered_set<gss_node*>& childs, int state) {
        for (auto child : childs) {
            child->parents++;
        }
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
        parents++;
        return node;
    }

    void push(gss_node* node) {
        node->childs.insert(this);
        parents++;
    }

    void pop_dfs(int n, std::unordered_set<gss_node*>& result) {
        if (n == 0) {
            result.insert(this);
            return;
        }
        std::unordered_set<gss_node*> heads = childs;
        if (this->parents == 0) {
            delete this;
            for (gss_node* head : heads) {
                head->pop_dfs(n-1, result);
            }
        } else {
            for (gss_node* head : heads) {
                head->look_dfs(n-1, result);
            }
        }
    }

    void look_dfs(int n, std::unordered_set<gss_node*>& result) {
        if (n == 0) {
            result.insert(this);
            return;
        }
        for (gss_node* parent : childs) {
            parent->pop_dfs(n-1, result);
        }
    }

    std::unordered_set<gss_node*> pop(int n) {
        assert(this->parents == 0); // нельзя удалять не-листья
        std::unordered_set<gss_node*> result;
        this->pop_dfs(n, result);
        return result;
    }

    std::unordered_set<gss_node*> look(int n) {
        std::unordered_set<gss_node*> result;
        this->look_dfs(n, result);
        return result;
    }

    ~gss_node() {
        for(gss_node* parent : childs) {
            parent->parents--;
        }
    }
};
