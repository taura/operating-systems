#!/usr/bin/python3
import matplotlib.collections as mc
import matplotlib.pyplot as plt

def read_dats(files):
    sched = {}
    for f in files:
        with open(f) as fp:
            for line in fp:
                fields = line.strip().split()
                [ pid,a,b,proc,dt ] = fields
                pid = int(pid)
                a = float(a)
                b = float(b)
                proc = int(proc)
                if pid not in sched:
                    sched[pid] = []
                sched[pid].append((a, b, proc))
    return sched

def sched_vis(files, start_t=0, end_t=float("inf")):
    '''
    files : sched_rec の結果が入ったファイル名のリスト 
            (例: [ "rec.0", "rec.1", .. ])
    start_t, end_t : その中で可視化したい区間の開始と終了
            (結果の中の一番早い時点を0として指定. 例えば
             begin_t=1, end_t=3, は開始から1秒目〜3秒目の
             2秒間を可視化する)
    '''
    log = read_dats(files)
    T0 = min(min(a for a, _, _ in T)  for T in log.values())
    n_procs = max(max(p for _, _, p in T) for T in log.values()) + 1
    cmap = plt.cm.get_cmap('RdYlGn', n_procs)
    segs = []
    cols = []
    fig, ax = plt.subplots()
    for i,(pid,T) in enumerate(sorted(log.items())):
        T.sort()
        for a,b,proc in T:
            t0 = max(a - T0, start_t)
            t1 = min(b - T0, end_t)
            if t0 >= t1:
                continue
            rect = plt.Rectangle((a - T0, i), b - a, 1, fc=cmap(proc))
            ax.add_patch(rect)
    ax.autoscale()
    plt.title("thread scheduling")
    plt.xlabel("time")
    plt.ylabel("thread")
    plt.ylim(0, len(log))
    plt.savefig("sched.svg")
    plt.show()
