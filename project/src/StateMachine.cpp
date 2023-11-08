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

void StateMachine::To_Graph(StateMachine& M, std::ostream& out) {
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

bool StateMachine::IsWordBelong(const std::string& word){
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
        if (IsFinal(state)) {
            return true;
        }
    }
    return false;
}

bool StateMachine::dfs (int v, std::vector<int>& colors) {
    colors[v] = 1; // grey
    for (int i=0;i<transitions.size();i++){
        if (!transitions[v][i].empty()){
            if (colors[i]==0){ // white
                if (dfs(i,colors)){
                    return true;
                }
            }
            if (colors[i]==1){ // grey
                return true;
            }
        }
    }
    colors[v]=2; // black
    return false;
}

bool StateMachine::IsAnyCycle(){
    // 0-white, 1-grey, 2-black
    std::vector<int> colors(stateCount+1);
    return dfs(0,colors);
}
