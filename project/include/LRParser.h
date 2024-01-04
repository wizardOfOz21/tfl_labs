#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "GSS.h"
#include "SLRTable.h"

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
    std::unordered_set<gss_node*> accepted;
    int screenshots = 0;

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
        out << "node [shape=box]" << std::endl;
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
        for (auto node : accepted) {
            tops[node] = "acc";
        }
        for (auto top : tops) {
            gss_node* node = top.first;
            std::string& xlabel = top.second;
            out << "\"" << node << "\"" 
                << "[" 
                << "xlabel=\"" << xlabel << "\", " 
                << "shape=ellipse" 
                <<  "]"
                << std::endl;
            To_Graph_Dfs(node, out, visited);
        }
        out << "}" << std::endl;
    }

    void To_Graph_Dfs(gss_node* t, std::ostream& out,
                      std::unordered_set<gss_node*>& visited) {
        visited.insert(t);
        out << "\"" << t << "\" [label=\"" << t->state << "\"]" << std::endl;
        for (gss_node* child : t->childs) {
            out << "\"" << t << "\""
                << " -> "
                << "\"" << child << "\"" << std::endl;
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
        accepted = {};
        screenshots = 0;
    }

    void update(gss_node* target, const std::string& token) {
        auto actions = table.GetActions(target->state, token);
        if (actions.is_acc) {
            accepted.insert(target);
        }
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
            if (accepted.size() != 0) {
                return true;
            }
            if (reduce_stack.size() == 0 && shift_map.size() == 0) {
                return false;
            }
            //
        }
    }
};
