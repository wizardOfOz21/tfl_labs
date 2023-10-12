#include <cassert>
#include <memory>
#include <vector>

#include "StateMachine.h"
#include "parser/small_tree.hpp"

using std::vector;

struct Node;
using node_ptr = std::unique_ptr<Node>;

enum class NodeType {
    ALTER,
    CONCAT,
    ITER,
    LOOKAHEAD,
    REGEX,
};

struct Node {
    NodeType type;
    vector<node_ptr> args;
    s_node_ptr syntax_tree = nullptr;  // дерево разбора регулярки, если это регулярка, nullptr, если
                  // не ругулярка или пустое слово

    string value = "";  // внешний вид регулярки, для отладки

    static int graph_vertex_count;

    Node(NodeType type_) : type(type_) {}
    bool is_regex() { return type == NodeType::REGEX; }

    static node_ptr empty_regex();
    static node_ptr symbol_regex(char s);
    static node_ptr _node(NodeType type_);

    void iter_regex();
    void plus_regex(node_ptr arg);
    void concat_regex(node_ptr arg);
    void lookahead();

    void add_node(node_ptr arg);
    void merge_node(node_ptr arg);

    static node_ptr paren(node_ptr arg);

    StateMachine to_machine_dfs(int start = 0);

    void to_graph(std::ostream& out);
    void to_graph_dfs(std::ostream& out, const string& parent_name);
};
