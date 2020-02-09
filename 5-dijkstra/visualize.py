#!/usr/bin/python3.7

import matplotlib.pyplot as plt
import argparse

from itertools import chain


def read_roads(filename: str):
    with open(filename, 'r') as f:
        n = int(f.readline())

        segments = []
        for i in range(n):
            segments.append(list(map(int, f.readline().split())))

        start = list(map(int, f.readline().split()))
        end = list(map(int, f.readline().split()))

    return segments, start, end


def main(n: int):
    n = "{:0>3}".format(n)
    input_filename = f"tests/input/{n}.txt"

    segments, start, end = read_roads(input_filename)

    plt.scatter([start[0], end[0]], [start[1], end[1]])

    for segment in segments:
        plt.plot([segment[0], segment[2]], [segment[1], segment[3]])

    left = min(chain(map(lambda t: t[0], segments), map(lambda t: t[2], segments)))
    right = max(chain(map(lambda t: t[0], segments), map(lambda t: t[2], segments)))
    bottom = min(chain(map(lambda t: t[1], segments), map(lambda t: t[3], segments)))
    top = max(chain(map(lambda t: t[1], segments), map(lambda t: t[3], segments)))

    plt.axis([left-1, right+1, bottom-1, top+2])
    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='City roads visualizer.')

    parser.add_argument('n', metavar='N', type=int, help='Number of test file.')

    args = parser.parse_args()

    main(args.n)
