#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "GSS.h"
#include "SLRTable.h"

using forest = std::unordered_set<gss_node*>;

void To_Graph_Dfs(gss_node* t, std::ostream& out,
                  std::unordered_set<gss_node*>& visited) {
    visited.insert(t);
    for (gss_node* child : t->childs) {
        out << "\"" << t->to_graph_vertex() << "\""
            << " -> "
            << "\"" << child->to_graph_vertex() << "\"" << std::endl;
        if (visited.find(child) == visited.end()) {
            To_Graph_Dfs(child, out, visited);
        }
    }
};

void To_Graph(forest& f, std::ostream& out,
              const std::vector<gss_node*>& stack) {
    out << "digraph {" << std::endl;
    std::unordered_set<gss_node*> visited;
    for (auto t : f) {
        out << "\"" << t->to_graph_vertex() << "\""
            << " [fillcolor="
            << (std::find(stack.begin(), stack.end(), t) != stack.end()
                    ? "\"green\""
                    : "\"gray\"")
            << " style=\"filled\""
            << "]" << std::endl;
        To_Graph_Dfs(t, out, visited);
    }
    out << "}" << std::endl;
};

template <class T>
bool vec_compare(const std::vector<T>& first, const std::vector<T>& second) {
    if (first.size() != second.size()) {
        return false;
    }

    return std::is_permutation(first.begin(), first.end(), second.begin());
}

std::string join(std::vector<std::string>& src, std::string delim = "") {
    std::string res;
    for (int i = 0; i < src.size() - 1; ++i) {
        res += src[i] + delim;
    }
    res += src[src.size() - 1];
    return res;
}

class LRParser {
   private:
    SLRTable& table;
    int pos = 0;
    std::vector<std::pair<gss_node*, ExtendedRule>> reduce_stack;
    std::unordered_map<int, std::unordered_set<gss_node*>> shift_map;
    std::vector<gss_node*> just_created;
    bool is_accepted = false;
    int screenshots = 0;
    // std::unordered_set<gss_node*> tops;

    void make_screen() {
        std::string name = "tmp/forest_" + std::to_string(screenshots++);
        std::ofstream out(name);
        To_Graph(out);
        std::string command = "dot -Tpng " + name + " -o " + name + ".png";
        system(command.c_str());
    }

    void To_Graph(std::ostream& out) {
        out << "digraph {" << std::endl;
        out << "rankdir=RL" << std::endl;
        std::unordered_map<gss_node*, std::string> tops;
        std::unordered_set<gss_node*> visited;
        for (auto reduce : reduce_stack) {
            gss_node* node = reduce.first;
            ExtendedRule& rule = reduce.second;
            tops[node] +=
                "reduce(" + rule.LHS + " -> " + join(rule.RHS, " ") + ")" + "\\n";
        }
        for (auto shift : shift_map) {
            int shift_state = shift.first;
            auto shift_nodes = shift.second;
            for (auto node : shift_nodes) {
                tops[node] +=
                    "shift(" + std::to_string(shift_state) + ")" + "\\n";
            }
        }
        for (auto top : tops) {
            gss_node* node = top.first;
            std::string& xlabel = top.second;
            out << "\"" << node->to_graph_vertex() << "\" "
                << "[xlabel=\"" + xlabel + "\"]" << std::endl;
            To_Graph_Dfs(node, out, visited);
        }
        out << "}" << std::endl;
    }

    void To_Graph_Dfs(gss_node* t, std::ostream& out,
                      std::unordered_set<gss_node*>& visited) {
        visited.insert(t);
        for (gss_node* child : t->childs) {
            out << "\"" << t->to_graph_vertex() << "\""
                << " -> "
                << "\"" << child->to_graph_vertex() << "\"" << std::endl;
            if (visited.find(child) == visited.end()) {
                To_Graph_Dfs(child, out, visited);
            }
        }
    }

    void init() {
        pos = 0;
        reduce_stack = {};
        shift_map = {};
        just_created = {};
        // tops = {};
        is_accepted = false;
        screenshots = 0;
    }

    void update(gss_node* target, const std::string& token) {
        auto actions = table.GetActions(target->state, token);
        is_accepted = actions.is_acc;
        for (ExtendedRule& rule : actions.reduceActions) {
            reduce_stack.emplace_back(target, rule);
        }
        for (int shift_state : actions.shiftActions) {
            shift_map[shift_state].insert(target);
        }
        int shift_number = actions.shiftActions.size();
    }

   public:
    LRParser(SLRTable& _table) : table(_table) {}
    bool parse(std::vector<std::string>& in) {
        system("mkdir tmp");

        init();
        gss_node* bottom = gss_node::get_node(0);
        update(bottom, in[pos]);

        while (true) {
            // Reduce stage
            just_created = {};
            while (reduce_stack.size() != 0) {
                auto reduce = reduce_stack.back();
                ExtendedRule& rule = reduce.second;
                reduce_stack.pop_back();
                auto base_nodes = reduce.first->look(rule.RHS.size());
                std::unordered_map<int, std::unordered_set<gss_node*>>
                    goto_partition;
                for (gss_node* node : base_nodes) {
                    goto_partition[table.GoTo(node->state, rule.LHS)].insert(node);
                }

                for (auto part : goto_partition) {
                    int s = part.first;
                    std::unordered_set<gss_node*>& s_part = part.second;
                    bool amb_flag = false;
                    for (auto node : just_created) {
                        if (s == node->state && s_part == node->childs) {
                            amb_flag = true;
                            break;
                        }
                    }
                    if (amb_flag) {
                        continue;
                    }
                    gss_node* node = gss_node::get_node(s_part, s);
                    just_created.push_back(node);
                    update(node, in[pos]);
                }
                make_screen();
            }
            //
            // Shift stage
            std::unordered_set<gss_node*> next_tops;
            for (auto shift : shift_map) {
                int shift_state = shift.first;
                auto shift_nodes = shift.second;
                gss_node* accum = gss_node::get_node(shift_nodes, shift_state);
                next_tops.insert(accum);
            }
            shift_map = {};
            pos++;
            for (auto top : next_tops) {
                update(top, in[pos]);
            }
            make_screen();
            //
            // Check stage
            if (is_accepted) {
                return true;
            }
            if (reduce_stack.size() == 0 && shift_map.size() == 0) {
                return false;
            }
            //
        }
    }
};
