// MultiDiGraph.hpp
#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_map>
#include <string>
#include <tuple>

// Forward declaration for the structs
struct NodeData;
struct EdgeData;

class MultiDiGraph {
private:
    std::unordered_map<std::string, NodeData> node_data;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>>> edge_data;

    void push_edge_data(std::tuple<std::string, std::string, std::string> edge_id, EdgeData data);

public:
    // Constructor declarations
    MultiDiGraph(std::unordered_map<std::string, NodeData> nodeData,
                 std::unordered_map<std::tuple<std::string, std::string, std::string>, EdgeData> edgeData);

    MultiDiGraph(std::string pathToEdges, std::string pathToNodes);

    // Method declarations
    NodeData get_node_data(std::string node_id);
    EdgeData get_edge_data(std::string u, std::string v, std::string key);
    std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>> adj(std::string u);
    bool has_node(std::string node_id);
    int number_of_nodes();
};

// Structs definition
struct NodeData {
    std::pair<float, float> pos;
    NodeData(const std::unordered_map<std::string, std::pair<float, float>>& nodeData);
};

struct EdgeData {
    int weight;
    EdgeData(const std::unordered_map<std::string, int>& edgeData);
};
