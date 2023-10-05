#include "../include/StringGenerator.h"
#include <ctime>
#include <queue>

void StringGenerator::changeSeed() {
    seed++;
    srand((size_t)time(nullptr) + seed + rand());
}

std::string StringGenerator::GenerateString(const StateMachine& stM){
    changeSeed();
    std::string res;
    std::queue<int> Q;
    Q.push(0);
    while (!Q.empty()){
        int curState=Q.front();
        auto it=stM.finalStates.find(curState);
        int v=rand()%5;
        if (it!=stM.finalStates.end() && curState!=0 && !v){
            return res;
        }
        Q.pop();
        std::vector<int> validTrans;
        for (int i=0;i<stM.transitions[curState].size();i++){
            if (stM.transitions[curState][i]!=' '){
                validTrans.push_back(i);
            }
        }
        if (validTrans.empty()){
            break;
        }
        v = rand()%validTrans.size();
        Q.push(validTrans[v]);
        res+=std::string(1,stM.transitions[curState][validTrans[v]]);
    }
    return res;
}
