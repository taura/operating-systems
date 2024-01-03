#!/usr/bin/python3
import os
import sys
import getpass
import numpy as np

def mk_sorted_file(filename, size):
    assert(size <= 1024 * 1024 * 1024), "don't make it > 4GB"
    rg = np.random.RandomState()
    rg.seed(1234)
    gap = 1024 * 1024 * 1024 / size
    a = rg.randint(0, gap, size=size, dtype=np.uint32)
    s = 0
    for i in range(size):
        s += a[i]
        a[i] = s
    dirname = os.path.dirname(filename)
    if dirname == "":
        dirname = "."
    os.makedirs(dirname, exist_ok=True)
    with open(filename, "wb") as wp:
        a.tofile(wp)

def main():
    filename = sys.argv[1]
    sz = int(sys.argv[2])
    mk_sorted_file(filename, sz)
        
if sys.argv[0].endswith("mk_sorted_file.py"):
    main()

