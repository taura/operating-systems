#!/usr/bin/python3
import sys
import re
import matplotlib.pyplot as plt
import numpy as np

def read_dat(dat):
    pat = re.compile("(?P<var>[A-Za-z0-9_]+)@(?P<fun>[A-Za-z0-9_]+)\([A-Za-z0-9_]*\) *: *(?P<addr>\d+)")
    log = {}
    with open(dat) as fp:
        idx = 0
        for line in fp:
            m = pat.match(line)
            if not m:
                # sys.stderr.write("warning: ignore line [%s]\n" % line.rstrip())
                continue
            var = m.group("var")
            fun = m.group("fun")
            addr = int(m.group("addr"))
            if var not in log:
                log[var] = []
            log[var].append((idx, addr))
            idx += 1
    return log
    
def addrs_plt(dat):
    log = read_dat(dat)
    nvars = len(log)
    cmap = plt.cm.get_cmap('RdYlGn', nvars)
    A0 = min(min(addr for _, addr in addrs) for addrs in log.values())
    lines = []
    fig, ax = plt.subplots()
    for i, (var, addrs) in enumerate(sorted(log.items())):
        I = [line_no for line_no, _ in addrs]
        A = [addr for _, addr in addrs]
        color = cmap(i)
        lines.extend(ax.plot(I, A, marker='o', label=var, color=color))
    ax.autoscale()
    ax.legend()
    plt.title("addrs (%s)" % dat)
    plt.xlabel("line")
    plt.ylabel("addrs")
    plt.show()

def main():
    dat = sys.argv[1]
    addrs_plt(dat)

if sys.argv[0] == "./addrs_vis.py":
    main()
