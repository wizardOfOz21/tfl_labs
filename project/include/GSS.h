#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Forest.h"

struct gss_node;

using gss_node_sp = std::shared_ptr<gss_node>;
using path_map = std::unordered_map<gss_node_sp, std::vector<std::vector<parse_vertex_sp>>>;

struct gss_node {
    std::unordered_set<gss_node_sp> childs;
    int state;
    parse_vertex_sp parse_vertex;

    gss_node(std::unordered_set<gss_node_sp>& childs, int state,
             parse_vertex_sp pp)
        : childs(childs), state(state), parse_vertex(pp) {}
    gss_node() = default;
    static gss_node_sp get_node(std::unordered_set<gss_node_sp>& childs,
                                int state, parse_vertex_sp pp = nullptr);
    static gss_node_sp get_node(int state, parse_vertex_sp pp = nullptr);
    static path_map look(gss_node_sp& target, int n) {
        path_map result;
        look_dfs(target, n, result, {});
        return result;
    }

   private:
    static void look_dfs(const gss_node_sp& target, int n, path_map& result,
                         std::vector<parse_vertex_sp> path) {
        if (n == 0) {
            result[target].push_back(path);
            return;
        }
        path.push_back(target->parse_vertex);
        for (const gss_node_sp& child : target->childs) {
            look_dfs(child, n - 1, result, path);
        }
    }
};
