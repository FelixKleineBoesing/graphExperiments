from typing import Dict, Tuple


class MultiDiGraph:

    def __init__(self, node_data: Dict[str, int], edge_data: Dict[Tuple[str, str, str], int]):
        self._node_data = node_data
        self._edge_data : Dict[str, Dict[str, Dict[str, int]]] = {}
        for edge_id, data in edge_data.items():
            self._push_edge_data(edge_id, data)

    def _push_edge_data(self, edge_id: Tuple, data: int):
        if edge_id[0] not in self._edge_data:
            self._edge_data[edge_id[0]] = {}
        if edge_id[1] not in self._edge_data[edge_id[0]]:
            self._edge_data[edge_id[0]][edge_id[1]] = {}
        self._edge_data[edge_id[0]][edge_id[1]][edge_id[2]] = data

    def __getitem__(self, item):
        return self._edge_data[item]

    def loop_get_item(self, number_loops: int):
        for i in range(number_loops):
            self._edge_data['a']['b']['99']

    def loop_set_item(self, number_loops: int):
        for i in range(number_loops):
            self._edge_data['a']['b']['99'] = 99
