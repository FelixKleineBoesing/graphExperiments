#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <queue>
#include <stdexcept>
#include <iostream>
#include <static/multidigraph.h>

class NodeNotFound : public std::runtime_error {
public:
    explicit NodeNotFound(const std::string& message) : std::runtime_error(message) {}
};

class NoPath : public std::runtime_error {
public:
    explicit NoPath(const std::string& message) : std::runtime_error(message) {}
};

class HeuristicFunction {
public:
    virtual double call(const std::string& u, const NodeData& u_data,
                        const std::string& v, const NodeData& v_data) = 0;
};

class DummyHeuristic : public HeuristicFunction {
public:
    double call(const std::string& u, const NodeData& u_data,
                const std::string& v, const NodeData& v_data) override {
        return 0.0;
    }
};

class WeightFunction {
public:
    virtual double call(MultiDiGraph& g, const std::string& u, const std::string& v,
                        const std::string& key, const std::string& key_previous, const std::string& v_previous) = 0;
};

class DummyWeightFunction : public WeightFunction {
public:
    double call(MultiDiGraph& g, const std::string& u, const std::string& v,
                const std::string& key, const std::string& key_previous, const std::string& v_previous) override {
        return 1.0;
    }
};

class GetWeightFunction : public WeightFunction {
    std::string weight;
public:
    explicit GetWeightFunction(std::string  weight) : weight(std::move(weight)) {}

    double call(MultiDiGraph& g, const std::string& u, const std::string& v,
                const std::string& key, const std::string& key_previous, const std::string& v_previous) override {
        EdgeData edge_data = g.adj(u)[v][key];
        return edge_data[weight];
    }
};


struct QueueItem {
    double priority;
    int counter;
    std::string node;
    double dist;
    std::string parent;
    std::string key;

        // Constructor
    QueueItem(double _priority, int _counter, std::string  _node, double _dist,
              std::string  _parent, std::string  _key)
        : priority(_priority), counter(_counter), node(std::move(_node)), dist(_dist),
          parent(std::move(_parent)), key(std::move(_key)) {}

    bool operator<(const QueueItem& other) const {
        return priority > other.priority;
    }
};


std::vector<std::tuple<std::string, std::string, std::string>>
astar_path(MultiDiGraph& G, const std::string& source, const std::string& target,
           HeuristicFunction* heuristic = nullptr, WeightFunction* weight = nullptr) {

    std::unordered_map<std::string, std::pair<double, double>> enqueued;
    std::unordered_map<std::string, std::pair<std::string, std::string>> explored;
    std::pair<std::string, std::string> empty_node("", "");

    if (!G.has_node(source) || !G.has_node(target)) {
        throw NodeNotFound("Either source " + source + " or target " + target + " is not in G");
    }

    WeightFunction* weight_func = weight ? weight : new DummyWeightFunction();
    HeuristicFunction* heuristic_func = heuristic ? heuristic : new DummyHeuristic();

    std::priority_queue<QueueItem> queue;
    int counter = 0;
    queue.push(QueueItem(0.0, counter, source, 0.0, std::string(""), std::string("")));

    while (!queue.empty()) {
        QueueItem topItem = queue.top();
        queue.pop();
        std::string curnode = topItem.node;
        double dist = topItem.dist;
        std::string parent = topItem.parent;
        std::string key = topItem.key;
        if (curnode == target) {
            std::vector<std::tuple<std::string, std::string, std::string>> path;
            std::string node = parent;
            while (!node.empty()) {
                path.emplace_back(node, curnode, key);
                node = explored[node].first;
                key = explored[node].second;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        if (explored.find(curnode) != explored.end() && explored[curnode] != empty_node) {
            if (enqueued[curnode].first < dist) {
                continue;
            }
        }

        explored[curnode] = {parent, key};
        std::string key_previous = key;

        auto adj = G.adj(curnode);
        for (auto& [neighbor, w] : adj) {
            for (const auto& [key, data] : w) {
                double ncost = dist + weight_func->call(G, curnode, neighbor, key, key_previous, parent);
                if (enqueued.find(neighbor) != enqueued.end() && enqueued[neighbor].first <= ncost) {
                    continue;
                } else {
                    NodeData neighbor_node_data = G.get_node_data(neighbor);
                    NodeData target_node_data = G.get_node_data(target);
                    double h = heuristic_func->call(neighbor, neighbor_node_data, target, target_node_data);
                    enqueued[neighbor] = {ncost, h};
                    counter++;
                    queue.push(QueueItem(ncost + h, counter, neighbor, ncost, curnode, key));
                }
            }
        }
    }

    throw NoPath("No path found between " + source + " and " + target);
}
