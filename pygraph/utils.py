import cProfile



def convert_edge_list(nodes: dict):
    edge_list = {}
    for node in nodes:
        for node_to in nodes[node]:
            for key in nodes[node][node_to]:
                edge_list[(node, node_to, key)] = nodes[node][node_to][key]
    return edge_list


def profile_and_dump_stats(func, args, kwargs):
    profiler = cProfile.Profile()
    profiler.enable()

    # Run the function with provided arguments and kwargs
    result = func(*args, **kwargs)

    profiler.disable()
    profiler.dump_stats('output.pstats')
    # Dump the stats to a file
    return result
