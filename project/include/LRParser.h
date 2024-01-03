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
    for (gss_node* parent : t->parents) {
        out << "\"" << t->to_graph_vertex() << "\""
            << " -> "
            << "\"" << parent->to_graph_vertex() << "\"" << std::endl;
        if (visited.find(parent) == visited.end()) {
            To_Graph_Dfs(parent, out, visited);
        }
    }
};

void To_Graph(forest& f, std::ostream& out, const std::vector<gss_node*>& stack) {
    out << "digraph {" << std::endl;
    std::unordered_set<gss_node*> visited;
    for (auto t : f) {
        out << "\"" << t->to_graph_vertex() << "\""
            << " [fillcolor=" << 
            (std::find(stack.begin(), stack.end(), t) != stack.end() ? "\"green\"" : "\"gray\"")
            << " style=\"filled\""
            << "]" << std::endl;
        To_Graph_Dfs(t, out, visited);
    }
    out << "}" << std::endl;
};

int screenshots = 0;

void make_screen(forest& f, const std::vector<gss_node*>& stack) {
    std::string name = "tmp/forest_" + std::to_string(screenshots++);
    std::ofstream out(name);
    To_Graph(f, out, stack);
    std::string command = "dot -Tpng " + name + " -o " + name + ".png";
    system(command.c_str());
}

class LRParser {
   private:
    int pos = -1;

   public:
    bool parse(SLRTable& table, std::vector<std::string>& in) {
        system("mkdir tmp");
        pos = 0;
        forest f;
        f.insert(new gss_node{{}, 0});

        while (true) {
            std::string token = in[pos];
            std::vector<gss_node*> stack;
            for (auto t : f) {
                stack.push_back(t);
            }
            std::unordered_map<int, forest> shifts;
            while (stack.size() != 0) {
                gss_node* t = stack.back();
                stack.pop_back();

                auto actions = table.GetActions(t->state, token);
                int shift_number = actions.shiftActions.size();
                int reduce_number = actions.reduceActions.size();
                if (actions.is_acc) {
                    return true;
                }
                if (shift_number == 0 && reduce_number == 0) {
                    return false;
                }
                if (reduce_number != 0) {
                    for (int i = 0; i < reduce_number - 1; ++i) {
                        ExtendedRule& rule = actions.reduceActions[i];
                        forest heads = t->look(rule.RHS.size());
                        for (auto head : heads) {
                            int target = table.GoTo(head->state, rule.LHS);
                            // поискать среди листьев target, если есть, не
                            // создавать новый
                            //  for (auto )
                            gss_node* node = head->push(target);
                            f.insert(node);
                            stack.push_back(node);
                        }
                        make_screen(f, stack);
                    }
                    ExtendedRule& rule = actions.reduceActions.back();
                    forest heads;
                    if (shift_number == 0) {
                        f.extract(t);
                        heads = t->pop(rule.RHS.size());
                    } else {
                        heads = t->look(rule.RHS.size());
                    }
                    for (auto head : heads) {
                        gss_node* node =
                            head->push(table.GoTo(head->state, rule.LHS));
                        f.insert(node);
                        stack.push_back(node);
                    }
                    make_screen(f, stack);
                }
                for (auto shift_state : actions.shiftActions) {
                    shifts[shift_state].insert(t);
                }
            }

            f = {};
            for (auto shift : shifts) {
                auto parent_vector =
                    std::vector(shift.second.begin(), shift.second.end());
                gss_node* node = gss_node::get_node(parent_vector, shift.first);
                f.insert(node);
            }
            make_screen(f, stack);
            pos++;
        }
    }
};
