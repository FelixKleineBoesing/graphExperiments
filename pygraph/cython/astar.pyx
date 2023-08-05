# distutils: language=c++
# cython: c_string_type=unicode, c_string_encoding=utf8
from libcpp.unordered_map cimport unordered_map
from cpython cimport bool
cimport cython
from pygraph.cython cimport multidigraph
from pygraph.cython cimport queue_types
from libcpp.string cimport string
from libcpp.vector cimport vector

from libcpp.queue cimport priority_queue
from libcpp.pair cimport pair
from cython.operator cimport dereference as deref, preincrement as inc


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
        raise TypeError("weight must be None, a string or a function")
    return weight_func


cdef class HeuristicFunction(object):

    cdef double call(self, string u, unordered_map[string, double] u_data, string v, unordered_map[string, double] v_data):
        raise NotImplementedError


cdef class DummyHeuristic(HeuristicFunction):

    @cython.boundscheck(False)
    @cython.wraparound(False)
    cdef double call(self, string u, unordered_map[string, double] u_data, string v, unordered_map[string, double] v_data):
        return 0.0



cdef class WeightFunction(object):

    cdef double call(self, multidigraph.MultiDiGraph g, string u, string v, string key, string key_previous, string v_previous):
        raise NotImplementedError


cdef class DummyWeightFunction(WeightFunction):

    @cython.boundscheck(False)
    @cython.wraparound(False)
    cdef double call(self, multidigraph.MultiDiGraph g, string u, string v, string key, string key_previous, string v_previous):
        return 1.0

cdef class GetWeightFunction(WeightFunction):
    cdef string weight

    def __init__(self, string weight):
        self.weight = weight

    @cython.boundscheck(False)
    @cython.wraparound(False)
    cdef double call(self, multidigraph.MultiDiGraph g, string u, string v, string key, string key_previous, string v_previous):
        return g.adj(u).at(v).at(key).at(self.weight)


@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
def astar_path(multidigraph.MultiDiGraph G, string source, string target, HeuristicFunction heuristic=None, weight="weight"):
    cdef unordered_map[string, pair[double, double]] enqueued
    cdef unordered_map[string, pair[string, string]] explored
    cdef pair[string, string] explored_pair
    cdef pair[double, double] enqueued_pair
    cdef string curnode
    cdef double dist
    cdef double ncost
    cdef double qcost
    cdef double h
    cdef string parent
    cdef string neighbor
    cdef string key
    cdef string key_previous
    cdef string node
    cdef queue_types.queueItem queue_item
    cdef unordered_map[string, unordered_map[string, unordered_map[string, double]]] adj
    cdef unordered_map[string, unordered_map[string, double]] w
    cdef unordered_map[string, unordered_map[string, unordered_map[string, double]]].iterator itAdj
    cdef unordered_map[string, unordered_map[string, double]].iterator itW
    cdef unordered_map[string, double] neighbor_node_data
    cdef unordered_map[string, double] target_node_data
    cdef pair[string, string] empty_node = pair[string, string](string(), string())

    if  not G.isin(source) or not G.isin(target):
        msg = f"Either source {source} or target {target} is not in G"
        raise NodeNotFound(msg)

    cdef WeightFunction weight_func
    cdef HeuristicFunction heuristic_func
    weight_func = _weight_function(G, weight)
    if heuristic is None:
        heuristic_func = DummyHeuristic()
    else:
        heuristic_func = heuristic

    cdef priority_queue[queue_types.queueItem] queue
    cdef int counter = 0
    queue.push(queue_types.queueItem(0, counter, source, 0, string(), string()))

    while not queue.empty():
        queue_item = queue.top()
        queue.pop()
        curnode, dist, parent, key = queue_item.node, queue_item.dist, queue_item.parent, queue_item.key
        if curnode == target:
            print("found target")
            path = [(parent, curnode, key)]
            node = parent
            while node != string():
                parent = node
                node, key = explored[node]
                if node != string():
                    path.append((node, parent, key))
            path.reverse()
            return path

        if explored.find(curnode) != explored.end():
            if explored[curnode] == empty_node:
                continue
            enqueued_pair = enqueued.at(curnode)
            qcost, h = enqueued_pair.first, enqueued_pair.second
            if qcost < dist:
                continue

        explored_pair = pair[string, string](parent, key)
        explored[curnode] = explored_pair
        key_previous = key

        adj = G.adj(curnode)
        itAdj = adj.begin()
        while itAdj != adj.end():
            neighbor, w = deref(itAdj).first, deref(itAdj).second
            itAdj = inc(itAdj)
            itW = w.begin()
            while itW != w.end():
                key, data = deref(itW).first, deref(itW).second
                itW = inc(itW)
                ncost = dist + weight_func.call(
                    g=G,
                    u=curnode,
                    v=neighbor,
                    key=key,
                    key_previous=key_previous,
                    v_previous=parent,
                )
                if enqueued.find(neighbor) != enqueued.end():
                    cost_pair = enqueued.at(neighbor)
                    qcost, h = cost_pair.first, cost_pair.second
                    if qcost <= ncost:
                        continue
                else:
                    neighbor_node_data = G.get_node_data(neighbor)
                    target_node_data = G.get_node_data(target)
                    h = heuristic_func.call(
                        neighbor,
                        neighbor_node_data,
                        target,
                        target_node_data,
                    )
                enqueued[neighbor] = pair[double, double](ncost, h)
                counter += 1
                queue.push(queue_types.queueItem(ncost + h, counter, neighbor, ncost, curnode, key))

    raise NoPath("No path found between %s and %s." % (source, target))
