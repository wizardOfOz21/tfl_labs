#pragma once
#include <cassert>
#include <string>
#include "parser/small_tree.hpp"

using std::string;

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
    INSERT,
    REGEX,
};

struct Node {
    NodeType type;
    string value = "";
    Node* left;
    Node* right;
    m_node* tree; // дерево разбора регулярки, если это регулярка

    Node(NodeType t, const string& v, Node* l = nullptr, Node* r = nullptr)
        : type(t), value(v), left(l), right(r) {}
    Node(string v) : type(NodeType::REGEX), value(v), left(nullptr), right(nullptr) {}

    bool is_regex() { return type == NodeType::REGEX; }

    static Node* regex(char s) {
        Node* res = new Node(string(1,s));
        res->tree = new m_node{node_type::SYMBOL, s};
        return res;
    }

    static Node* paren(Node* arg) {
        assert(arg);
        if (arg->is_regex()) {
            arg->value = "(" + arg->value + ")";
            return arg;
        }
        return arg;
    }

    static Node* iter(Node* arg) {
        assert(arg);
        assert(arg->type ==
               NodeType::REGEX);  // lookahead под звездочкой не бывает
        arg->value += "*";
        arg->tree = new m_node{node_type::ITER, '*', arg->tree};
        return arg;
    }

    static Node* lookahead(Node* arg, bool is_end) {
        assert(arg);
        if (is_end) return arg;
        arg->value += ".*";
        arg->tree = new m_node{node_type::CONCAT, '.', arg->tree, new m_node{node_type::ITER, '*', 
        new m_node{node_type::SYMBOL, '.'}}};
        return arg;
    }

    static Node* insert(Node* arg1, Node* arg2) {
        assert(arg1);
        if (!arg2) {
            arg2 = Node::regex(0);
        }
        return new Node{NodeType::INSERT, "^", arg1, arg2};
    }

    static Node* concat(Node* arg1, Node* arg2) {
        if (!arg1) return arg2;  // соптимизировать, чтобы не было кучи проверок
        if (!arg2) return arg1;
        if (arg1->type == NodeType::REGEX && arg2->type == NodeType::REGEX) { // повторная проверка
            arg1->value += arg2->value;
            arg1->tree = new m_node{node_type::CONCAT, '.', arg1->tree, arg2->tree};
            delete arg2;
            return arg1;
        }
        return new Node{NodeType::CONCAT, ".", arg1, arg2};
    }

    static Node* alter(Node* arg1, Node* arg2) {
        if (!arg1) return arg2;  // соптимизировать, чтобы не было кучи проверок
        if (!arg2) return arg1;
        if (arg1->type == NodeType::REGEX && arg2->type == NodeType::REGEX) { // повторная проверка
            arg1->value += "|" + arg2->value;
            arg1->tree = new m_node{node_type::ALTER, '|', arg1->tree, arg2->tree};
            delete arg2;
            return arg1;
        }
        return new Node{NodeType::ALTER, "|", arg1, arg2};
    }
};

string alf = "ab";

class Parser {
   private:
    const char* s;
    int size;
    int next;
    int index;

   public:
    Parser(const char* s_, int size_) : s(s_), size(size_), next(0) {}

    char nchar(int i = 0) { return next + i < size ? s[next + i] : _EOF; }

    static bool Check(const string& set, char x) {
        for (char a : set) {
            if (a == x) return true;
        }
        return false;
    }

    bool is_lookahead() {
        return nchar() == '(' && nchar(1) == '?' && nchar(2) == '=';
    }

    Node* Parse() {
        if (nchar() != _START) return nullptr;
        next++;
        Node* R0 = ParseR0();
        if (nchar() != _END) return nullptr;
        next++;
        return R0;
    }

    Node* ParseR0() {
        Node* R = ParseR();
        if (!R) return Node::regex(0);
        return R;
    }

    Node* ParseR() {
        Node* node_accum = nullptr;
        Node* regex_accum = nullptr;

        Node* T = ParseT();
        while (T) {
            if (T->is_regex()) {
                regex_accum = Node::alter(regex_accum, T);
            } else {
                node_accum = Node::alter(node_accum, T);
            }
            if (nchar() != _PLUS) {
                return Node::alter(node_accum, regex_accum);
            }
            next++;
            T = ParseT();
        }

        return nullptr;  // был +, а дальше пусто
    }

    Node* ParseL() {
        if (is_lookahead()) {
            int pos = next;
            next += 3;
            Node* L = ParseR0();  // R0 всегда парсится
            assert(L->is_regex());  // не бывает lookahead внутри lookahead
            char c = nchar();
            if (c != _END || nchar(1) != _RPAREN) {
                next = pos;
                return nullptr;  // не закрытый lookahead
            }
            L = Node::lookahead(L, c != _RPAREN);
            next += 2;
            pos = next;
            Node* T = ParseT();
            if (!T) {
                next = pos;  // убрать, если T безопасна
            }
            return Node::insert(L, T);
        }
        return nullptr;
    }

    Node* ParseT() {
        int pos = next;
        Node* L = ParseL();
        if (L) return L;

        next = pos;
        Node* F = ParseF();
        if (!F) return nullptr;  // если не lookahead и не F, то уже ничего

        Node* node_accum = nullptr;
        Node* regex_accum = nullptr;

        while (F) {
            if (F->is_regex()) {
                regex_accum = Node::concat(regex_accum, F);
            } else {
                node_accum =
                    Node::concat(node_accum, Node::concat(regex_accum, F));
                regex_accum = nullptr;
            }
            pos = next;
            Node* L = ParseL();
            if (L) return L;
            next = pos;
            F = ParseF();
        }

        return Node::concat(node_accum, regex_accum);
    }

    Node* ParseF() {
        int pos = next;
        Node* A = ParseA();
        if (!A) return nullptr;  // если не A, то ничего
        if (nchar() == '*') {
            next++;
            A = Node::iter(A);
        }
        return A;
    }

    Node* ParseA() {
        char a = nchar();
        if (Check(alf, a) || a == '.') {
            next++;
            return Node::regex(a);
        }
        if (a == '(') {
            int pos = next;
            next++;
            Node* R0 = ParseR0();  // не проверяем, так как R0 всегда парсится
            if (nchar() != ')') {
                next = pos;
                return nullptr;
            }
            next++;
            return Node::paren(R0);
        }
        return nullptr;
    }
};
