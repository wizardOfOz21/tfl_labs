#include <iostream>
#include "project/include/StateMachine.h"
#include "project/include/RegexGenerator.h"
#include "project/include/StringGenerator.h"

int main(){
    // (ba|b)*a*
    std::vector<std::vector<char>> vect1 {{' ','b',' ','b','a'},
                                          {' ',' ','a',' ',' '},
                                          {' ','b',' ','b','a'},
                                          {' ','b',' ','b','a'},
                                          {' ',' ',' ',' ',' '}};
    std::unordered_set<int> finalStates1 {4};
    StateMachine automata1 (vect1, finalStates1,4);

    //aa*b
    std::vector<std::vector<char>> vect2 {{' ','a',' ',' '},
                                          {' ',' ','a','b'},
                                          {' ',' ','a','b'},
                                          {' ',' ',' ',' '}};
    std::unordered_set<int> finalStates2 {3};
    StateMachine automata2 (vect2,finalStates2,3);
    auto res = StateMachine::ConcatStateMachines(automata1,automata2);
    std::cout<<res.ConvertToRegularExpr()<<std::endl;
    res = StateMachine::UnionStateMachines(automata1,automata2);
    std::cout<<res.ConvertToRegularExpr()<<std::endl;
    res = StateMachine::IntersectStateMachines(automata1,automata2);
    std::cout<<res.ConvertToRegularExpr()<<std::endl;
    RegexGenerator gen;
    std::cout<<gen.GenerateRegex()<<std::endl;
    StringGenerator sg;
    for (int i=0;i<10;i++){
        std::cout<<sg.GenerateString(automata1)<<std::endl;
    }
}
