# astar_path.pyx

from heapq import heappop, heappush
from itertools import count
from pygraph.cypy cimport multidigraph
import cython



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
    cdef WeightFunction weight_func
    if isinstance(weight, WeightFunction):
        weight_func = weight
    elif weight is None:
        weight_func = DummyWeightFunction()
    elif type(weight) == str:
        weight_func = GetWeightFunction(weight)
    else:
        raise TypeError("weight must be None, a str or a function")
    return weight_func


cdef class HeuristicFunction(object):

    cdef double call(self, str u, dict u_data, str v, dict v_data):
        raise NotImplementedError


cdef class DummyHeuristic(HeuristicFunction):

    @cython.boundscheck(False)
    @cython.wraparound(False)
    cdef double call(self, str u, dict u_data, str v, dict v_data):
        return 0.0



cdef class WeightFunction(object):

    cdef double call(self, multidigraph.MultiDiGraph g, str u, str v, str key, str key_previous, str v_previous):
        raise NotImplementedError


cdef class DummyWeightFunction(WeightFunction):

    @cython.boundscheck(False)
    @cython.wraparound(False)
    cdef double call(self, multidigraph.MultiDiGraph g, str u, str v, str key, str key_previous, str v_previous):
        return 1.0


cdef class GetWeightFunction(WeightFunction):
    cdef str weight

    def __init__(self, str weight):
        self.weight = weight

    @cython.boundscheck(False)
    @cython.wraparound(False)
    cdef double call(self, multidigraph.MultiDiGraph g, str u, str v, str key, str key_previous, str v_previous):
        return g.adj(u).at(v).at(key).at(self.weight)



def astar_path(multidigraph.MultiDiGraph G , str source, str target, HeuristicFunction heuristic=None, str weight="weight"):
    if source not in G or target not in G:
        msg = f"Either source {source} or target {target} is not in G"
        raise NodeNotFound(msg)

    cdef WeightFunction weight_func
    cdef HeuristicFunction heuristic_func
    weight_func = _weight_function(G, weight)
    if heuristic is None:
        heuristic_func = DummyHeuristic()
    else:
        heuristic_func = heuristic
    # The queue stores priority, node, cost to reach, parent and key.
    # Uses Python heapq to keep in priority order.
    # Add a counter to the queue to prevent the underlying heap from
    # attempting to compare the nodes themselves. The hash breaks ties in the
    # priority and is guaranteed unique for all nodes in the graph.
    c = count()
    queue = [(0, next(c), source, 0, None, None)]
    # Maps enqueued nodes to distance of discovered paths and the
    # computed heuristics to target. We avoid computing the heuristics
    # more than once and inserting the node into the queue too many times.
    enqueued = {}
    # Maps explored nodes to parent closest to the source.
    explored = {}
    while queue:
        # Pop the smallest item from queue.
        _, __, curnode, dist, parent, key = heappop(queue)
        if curnode == target:
            path = [
                (parent, curnode, key)
            ]
            node = parent
            while node is not None:
                parent = node
                node, key = explored[node]
                if node is not None:
                    path.append(
                        (node, parent, key)
                    )
            path.reverse()

            return path

        if curnode in explored:
            # Do not override the parent of starting node
            if explored[curnode] == (None, None):
                continue

            # Skip bad paths that were enqueued before finding a better one
            qcost, h = enqueued[curnode]
            if qcost < dist:
                continue

        explored[curnode] = (parent, key)
        key_previous = key
        for neighbor, w in G[curnode].items():
            # loop over all existing edges
            for key in w.keys():
                ncost = dist + weight_func.call(
                    g=G,
                    u=curnode,
                    v=neighbor,
                    key=key,
                    key_previous=key_previous,
                    v_previous=parent,
                )
                if neighbor in enqueued:
                    qcost, h = enqueued[neighbor]
                    # if qcost <= ncost, a less costly path from the
                    # neighbor to the source was already determined.
                    # Therefore, we won't attempt to push this neighbor
                    # to the queue
                    if qcost <= ncost:
                        continue
                else:
                    h = heuristic_func.call(
                        neighbor,
                        G.get_node_data(neighbor),
                        target,
                        G.get_node_data(target),
                    )
                enqueued[neighbor] = ncost, h
                heappush(queue, (ncost + h, next(c), neighbor, ncost, curnode, key))

    raise NoPath("No path found between %s and %s." % (source, target))
