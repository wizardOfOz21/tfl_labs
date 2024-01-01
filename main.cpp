#include "iostream"
#include "SLRTable.h"

int main(){
    Grammar gr ("/Users/d.okutin/CLionProjects/tfl_labs/grammar.txt");
    SLRTable t(gr);
    auto actions = t.GetActions(8,"b");
}
