#include "../include/StateMachine.h"


StateMachine::StateMachine(int statesNum, int signalsNum){
    std::vector<int> v(signalsNum, 0);
    this->finalStates=std::vector<std::vector<int>> (statesNum+1, v);
    this->transitions=std::vector<std::vector<int>> (statesNum+1, v);
    this->nonAlphabetSymbols=std::unordered_map<int,int>();
}

void StateMachine::AddTransition(int curState, int curSignal){
    transitions[curState][curSignal]=1;
}

void StateMachine::AddNonAlphabetSymbol(int state,int count){
    nonAlphabetSymbols[state]=count;
}
