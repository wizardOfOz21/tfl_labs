#pragma once
#include <cassert>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "parser/small_tree.hpp"

using std::string;
using std::unordered_set;
using std::vector;

struct Node;
using node_ptr = std::unique_ptr<Node>;

enum L {
    _EOF = 0,
    _PLUS = '|',
    _LPAREN = '(',
    _RPAREN = ')',
    _ITER = '*',
    _START = '^',
    _END = '$',
};

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
    m_node* syntax_tree =
        nullptr;  // дерево разбора регулярки, если это регулярка, nullptr, если не ругулярка или пустое слово

    string value = "";  // внешний вид регулярки, для отладки

    static int graph_vertex_count;

    Node(NodeType type_) : type(type_) {}
    bool is_regex() { return type == NodeType::REGEX; }
    static node_ptr empty_regex() {
        return std::make_unique<Node>(NodeType::REGEX);
    }

    static node_ptr symbol_regex(char s) {
        node_ptr res = std::make_unique<Node>(NodeType::REGEX);
        res->value = string(1, s);
        res->syntax_tree = new m_node{node_type::SYMBOL, s};
        return res;
    }

    static node_ptr _node(NodeType type_) {
        return std::make_unique<Node>(type_);
    }

    void iter_regex() {
        assert(type == NodeType::REGEX);  // lookahead под звездочкой не бывает
        value += "*";
        syntax_tree = new m_node{node_type::ITER, '*', syntax_tree};
    }

    void plus_regex(node_ptr arg) {
        assert(arg->type == NodeType::REGEX);
        value += "|" + arg->value;
        syntax_tree =
            new m_node{node_type::ALTER, '|', syntax_tree, arg->syntax_tree};
    };

    void concat_regex(node_ptr arg) {
        assert(arg->type == NodeType::REGEX);
        value += arg->value;
        syntax_tree =
            new m_node{node_type::CONCAT, '.', syntax_tree, arg->syntax_tree};
    };

    void lookahead() {
        value += ".*";
        syntax_tree =
            new m_node{node_type::CONCAT, '.', syntax_tree,
                       new m_node{node_type::ITER, '*',
                                  new m_node{node_type::SYMBOL, '.'}}};
    }

    void add_node(node_ptr arg) {
        assert(type == NodeType::ALTER || type == NodeType::CONCAT);
        args.push_back(std::move(arg));
    }

    void merge_node(node_ptr arg) {
        assert(type == NodeType::ALTER || type == NodeType::CONCAT);
        assert(arg->type == NodeType::ALTER || arg->type == NodeType::CONCAT);
        for (node_ptr& p : arg->args) {
            args.push_back(std::move(p));
        }
    }

    static node_ptr paren(node_ptr arg) {
        assert(arg);
        if (arg->is_regex()) {
            arg->value = "(" + arg->value + ")";
            return arg;
        }
        return arg;
    }

    StateMachine to_machine_dfs(int start = 0) {
        switch (type) {
            case NodeType::ALTER: {
                StateMachine accum;
                for (node_ptr& np : args) {
                    assert(np->type !=
                           NodeType::ALTER);  // все альтернативы на этапе
                                              // парсинга сливаются в одну
                    accum = StateMachine::UnionStateMachines(
                        accum, np->to_machine_dfs());
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
                        return StateMachine::ConcatStateMachines(accum,
                                                                 intersect);
                    }
                    accum = StateMachine::ConcatStateMachines(
                        accum, np->to_machine_dfs());
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

    void to_graph(std::ostream& out) {
        Node::graph_vertex_count = 0;
        out << "digraph { graph [ dpi = 300 ]; " << std::endl;
        to_graph_dfs(out, "\"\"");
        out << "}" << std::endl;
    }

    void to_graph_dfs(std::ostream& out, const string& parent_name) {
        switch (type) {
            case NodeType::ALTER: {
                string own_name = "\"" + std::to_string(Node::graph_vertex_count++) + ": |\"";
                out << own_name << "[shape=square]" << std::endl;
                out << parent_name << "->" << own_name << std::endl;
                for (node_ptr& np : args) {
                    np->to_graph_dfs(out, own_name);
                }
                break;
            }
            case NodeType::CONCAT: {
                string own_name = "\"" + std::to_string(Node::graph_vertex_count++) + ": .\"";
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
                string own_name =
                    "\"" + std::to_string(Node::graph_vertex_count++) + ": " + value + "\"";
                out << parent_name << "->" << own_name << std::endl;
                break;
            }
            case NodeType::LOOKAHEAD: {
                string own_name =
                    "\"" + std::to_string(Node::graph_vertex_count++) + ": " + value + "\"";
                out << parent_name << "->" << own_name << std::endl;
                out << own_name << "[peripheries=2]" << std::endl;
                break;
            }
        };
    }
};

int Node::graph_vertex_count = 0;

class Parser {
   private:
    const char* s;
    int size;
    int next_index;
    unordered_set<char> alphabet;

   public:
    Parser(const char* s_, int size_, const unordered_set<char>& alphabet_)
        : s(s_), size(size_), alphabet(alphabet_), next_index(0) {}

    char next(int i = 0) {
        return next_index + i < size ? s[next_index + i] : _EOF;
    }

    void skip(int i = 1) { next_index += i; }

    bool is_lookahead() {
        return next() == '(' && next(1) == '?' && next(2) == '=';
    }

    bool check(char x) { return alphabet.find(x) != alphabet.end(); }

    node_ptr Parse() {
        if (next() != _START) return nullptr;
        skip();
        node_ptr R0 = ParseR0();
        if (next() != _END) return nullptr;
        skip();
        return R0;
    }

    node_ptr ParseR0() {
        node_ptr R = ParseR();
        if (!R) return Node::empty_regex();
        return R;
    }

    node_ptr ParseR() {
        node_ptr node_accum = Node::_node(NodeType::ALTER);
        node_ptr regex_accum = nullptr;

        int pos = next_index;
        node_ptr T = ParseT();
        while (T) {
            if (T->is_regex()) {
                if (!regex_accum) {
                    regex_accum = std::move(T);
                } else {
                    regex_accum->plus_regex(std::move(T));
                }
            } else if (T->type == NodeType::ALTER) {
                node_accum->merge_node(std::move(T));
            } else {
                node_accum->add_node(std::move(T));
            }
            if (next() != _PLUS) {
                if (!regex_accum) {
                    if (node_accum->args.size() == 1) {
                        return std::move(node_accum->args[0]);
                    }
                    return node_accum;
                }
                if (node_accum->args.size() == 0) return regex_accum;
                node_accum->add_node(std::move(regex_accum));
                return node_accum;
            }
            pos = next_index;
            skip();
            T = ParseT();
        }

        next_index = pos; // возвращаемя к плюсу
        return nullptr;  // был +, а дальше пусто
    }

    node_ptr ParseT() {
        node_ptr F = ParseF();
        if (!F) return nullptr;

        node_ptr node_accum = Node::_node(NodeType::CONCAT);
        node_ptr regex_accum = nullptr;

        while (F) {
            if (F->is_regex()) {
                if (!regex_accum) {
                    regex_accum = std::move(F);
                } else {
                    regex_accum->concat_regex(std::move(F));
                }
            } else if (F->type == NodeType::CONCAT) {
                if (regex_accum) {
                    node_accum->add_node(std::move(regex_accum));
                }
                node_accum->merge_node(std::move(F));
                regex_accum = nullptr;
            } else {
                if (regex_accum) {
                    node_accum->add_node(std::move(regex_accum));
                }
                node_accum->add_node(std::move(F));
                regex_accum = nullptr;
            }
            F = ParseF();
        }
        if (!regex_accum) {
            if (node_accum->args.size() == 1 &&
                node_accum->args[0]->type != NodeType::LOOKAHEAD) {
                return std::move(node_accum->args[0]);
            }
            return node_accum;
        }
        if (node_accum->args.size() == 0) return regex_accum;
        node_accum->add_node(std::move(regex_accum));
        return node_accum;
    }

    node_ptr ParseF() {
        if (is_lookahead()) {
            skip(3);
            int pos = next_index;
            node_ptr L = ParseR0();
            // assert(L->is_regex());  // не бывает lookahead внутри lookahead
            if (!L->is_regex()) {
                next_index = pos;
                return nullptr;  // незакрытый lookahead
            }
            char c = next();
            if (c != _END && c != _RPAREN) {
                next_index = pos;
                return nullptr;  // незакрытый lookahead
            }
            if (c == _RPAREN) {
                L->lookahead();
                skip();
            } else {
                skip();
                if (next() != _RPAREN) {
                    next_index = pos;
                    return nullptr;  // незакрытый lookahead
                }
                skip();
            }
            L->type = NodeType::LOOKAHEAD;
            return L;
        }
        node_ptr A = ParseA();
        if (!A) return nullptr;  // если не A, то ничего
        if (next() == '*') {
            skip();
            A->iter_regex();
        }
        return A;
    }

    node_ptr ParseA() {
        char a = next();
        if (check(a) || a == '.') {
            skip();
            return Node::symbol_regex(a);
        }
        if (a == '(') {
            int pos = next_index;
            skip();
            node_ptr R0 =
                ParseR0();  // не проверяем, так как R0 всегда парсится
            if (next() != ')') {
                next_index = pos;
                return nullptr;
            }
            skip();
            return Node::paren(std::move(R0));
        }
        return nullptr;
    }
};
