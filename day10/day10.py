import sys
import re
from functools import cache
from z3 import *


class Machine:
    lights: str
    buttons: list[tuple[int, ...]]
    joltages: tuple[int, ...]

    def __init__(self, lights, buttons, joltages) -> None:
        self.lights = lights
        self.buttons = buttons
        self.joltages = joltages

    def part1(self):
        @cache
        def solve(lights: str, used_buttons: tuple[int, ...]):
            if lights == self.lights:
                return 0

            best = None

            for i, button in enumerate(self.buttons):
                if i in used_buttons:
                    continue

                for light in button:
                    if lights[light] != self.lights[light]:
                        lights_copy = list(lights)
                        for light in button:
                            lights_copy[light] = (
                                "#" if lights_copy[light] == "." else "."
                            )

                        result = solve(
                            "".join(lights_copy), tuple(sorted(used_buttons + (i,)))
                        )
                        if result is not None:
                            if best is None or result + 1 < best:
                                best = result + 1

            return best

        return solve("." * len(self.lights), ())

    def part2(self):
        @cache
        def solve(joltages: tuple[int, ...], index: int):
            if joltages == self.joltages:
                return 0
            if index >= len(self.buttons):
                return None
            for a, b in zip(joltages, self.joltages):
                if a > b:
                    return None

            best = None

            button = self.buttons[index]
            max_presses = min(
                self.joltages[light] - joltages[light] for light in button
            )

            for presses in range(max_presses, -1, -1):
                joltages_copy = list(joltages)
                for light in button:
                    joltages_copy[light] += presses

                result = solve(tuple(joltages_copy), index + 1)
                if result is not None:
                    if best is None or result + presses < best:
                        best = result + presses

            return best

        return solve((0,) * len(self.joltages), 0)

    def part2z3(self):
        smt = []

        for i in range(len(self.buttons)):
            smt.append(f"(declare-const x{i} Int)")
            smt.append(f"(assert (>= x{i} 0))")

        for i, joltage in enumerate(self.joltages):
            xs = []
            for x, button in enumerate(self.buttons):
                if i in button:
                    xs.append(f"x{x}")
            smt.append(f"(assert (= {joltage} (+ {' '.join(xs)})))")

        smt.append(
            f"(assert (= n (+ {' '.join([f'x{i}' for i in range(len(self.buttons))])})))"
        )

        n = Int("n")
        p = parse_smt2_string("\n".join(smt), decls={"n": n})

        opt = Optimize()
        opt.add(p)
        opt.minimize(n)
        opt.check()

        return opt.model()[n].as_long()


def parse(file_name):
    machines = []
    with open(file_name, mode="r") as in_file:
        for line in in_file:
            match = re.match(r"\[(.*)\] (.*) {(.*)}", line)
            groups = match.groups()

            buttons = groups[1][1:-1].split(") (")
            buttons = list(map(lambda e: tuple(map(int, e.split(","))), buttons))

            joltages = tuple(map(int, groups[2].split(",")))

            machines.append(Machine(groups[0], buttons, joltages))

    return machines


def solve():
    machines = parse(sys.argv[1])

    p1 = sum(m.part1() for m in machines)
    print(f"p1: {p1}")

    p2 = sum(m.part2z3() for m in machines)
    print(f"p2: {p2}")


solve()
