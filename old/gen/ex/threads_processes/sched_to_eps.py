#!/usr/bin/python

import sys

def Ws(s):
    sys.stdout.write(s)

def Es(s):
    sys.stderr.write(s)

def set_color(idx):
    if idx == 0:
        Ws("0.0 0.0 1.0 setrgbcolor\n")
    elif idx == 1:
        Ws("0.0 1.0 0.0 setrgbcolor\n")
    elif idx == 2:
        Ws("1.0 0.0 0.0 setrgbcolor\n")
    elif idx == 3:
        Ws("0.0 1.0 1.0 setrgbcolor\n")
    elif idx == 4:
        Ws("1.0 0.0 1.0 setrgbcolor\n")
    elif idx == 5:
        Ws("1.0 1.0 0.0 setrgbcolor\n")
    elif idx == 6:
        Ws("0.0 0.0 0.5 setrgbcolor\n")
    elif idx == 7:
        Ws("0.0 0.5 0.0 setrgbcolor\n")
    elif idx == 8:
        Ws("0.5 0.0 0.0 setrgbcolor\n")
    else:
        bomb
    

def draw_rectangle(xa, xb, ya, yb):
    Ws("%.6f %.6f moveto " % (xa, ya))
    Ws("%.6f %.6f lineto " % (xb, ya))
    Ws("%.6f %.6f lineto " % (xb, yb))
    Ws("%.6f %.6f lineto " % (xa, yb))
    Ws("closepath fill\n")

def main():
    if 0:
        fp = open("x", "rb")
    elif len(sys.argv) < 2:
        Es("usage: ./sched_to_eps filename [ start [ end ] ] > result.eps\n")
        sys.exit(1)
    fp = open(sys.argv[1], "rb")
    sched = {}
    T0 = float("inf")
    T1 = - float("inf")
    line_no = 0
    for line in fp.readlines():
        line_no = line_no + 1
        fields = line.strip().split()
        if len(fields) == 3:
            [ pid,a,b ] = fields
            proc = "0"
        elif len(fields) == 4:
            [ pid,a,b,proc ] = fields
        elif len(fields) == 5:
            [ pid,a,b,proc,dt ] = fields
        else:
            Es("warning : skipping line %d '%s'\n" 
               % (line_no, line.strip()))
            continue
        pid = int(pid)
        a = float(a)
        b = float(b)
        proc = int(proc)
        if pid not in sched: sched[pid] = []
        sched[pid].append((a, b, proc))
        T0 = min(T0, a)
        T1 = max(T1, b)
    # Es("T0 = %f T1 = %f, but\n" % (T0, T1))
    if len(sys.argv) > 3:
        T1 = min(T1, T0 + float(sys.argv[3]))
        # Es("T1 -> %f\n" % T1)
    if len(sys.argv) > 2:
        T0 = max(T0, T0 + float(sys.argv[2]))
        # Es("T0 -> %f\n" % T0)
    sx = 800.0 / (T1 - T0)
    sy = 300.0 / len(sched)
    # Es("T0 = %f T1 = %f sx = %f sy = %f\n" % (T0, T1, sx, sy))
    Ws("%!PS-Adobe-3.0 EPSF-3.0\n")
    Ws("%%BoundingBox: 0 0 800 300\n")
    sched_items = sched.items()
    sched_items.sort()
    for i,(pid,T) in enumerate(sched_items):
        T.sort()
        for a,b,proc in T:
            if a > T1: break
            if b < T0: continue
            if a < T0: a = T0
            if b > T1: b = T1
            set_color(proc)
            draw_rectangle((a - T0) * sx, (b - T0) * sx, i * sy, (i + 1) * sy)
    Ws("showpage\n")

main()
