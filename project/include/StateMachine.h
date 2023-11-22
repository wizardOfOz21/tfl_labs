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
    void FindPathsDfs(int v, int target,
                   std::vector<bool> visited, std::vector<char>& path,
                   std::unordered_set<std::string>& dest);
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
    std::vector<std::vector<std::string>> GetTransitions() const;
    bool IsWordBelong(const std::string& word);
    bool IsFinal(int state);
    bool IsAnyCycle();
    void FixStates();

    void FindCycles(std::vector<std::vector<std::string>>& cycles);
    // Ищет все пути из состояния source в состояние target, не содержащие циклов,
    // возвращает множество слов вдоль этих путей
    std::unordered_set<std::string> FindPaths(int source, int target);
    std::unordered_set<std::string> FindPaths(int source, const std::unordered_set<int>& targets);
    // Восстанавливает слово вдоль пути в автомате
    // path - путь в формати "q1 q2 q3 q4 q5 ..."
    std::string GetPathWord(const std::string& path);

    static void To_Graph(StateMachine& M, std::ostream& out);

    ~StateMachine()=default;
};
