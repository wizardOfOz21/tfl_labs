#include <vector>
#include <cassert>
#include <sstream>
#include <unordered_set>

struct gss_node {
    std::vector<gss_node*> parents;
    int state;
    int childs = 0;
    
    static gss_node* get_node(std::vector<gss_node*>& parents, int state) {
        for (auto parent : parents) {
            parent->childs++;
        }
        return new gss_node{parents, state};
    }

    std::string to_graph_vertex() {
        std::stringstream ss;
        ss << state << "(" << this << ")"; 
        return ss.str();
    }

    gss_node* push(int value) {
        gss_node* node = new gss_node{{this}, value};
        childs++;
        return node;
    }

    void push(gss_node* node) {
        node->parents.push_back(this);
        childs++;
    }

    void pop_dfs(int n, std::unordered_set<gss_node*>& result) {
        if (n == 0) {
            result.insert(this);
            return;
        }
        std::vector<gss_node*> heads = parents;
        if (this->childs == 0) {
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
        for (gss_node* parent : parents) {
            parent->pop_dfs(n-1, result);
        }
    }

    std::unordered_set<gss_node*> pop(int n) {
        assert(this->childs == 0); // нельзя удалять не-листья
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
        for(gss_node* parent : parents) {
            parent->childs--;
        }
    }
};
