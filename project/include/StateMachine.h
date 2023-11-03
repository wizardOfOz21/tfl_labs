#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

class StateMachine {
private:
    int stateCount;
    std::vector<std::vector<std::string>> transitions;
    std::unordered_set<int> finalStates;

public:
    StateMachine(){
        transitions=std::vector<std::vector<std::string>>();
        stateCount=0;
        finalStates=std::unordered_set<int>();
    }
    explicit StateMachine(int statesNum);

    StateMachine(std::vector<std::vector<std::string>>& transitions1,std::unordered_set<int>& finalStates1,
                 int statesCount1);

    void AddTransition(int curState, char curSignal, int nextState);
    void SetFinalStates(std::unordered_set<int> final);
    void AddFinalState(int);
    [[nodiscard]] int GetStateNum() const;
    std::unordered_set<int> GetFinalStates();
    std::vector<std::vector<std::string>> GetTransitions();
    std::string ConvertToRegularExpr();
    bool IsFinal(int state);

    static void To_Graph(const StateMachine& M, std::ostream& out);

    ~StateMachine()=default;
};