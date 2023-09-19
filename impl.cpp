#include "include.h"

string param(char constructor, int num) {
    std::stringstream str;
    str << constructor << num;
    return std::move(str.str());
}

struct Function {
    vector<string> mc;
    vector<string> sc;

    Function(char constructor) {
        mc.push_back(param(constructor, 2));
        mc.push_back(param(constructor, 1));
        sc.push_back(param(constructor, 4));
        sc.push_back(param(constructor, 3));
    }
};

Function& super(Function& f_arg, char constructor) {
    string a_n = f_arg.mc.back();
    f_arg.mc.back() = "(* " + a_n + " " + param(constructor, 2) + ")";
    f_arg.mc.push_back(param(constructor, 1));
    f_arg.sc.back() = "(+(* " + a_n + " " + param(constructor, 4) + ")" +
                      f_arg.sc.back() + ")";
    f_arg.sc.push_back(param(constructor, 3));
    return f_arg;
}

string coef(vector<string>& x, int n) {
    if (n < x.size()) return x[n];
    return "0";
}

string greater_fool(vector<string>& x, vector<string>& y) {
    std::stringstream res;
    int power = std::max(x.size(), y.size());
    res << "(or ";
    for (int i = power - 1; i >= 0; --i) {
        res << "(and ";
        for (int j = power - 1; j > i; --j) {
            res << "(= " << coef(x, j) << " " << coef(y, j) << ")";
        }
        res << "(> " << coef(x, i) << " " << coef(y, i) << ")";
        res << ")";
    }
    res << ")";
    return res.str();
}

string equal_or_greater_fool(vector<string>& x, vector<string>& y) {
    std::stringstream res;
    int power = std::max(x.size(), y.size());
    res << "(or ";
    for (int i = power - 1; i >= 1; --i) {
        res << "(and ";
        for (int j = power - 1; j > i; --j) {
            res << "(= " << coef(x, j) << " " << coef(y, j) << ")";
        }
        res << "(> " << coef(x, i) << " " << coef(y, i) << ")";
        res << ")";
    }
    res << "(and ";
    for (int j = power - 1; j > 0; --j) {
        res << "(= " << coef(x, j) << " " << coef(y, j) << ")";
    }
    res << "(>= " << coef(x, 0) << " " << coef(y, 0) << ")";
    res << ")";
    res << ")";
    return res.str();
}

string greater(vector<string>& x, vector<string>& y, bool equal_flag = false) {
    std::stringstream res;
    int power = std::max(x.size(), y.size());
    for (int i = power - 1; i >= 1; --i) {
        string xi = coef(x, i);
        string yi = coef(y, i);
        res << "(or "
            << "(> " << xi << " " << yi << ")";
        res << "(and "
            << "(= " << xi << " " << yi << ")";
    }
    equal_flag ? res << "(>= " : res << "(> ";
    res << coef(x, 0) << " " << coef(y, 0) << ")";
    res << string((power - 1) * 2, ')');
    return res.str();
}

string equal(vector<string>& x, vector<string>& y) {
    std::stringstream res;
    int power = std::max(x.size(), y.size());
    res << "(and ";
    for (int i = power - 1; i >= 0; --i) {
        res << "(= " << coef(x, i) << " " << coef(y, i) << ")";
    }
    res << ")";
    return res.str();
}

string make_condition(std::string lhs, std::string rhs, set& alf) {
    std::stringstream res;
    Function rhs_f(rhs[0]);
    Function lhs_f(lhs[0]);
    for (int i = 1; i < rhs.length(); ++i) {
        char constr = rhs[i];
        if (!alf.count(constr)) {
            for (int i = 1; i < 5; ++i) {
                res << "(declare-const " << param(constr, i) << " Int)\n";
                res << "(assert (>= " << param(constr, i) << " 0))\n";
            }
            alf.insert(constr);
        }
        super(rhs_f, constr);
    }
    for (int i = 1; i < lhs.length(); ++i) {
        char constr = lhs[i];
        if (!alf.count(constr)) {
            for (int i = 1; i < 5; ++i) {
                res << "(declare-const " << param(constr, i) << " Int)\n";
                res << "(assert (>= " << param(constr, i) << " 0))\n";
            }
            alf.insert(constr);
        }
        super(lhs_f, constr);
    }
    res << "(assert ";
    res << "(or ";
    res << "(and ";
    res << greater(lhs_f.mc, rhs_f.mc);
    res << greater(lhs_f.sc, rhs_f.sc, true);
    res << ")";
    res << "(and ";
    res << equal(lhs_f.mc, rhs_f.mc);
    res << greater(lhs_f.sc, rhs_f.sc);
    res << ")";
    res << ")";
    res << ")";
    return res.str();
}

void get_rules(vector<rule>& res, std::ifstream& src) {
    string srs_rule;
    while (getline(src, srs_rule)) {
        std::stringstream r(srs_rule);
        string skip;
        res.push_back(rule{});
        r >> res.back().first;
        r >> skip;
        r >> res.back().second;
    }
}
