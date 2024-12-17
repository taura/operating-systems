#!/usr/bin/python3
import sys
import re
import matplotlib.collections as mc
import matplotlib.pyplot as plt
import numpy as np

def read_dat(dat):
    # 2	0.000598	0	1025
    pat = re.compile(r"(?P<accessed>\d+)	(?P<t>\d+\.\d+)	(?P<minflt>\d+)	(?P<majflt>\d+)")
    log = []
    with open(dat) as fp:
        for line in fp:
            m = pat.match(line)
            if not m:
                sys.stderr.write("warning: ignore line [%s]\n" % line.rstrip())
                continue
            D = m.groupdict()
            D = {k : float(D[k]) for k in D}
            log.append(D)
    return log
    
def progress_plt(dat, xlabel, ylabel):
    log = read_dat(dat)
    fig, ax = plt.subplots()
    X = np.array([d[xlabel] for d in log])
    Y = np.array([d[ylabel] for d in log])
    ax.plot(X, Y - Y[0])
    ax.autoscale()
    plt.title(dat)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.show()

def main():
    time_rusage_dat = sys.argv[1]
    progress_plt(time_rusage_dat, "accessed", "t")
    progress_plt(time_rusage_dat, "accessed", "minflt")
    progress_plt(time_rusage_dat, "accessed", "majflt")
    
if sys.argv[0].endswith("page_fault_vis.py"):
    main()

