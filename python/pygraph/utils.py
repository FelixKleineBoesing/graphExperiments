import cProfile
from pathlib import Path


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


def get_root_dir(orientation_dir: str = "pygraph"):
    cur_dir = Path(".").absolute()
    i = 0
    while True:
        for path_ in cur_dir.glob("*"):
            if path_.name == orientation_dir:
                return path_.parent().absolute()
        cur_dir = cur_dir.parent
        i += 1
        if i > 20:
            break
    return cur_dir
