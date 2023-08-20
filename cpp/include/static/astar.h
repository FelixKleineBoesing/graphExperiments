#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <stdexcept>
#include <static/multidigraph.h>

// Exception Classes
class NodeNotFound : public std::runtime_error {
public:
    explicit NodeNotFound(const std::string& message);
};

class NoPath : public std::runtime_error {
public:
    explicit NoPath(const std::string& message);
};

// Heuristic Classes
class HeuristicFunction {
public:
    virtual double call(const std::string& u, const NodeData& u_data,
                        const std::string& v, const NodeData& v_data) = 0;
};

class DummyHeuristic : public HeuristicFunction {
public:
    double call(const std::string& u, const NodeData& u_data,
                const std::string& v, const NodeData& v_data) override;
};

// Weight Function Classes
class WeightFunction {
public:
    virtual double call(MultiDiGraph& g, const std::string& u, const std::string& v,
                        const std::string& key, const std::string& key_previous, const std::string& v_previous) = 0;
};

class DummyWeightFunction : public WeightFunction {
public:
    double call(MultiDiGraph& g, const std::string& u, const std::string& v,
                const std::string& key, const std::string& key_previous, const std::string& v_previous) override;
};

class GetWeightFunction : public WeightFunction {
private:
    const std::string& weight;
public:
    explicit GetWeightFunction(const std::string& weight);
    double call(MultiDiGraph& g, const std::string& u, const std::string& v,
                const std::string& key, const std::string& key_previous, const std::string& v_previous) override;
};

// A* Algorithm Data Structure
struct QueueItem {
    double priority;
    int counter;
    std::string node;
    double dist;
    std::string parent;
    std::string key;

    QueueItem(double _priority, int _counter, std::string  _node, double _dist,
              std::string  _parent, std::string  _key);

    bool operator<(const QueueItem& other) const;
};

// A* Algorithm Function Declaration
std::vector<std::tuple<std::string, std::string, std::string>>
astar_path(MultiDiGraph& G, const std::string& source, const std::string& target,
           HeuristicFunction* heuristic = nullptr, WeightFunction* weight = nullptr);
