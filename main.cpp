#include "iostream"
#include "SLRTable.h"
#include "LRParser.h"

int main(){
    // Grammar gr ("/Users/d.okutin/CLionProjects/tfl_labs/grammar.txt");
    Grammar gr ("../arith_grammar.txt");
    SLRTable t(gr);
    t.printTable();
    LRParser parser;
    std::vector<std::string> in = {"id", "*", "id", "+", "id", "@"};
    parser.parse(t, in);
}
