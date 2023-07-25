# astar_path.pyx

from heapq import heappop, heappush
from itertools import count
from pygraph.nativepy.multidigraph import MultiDiGraph


class NodeNotFound(Exception):
    """Exception raised if node not present in graph.
    """
    pass


class NoPath(Exception):
    """Exception raised if no path can be found.
    """
    pass


def _weight_function(G: MultiDiGraph , weight):
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


def base_heuristic(u, u_data, v, v_data):
    return 0


def astar_path(G: MultiDiGraph, source, target, heuristic=None, weight="weight") -> list:
    if source not in G or target not in G:
        msg = f"Either source {source} or target {target} is not in G"
        raise NodeNotFound(msg)

    weight = _weight_function(G, weight)
    if heuristic is None:
        heuristic = base_heuristic
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
                    # if qcost <= ncost, a less costly path from the
                    # neighbor to the source was already determined.
                    # Therefore, we won't attempt to push this neighbor
                    # to the queue
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
                heappush(queue, (ncost + h, next(c), neighbor, ncost, curnode, key))

    raise NoPath("No path found between %s and %s." % (source, target))
