# distutils: language=c++
# cython: c_string_type=unicode, c_string_encoding=utf8
import cython
from libcpp.unordered_map cimport unordered_map
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool as cbool


cdef class MultiDiGraph:

    def __init__(self, node_data: dict, edge_data: dict):
        self._node_data = <unordered_map[string, unordered_map[string, double]]> node_data
        self._edge_data = unordered_map[string, unordered_map[string, unordered_map[string, unordered_map[string, double]]]]()
        for edge_id, data in edge_data.items():
            self._push_edge_data(edge_id, data)

    cdef void _push_edge_data(self, tuple edge_id, dict data):
        cdef string first_key = edge_id[0].encode()
        cdef string second_key = edge_id[1].encode()
        cdef string third_key = edge_id[2].encode()
        cdef unordered_map[string, double] new_map = unordered_map[string, double]()
        for k, v in data.items():
            new_map[k.encode()] = v

        self._edge_data[first_key][second_key][third_key] = new_map

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
        cdef unordered_map[string, double] new_map = unordered_map[string, double]()
        new_map["99"] = 0.01
        for i in range(number_loops):
            self._edge_data['a']['b']['99'] = new_map

    def __contains__(self, item):
        cdef string key = item.encode()
        c_bool =  self.isin(key)

    cdef cbool isin(self, string node):
        if self._edge_data.find(node) != self._edge_data.end():
            return True
        else:
            return False

    cdef unordered_map[string, double] get_node_data(self, string node_id):
        return self._node_data.at(node_id)


cdef class MultiDiGraphDict:

    def __init__(self, edge_data: dict, node_data: dict = None):
        if node_data is None:
            node_data = {}
        self._node_data = node_data
        self._edge_data = {}
        for edge_id, data in edge_data.items():
            self._push_edge_data(edge_id, data)

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

    def __getitem__(self, item):
        cdef string key = item.encode()
        return self._edge_data[key]

    def __contains__(self, item):
        cdef string key = item.encode()
        return self.isin(key)

    cdef cbool isin(self, string node):
        py_string = node.decode('utf-8')
        return py_string in self._edge_data

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
            self._edge_data['a']['b']['99'] = 0.01

    cdef dict get_node_data(self, string node_id):
        # convert cpp string to python string
        py_string = node_id.decode('utf-8')
        return self._node_data[py_string]
