#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <stdexcept>
#include <iostream>

class NodeNotFound : public std::runtime_error {
public:
    NodeNotFound(const std::string& message) : std::runtime_error(message) {}
};

class NoPath : public std::runtime_error {
public:
    NoPath(const std::string& message) : std::runtime_error(message) {}
};

class HeuristicFunction {
public:
    virtual double call(const std::string& u, const std::unordered_map<std::string, double>& u_data,
                        const std::string& v, const std::unordered_map<std::string, double>& v_data) = 0;
};

class DummyHeuristic : public HeuristicFunction {
public:
    double call(const std::string& u, const std::unordered_map<std::string, double>& u_data,
                const std::string& v, const std::unordered_map<std::string, double>& v_data) override {
        return 0.0;
    }
};

// ... [Other classes like WeightFunction, DummyWeightFunction, GetWeightFunction]

class queueItem {
    // Define the structure of queueItem here...
};

std::vector<std::tuple<std::string, std::string, std::string>>
astar_path(/*MultiDiGraph G and other parameters*/) {

    // Rest of the logic...

    if (/* condition */){
        throw NodeNotFound("Either source or target is not in G");
    }

    // More code...

    return /* the appropriate path */;
}
