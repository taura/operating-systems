#!/usr/bin/env python
import random,struct,sys

# make file of n MB
n = 256

def main():
    random.seed(123)
    PAD = "\x00" * 510 
    wp = open("A%d" % n, "wb")
    for i in xrange(0, n * (1024 * 1024 / 512)):
        r = random.randint(0, 65535)
        wp.write(struct.pack('H', r) + PAD)
    wp.close()

if __name__ == "__main__":
    main()
