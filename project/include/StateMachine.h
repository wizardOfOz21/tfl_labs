#ifndef TFL_LABS_STATEMACHINE_H
#define TFL_LABS_STATEMACHINE_H
#include <vector>
#include <unordered_map>

class StateMachine{
private:
    std::vector<std::vector<std::pair<char, int>>> transitions;
    std::vector<std::pair<char, int>> finalStates;
    std::unordered_map<int,int> nonAlphabetSymbols;

public:
    StateMachine()=default;
    StateMachine(int statesNum, int signalsNum);

    void AddTransition(int curState, std::pair<char,int> curSignal);
    void SetFinalStates(std::vector<std::pair<char, int>>& final);
    void AddNonAlphabetSymbol(int state,int count);

    ~StateMachine()=default;
};

#endif //TFL_LABS_STATEMACHINE_H
