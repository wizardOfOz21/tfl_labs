#include <iostream>
#include <sstream>
#include <cassert>
#include "SLRTable.h"
#include "GSS.h"

template<class T>
std::string print(const std::vector<T>& p, int start = 0, int end = -1) {
    if (end == -1) {
        end = p.size();
    }
    assert(start < end);
    assert(end <= p.size());
    std::ostringstream res("");
    res << p[start];
    for (int i = start+1; i < end; ++i) {
        res << " " << p[i];
    }
    return res.str();
}

class LRParser {
    private:
        int pos = -1;

    public:

    bool parse(SLRTable& table, std::vector<std::string>& in) {
        pos = 0;
        // GSS stack;
        std::vector<int> stack;
        stack.push_back(0);
        while (true) {
            std::string token = in[pos];
            auto actions = table.GetActions(stack.back(), token);
            assert(actions.reduceActions.size() + actions.shiftActions.size() <= 1);
            if (actions.shiftActions.size() == 1) {
                std::cout << print(stack) << ";  ";
                int shift_state = actions.shiftActions[0];
                stack.push_back(shift_state);
                ++pos;
                std::cout << "SHIFT : " << token << ";  ";
                std::cout << print(in, pos) << std::endl;
            } else if (actions.reduceActions.size() == 1) {
                std::cout << print(stack) << ";  ";
                ExtendedRule rule = actions.reduceActions[0];
                for (int i = 0; i < rule.RHS.size(); ++i) {
                    stack.pop_back();
                }
                int top = stack.back();
                stack.push_back(table.GoTo(top, rule.LHS));
                std::cout << "REDUCE : " << rule.LHS << " -> " << print(rule.RHS) << ";  ";
                std::cout << print(in, pos) << std::endl;
            } else if (actions.is_acc) {
                std::cout << print(stack) << ";  ";
                std::cout << "ACCESS;";
                return true;
            } else {
                return false;
            }
        }
    }
};
