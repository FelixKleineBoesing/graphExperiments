# distutils: language=c++
# cython: c_string_type=unicode, c_string_encoding=utf8
from libcpp.unordered_map cimport unordered_map
from libcpp.string cimport string
from libcpp cimport bool


cdef class MultiDiGraph:
    cdef unordered_map[string, unordered_map[string, unordered_map[string, unordered_map[string, double]]]] _edge_data
    cdef unordered_map[string, unordered_map[string, double]] _node_data  # assuming int data type for node_data values
    cdef void _push_edge_data(self, tuple edge_id, dict data)
    cdef bool isin(self, string node)
    cdef unordered_map[string, double] get_node_data(self, string node)
    cdef unordered_map[string, unordered_map[string, unordered_map[string, double]]] adj(self, string node_id)
