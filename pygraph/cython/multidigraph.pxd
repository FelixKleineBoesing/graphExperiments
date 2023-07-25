# multidigraphdict.pxd

cdef class MultiDiGraphDict:
    cdef dict _edge_data
    cdef dict _node_data

    cdef _push_edge_data(self, tuple edge_id, dict data)
    cpdef loop_get_item(self, int number_loops)
    cpdef loop_set_item(self, int number_loops)
    cpdef get_node_data(self, str node_id)

# multidigraphdict.pxd

cdef class MultiDiGraph:
    cdef dict _edge_data
    cdef dict _node_data

    cdef _push_edge_data(self, tuple edge_id, dict data)
    cpdef loop_get_item(self, int number_loops)
    cpdef loop_set_item(self, int number_loops)
    cpdef get_node_data(self, str node_id)
