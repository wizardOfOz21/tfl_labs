#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using rule = std::pair<string, string>;
using uint = unsigned int;

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
        res << "(or " << "(> " << xi << " " << yi << ")"; 
        res << "(and " << "(= " << xi << " " << yi << ")";
    }
    equal_flag ? res << "(>= " : res << "(> ";
    res << coef(x, 0) << " " << coef(y, 0) << ")";
    res << string((power-1)*2, ')');
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

string half(std::string lhs, std::string rhs) {
    std::stringstream res;
    Function rhs_f(rhs[0]);
    Function lhs_f(lhs[0]);
    for (int i = 1; i < rhs.length(); ++i) {
        super(rhs_f, rhs[i]);
    }
    for (int i = 1; i < lhs.length(); ++i) {
        super(lhs_f, lhs[i]);
    }
    res << "(assert ";
    res << "(or ";
    res << "(and ";
    // res << greater_fool(lhs_f.mc, rhs_f.mc);
    // res << equal_or_greater_fool(lhs_f.sc, rhs_f.sc);
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

void declare(std::ostream& out, string& alf) {
    for (char k : alf) {
        for (int i = 1; i < 5; ++i) {
            out << "(declare-const " << param(k, i) << " Int)\n";
            out << "(assert (>= " << param(k, i) << " 0))\n";
        }
    }
}

int main(int argc, char* argv[]) {
    Function f('g');
    std::ostream* out;
    // out = &std::cout;
    if (argc < 2) return 0;
    string smt_spec_filename = argv[1];
    std::ofstream file(smt_spec_filename);
    out = &file;
    if (!out) return 0;

    string alf = "fg";
    vector<rule> rules{
        // rule("kkkkkkkkk", "wwwwwwwwww"),
        // rule("aaab", "bbbbbaa"),
        // rule("cdcdcdcdc", "cccaaaaaaaaa"),
        // rule("kwa", "awc"),
        rule("f", "g"),
    };

    // *out << "(set-logic QF_NIA)" << std::endl;

    std::ifstream head("head.smt2");
    *out << head.rdbuf();
    head.close();

    declare(*out, alf);

    for (rule r : rules) {
        *out << half(r.first, r.second) << std::endl;
    }

    std::ifstream tail("tail.smt2");
    *out << tail.rdbuf();
    tail.close();
    file.close();
    return 0;
}
