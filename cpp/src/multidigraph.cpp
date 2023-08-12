#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_map>
#include <string>
#include <tuple>


struct NodeData {
    std::pair<double, double> pos;

    NodeData(const std::unordered_map<std::string, std::pair<double, double>>& nodeData) {
        if (nodeData.find("pos") != nodeData.end()) {
            pos = std::get<std::pair<double, double>>(nodeData.at("pos"));
        }
    }
    std::pair<double, double> operator[](std::string key) {
        if key == "pos" {
            return pos;
        }
    }
};

struct EdgeData {
    double weight;

    EdgeData(double weight) : weight(weight) {}

    EdgeData(const std::unordered_map<std::string, double>& edgeData) {
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

    void push_edge_data(std::tuple<std::string, std::string, std::string> edge_id, EdgeData data) {
        if (!edge_data.count(std::get<0>(edge_id))) {
            edge_data[std::get<0>(edge_id)] = {};
        }

        auto& edge_data_lvl1 = edge_data[std::get<0>(edge_id)];

        if (!edge_data_lvl1.count(std::get<1>(edge_id))) {
            edge_data_lvl1[std::get<1>(edge_id)] = {};
        }

        auto& edge_data_lvl2 = edge_data_lvl1[std::get<1>(edge_id)];

        edge_data_lvl2[std::get<2>(edge_id)] = data;
    }

public:
    MultiDiGraph(std::unordered_map<std::string, NodeData> nodeData,
                 std::unordered_map<std::tuple<std::string, std::string, std::string>, EdgeData> edgeData)
    {
        node_data = nodeData;

        for (auto const& [edge_id, data] : edgeData) {
            push_edge_data(edge_id, data);
        }
    }

    MultiDiGraph(std::string pathToEdges, std::string pathToNodes) {
        // both files are in json format
        std::ifstream nodeFile(pathToNodes);
        if (nodeFile.is_open()) {
            json node_data;
            nodeFile >> j;

            for (auto& [key, value] : node_data.items()) {
                // Assuming NodeData can be constructed from a JSON object
                NodeData nd = value;
                node_data[key] = nd;
            }
        }
        nodeFile.close();

        // Reading edges from pathToEdges
        std::ifstream edgeFile(pathToEdges);
        if (edgeFile.is_open()) {
            json edge_data;
            edgeFile >> edge_data;

            for (auto& [srcNode, targetEdges] : edge_data.items()) {
                for (auto& [tgtNode, edges] : targetEdges.items()) {
                    for (auto& [edgeId, edgeAttrs] : edges.items()) {
                        EdgeData ed(edgeAttrs);
                        edge_data[srcNode][tgtNode][edgeId] = ed;
                    }
                }
            }
        }
        edgeFile.close();
    }

    NodeData get_node_data(string node_id) {
        return node_data[node_id];
    }

    EdgeData get_edge_data(string u, string v, string key) {
        return edge_data[u][v][key];
    }

    std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>> adj(string u) {
        return edge_data[u];
    }

    bool has_node(string node_id) {
        return node_data.count(node_id);
    }

    int number_of_nodes() {
        return node_data.size();
    }

};
