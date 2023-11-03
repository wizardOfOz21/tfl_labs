#include <iostream>
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
    std::string alhabet="ab";
    MATMock m(automata1);
    EquivalenceClassesTable::LStar(alhabet,10,std::make_unique<MATMock>(m));
}
