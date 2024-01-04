#include <memory>
#include <unordered_set>

struct gss_node;

using gss_node_sp = std::shared_ptr<gss_node>;

struct gss_node {
public:
    std::unordered_set<gss_node_sp> childs;
    int state;

    static gss_node_sp get_node(std::unordered_set<gss_node_sp>& childs,
                                int state);
    static gss_node_sp get_node(int state);
    static std::unordered_set<gss_node_sp> look(gss_node_sp& target, int n);
    gss_node(std::unordered_set<gss_node_sp> childs,int state):childs(childs),state(state){}
    gss_node()=default;

   private:
    static void look_dfs(const gss_node_sp& target, int n, std::unordered_set<gss_node_sp>& result);
};
