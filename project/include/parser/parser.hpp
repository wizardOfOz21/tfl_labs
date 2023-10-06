#pragma once
#include <cassert>
#include <string>

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

    void Iter() {
        assert(type == NodeType::REGEX); // loockahead под звездочкой не бывает
        value += "*";
        return;
    }
};

string alf = "ab";

class Regex {
   private:
    const char* s;
    int size;
    int next;
    int index;

   public:
    Regex(const char* s_, int size_) : s(s_), size(size_), next(0) {}

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

    Node* ParseR0() {
        int pos = next;
        Node* R = ParseR();
        if (!R) {
            next = pos;
            return new Node{NodeType::REGEX, ""};
        }
        return R;
    }

    Node* ParseR() {
        int pos = next;
        Node* T = ParseT();
        if (!T) {
            next = pos;
            return nullptr;  // если не T, то ничего
        }

        Node* root = nullptr;
        string regex = "";

        while (T) {
            if (T->type == NodeType::REGEX) {
                if (regex == "") regex = T->value; 
                else regex += "|" + T->value;
                delete T;
            } else {
                if (!root)
                    root = T;
                else {
                    root = new Node{NodeType::ALTER, "|", root, T};
                }
            }
            if (nchar() != _PLUS) {
                if (regex == "") return root;
                if (!root) return new Node{NodeType::REGEX, regex};
                return new Node{NodeType::ALTER, "|", root,
                                new Node{NodeType::REGEX, regex}};
            }
            next++;
            pos = next;
            T = ParseT();
        }

        next = pos;
        return nullptr;  // был +, а дальше пусто
    }

    Node* ParseL() {
        int pos = 0;
        if (is_lookahead()) {
            next += 3;
            pos = next;
            Node* R0 = ParseR0();
            assert(R0->type ==
                   NodeType::REGEX);  // не бывает lookahead внутри lookahead
            char c = nchar();
            if (c == _RPAREN) {
                R0->value += ".*";
            } else if (c != _END || nchar(1) != _RPAREN) {
                next = pos;
                return nullptr;  // не закрытый lookahead
            }
            next += 2;
            pos = next;
            Node* T = ParseT();
            if (!T) {
                next = pos;
                T = new Node{NodeType::REGEX, ""};
            }
            return new Node{NodeType::INSERT, "^", T, R0};
        }
        return nullptr;
    }

    Node* ParseT() {
        int pos = next;
        Node* L = ParseL();
        if (L) return L;

        next = pos;
        Node* F = ParseF();
        if (!F) {
            next = pos;
            return nullptr;  // если не lookahead и F, то уже ничего
        }

        Node* root = nullptr;
        string regex = "";

        while (F) {
            if (F->type == NodeType::REGEX) {
                regex += F->value;
                delete F;
            } else {
                if (regex != "") {
                    if (!root) {
                        root = new Node{NodeType::CONCAT, ".",
                                        new Node{NodeType::REGEX, regex}, F};
                    } else {
                        root = new Node{
                            NodeType::CONCAT, ".", root,
                            new Node{NodeType::CONCAT, ".",
                                     new Node{NodeType::REGEX, regex}, F}};
                    }
                    regex = "";
                } else {
                    root = new Node{NodeType::CONCAT, ".", root, F};
                }
            }
            pos = next;
            Node* L = ParseL();
            if (L) return L;
            next = pos;
            F = ParseF();
        }

        if (regex != "") {
            if (!root) {
                return new Node{NodeType::REGEX, regex};
            } else {
                return new Node{NodeType::CONCAT, ".", root,
                                new Node{NodeType::REGEX, regex}};
            }
        }

        return root;
    }

    Node* ParseF() {
        int pos = next;
        Node* A = ParseA();
        if (!A) {
            next = pos;
            return nullptr;  // если не A, то ничего
        }
        if (nchar() == '*') {
            next++;
            A->Iter();
        }
        return A;
    }

    Node* ParseA() {
        char a = nchar();
        if (Check(alf, a)) {
            Node* res = new Node{NodeType::REGEX, string(1, a)};
            next++;
            return res;
        }
        if (a == '.') {
            Node* res = new Node{NodeType::REGEX, string(1, a)};
            next++;
            return res;
        }
        if (a == '(') {
            next++;
            Node* R = ParseR();
            if (nchar() != ')') return nullptr;
            next++;
            if (R->type == NodeType::REGEX) {
                R->value = "(" + R->value + ")";
                return R;
            }
            return R;
        }
        return nullptr;
    }
};
