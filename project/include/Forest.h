#include <memory>
#include <unordered_set>
#include <vector>

struct parse_vertex;

using parse_vertex_sp = std::shared_ptr<parse_vertex>;

struct parse_vertex {
    std::vector<std::vector<parse_vertex_sp>> paths;
    std::string name;

    static parse_vertex_sp get_vertex(std::vector<parse_vertex_sp>& path,
                                      std::string& name) {
        return std::make_shared<parse_vertex>(path, name);
    };
    static parse_vertex_sp get_vertex(std::string& name) {
        auto vertex = std::make_shared<parse_vertex>();
        vertex->name = name;
        return vertex;
    };
    parse_vertex(std::vector<parse_vertex_sp>& path, std::string& _name)
        : name(_name) {
        paths.push_back(path);
    }
    parse_vertex() = default;

   private:
};
