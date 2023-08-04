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


@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
cdef base_heuristic(u, u_data, v, v_data):
    return 0


@cython.boundscheck(False) # compiler directive
@cython.wraparound(False) # compiler directive
def astar_path(multidigraph.MultiDiGraph G, str src, str trgt, heuristic=None, weight="weight"):
    cdef string source = src
    cdef string target = trgt
    if  not G.isin(source) or not G.isin(target):
        msg = f"Either source {source} or target {target} is not in G"
        raise NodeNotFound(msg)

    weight = _weight_function(G, weight)
    if heuristic is None:
        heuristic = base_heuristic

    cdef priority_queue[queue_types.queueItem] queue
    cdef int counter = 0
    queue.push(queue_types.queueItem(0, counter, source, 0, string(), string()))

    cdef unordered_map[string, pair[double, double]] enqueued
    cdef unordered_map[string, pair[string, string]] explored
    cdef string curnode
    cdef double dist
    cdef string parent
    cdef string key
    cdef string key_previous
    cdef string node
    cdef pair[string, string] explored_pair
    cdef queue_types.queueItem queue_item
    cdef unordered_map[string, unordered_map[string, unordered_map[string, double]]] adj
    cdef unordered_map[string, unordered_map[string, double]] w

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
            if explored[curnode] == (string(), string()):
                continue

            qcost, h = enqueued[curnode]
            if qcost < dist:
                continue

        explored_pair = pair[string, string](parent, key)
        explored[curnode] = explored_pair
        key_previous = key

        adj = G.adj(curnode)
        for neighbor, w in adj:
            for key, data in w:
                ncost = dist + weight(
                    g=G,
                    u=curnode,
                    v=neighbor,
                    key=key,
                    key_previous=key_previous,
                    v_previous=parent,
                )
                if enqueued.find(neighbor) != enqueued.end():
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
                counter += 1
                queue.push(queue_types.queueItem(ncost + h, counter, neighbor, ncost, curnode, key))

    raise NoPath("No path found between %s and %s." % (source, target))
