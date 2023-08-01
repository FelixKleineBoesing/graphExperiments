# distutils: language=c++
# cython: c_string_type=unicode, c_string_encoding=utf8

from heapq import heappop, heappush
from itertools import count
from cpython cimport bool
cimport cython
from pygraph.cython cimport multidigraph
from libcpp.string cimport string


cdef class NodeNotFound(Exception):
    """Exception raised if node not present in graph.
    """
    pass

cdef class NoPath(Exception):
    """Exception raised if no path can be found.
    """
    pass


@cython.boundscheck(False)  # turn off bounds-checking for entire function
@cython.wraparound(False)   # turn off negative index wrapping for entire function
cdef _weight_function(multidigraph.MultiDiGraph G, weight):
    if callable(weight):
        return weight
    elif weight is None:
        return lambda u, v, d: 1.0
    elif type(weight) == str:
        def weight_func(g, u, v, key, key_previous, v_previous):
            d = g[u][v][key]
            return d.get(weight, 1.0)
        return weight_func
    else:
        raise TypeError("weight must be None, a string or a function")


cdef base_heuristic(u, u_data, v, v_data):
    return 0


cpdef astar_path(multidigraph.MultiDiGraph G, string source, string target, heuristic=None, weight="weight"):
    if  not G.isin(source) or not G.isin(target):
        msg = f"Either source {source} or target {target} is not in G"
        raise NodeNotFound(msg)

    weight = _weight_function(G, weight)
    if heuristic is None:
        heuristic = base_heuristic

    cdef list queue = [(0, next(count()), source, 0, None, None)]
    cdef dict enqueued = {}
    cdef dict explored = {}
    cdef string curnode

    while queue:
        _, _, curnode, dist, parent, key = heappop(queue)
        if curnode == target:
            path = [(parent, curnode, key)]
            node = parent
            while node is not None:
                parent = node
                node, key = explored[node]
                if node is not None:
                    path.append((node, parent, key))
            path.reverse()
            return path

        if curnode in explored:
            if explored[curnode] == (None, None):
                continue

            qcost, h = enqueued[curnode]
            if qcost < dist:
                continue

        explored[curnode] = (parent, key)
        key_previous = key

        for neighbor, w in G[curnode].items():
            for key in w.keys():
                ncost = dist + weight(
                    g=G,
                    u=curnode,
                    v=neighbor,
                    key=key,
                    key_previous=key_previous,
                    v_previous=parent,
                )
                if neighbor in enqueued:
                    qcost, h = enqueued[neighbor]
                    if qcost <= ncost:
                        continue
                else:
                    h = heuristic(
                        neighbor,
                        G.get_node_data(neighbor),
                        target,
                        G.get_node_data(target),
                    )
                enqueued[neighbor] = ncost, h
                heappush(queue, (ncost + h, next(count()), neighbor, ncost, curnode, key))

    raise NoPath("No path found between %s and %s." % (source, target))
