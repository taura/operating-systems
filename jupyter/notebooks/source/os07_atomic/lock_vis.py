#!/usr/bin/python3
import sys
import re
import matplotlib.collections as mc
import matplotlib.pyplot as plt
import numpy as np

def read_dat(dat):
    pat = re.compile(r"(?P<i>\d+) (?P<thread>\d+) (?P<cpu>\d+) (?P<lock_enter>\d+\.\d+) (?P<lock_acq>\d+\.\d+) (?P<unlock_enter>\d+\.\d+)")
    log = {}
    with open(dat) as fp:
        for line in fp:
            m = pat.match(line)
            if not m:
                sys.stderr.write("warning: ignore line [%s]\n" % line.rstrip())
                continue
            i = int(m.group("i"))
            thread = int(m.group("thread"))
            cpu = int(m.group("cpu"))
            lock_enter = float(m.group("lock_enter"))
            lock_acq = float(m.group("lock_acq"))
            unlock_enter = float(m.group("unlock_enter"))
            if thread not in log:
                log[thread] = []
            log[thread].append((i, cpu, lock_enter, lock_acq, unlock_enter))
    return log
    
def progress_threads_plt(dat, start_t, end_t):
    log = read_dat(dat)
    nthreads = len(log)
    #cmap = plt.cm.get_cmap('RdYlGn', nthreads)
    cmap = plt.get_cmap('RdYlGn', nthreads)
    T0 = min(min(t for _, _, t, _, _ in recs) for recs in log.values())
    lines = []
    names = []
    fig, ax = plt.subplots()
    for thread, recs in sorted(log.items()):
        T = [unlock - T0 for _, _, _, _, unlock in recs if start_t <= unlock - T0 < end_t]
        N = list(range(len(T)))
        color = cmap(thread)
        lines.extend(ax.plot(T, N, 'o', markersize=0.5, color=color))
        names.append(thread)
    ax.autoscale()
    plt.title("progress of threads (%s)" % dat)
    plt.xlabel("time")
    plt.ylabel("updates (by thread)")
    # plt.legend(lines, names)
    plt.show()

def progress_threads_plts(dats, start_t=0, end_t=float("inf")):
    for dat in dats:
        progress_threads_plt(dat, start_t, end_t)

def compare_progress_plt(dat, color, fig, ax, start_t, end_t):
    log = read_dat(dat)
    T0 = min(min(t for _, _, t, _, _ in recs) for recs in log.values())
    T = []
    for thread, recs in sorted(log.items()):
        T.extend([unlock - T0 for _, _, _, _, unlock in recs if start_t <= unlock - T0 < end_t])
        # G.extend([i             for i, _, _, _, _ in recs])
        # color = cmap(thread)
    T.sort()
    N = list(range(len(T)))
    [l] = ax.plot(T, N, 'o', markersize=0.5, color=color)
    return (l, dat)
        
def compare_progress_plts(dats, start_t=0, end_t=float("inf")):
    n_methods = len(dats)
    #cmap = plt.cm.get_cmap('RdYlGn', n_methods)
    cmap = plt.get_cmap('RdYlGn', n_methods)
    fig, ax = plt.subplots()
    labels = []
    for i,dat in enumerate(dats):
        color = cmap(i)
        labels.append(compare_progress_plt(dat, color, fig, ax, start_t, end_t))
    ax.autoscale()
    lines = [l   for l,dat in labels]
    names = [dat for l,dat in labels]
    plt.title("compare methods")
    plt.xlabel("time")
    plt.ylabel("updates (total)")
    plt.legend(lines, names)
    plt.show()

def lock_wait_gantt(dat, start_t, end_t):
    log = read_dat(dat)
    n_cpus = max(max(cpu for _, cpu, _, _, _ in recs) for recs in log.values()) + 1
    n_threads = len(log)
    #cmap = plt.cm.get_cmap('RdYlGn', n_threads)
    cmap = plt.get_cmap('RdYlGn', n_threads)
    T0 = min(min(t for _, _, t, _, _ in recs) for recs in log.values())
    lock_wait_segs = []
    lock_held_segs = []
    wait_colors = []
    held_colors = []
    fig, ax = plt.subplots()
    for thread, recs in sorted(log.items()):
        c = cmap(thread)
        d = tuple([x + 0.8 * (1 - x) for x in c])
        for i, cpu, lock_enter, lock_acq, unlock_enter in recs:
            w0 = max(lock_enter - T0, start_t)
            w1 = min(lock_acq - T0, end_t)
            h0 = max(lock_acq - T0, start_t)
            h1 = min(unlock_enter - T0, end_t)
            if w0 <= w1:
                lock_wait_segs.append([(w0, thread), (w1, thread)])
                wait_colors.append(c)
            if h0 <= h1:
                lock_held_segs.append([(h0, thread), (h1, thread)])
                held_colors.append(d)
    lw = 320 / n_threads
    wc = mc.LineCollection(lock_wait_segs, colors=held_colors, linewidths=lw/2)
    hc = mc.LineCollection(lock_held_segs, colors=wait_colors, linewidths=lw)
    ax.add_collection(wc)
    ax.add_collection(hc)
    ax.autoscale()
    plt.title("thread status (%s)" % dat)
    plt.xlabel("time")
    plt.ylabel("threads")
    plt.show()

def lock_wait_gantts(dats, start_t=0, end_t=float("inf")):
    for dat in dats:
        lock_wait_gantt(dat, start_t, end_t)

def main():
    dats = sys.argv[1:]
    compare_progress_plts(dats)
    lock_wait_gantts(dats)
    progress_threads_plts(dats)

#if __name__ == "__main__":
#    main()
