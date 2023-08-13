#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_map>
#include <string>
#include <tuple>


struct NodeData {
    std::pair<double, double> pos;

    explicit NodeData() : pos({0.0, 0.0}) {} // Default constructor

    explicit NodeData(const std::unordered_map<std::string, std::pair<double, double>>& nodeData) {
        auto it = nodeData.find("pos");
        if (it != nodeData.end()) {
            pos = it->second;
        } else {
            pos = {0.0, 0.0};  // Default position, modify as necessary.
        }
    }

    std::pair<double, double> operator[](const std::string& key) const {
        if (key == "pos") {
            return pos;
        }
        // Return a default value or handle the unrecognized key in some other manner.
        return {0.0, 0.0};  // Default position for now.
    }
};

struct EdgeData {
    double weight;

    explicit  EdgeData() : weight(0.0) {} // Default constructor

    explicit EdgeData(double weight) : weight(weight) {}

    explicit EdgeData(const std::unordered_map<std::string, double>& edgeData) {
        auto it = edgeData.find("weight");
        if (it != edgeData.end()) {
            weight = it->second;
        } else {
            weight = 0.0;  // Or some default value if "weight" key is not found.
        }
    }

    double operator[](const std::string& key) const {
        if (key == "weight") {
            return weight;
        }
        // Maybe throw an exception or return a default value
        // if the key doesn't match any known keys.
        return 0.0;  // For now, returning 0.0 as default.
    }
};


class MultiDiGraph {
private:
    std::unordered_map<std::string, NodeData> node_data;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>>> edge_data;

    void push_edge_data(const std::tuple<std::string, std::string, std::string>& edge_id, EdgeData data) {
        const auto& [u, v, key] = edge_id;
        if (!edge_data.count(u)) {
            edge_data[u] = std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>>();
        }

        if (!edge_data[u].count(v)) {
            edge_data[u][v] = std::unordered_map<std::string, EdgeData>();
        }

        edge_data[u][v][key] = data;
    }

public:
    MultiDiGraph(std::unordered_map<std::string, NodeData>& nodeData,
                 std::unordered_map<std::tuple<std::string, std::string, std::string>, EdgeData>& edgeData)
    {
        node_data = nodeData;

        for (auto& [edge_id, data]: edgeData) {
            push_edge_data(edge_id, data);
        }
    }

    MultiDiGraph(std::string pathToEdges, std::string pathToNodes) {
        // both files are in json format
        std::ifstream nodeFile(pathToNodes);
        if (nodeFile.is_open()) {
            nlohmann::json nodeDataJson;
            nodeFile >> nodeDataJson;

            for (auto& [key, value] : nodeDataJson.items()) {
                // Assuming NodeData can be constructed from a JSON object
                NodeData nd(value.at("pos"));
                node_data[key] = nd;
            }
        }
        nodeFile.close();

        // Reading edges from pathToEdges
        std::ifstream edgeFile(pathToEdges);
        if (edgeFile.is_open()) {
            nlohmann::json edgeDataJson;
            edgeFile >> edgeDataJson;

            for (auto& [srcNode, targetEdges] : edgeDataJson.items()) {
                for (auto& [tgtNode, edges] : targetEdges.items()) {
                    for (auto& [edgeId, edgeAttrs] : edges.items()) {
                        EdgeData ed(edgeAttrs.at("weight"));
                        edge_data[srcNode][tgtNode][edgeId] = ed;
                    }
                }
            }
        }
        edgeFile.close();
    }

    NodeData get_node_data(std::string node_id) {
        return node_data[node_id];
    }

    EdgeData get_edge_data(std::string u, std::string v,std:: string key) {
        return edge_data[u][v][key];
    }

    std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>> adj(std::string u) {
        return edge_data[u];
    }

    bool has_node(std::string node_id) {
        return node_data.count(node_id);
    }

    int number_of_nodes() {
        return node_data.size();
    }

};
