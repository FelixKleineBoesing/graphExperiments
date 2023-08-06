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

    @cython.wraparound(False)  # Deactivate negative indexing.
    @cython.boundscheck(False)  # Deactivate bounds checking
    cdef void _push_edge_data(self, tuple edge_id, dict data):
        cdef string first_key = edge_id[0].encode()
        cdef string second_key = edge_id[1].encode()
        cdef string third_key = edge_id[2].encode()
        cdef unordered_map[string, double] new_map = unordered_map[string, double]()
        for k, v in data.items():
            new_map[k.encode()] = v

        self._edge_data[first_key][second_key][third_key] = new_map

    @cython.wraparound(False)  # Deactivate negative indexing.
    @cython.boundscheck(False)  # Deactivate bounds checking
    def __getitem__(self, str item):
        cdef string key = item.encode()
        return self.adj(key)

    @cython.wraparound(False)  # Deactivate negative indexing.
    @cython.boundscheck(False)  # Deactivate bounds checking
    cdef unordered_map[string, unordered_map[string, unordered_map[string, double]]] adj(self, string node_id):
        return self._edge_data[node_id]

    @cython.boundscheck(False)  # Deactivate bounds checking
    @cython.wraparound(False)  # Deactivate negative indexing.
    def __contains__(self, item):
        cdef string key = item.encode()
        c_bool =  self.isin(key)

    @cython.boundscheck(False)  # Deactivate bounds checking
    @cython.wraparound(False)  # Deactivate negative indexing.
    cdef cbool isin(self, string node):
        if self._edge_data.find(node) != self._edge_data.end():
            return True
        else:
            return False

    @cython.boundscheck(False)  # Deactivate bounds checking
    @cython.wraparound(False)  # Deactivate negative indexing.
    cdef unordered_map[string, double] get_node_data(self, string node_id):
        return self._node_data.at(node_id)
