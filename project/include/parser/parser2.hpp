#pragma once
#include <unordered_set>

#include "parser/big_tree.hpp"

using std::unordered_set;

enum L {
    _EOF = 0,
    _PLUS = '|',
    _LPAREN = '(',
    _RPAREN = ')',
    _ITER = '*',
    _START = '^',
    _END = '$',
};

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

    node_ptr Parse();
    node_ptr ParseR0();
    node_ptr ParseR();
    node_ptr ParseT();
    node_ptr ParseF();
    node_ptr ParseA();
};
