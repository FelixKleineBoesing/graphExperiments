import os
import pandas as pd
from unittest import TestCase
from datetime import datetime
import networkx as nx
import json
from pygraph.cypy import multidigraph
from pygraph.cypy import astar
from pygraph.utils import convert_edge_list
from utils import GraphTester


class TestMultiDiGraph(TestCase):

    def setUp(self):
        self.node_data = {
            "a": {"pos": 1.1},
            "b": {"pos": 2.1},
            "c": {"pos": 3.2},
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
            edge_data[("a", "b", str(i))] = {"weight": i}
        return multidigraph.MultiDiGraph(edge_data=edge_data, node_data=self.node_data)

    def test_init(self):
        graph = multdigraph.MultiDiGraph(edge_data=self.edge_data, node_data=self.node_data)

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


class TestAStar(GraphTester):

    def test_astar(self):
        print("NetworkX AStar")
        edge_data_list = convert_edge_list(self.edge_data)
        g = multidigraph.MultiDiGraph(edge_data=edge_data_list, node_data=self.node_data)
        start = datetime.now()
        shortest_path = astar.astar_path(g, "0", "7996", weight="weight")
        end = datetime.now()
        print(f"Path: {shortest_path}")
        print(f"Time: {end - start}")



