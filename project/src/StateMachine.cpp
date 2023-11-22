#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include "StateMachine.h"
#include <sstream>
#include <set>
#include <list>
#include <algorithm>

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

std::unordered_set<int> StateMachine::GetFinalStates() const {
    return finalStates;
}

std::vector<std::vector<std::string>> StateMachine::GetTransitions() const {
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

void unblock(int v,std::vector<bool>& blocked,std::vector<std::list<int>>&b){
    blocked[v] = false;

    while (!b[v].empty()) {
        int W = b[v].front();
        b[v].pop_front();

        if (blocked[W]) {
            unblock(W,blocked,b);
        }
    }
}

// Возвращает все слова, раскрывая альтернативы, которые встречаются в пути
std::vector<std::string> get_words_by_path(std::vector<std::string>& path) {
    std::vector<std::string> words(1, "");
    for (int i = 0; i < path.size(); ++i) {
        const std::string& t_string = path[i];
        if (t_string.length() == 1) {
            for (std::string& word : words) {
                word += t_string;
            }
        } else {
            std::stringstream trans_stream(t_string);
            std::vector<char> trans_labels;
            char label = 0;
            while (trans_stream >> label) {
                trans_labels.push_back(label);
            }
            int original_size = words.size();
            for (int j = 0; j < original_size; ++j) {
                for (int k = 1; k < trans_labels.size(); ++k) {
                    words.push_back(words[j] + trans_labels[k]);
                }
                words[j] += trans_labels[0];
            }
        }
    }
    return words;
}

void StateMachine::FindPathsDfs(int v, int target, std::vector<bool> visited,
                                std::vector<std::string>& path,
                                std::unordered_set<std::string>& dest) const {
    visited[v] = true;
    if (v == target) {
        std::vector<std::string> words = get_words_by_path(path);
        for (std::string& word : words) {
            dest.insert(word);
        }
    } else {
        const std::vector<std::string>& v_trans = transitions[v];
        for (int i = 0; i < transitions.size(); ++i) {
            std::string t_string = v_trans[i];
            if (!t_string.empty() && !visited[i]) {
                path.push_back(t_string);
                FindPathsDfs(i, target, visited, path, dest);
            }
        }
    }
    path.pop_back();
    visited[v] = false;
    return;
};

std::unordered_set<std::string> StateMachine::FindPaths(
    int source, const std::unordered_set<int>& targets) const {
    std::unordered_set<std::string> dest;
    std::vector<bool> visited(GetStateNum() + 1, false);
    for (int target : targets) {
        if (source == target) {
            visited[source] = false;
        } else {
            visited[source] = true;
        }
        std::vector<std::string> path;
        const std::vector<std::string>& v_trans = transitions[source];
        for (int i = 0; i < transitions.size(); ++i) {
            std::string t_string = v_trans[i];
            if (!t_string.empty() && !visited[i]) {
                path.push_back(t_string);
                FindPathsDfs(i, target, visited, path, dest);
            }
        }
    }
    return dest;
}

void dfs (int v, std::unordered_set<int>& globalUsed, std::vector<char>& curUsed, StateMachine& automata) {
    curUsed[v] = true;
    globalUsed.insert(v);
    for (int i=0;i<automata.transitions.size();i++){
        if (automata.transitions[i][v]!="" && !curUsed[i]){
            dfs(i,globalUsed,curUsed,automata);
        }
    }
}

void StateMachine::FixStates(){
    std::unordered_set<int> globalUsed;
    for (auto finalState: finalStates){
        auto it=globalUsed.find(finalState);
        if (it==globalUsed.end()){
            std::vector<char> curUsed(stateCount+1);
            ::dfs(finalState,globalUsed,curUsed, *this);
        }
    }
    for (int i=transitions.size()-1;i>0;i--){
        auto it = globalUsed.find(i);
        if (it==globalUsed.end()){
            for(auto& row:transitions) row.erase(next(row.begin(), i));
            transitions.erase(transitions.begin()+i);
            stateCount--;
            std::set<int> needToChange;
            for (auto finalState: finalStates){
                if (finalState>i){
                    needToChange.insert(finalState);
                }
            }
            for (auto j : needToChange){
                finalStates.erase(j);
                finalStates.insert(j-1);
            }
        }
    }
}
