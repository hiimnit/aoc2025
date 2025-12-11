import sys
from collections import defaultdict


def parse(file: str) -> dict[str, list[str]]:
    nodes = {}
    with open(file, mode="r") as in_file:
        for line in in_file:
            node, outputs = line.strip().split(": ")
            outputs = outputs.split(" ")
            nodes[node] = outputs

    return nodes


def count_paths(nodes: dict[str, list[str]], from_node: str, to_node: str):
    queue = [from_node]

    sources = defaultdict(set)
    visited = set()

    while queue:
        node = queue.pop()
        if node in visited:
            continue
        if node == to_node:
            continue

        visited.add(node)

        for output in nodes[node]:
            sources[output].add(node)
            queue.append(output)

    queue = [node for node in sources[to_node]]

    result = 0

    while queue:
        node = queue.pop()

        if node == from_node:
            result += 1
            continue

        for source in sources[node]:
            queue.append(source)

    return result


def part1(nodes: dict[str, list[str]]):
    return count_paths(nodes, "you", "out")


def part2(nodes: dict[str, list[str]]):
    return (
        count_paths(nodes, "svr", "fft")
        * count_paths(nodes, "fft", "dac")
        * count_paths(nodes, "dac", "out")
    )


def solve():
    nodes = parse(sys.argv[1])
    nodes["out"] = []

    p1 = part1(nodes)
    print(f"p1: {p1}")

    p2 = part2(nodes)
    print(f"p2: {p2}")


solve()
