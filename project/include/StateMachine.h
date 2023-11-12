#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <stack>
#include <list>

class StateMachine {
    friend void dfs (int v, std::unordered_set<int>& globalUsed, std::vector<char>& curUsed,
                     StateMachine& automata);
private:
    int stateCount;
    std::vector<std::vector<std::string>> transitions;
    std::unordered_set<int> finalStates;

    bool dfs (int v, std::vector<int>& colors);
    bool circuit(int v,
                 const int start, std::vector<int>& stack,
                 std::vector<bool>& blocked,std::vector<std::list<int>>&b,
                 std::vector<std::vector<std::string>>& cycles);
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
    bool IsWordBelong(const std::string& word);
    bool IsFinal(int state);
    bool IsAnyCycle();
    void FixStates();

    bool FindCycles(std::vector<std::vector<std::string>>& cycles);

    static void To_Graph(StateMachine& M, std::ostream& out);

    ~StateMachine()=default;
};