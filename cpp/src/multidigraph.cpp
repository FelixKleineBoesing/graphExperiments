#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_map>
#include <string>
#include <tuple>
#include <static/multidigraph.h>

// Definition for NodeData member functions
NodeData::NodeData(const std::unordered_map<std::string, std::pair<double, double>>& nodeData) {
    auto it = nodeData.find("pos");
    if (it != nodeData.end()) {
        pos = it->second;
    } else {
        pos = {0.0, 0.0};  // Default position, modify as necessary.
    }
}

std::pair<double, double> NodeData::operator[](const std::string& key) const {
    if (key == "pos") {
        return pos;
    }
    // Return a default value or handle the unrecognized key in some other manner.
    return {0.0, 0.0};  // Default position for now.
}

// Definition for EdgeData member functions
EdgeData::EdgeData(double w) : weight(w) {}

EdgeData::EdgeData(const std::unordered_map<std::string, double>& edgeData) {
    auto it = edgeData.find("weight");
    if (it != edgeData.end()) {
        weight = it->second;
    } else {
        weight = 0.0;  // Or some default value if "weight" key is not found.
    }
}

double EdgeData::operator[](const std::string& key) const {
    if (key == "weight") {
        return weight;
    }
    // Maybe throw an exception or return a default value
    // if the key doesn't match any known keys.
    return 0.0;  // For now, returning 0.0 as default.
}



// push_edge_data private member function
void MultiDiGraph::push_edge_data(const std::tuple<std::string, std::string, std::string>& edge_id, EdgeData data) {
    const auto& [u, v, key] = edge_id;
    if (!edge_data.count(u)) {
        edge_data[u] = std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>>();
    }

    if (!edge_data[u].count(v)) {
        edge_data[u][v] = std::unordered_map<std::string, EdgeData>();
    }

    edge_data[u][v][key] = data;
}

// Constructor using nodeData and edgeData
MultiDiGraph::MultiDiGraph(std::unordered_map<std::string, NodeData>& nodeData,
                           std::unordered_map<std::tuple<std::string, std::string, std::string>, EdgeData>& edgeData) {
    node_data = nodeData;

    for (auto& [edge_id, data] : edgeData) {
        push_edge_data(edge_id, data);
    }
}

// Constructor using file paths
MultiDiGraph::MultiDiGraph(const std::string& pathToEdges, const std::string& pathToNodes) {
    //check if both files exist
    if (!std::filesystem::exists(pathToEdges)) {
        throw std::runtime_error("File " + pathToEdges + " does not exist");
    }
    if (!std::filesystem::exists(pathToNodes)) {
        throw std::runtime_error("File " + pathToNodes + " does not exist");
    }

    // Reading nodes from pathToNodes
    std::ifstream nodeFile(pathToNodes);
    if (nodeFile.is_open()) {
        nlohmann::json nodeDataJson;
        nodeFile >> nodeDataJson;

        for (auto& [key, value] : nodeDataJson.items()) {
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
                    double weight = edgeAttrs.at("weight");
                    EdgeData ed(weight);
                    edge_data[srcNode][tgtNode][edgeId] = ed;
                }
            }
        }
    }
    edgeFile.close();
}

// get_node_data function
NodeData MultiDiGraph::get_node_data(const std::string& node_id) {
    return node_data[node_id];
}

// get_edge_data function
EdgeData MultiDiGraph::get_edge_data(const std::string& u, const std::string& v, const std::string& key) {
    return edge_data[u][v][key];
}

// adj function
std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>> MultiDiGraph::adj(const std::string& u) {
    return edge_data[u];
}

// has_node function
bool MultiDiGraph::has_node(const std::string& node_id) {
    return node_data.count(node_id);
}

// number_of_nodes function
unsigned long long MultiDiGraph::number_of_nodes() {
    return node_data.size();
}