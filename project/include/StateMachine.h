#ifndef TFL_LABS_STATEMACHINE_H
#define TFL_LABS_STATEMACHINE_H
#include <vector>
#include <unordered_set>

class StateMachine{
private:
    int stateNum;
    std::vector<std::vector<char>> transitions;
    std::unordered_set<int> finalStates;

public:
    StateMachine(){
        transitions=std::vector<std::vector<char>>();
        stateNum=0;
        finalStates=std::unordered_set<int>();
    }
    explicit StateMachine(int statesNum);

    StateMachine(std::vector<std::vector<char>>& transitions1,std::unordered_set<int>& finalStates1,
                 int statesCount1);

    void AddTransition(int curState, char curSignal, int nextState);
    void SetFinalStates(std::unordered_set<int> final);
    [[nodiscard]] int GetStateNum() const;
    static StateMachine ConcatStateMachines(const StateMachine& stM1, const StateMachine& stM2);
    static StateMachine IntersectStateMachines(const StateMachine& stM1, const StateMachine& stM2);
    static StateMachine UnionStateMachines(const StateMachine& stM1, const StateMachine& stM2);

    ~StateMachine()=default;
};

#endif //TFL_LABS_STATEMACHINE_H
