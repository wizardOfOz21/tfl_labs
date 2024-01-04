#include "iostream"
#include "SLRTable.h"
#include "LRParser.h"

int main(){
    // Grammar gr ("/Users/d.okutin/CLionProjects/tfl_labs/grammar.txt");
    Grammar gr ("../ext_nformal_grammar.txt");
    SLRTable t(gr);
    t.printTable();
    LRParser parser(t);
    // std::vector<std::string> in = {"id", "*", "id", "+", "id", "@"};
    
    // std::vector<std::string> in = {"a", "a", "b", "b", "a", "a", "b", "@"};
    // std::vector<std::string> in = {"a", "a", "b", "a", "a", "a", "a", "b", "@"};
    // std::vector<std::string> in = {"b", "a", "a", "a", "a", "b", "@"};
    // std::vector<std::string> in = {"b", "b", "b", "b", "b", "b", "b", "@"};
    // std::vector<std::string> in = {"n", "v", "n", "and", "n", "v", "det", "n", "p", "det", "n", "@"};
    // parser.parse(in);
    auto actions = t.GetActions(14, "and");
}
