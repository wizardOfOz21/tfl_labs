#include <cassert>
#include <memory>
#include <sstream>
#include <unordered_set>
#include <vector>

struct gss_node;

using gss_node_sp = std::shared_ptr<gss_node>;

struct gss_node {
    std::unordered_set<gss_node_sp> childs;
    int state;

    static gss_node_sp get_node(std::unordered_set<gss_node_sp>& childs,
                                int state) {
        return std::make_shared<gss_node>(childs, state);
    }

    static gss_node_sp get_node(int state) {
        auto sp = std::make_shared<gss_node>();
        sp->state = state;
        return sp;
    }

    static std::unordered_set<gss_node_sp> look(gss_node_sp& target, int n) {
        std::unordered_set<gss_node_sp> result;
        look_dfs(target, n, result);
        return result;
    }

   private:
    static void look_dfs(const gss_node_sp& target, int n, std::unordered_set<gss_node_sp>& result) {
        if (n == 0) {
            result.insert(target);
            return;
        }
        for (const gss_node_sp& child : target->childs) {
            look_dfs(child, n - 1, result);
        }
    }
};
