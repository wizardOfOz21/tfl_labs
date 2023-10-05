#include <string>
#include <unordered_map>
#include <queue>
#include "../include/StateMachine.h"

StateMachine::StateMachine(int statesNum){
    std::vector<char> v(statesNum+1, ' ');
    transitions=std::vector<std::vector<char>> (statesNum+1, v);
    finalStates=std::unordered_set<int>();
    stateCount = statesNum;
}

StateMachine::StateMachine(std::vector<std::vector<char>>& transitions1,std::unordered_set<int>& finalStates1,
                           int statesCount1){
    transitions=transitions1;
    finalStates=finalStates1;
    stateCount=statesCount1;
};

int StateMachine::GetStateNum() const{
    return stateCount;
}

void StateMachine::AddTransition(int curState, char curSignal, int nextState){
    transitions[curState][nextState]=curSignal;
}

void StateMachine::SetFinalStates(std::unordered_set<int> final) {
    finalStates=std::move(final);
}

StateMachine StateMachine::ConcatStateMachines(const StateMachine& stM1, const StateMachine& stM2) {
    StateMachine res(stM1.GetStateNum()+stM2.GetStateNum());
    for (int i=0;i<stM1.transitions.size();i++){
        for (int j=0;j<stM1.transitions[i].size();j++){
            if (stM1.transitions[i][j]!=' '){
                res.AddTransition(i,stM1.transitions[i][j],j);
            }
        }
    }
    for (auto finalState:stM1.finalStates){
        for (int j=0;j<stM2.transitions[0].size();j++){
            if (stM2.transitions[0][j]!=' '){
                res.AddTransition(finalState,stM2.transitions[0][j],j+stM1.GetStateNum());
            }
        }
    }
    for (int i=1;i<stM2.transitions.size();i++){
        for (int j=0;j<stM2.transitions[i].size();j++){
            if (stM2.transitions[i][j]!=' '){
                res.AddTransition(i+stM1.GetStateNum(),stM2.transitions[i][j],j+stM1.GetStateNum());
            }
        }
    }
    std::unordered_set<int> newFinalStates;
    for (auto finalState:stM2.finalStates){
        newFinalStates.insert(finalState+stM1.GetStateNum());
    }
    res.SetFinalStates(newFinalStates);
    return res;
}

bool dfs (int v, std::vector<std::pair<char,char>>& used, StateMachine& automata) {
    used[v] = std::make_pair(true, false);
    int count=0;
    bool canToDel= false;
    for (int i=0;i<automata.transitions[v].size();i++){
        if (automata.transitions[v][i]!=' '){
            if (!used[i].first) {
                canToDel=dfs (i,used,automata);
                if (!canToDel)
                    count++;
            }
        }
    }
    auto it1 = automata.finalStates.find(v);
    if (it1 == automata.finalStates.end() && count==0){
        used[v].second= true;
        return true;
    }
    return false;
}

StateMachine StateMachine::IntersectStateMachines(const StateMachine& stM1, const StateMachine& stM2) {
    std::unordered_map<int,std::pair<int,int>> Q;
    std::queue<int> que;
    StateMachine res(stM1.GetStateNum()*stM2.GetStateNum());
    std::unordered_set<int> newFinalStates;
    Q[0]=std::make_pair(0,0);
    que.push(0);
    int stateCount=0;
    while (!que.empty()){
        auto curStateCount=que.front();
        auto curState=Q[que.front()];
        que.pop();
        for (int i=0;i<stM1.transitions[curState.first].size();i++){
            char curTransStM1=stM1.transitions[curState.first][i];
            if (curTransStM1!=' '){
                for (int j=0;j<stM2.transitions[curState.second].size();j++){
                    char curTransStM2=stM2.transitions[curState.second][j];
                    if (curTransStM2==curTransStM1){
                        if (Q[curStateCount].first==i && Q[curStateCount].second==j){
                            break;
                        }
                        stateCount++;
                        Q[stateCount]=std::make_pair(i,j);
                        auto it1 = stM1.finalStates.find(i);
                        auto it2 = stM2.finalStates.find(j);
                        if (it1 != stM1.finalStates.end() && it2!=stM2.finalStates.end()){
                            newFinalStates.insert(stateCount);
                        }else {
                            que.push(stateCount);
                        }
                        res.AddTransition(curStateCount,curTransStM1,stateCount);
                    }
                }
            }
        }
    }
    res.SetFinalStates(newFinalStates);
    // Удаление лишнего
    std::vector<std::pair<char,char>> used(res.stateCount+1);
    used[0]=std::make_pair(true, false);
    for (int i=0;i<res.transitions[0].size();i++){
        if (res.transitions[0][i]!=' '){
            dfs(i,used,res);
        }
    }
    for (int i=used.size()-1;i>0;i--){
        if (used[i].second || !used[i].first){
            for(auto& row:res.transitions) row.erase(next(row.begin(), i));
            res.transitions.erase(res.transitions.begin()+i);
            res.stateCount--;
            for (auto finalState: res.finalStates){
                if (finalState>i){
                    res.finalStates.erase(finalState);
                    res.finalStates.insert(finalState-1);
                }
            }
        }
    }
    return res;
}

