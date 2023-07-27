# distutils: language=c++
# cython: c_string_type=unicode, c_string_encoding=utf8
from libcpp.unordered_map cimport unordered_map
from libcpp.string cimport string
cimport numpy


cdef class MultiDiGraphDict:
    cdef dict _edge_data
    cdef dict  _node_data
    cdef void _push_edge_data(self, tuple edge_id, dict data)


cdef class MultiDiGraph:
    cdef unordered_map[string, unordered_map[string, unordered_map[string, unordered_map[string, double]]]] _edge_data
    cdef unordered_map[string, unordered_map[string, double]] _node_data  # assuming int data type for node_data values
    cdef void _push_edge_data(self, tuple edge_id, dict data)
