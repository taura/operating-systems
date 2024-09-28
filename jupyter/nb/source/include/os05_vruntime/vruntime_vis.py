#!/usr/bin/python3
import sys
import re
import matplotlib.collections as mc
import matplotlib.pyplot as plt
import numpy as np

def read_dat(dat):
    pat = re.compile("(?P<pid>\d+) (?P<cpu>\d+) (?P<begin>\d+\.\d+) (?P<end>\d+\.\d+) (?P<dt>\d+\.\d+) (?P<vruntime>\d+)")
    log = {}
    with open(dat) as fp:
        for line in fp:
            m = pat.match(line)
            if not m:
                sys.stderr.write("warning: ignore line [%s]\n" % line.rstrip())
                continue
            pid = int(m.group("pid"))
            cpu = int(m.group("cpu"))
            begin = float(m.group("begin"))
            end = float(m.group("end"))
            dt = float(m.group("dt"))
            vruntime = int(m.group("vruntime"))
            if pid not in log:
                log[pid] = []
            log[pid].append((begin, end, vruntime, cpu))
    return log

def read_dats(dats):
    log = {}
    for dat in dats:
        log.update(read_dat(dat))
    return log

def vruntime_vis(files, start_t=0, end_t=float("inf")):
    log = read_dats(files)
    n_procs = len(log)
    n_cpus = max(max(cpu for _, _, _, cpu in recs) for recs in log.values()) + 1
    cmap = plt.cm.get_cmap('RdYlGn', n_procs)
    T0 = min(min(begin for begin, _, _, _ in recs) for recs in log.values())
    V0 = min(min(vrunt for _, _, vrunt, _ in recs) for recs in log.values())
    segs = []
    cols = []
    fig, ax = plt.subplots()
    for i, (pid, recs) in enumerate(log.items()):
        recs.sort()
        for begin, end, vruntime, cpu in recs:
            t0 = max(begin - T0, start_t)
            t1 = min(end - T0, end_t)
            if t0 >= t1:
                continue
            c = cmap(i)
            segs.append([(t0, vruntime), (t1, vruntime)])
            cols.append(c)
    wc = mc.LineCollection(segs, colors=cols, linewidths=3)
    ax.add_collection(wc)
    ax.autoscale()
    plt.title("thread vruntime")
    plt.xlabel("time")
    plt.ylabel("vruntime")
    plt.savefig("vruntime.svg")
    plt.show()
    
# usage:
# if you have n processes
# vruntime_vis(["vr.{}".format(i) for i in range(n)])
