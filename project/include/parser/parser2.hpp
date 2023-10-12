/*
Парсер 2 (v.6)
Допускает все строчные латинские буквы
Грамматика:
E = ^R0$
R0 = R | e
R = T + R | T
T = FT | F 
F = A* | A | (?=R0$) | (?=R0)
A = {s} | (R0)
*/
#pragma once
#include <unordered_set>

#include "parser/big_tree.hpp"

using std::unordered_set;

// Лексемы
enum L {
    _EOF = 0,
    _PLUS = '|',
    _LPAREN = '(',
    _RPAREN = ')',
    _ITER = '*',
    _START = '^',
    _END = '$',
    _QSTN = '?',
    _EQ = '=',
};

class Parser {
   private:
    const char* s;
    int size;
    int next_index;

   public:
    Parser(const char* s_, int size_)
        : s(s_), size(size_), next_index(0) {}

    char next(int i = 0) {
        return next_index + i < size ? s[next_index + i] : _EOF;
    }
    void skip(int i = 1) { next_index += i; }
    bool is_lookahead() {
        return next() == _LPAREN && next(1) == _QSTN && next(2) == _EQ;
    }
    bool check(char x) { return x >= 'a' && x <= 'z'; }

    // Должна называться ParseE согласно грамматике
    node_ptr Parse();
    node_ptr ParseR0();
    // Альтернатива: накапливает все аргументы без lookahead-ов и объединяет
    // в один узел большого дерева, все вложенные альтернативы сливаются в родительскую
    node_ptr ParseR();
    // Конкатенация: накапливает все последовательные аргументы без lookahead-ов 
    // в один узел большого дерева, все вложенные конкатенации сливаются в родительскую
    node_ptr ParseT();
    // Ничего необычного
    node_ptr ParseF();
    // Скобочки или буковка
    node_ptr ParseA();
};
