#include "../include/StateMachine.h"


StateMachine::StateMachine(int statesNum, int signalsNum){
    std::vector<std::pair<char, int>> v(signalsNum+1, std::make_pair(' ',0));
    this->finalStates=v;
    this->transitions=std::vector<std::vector<std::pair<char, int>>>  (statesNum+1, v);
    this->nonAlphabetSymbols=std::unordered_map<int,int>();
}

void StateMachine::AddTransition(int curState, std::pair<char,int> curSignal){
    transitions[curState][curSignal.second]=curSignal;
}

void StateMachine::AddNonAlphabetSymbol(int state,int count){
    nonAlphabetSymbols[state]=count;
}

void StateMachine::SetFinalStates(std::vector<std::pair<char, int>>& final) {
    finalStates=final;
}
