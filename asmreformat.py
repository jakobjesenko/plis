#!/usr/bin/python3

import sys
lines = []
with open(sys.argv[1], "r") as f:
    lines = f.readlines()

lines = list(map(
    lambda line: ";".join(
        [(splitted := line.split(';'))[0].rstrip('\t').ljust(40), splitted[1]] if ';' in line else [line]
    ), lines)
)

with open(sys.argv[1], "w") as f:
    lines = f.writelines(lines)