# cython: c_string_type=unicode, c_string_encoding=utf8

cdef class MultiDiGraph:
    cdef dict _edge_data
    cdef dict  _node_data
    cdef void _push_edge_data(self, tuple edge_id, dict data)
    cdef bint isin(self, str node)
    cdef dict get_node_data(self, str node)
