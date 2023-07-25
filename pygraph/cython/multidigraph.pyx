# distutils: language=c++
# cython: c_string_type=unicode, c_string_encoding=utf8
import cython
from libcpp.unordered_map cimport unordered_map
from libcpp.string cimport string
from libcpp.vector cimport vector

cdef class MultiDiGraph:
    cdef unordered_map[string, unordered_map[string, unordered_map[string, unordered_map]]] _edge_data
    cdef unordered_map[string, unordered_map] _node_data  # assuming int data type for node_data values

    def __init__(self, node_data: dict, edge_data: dict):
        self._node_data = <unordered_map[string, int]> node_data
        self._edge_data = unordered_map[string, unordered_map[string, unordered_map[string, unordered_map]]]()
        for edge_id, data in edge_data.items():
            self._push_edge_data(edge_id, data)

    cdef _push_edge_data(self, edge_id: tuple, data: int):  # assuming int data type for data
        cdef string first_key = edge_id[0].encode()
        cdef string second_key = edge_id[1].encode()
        cdef string third_key = edge_id[2].encode()

        self._edge_data[first_key][second_key][third_key] = data

    def __getitem__(self, item):
        cdef string key = item.encode()
        return self._edge_data[key]

    @cython.boundscheck(False)  # Deactivate bounds checking
    @cython.wraparound(False)  # Deactivate negative indexing.
    def loop_get_item(self, int number_loops):
        cdef int i
        for i in range(number_loops):
            self._edge_data['a']['b']['99']

    @cython.boundscheck(False)  # Deactivate bounds checking
    @cython.wraparound(False)  # Deactivate negative indexing.
    def loop_set_item(self, int number_loops):
        cdef int i
        for i in range(number_loops):
            self._edge_data['a']['b']['99'] = 99


cdef class MultiDiGraphDict:

    cdef dict _edge_data
    cdef dict _node_data

    def __init__(self, edge_data: dict, node_data: dict = None):
        if node_data is None:
            node_data = {}
        self._node_data = node_data
        self._edge_data = {}
        for edge_id, data in edge_data.items():
            self._push_edge_data(edge_id, data)

    cdef _push_edge_data(self, edge_id: tuple, data: dict):
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
        cdef string key = item.encode()
        return self._edge_data[key]

    def __contains__(self, item):
        cdef string key = item.encode()
        return key in self._edge_data

    @cython.boundscheck(False)  # Deactivate bounds checking
    @cython.wraparound(False)  # Deactivate negative indexing.
    def loop_get_item(self, int number_loops):
        cdef int i
        for i in range(number_loops):
            self._edge_data['a']['b']['99']

    @cython.boundscheck(False)  # Deactivate bounds checking
    @cython.wraparound(False)  # Deactivate negative indexing.
    def loop_set_item(self, int number_loops):
        cdef int i
        for i in range(number_loops):
            self._edge_data['a']['b']['99'] = 99

    def get_node_data(self, node_id):
        return self._node_data[node_id]
