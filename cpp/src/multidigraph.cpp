#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_map>
#include <string>
#include <tuple>
#include <static/multidigraph.h>
#include <simdjson.h>
#include <iostream>

using namespace simdjson;

// Definition for NodeData member functions
NodeData::NodeData(const std::unordered_map<std::string, std::pair<double, double>>& nodeData) {
    auto it = nodeData.find("pos");
    if (it != nodeData.end()) {
        pos = it->second;
    } else {
        pos = {0.0, 0.0};  // Default position, modify as necessary.
    }
}

NodeData::NodeData(const std::pair<double, double>& p) : pos(p) {}

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
    simdjson::dom::parser parser;
    simdjson::dom::object doc = parser.load(pathToNodes);

    for (auto& [key, value] : doc) {
        std::string key_str = std::string(key.data(), key.size());

        // Access the "pos" array and extract its values
        auto single_node_data = value.get_object();
        auto single_node_pos = single_node_data["pos"].get_array();
        double x, y;

        x = single_node_pos.at(0).get_double();
        y = single_node_pos.at(1).get_double();
        std::pair<double, double> pos(x, y);
        NodeData nd(pos);
        node_data[key_str] = nd;
    }


    // Reading edges from pathToEdges
    simdjson::dom::object docEdges = parser.load(pathToEdges);
    int counter = 0;
    for (auto& [key, value]: docEdges) {
        for (auto& [key2, value2]: value.get_object()) {
            for (auto& [key3, value3]: value2.get_object()) {
                counter++;
                if (counter % 10000 == 0) {
                    std::cout << counter << std::endl;
                }
                std::string key_str = std::string(key.data(), key.size());
                std::string key2_str = std::string(key2.data(), key2.size());
                std::string key3_str = std::string(key3.data(), key3.size());

                auto single_edge_data = value3.get_object();
                double weight = single_edge_data["weight"].get_double();
                EdgeData ed(weight);
                edge_data[key_str][key2_str][key3_str] = ed;
            }
        }
    }
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