import typing
from typing import Dict, Tuple
import cython


cdef class MultiDiGraph:

    def __init__(self, dict edge_data, dict node_data = None):
        if node_data is None:
            node_data = {}
        self._node_data = node_data
        self._edge_data = {}
        for edge_id, data in edge_data.items():
            self._push_edge_data(edge_id, data)

    @cython.wraparound(False)  # Deactivate negative indexing.
    @cython.boundscheck(False)  # Deactivate bounds checking
    cdef void _push_edge_data(self, tuple edge_id, dict data):
        if edge_id[0] not in self._node_data:
            self._node_data[edge_id[0]] = {}
        if edge_id[1] not in self._node_data:
            self._node_data[edge_id[1]] = {}

        if edge_id[0] not in self._edge_data:
            self._edge_data[edge_id[0]] = {}
        if edge_id[1] not in self._edge_data[edge_id[0]]:
            self._edge_data[edge_id[0]][edge_id[1]] = {}
        self._edge_data[edge_id[0]][edge_id[1]][edge_id[2]] = data

    @cython.wraparound(False)  # Deactivate negative indexing.
    @cython.boundscheck(False)  # Deactivate bounds checking
    def __getitem__(self, str item):
        return self._edge_data[item]

    @cython.wraparound(False)  # Deactivate negative indexing.
    @cython.boundscheck(False)  # Deactivate bounds checking
    def __contains__(self, str item):
        return self.isin(item)

    @cython.wraparound(False)  # Deactivate negative indexing.
    @cython.boundscheck(False)  # Deactivate bounds checking
    cdef bint isin(self, str node):
        return node in self._edge_data

    @cython.wraparound(False)  # Deactivate negative indexing.
    @cython.boundscheck(False)  # Deactivate bounds checking
    cdef dict get_node_data(self, str node_id):
        return self._node_data[node_id]
