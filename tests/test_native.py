from pygraph.nativepy.multidigraph import MultiDiGraph
from unittest import TestCase
from datetime import datetime


class TestMultiDiGraph(TestCase):

    def setUp(self):
        self.node_data = {
            "a": 1,
            "b": 2,
            "c": 3,
        }
        self.edge_data = {
            ("a", "b", "1"): 2,
            ("a", "b", "2"): 3,
            ("a", "c", "1"): 4,
            ("a", "c", "2"): 5,
            ("c", "a", "2"): 6,
        }

    def get_graph(self, number_edges: int):
        edge_data = {}
        for i in range(number_edges):
            edge_data[("a", "b", str(i))] = i
        return MultiDiGraph(node_data=self.node_data, edge_data=edge_data)

    def test_init(self):
        graph = MultiDiGraph(node_data=self.node_data, edge_data=self.edge_data)

    def test_getitem(self):
        print("Native Python get item")
        edges = [100, 1000, 10000, 50000]
        number_access = 10000000
        for edge_number in edges:
            graph = self.get_graph(edge_number)
            start = datetime.now()
            graph.loop_get_item(number_access)
            end = datetime.now()
            print(f"Edges: {edge_number}, Access: {number_access}, Time: {end - start}")

    def test_setitem(self):
        print("Native Python set item")
        edges = [100, 1000, 10000, 50000]
        number_access = 10000000
        for edge_number in edges:
            graph = self.get_graph(edge_number)
            start = datetime.now()
            graph.loop_set_item(number_access)
            end = datetime.now()
            print(f"Edges: {edge_number}, Access: {number_access}, Time: {end - start}")

