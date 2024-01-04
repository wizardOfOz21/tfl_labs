#include <iostream>
#include <unordered_map>

#include "GSS.h"
#include "SLRTable.h"

class LRParser {
   private:
    SLRTable& table;
    int pos = 0;
    std::vector<std::pair<gss_node_sp, ExtendedRule>> reduce_stack;
    std::unordered_map<int, std::unordered_set<gss_node_sp>> shift_map;
    std::vector<gss_node_sp> just_created;
    std::unordered_set<gss_node_sp> accepted;
    int screenshots = 0;

    void make_screen();
    void To_Graph(std::ostream& out);
    void To_Graph_Dfs(gss_node_sp& t, std::ostream& out,
                      std::unordered_set<gss_node_sp>& visited);
    void init();
    void update(gss_node_sp& target, const std::string& token);

   public:
    LRParser(SLRTable& _table);
    bool parse(std::vector<std::string>& in);
};
