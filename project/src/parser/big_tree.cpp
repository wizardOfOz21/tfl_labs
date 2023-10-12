#include "parser/big_tree.hpp"

node_ptr Node::empty_regex() { return std::make_unique<Node>(NodeType::REGEX); }

node_ptr Node::symbol_regex(char s) {
    node_ptr res = std::make_unique<Node>(NodeType::REGEX);
    res->value = string(1, s);
    res->syntax_tree = s_node::symbol_node(s);
    return res;
}

node_ptr Node::_node(NodeType type_) { return std::make_unique<Node>(type_); }

void Node::iter_regex() {
    assert(type == NodeType::REGEX);  // lookahead под звездочкой не бывает
    value += "*";
    syntax_tree =
        s_node::_node(node_type::ITER, std::move(syntax_tree), nullptr);
}

void Node::plus_regex(node_ptr arg) {
    assert(arg->type == NodeType::REGEX);
    value += "|" + arg->value;
    syntax_tree = s_node::_node(node_type::ALTER, std::move(syntax_tree),
                                std::move(arg->syntax_tree));
};

void Node::concat_regex(node_ptr arg) {
    assert(arg->type == NodeType::REGEX);
    value += arg->value;
    syntax_tree = s_node::_node(node_type::CONCAT, std::move(syntax_tree),
                                std::move(arg->syntax_tree));
};

void Node::lookahead() {
    value += ".*";
    s_node_ptr dot = s_node::symbol_node('.');
    dot = s_node::_node(node_type::ITER, std::move(dot), nullptr);
    syntax_tree = s_node::_node(node_type::CONCAT, std::move(syntax_tree),
                                std::move(dot));
}

void Node::add_node(node_ptr arg) {
    assert(type == NodeType::ALTER || type == NodeType::CONCAT);
    args.push_back(std::move(arg));
}

void Node::merge_node(node_ptr arg) {
    assert(type == NodeType::ALTER || type == NodeType::CONCAT);
    assert(arg->type == NodeType::ALTER || arg->type == NodeType::CONCAT);
    for (node_ptr& p : arg->args) {
        args.push_back(std::move(p));
    }
}

node_ptr Node::paren(node_ptr arg) {
    assert(arg);
    if (arg->is_regex()) {
        arg->value = "(" + arg->value + ")";
        return arg;
    }
    return arg;
}

StateMachine Node::to_machine_dfs(int start) {
    switch (type) {
        case NodeType::ALTER: {
            StateMachine accum;
            for (node_ptr& np : args) {
                assert(np->type !=
                       NodeType::ALTER);  // все альтернативы на этапе
                                          // парсинга сливаются в одну
                accum = StateMachine::UnionStateMachines(accum,
                                                         np->to_machine_dfs());
            }
            return accum;
        }
        case NodeType::CONCAT: {
            StateMachine accum(0);
            accum.AddFinalState(0);
            for (int i = start; i < args.size(); ++i) {
                node_ptr& np = args[i];
                assert(np->type != NodeType::CONCAT);  // - . -
                if (np->type == NodeType::LOOKAHEAD) {
                    StateMachine argument(0);
                    if (i != args.size() - 1) {
                        argument = to_machine_dfs(i + 1);
                    } else {
                        argument.AddFinalState(0);
                    }
                    StateMachine intersect =
                        StateMachine::IntersectStateMachines(
                            np->to_machine_dfs(), argument);
                    return StateMachine::ConcatStateMachines(accum, intersect);
                }
                accum = StateMachine::ConcatStateMachines(accum,
                                                          np->to_machine_dfs());
            }
            return accum;
        }
        case NodeType::ITER: {
            assert(false);  // lookahead под итерацией
            return StateMachine();
        }
        case NodeType::REGEX:
        case NodeType::LOOKAHEAD: {
            assert(syntax_tree);
            return syntax_tree->to_machine();
        }
        default:
            return StateMachine();
    }
}

void Node::to_graph(std::ostream& out) {
    Node::graph_vertex_count = 0;
    out << "digraph { graph [ dpi = 300 ]; " << std::endl;
    to_graph_dfs(out, "\"\"");
    out << "}" << std::endl;
}

void Node::to_graph_dfs(std::ostream& out, const string& parent_name) {
    switch (type) {
        case NodeType::ALTER: {
            string own_name =
                "\"" + std::to_string(Node::graph_vertex_count++) + ": |\"";
            out << own_name << "[shape=square]" << std::endl;
            out << parent_name << "->" << own_name << std::endl;
            for (node_ptr& np : args) {
                np->to_graph_dfs(out, own_name);
            }
            break;
        }
        case NodeType::CONCAT: {
            string own_name =
                "\"" + std::to_string(Node::graph_vertex_count++) + ": .\"";
            out << own_name << "[shape=square]" << std::endl;
            out << parent_name << "->" << own_name << std::endl;
            for (node_ptr& np : args) {
                np->to_graph_dfs(out, own_name);
            }
            break;
        }
        case NodeType::ITER: {
            assert(false);  // lookahead под итерацией
        }
        case NodeType::REGEX: {
            string own_name = "\"" +
                              std::to_string(Node::graph_vertex_count++) +
                              ": " + value + "\"";
            out << parent_name << "->" << own_name << std::endl;
            break;
        }
        case NodeType::LOOKAHEAD: {
            string own_name = "\"" +
                              std::to_string(Node::graph_vertex_count++) +
                              ": " + value + "\"";
            out << parent_name << "->" << own_name << std::endl;
            out << own_name << "[peripheries=2]" << std::endl;
            break;
        }
    };
}

int Node::graph_vertex_count = 0;
