#include "parser/tset.hpp"

bool operator<(const cchar& a, const cchar& b) {
    if (a.s < b.s) return true;
    if (a.s == b.s) return a.num < b.num;
    return false;
}

bool operator==(const cchar& a, const cchar& b) {
    return a.s == b.s && a.num == b.num;
}

template <class T>
std::set<T>& set_union(std::set<T>& A, const std::set<T>& B) {
    for (T b : B) {
        A.insert(b);
    }
    return A;
}

void TSet::plus(const TSet& add) {
    set_union(first, add.first);
    set_union(follow, add.follow);
    set_union(last, add.last);
    e_flag = e_flag || add.e_flag;
    num += add.num;
}

void TSet::concat(const TSet& add) {
    if (e_flag) {
        for (cchar b : add.first) {
            first.insert(b);
        }
    }

    for (auto ab : add.follow) {
        follow.insert(ab);
    }

    for (cchar a : last) {
        for (cchar b : add.first) {
            follow.insert(pair(a, b));
        }
    }

    if (add.e_flag) {
        for (cchar b : add.last) {
            last.insert(b);
        }
    } else {
        last = add.last;
    }

    e_flag = e_flag && add.e_flag;
    num += add.num;
}

void TSet::iter() {
    for (cchar a : last) {
        for (cchar b : first) {
            bool c = follow.insert(pair(a, b)).second;
        }
    }
    e_flag = true;
}

StateMachine TSet::to_machine() {
    StateMachine machine(num);
    for (cchar a : first) {
        machine.AddTransition(0, a.s, a.num);
    }
    for (auto ab : follow) {
        machine.AddTransition(ab.first.num, ab.second.s, ab.second.num);
    }
    for (cchar b : last) {
        machine.AddFinalState(b.num);
    }
    if (e_flag) {
        machine.AddFinalState(0);
    }
    return std::move(machine);
};
