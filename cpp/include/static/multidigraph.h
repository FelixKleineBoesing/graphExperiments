// MultiDiGraph.hpp
#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_map>
#include <string>
#include <tuple>

// Forward declaration for the structs
struct NodeData {
    std::pair<double, double> pos;

    explicit NodeData() : pos({0.0, 0.0}) {} // Default constructor

    explicit NodeData(const std::pair<double, double>& pos);

    explicit NodeData(const std::unordered_map<std::string, std::pair<double, double>>& nodeData);

    std::pair<double, double> operator[](const std::string& key) const;
};

struct EdgeData {
    double weight;

    explicit  EdgeData() : weight(0.0) {} // Default constructor

    explicit EdgeData(double weight);

    explicit EdgeData(const std::unordered_map<std::string, double>& edgeData);

    double operator[](const std::string& key) const;
};

class MultiDiGraph {
private:
    std::unordered_map<std::string, NodeData> node_data;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>>> edge_data;

    void push_edge_data(const std::tuple<std::string, std::string, std::string>& edge_id, EdgeData data);

public:
    MultiDiGraph(std::unordered_map<std::string, NodeData>& nodeData,
                 std::unordered_map<std::tuple<std::string, std::string, std::string>, EdgeData>& edgeData);

    MultiDiGraph(const std::string& pathToEdges, const std::string& pathToNodes);

    NodeData get_node_data(const std::string &node_id);

    EdgeData get_edge_data(const std::string& u, const std::string& v, const std::string& key);

    std::unordered_map<std::string, std::unordered_map<std::string, EdgeData>> adj(const std::string& u);

    bool has_node(const std::string& node_id);

    unsigned long long number_of_nodes();
};

