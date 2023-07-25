

def convert_edge_list(nodes: dict):
    edge_list = {}
    for node in nodes:
        for node_to in nodes[node]:
            for key in nodes[node][node_to]:
                edge_list[(node, node_to, key)] = nodes[node][node_to][key]
    return edge_list
