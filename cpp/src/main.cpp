#include <map>
#include <iostream>
#include <static/astar.h>
#include <static/multidigraph.h>
#include <chrono>


int main() {
    std::cout << "Starting" << std::endl;
    std::string file_path_edges = "../python/tests/random_graph.json";
    std::string file_path_nodes = "../python/tests/node_data.json";

    MultiDiGraph graph(file_path_edges, file_path_nodes);
    std::cout << "Graph loaded" << std::endl;
    std::cout << "Number of nodes: " << graph.number_of_nodes() << std::endl;

    // measure the execution time of this call
    auto start = std::chrono::high_resolution_clock::now();
    auto shortest_path = astar_path(graph, "0", "79999", new DummyHeuristic(), new GetWeightFunction("weight"));
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;

    std::cout << "Shortest path found" << std::endl;
    std::cout << "Path length: " << shortest_path.size() << std::endl;
    std::cout << "Path: " << std::endl;
    for (auto& [u, v, key] : shortest_path) {
        std::cout << u << " " << v << " " << key << std::endl;
    }

    return 0;
}
