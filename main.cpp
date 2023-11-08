#include <iostream>
#include <fstream>
#include "EquivalenceClassesTable.h"
#include "MATMock.h"


int main(){
//    std::vector<std::vector<std::string>> vect1{{"", "a", "b", "a", "",""},
//                                         {' ', 'a', 'b', 'a', ' ',' '},
//                                         {' ', 'a', 'b', 'a', ' ', ' '},
//                                         {' ', ' ', ' ', ' ', 'a','b'},
//                                         {' ', ' ', ' ', ' ', ' ',' '},
//                                         {' ', ' ', ' ', ' ', ' ',' '}};
    std::vector<std::vector<std::string>> vect1{{"", "a", "b", "a", "",""},
                                                {"", "a", "b", "a", "",""},
                                                {"", "a", "b", "a", "",""},
                                                {"", "", "", "", "a","b"},
                                                {"", "", "", "", "",""},
                                                {"", "", "", "", "",""}};
    std::unordered_set<int> finalStates1{4, 5};
    StateMachine automata1(vect1, finalStates1, 5);
    std::ofstream out("res");
    StateMachine::To_Graph(automata1,out);
    std::string alhabet="ab";
    MATMock m(automata1,alhabet,4);
    EquivalenceClassesTable::LStar(alhabet,10,4,std::make_unique<MATMock>(m));
}