StateMachine StateMachine::UnionStateMachines(const StateMachine& stM1, const StateMachine& stM2) {
    StateMachine res(stM1.GetStateNum()+stM2.GetStateNum());
    for (int i=0;i<stM1.transitions.size();i++){
        for (int j=0;j<stM1.transitions[i].size();j++){
            if (stM1.transitions[i][j]!=' '){
                res.AddTransition(i,stM1.transitions[i][j],j);
            }
        }
    }
    for (int i=0;i<stM2.transitions.size();i++){
        for (int j=0;j<stM2.transitions[i].size();j++){
            if (stM2.transitions[i][j]!=' '){
                if (i==0){
                    res.AddTransition(i,stM2.transitions[i][j],j+stM1.GetStateNum());
                } else {
                    res.AddTransition(i+stM1.GetStateNum(),stM2.transitions[i][j],j+stM1.GetStateNum());
                }
            }
        }
    }
    std::unordered_set<int> newFinalStates;
    for (auto finalState:stM1.finalStates){
        newFinalStates.insert(finalState);
    }
    for (auto finalState:stM2.finalStates){
        newFinalStates.insert(finalState+stM1.GetStateNum());
    }
    res.SetFinalStates(newFinalStates);
    return res;
}

std::string StateMachine::ConvertToRegularExpr() {
    if (stateCount==0){
        return "^$";
    }
    int curStatesCount=stateCount+2;
    std::vector<std::vector<std::string>> trans(stateCount+1,
                                                std::vector<std::string>(stateCount+1,""));
    for (int i=0;i<transitions.size();i++){
        for (int j=0;j<transitions.size();j++){
            if (transitions[i][j]!=' '){
                trans[i][j]=std::string(1,transitions[i][j]);
            }
        }
        auto it1 = finalStates.find(i);
        if (it1 != finalStates.end()){
            trans[i].emplace_back("@");
        } else {
            trans[i].emplace_back("");
        }
    }
    bool zeroIsFinal= false;
    if (trans[0][stateCount+1]=="@"){
        zeroIsFinal= true;
    }
    int i=1;
    std::unordered_set<int> alreadyDeleted;
    while (curStatesCount>2){
        for (int j=0;j<trans.size();j++){
            auto it1 = alreadyDeleted.find(j);
            if (!trans[j][i].empty() &&  i!=j && it1 == alreadyDeleted.end()){
                for (int k=0;k<trans[i].size();k++) {
                    it1 = alreadyDeleted.find(k);
                    if (!trans[i][k].empty() && i != k && it1 == alreadyDeleted.end()) {
                        std::string pathExpr;
                        if (!trans[j][k].empty() && trans[j][k]!="@") {
                            if (trans[j][k].length()>1 && trans[j][k].find('|') != std::string::npos){
                                pathExpr += "("+trans[j][k]+")";
                            } else {
                                pathExpr += trans[j][k];
                            }
                            pathExpr += "|";
                        }
                        if (trans[j][i].length()>1 && trans[j][i].find('|') != std::string::npos){
                            pathExpr += "("+trans[j][i]+")";
                        } else {
                            pathExpr += trans[j][i];
                        }
                        if (!trans[i][i].empty()) {
                            if (trans[i][i].length()>1){
                                pathExpr += "("+trans[i][i]+")";
                            } else {
                                pathExpr += trans[i][i];
                            }
                            pathExpr += "*";
                        }
                        if (trans[i][k]!="@"){
                            it1=finalStates.find(i);
                            if (it1!=finalStates.end() && k==stateCount+1){
                                pathExpr+="|"+pathExpr;
                            }
                            if (trans[i][k].length()>1 && trans[i][k].find('|') != std::string::npos){
                                pathExpr += "("+trans[i][k]+")";
                            } else {
                                pathExpr += trans[i][k];
                            }
                        }
                        trans[j][k]=pathExpr;
                    }
                }
            }
        }
        alreadyDeleted.insert(i);
        i++;
        curStatesCount--;
    }
    if (zeroIsFinal){
        return "("+trans[0][stateCount+1]+")?";
    }
    return "("+trans[0][stateCount+1]+")";
}

void StateMachine::To_Graph(const StateMachine& M, std::ostream& out) {
    out << "digraph {" << std::endl;
    out << 0 << " [color=\"green\"]" << std::endl;
    for (int state : M.finalStates) {
        out << state << " [peripheries = 2]" << std::endl;
    }
    for (int from = 0; from < M.stateCount+1; ++from) {
        const std::vector<char>& from_trans = M.transitions[from];
        for (int to = 0; to < M.stateCount+1; ++to) {
            if (from_trans[to] != ' ')
                out << from << " -> " << to << " [label=\"" << from_trans[to]
                    << "\"]" << std::endl;
        }
    }
    out << "}" << std::endl;
};

void StateMachine::AddFinalState(int a) { finalStates.insert(a); };
