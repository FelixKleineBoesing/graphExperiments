#include <map>
#include <iostream>
#include <static/astar.h>
#include <static/multidigraph.h>


int main() {
    std::cout << "Starting" << std::endl;
    std::string file_path_edges = "../python/tests/random_graph.json";
    std::string file_path_nodes = "../python/tests/node_data.json";

    MultiDiGraph graph(file_path_edges, file_path_nodes);
    std::cout << "Graph loaded" << std::endl;
    std::cout << "Number of nodes: " << graph.number_of_nodes() << std::endl;

    return 0;
}
