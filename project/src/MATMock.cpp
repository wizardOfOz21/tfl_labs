#include "MATMock.h"
#include <sstream>

MATMock::MATMock(StateMachine& m): ideal(m){}

bool MATMock::IsMembership(const std::string& word){
    int stateCount=ideal.GetStateNum();
    auto finalStates=ideal.GetFinalStates();
    auto transitions = ideal.GetTransitions();

    if (stateCount == 0 && finalStates.empty()) {
        return false;
    }
    std::unordered_set<int> curState = {0};
    for (char ch : word) {
        std::unordered_set<int> nStates = {};
        for (int state : curState) {
            std::vector<std::string>& trans = transitions[state];
            for (int i = 0; i < trans.size(); ++i) {
                std::stringstream ss(trans[i]);
                std::string word1;
                while (ss >> word1) {
                    if (std::string(1,ch) == word1 || word1 == std::string(1,'.')) {
                        nStates.insert(i);
                    }
                }
            }
        }
        if (nStates.empty()) {
            return false;
        }
        curState = nStates;
    }
    for (int state : curState) {
        if (ideal.IsFinal(state)) {
            return true;
        }
    }
    return false;
}

std::string MATMock::IsEqual(StateMachine& M) {
    return "aa";
}