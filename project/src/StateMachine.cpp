#include <string>
#include <unordered_map>
#include <queue>
#include "../include/StateMachine.h"

StateMachine::StateMachine(int statesNum){
    std::vector<char> v(statesNum+1, ' ');
    transitions=std::vector<std::vector<char>> (statesNum+1, v);
    finalStates=std::unordered_set<int>();
    stateNum = statesNum;
}

StateMachine::StateMachine(std::vector<std::vector<char>>& transitions1,std::unordered_set<int>& finalStates1,
                           int statesCount1){
    transitions=transitions1;
    finalStates=finalStates1;
    stateNum=statesCount1;
};

int StateMachine::GetStateNum() const{
    return stateNum;
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
