import typing
from typing import Dict, Tuple


class MultiDiGraph:

    def __init__(self, edge_data: Dict[Tuple[str, str, str], dict], node_data: Dict[str, dict] = None):
        if node_data is None:
            node_data = {}
        self._node_data = node_data
        self._edge_data : Dict[str, Dict[str, Dict[str, Dict[typing.Any, typing.Any]]]] = {}
        for edge_id, data in edge_data.items():
            self._push_edge_data(edge_id, data)

    def _push_edge_data(self, edge_id: Tuple, data: dict):
        if edge_id[0] not in self._node_data:
            self._node_data[edge_id[0]] = {}
        if edge_id[1] not in self._node_data:
            self._node_data[edge_id[1]] = {}

        if edge_id[0] not in self._edge_data:
            self._edge_data[edge_id[0]] = {}
        if edge_id[1] not in self._edge_data[edge_id[0]]:
            self._edge_data[edge_id[0]][edge_id[1]] = {}
        self._edge_data[edge_id[0]][edge_id[1]][edge_id[2]] = data

    def __getitem__(self, item):
        return self._edge_data[item]

    def get_node_data(self, node: str):
        return self._node_data[node]

    def loop_get_item(self, number_loops: int):
        for i in range(number_loops):
            self._edge_data['a']['b']['99']

    def loop_set_item(self, number_loops: int):
        for i in range(number_loops):
            self._edge_data['a']['b']['99'] = 99

    def __contains__(self, item):
        return item in self._edge_data
