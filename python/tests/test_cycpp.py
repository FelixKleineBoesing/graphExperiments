from pygraph.cycpp import multidigraph
from pygraph.cycpp import astar
from unittest import TestCase
from datetime import datetime
import cProfile
from pygraph.utils import convert_edge_list, profile_and_dump_stats
from utils import GraphTester


class TestMultiDiGraph(TestCase):

    def setUp(self):
        self.node_data = {
            "a": {"pos": 1.1},
            "b": {"pos": 2.2},
            "c": {"pos": 3.3},
        }
        self.edge_data = {
            ("a", "b", "1"): {"weight": 2.1},
            ("a", "b", "2"): {"weight": 3.2},
            ("a", "c", "1"): {"weight": 4.3},
            ("a", "c", "2"): {"weight": 5.1},
            ("c", "a", "2"): {"weight": 6.2},
        }

    def get_graph(self, number_edges: int):
        edge_data = {}
        for i in range(number_edges):
            edge_data[("a", "b", str(i))] = {"weight": 0.1}
        return multidigraph.MultiDiGraph(node_data=self.node_data, edge_data=edge_data)

    def test_init(self):
        graph = multidigraph.MultiDiGraph(node_data=self.node_data, edge_data=self.edge_data)

    def test_getitem(self):
        self.skipTest("Rework")
        print("Cython get item")
        edges = [100, 1000, 10000, 50000]
        number_access = 10000000
        for edge_number in edges:
            graph = self.get_graph(edge_number)
            start = datetime.now()
            graph.loop_get_item(number_access)
            end = datetime.now()
            print(f"Edges: {edge_number}, Access: {number_access}, Time: {end - start}")

    def test_setitem(self):
        self.skipTest("rework")
        print("Cython set item")
        edges = [100, 1000, 10000, 50000]
        number_access = 10000000
        for edge_number in edges:
            graph = self.get_graph(edge_number)
            start = datetime.now()
            graph.loop_set_item(number_access)
            end = datetime.now()
            print(f"Edges: {edge_number}, Access: {number_access}, Time: {end - start}")


class TestAStar(GraphTester):

    def test_astar(self):
        print("NetworkX AStar")
        edge_data_list = convert_edge_list(self.edge_data)
        node_data = {k: {"pos": 0.1} for k, v in self.node_data.items()}
        g = multidigraph.MultiDiGraph(edge_data=edge_data_list, node_data=node_data)
        start = datetime.now()
        heuristic = astar.DummyHeuristic()
        shortest_path = profile_and_dump_stats(astar.astar_path, args=(g, "0", "7996", heuristic, "weight"), kwargs={})
        end = datetime.now()
        print(f"Path: {shortest_path}")
        print(f"Time: {end - start}")
