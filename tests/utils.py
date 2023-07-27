import json
import os
from unittest import TestCase
import numpy as np
import networkx as nx


class GraphTester(TestCase):
    graph: nx.Graph
    edge_data: dict
    node_data: dict

    @classmethod
    def setUpClass(cls) -> None:
        def euclidean_distance(x, y):
            return ((x[0] - y[0]) ** 2 + (x[1] - y[1]) ** 2) ** 0.5

        if os.path.exists("./random_graph.json"):
            with open("./random_graph.json", "r") as f:
                cls.edge_data = json.load(f)
            with open("./node_data.json", "r") as f:
                cls.node_data = json.load(f)
        else:
            cls.graph: nx.Graph = nx.random_geometric_graph(10000, 0.02, seed=42, dim=2, pos=None)
            pos = nx.get_node_attributes(cls.graph, "pos")
            edge_data = {}
            for node in cls.graph.nodes:
                edge_data[node] = {}
                for node_to in cls.graph[node]:
                    edge_data[node][node_to] = {}
                    if node_to not in edge_data:
                        edge_data[node_to] = {}
                    edge_data[node_to][node] = {}
                    edge_data[node][node_to]["0"] = {"weight": euclidean_distance(pos[node], pos[node_to])}
                    edge_data[node_to][node]["0"] = {"weight": euclidean_distance(pos[node], pos[node_to])}

            cls.edge_data = edge_data
            cls.node_data = {n: {"pos": data} for n, data in pos.items()}
            with open("random_graph.json", "w") as f:
                json.dump(edge_data, f)
            with open("node_data.json", "w") as f:
                json.dump(cls.node_data, f)

