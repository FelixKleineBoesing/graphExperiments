#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <queue>
#include <stdexcept>
#include <iostream>
#include <static/astar.h>
#include <static/multidigraph.h>

// Exception Classes Implementation
NodeNotFound::NodeNotFound(const std::string& message) : std::runtime_error(message) {}

NoPath::NoPath(const std::string& message) : std::runtime_error(message) {}

// DummyHeuristic Implementation
double DummyHeuristic::call(const std::string& u, const NodeData& u_data,
                            const std::string& v, const NodeData& v_data) {
    return 0.0;
}

// DummyWeightFunction Implementation
double DummyWeightFunction::call(MultiDiGraph& g, const std::string& u, const std::string& v,
                                 const std::string& key, const std::string& key_previous, const std::string& v_previous) {
    return 1.0;
}

// GetWeightFunction Implementation
GetWeightFunction::GetWeightFunction(const std::string& weight) : weight(weight) {}

double GetWeightFunction::call(MultiDiGraph& g, const std::string& u, const std::string& v,
                               const std::string& key, const std::string& key_previous, const std::string& v_previous) {
    EdgeData edge_data = g.adj(u)[v][key];
    return edge_data[weight];
}

// QueueItem Implementation
QueueItem::QueueItem(double _priority, int _counter, std::string  _node, double _dist,
                     std::string  _parent, std::string  _key)
        : priority(_priority), counter(_counter), node(std::move(_node)), dist(_dist),
          parent(std::move(_parent)), key(std::move(_key)) {}

bool QueueItem::operator<(const QueueItem& other) const {
    return priority > other.priority;
}


std::vector<std::tuple<std::string, std::string, std::string>>
astar_path(MultiDiGraph& G, const std::string& source, const std::string& target,
           HeuristicFunction* heuristic, WeightFunction* weight) {

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
        std::cout << queue.size() << std::endl;
        const QueueItem& topItem = queue.top();
        queue.pop();
        std::string curnode = topItem.node;
        double dist = topItem.dist;
        std::string parent = topItem.parent;
        std::string key = topItem.key;
        if (curnode == target) {
            std::vector<std::tuple<std::string, std::string, std::string>> path;
            path.emplace_back(parent, curnode, key);
            std::string node = parent;
            while (!node.empty()) {
                std::cout << "Node: " << node << std::endl;
                parent = node;
                node = explored[node].first;
                key = explored[node].second;
                path.emplace_back(node, parent, key);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        auto [it, inserted] = explored.insert_or_assign(curnode, std::make_pair(parent, key));
        if (!inserted && it->second != empty_node && enqueued[curnode].first < dist) {
            continue;
        }

        explored[curnode] = {parent, key};
        std::string key_previous = key;

        for (const auto& [neighbor, w] : G.adj(curnode)) {
            for (const auto& [edge_key, data] : w) {
                double ncost = dist + weight_func->call(G, curnode, neighbor, edge_key, key_previous, parent);
                auto enqueuedIter = enqueued.find(neighbor);
                if (enqueuedIter != enqueued.end() && enqueuedIter->second.first <= ncost) {
                    continue;
                } else {
                    const NodeData& neighbor_node_data = G.get_node_data(neighbor);
                    const NodeData& target_node_data = G.get_node_data(target);
                    double h = heuristic_func->call(neighbor, neighbor_node_data, target, target_node_data);
                    enqueued[neighbor] = {ncost, h};
                    counter++;
                    queue.push(QueueItem(ncost + h, counter, neighbor, ncost, curnode, edge_key));
                }
            }
        }
    }

    throw NoPath("No path found between " + source + " and " + target);
}
