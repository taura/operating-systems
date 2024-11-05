#!/usr/bin/python3
import os
import numpy as np

def mk_random_file(filename, size):
    assert(size <= 1000 * 1024 * 1024), "don't make it > 1GB"
    rg = np.random.RandomState()
    rg.seed(1234)
    a = rg.randint(0, 256, size=size, dtype=np.uint8)
    with open(filename, "wb") as wp:
        a.tofile(wp)

