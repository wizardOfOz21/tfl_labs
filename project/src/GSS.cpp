#include "GSS.h"

gss_node_sp gss_node::get_node(std::unordered_set<gss_node_sp>& childs,
                                 int state) {
    return std::make_shared<gss_node>(childs, state);
}

gss_node_sp gss_node::get_node(int state) {
    auto sp = std::make_shared<gss_node>();
    sp->state = state;
    return sp;
}

std::unordered_set<gss_node_sp> gss_node::look(gss_node_sp& target, int n) {
    std::unordered_set<gss_node_sp> result;
    look_dfs(target, n, result);
    return result;
}

void gss_node::look_dfs(const gss_node_sp& target, int n,
                     std::unordered_set<gss_node_sp>& result) {
    if (n == 0) {
        result.insert(target);
        return;
    }
    for (const gss_node_sp& child : target->childs) {
        look_dfs(child, n - 1, result);
    }
}
