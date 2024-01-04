#include "LRParser.h"
#include <fstream>

std::string join(std::vector<std::string>& src, std::string delim = "") {
    std::string res;
    for (int i = 0; i < src.size() - 1; ++i) {
        res += src[i] + delim;
    }
    res += src[src.size() - 1];
    return res;
}

    void LRParser::next_step() {
        if (target_step == FULL_TRACE || (target_step >= 0 && step == target_step)) {
            std::string name = "tmp/step_" + std::to_string(step);
            std::ofstream out(name);
            To_Graph(out);
            std::string command = "dot -Tpng " + name + " -o " + name + ".png";
            system(command.c_str());
        }
        step++;
    }

    void LRParser::To_Graph(std::ostream& out) {
        out << "digraph {" << std::endl;
        out << "rankdir=RL" << std::endl;
        out << "label=\"token  " << token << " : " << pos << "\"" << std::endl;
        out << "node [shape=box]" << std::endl;
        std::unordered_map<gss_node_sp, std::string> tops;
        std::unordered_set<gss_node_sp> visited;
        for (int i = 0; i < reduce_stack.size(); ++i) {
            auto reduce = reduce_stack[i];
            gss_node_sp node = reduce.first;
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
            gss_node_sp node = top.first;
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

    void LRParser::To_Graph_Dfs(gss_node_sp& t, std::ostream& out,
                      std::unordered_set<gss_node_sp>& visited) {
        visited.insert(t);
        out << "\"" << t << "\" [label=\"" << t->state << "\"]" << std::endl;
        for (gss_node_sp child : t->childs) {
            out << "\"" << t << "\""
                << " -> "
                << "\"" << child << "\"" << std::endl;
            if (visited.find(child) == visited.end()) {
                To_Graph_Dfs(child, out, visited);
            }
        }
    }

    void LRParser::init(int _target_step) {
        target_step = _target_step;
        if (target_step == FULL_TRACE || target_step >= 0) {
            system("mkdir tmp");
        }
        pos = 0;
        reduce_stack = {};
        shift_map = {};
        just_created = {};
        accepted = {};
        step = 0;
    }

    void LRParser::update(gss_node_sp& target, const std::string& token) {
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

    LRParser::LRParser(SLRTable& _table) : table(_table) {}
    bool LRParser::parse(std::vector<std::string>& in, int target_step = NO_TRACE) {
        init(target_step);
        token = in[pos];
        gss_node_sp bottom = gss_node::get_node(0);
        update(bottom, in[pos]);
        next_step();

        while (true) {
            // Reduce stage
            just_created = {};
            while (reduce_stack.size() != 0) {
                auto reduce = reduce_stack.back();
                ExtendedRule& rule = reduce.second;
                reduce_stack.pop_back();
                auto base_nodes = gss_node::look(reduce.first, rule.RHS.size());
                std::unordered_map<int, std::unordered_set<gss_node_sp>>
                    goto_partition;
                for (gss_node_sp node : base_nodes) {
                    goto_partition[table.GoTo(node->state, rule.LHS)].insert(node);
                }

                for (auto part : goto_partition) {
                    int s = part.first;
                    std::unordered_set<gss_node_sp>& s_part = part.second;
                    bool amb_flag = false;
                    for (int i = 0; i < just_created.size(); ++i) {
                        auto node = just_created[i];
                        if (s == node->state && s_part == node->childs) {
                            amb_flag = true;
                            break;
                        }
                    }
                    if (amb_flag) {
                        continue;
                    }
                    auto node = gss_node::get_node(s_part, s);
                    just_created.push_back(node);
                    update(node, in[pos]);
                }
                next_step();
            }
            //
            // Shift stage
            std::unordered_set<gss_node_sp> next_tops;
            for (auto shift : shift_map) {
                int shift_state = shift.first;
                auto shift_nodes = shift.second;
                auto accum = gss_node::get_node(shift_nodes, shift_state);
                next_tops.insert(accum);
            }
            shift_map = {};
            pos++;
            token = in[pos];
            for (auto top : next_tops) {
                update(top, in[pos]);
            }
            next_step();
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
