#include <string>
#include <unordered_map>
#include <queue>
#include "StateMachine.h"
#include <sstream>

StateMachine::StateMachine(int statesNum){
    std::vector<std::string> v(statesNum+1);
    transitions=std::vector<std::vector<std::string>> (statesNum+1, v);
    finalStates=std::unordered_set<int>();
    stateCount = statesNum;
}

StateMachine::StateMachine(std::vector<std::vector<std::string>>& transitions1,std::unordered_set<int>& finalStates1,
                           int statesCount1){
    transitions=transitions1;
    finalStates=finalStates1;
    stateCount=statesCount1;
};

bool StateMachine::IsFinal(int state) {
    return finalStates.find(state) != finalStates.end();
}

int StateMachine::GetStateNum() const{
    return stateCount;
}

std::unordered_set<int> StateMachine::GetFinalStates() {
    return finalStates;
}

std::vector<std::vector<std::string>> StateMachine::GetTransitions() {
    return transitions;
}

void StateMachine::AddTransition(int curState, char curSignal, int nextState){
    transitions[curState][nextState]=curSignal;
}

void StateMachine::SetFinalStates(std::unordered_set<int> final) {
    finalStates=std::move(final);
}

void StateMachine::To_Graph(const StateMachine& M, std::ostream& out) {
    out << "digraph {" << std::endl;
    out << 0 << " [color=\"green\"]" << std::endl;
    for (int state : M.finalStates) {
        out << state << " [peripheries = 2]" << std::endl;
    }
    if (M.stateCount!=-1){
        for (int from = 0; from < M.stateCount+1; ++from) {
            const std::vector<std::string>& from_trans = M.transitions[from];
            for (int to = 0; to < M.stateCount+1; ++to) {
                if (!from_trans[to].empty()) {
                    std::stringstream ss(from_trans[to]);
                    std::string word;
                    while (ss >> word) { // Extract word from the stream.
                        out << from << " -> " << to << " [label=\"" << word
                            << "\"]" << std::endl;
                    }
                }
            }
        }
    }
    out << "}" << std::endl;
};

void StateMachine::AddFinalState(int a) { finalStates.insert(a); };
