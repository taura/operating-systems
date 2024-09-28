#!/usr/bin/python3
import pdb
import re
import sqlite3
import sys
import matplotlib.pyplot as plt
import numpy as np

def process_line(filename, line, exprs, row_exprs, V, K, R):
    for e in exprs:
        m = e.search(line)
        if m:
            V.update(m.groupdict())
            dK = set(V.keys()).difference(set(K))
            K.extend(list(dK))
    for r in row_exprs:
        if r.search(line):
            R.append(V.copy())
            break

def read_dats(result_dats, exprs, row_exprs):
    K = []
    R = []
    for result_dat in result_dats:
        V = {"file" : result_dat}
        with open(result_dat) as fp:
            for line in fp:
                process_line(result_dat, line, exprs, row_exprs, V, K, R)
    return K, R

def sqlite3_val(x):
    if x is None:
        return None
    try:
        return int(x)
    except ValueError:
        pass
    try:
        return float(x)
    except ValueError:
        pass
    try:
        return float(x)
    except ValueError:
        pass
    return x

def sql_exec(co, cmd, *args):
    #print(cmd)
    return co.execute(cmd, *args)

def dats_to_db(result_dats, exprs, row_exprs):
    K, R = read_dats(result_dats, exprs, row_exprs)
    co = sqlite3.connect(":memory:")
    sql_exec(co, "create table a({cols})".format(cols=",".join(K)))
    insert = ("insert into a({cols}) values({placeholders})"
              .format(cols=",".join(K), placeholders=",".join(["?"] * len(K))))
    for r in R:
        sql_exec(co, insert, tuple([sqlite3_val(r.get(k)) for k in K]))
    co.commit()
    return co

def draw(files, exprs, row_exprs, cmds, start=0, end=-1,
         xlabel=None, ylabel=None, col_is_x=False):
    co = dats_to_db(files, exprs, row_exprs)
    fig, ax = plt.subplots()
    if xlabel:
        plt.xlabel(xlabel)
    if ylabel:
        plt.ylabel(ylabel)
    for cmd,kw in cmds:
        res = list(sql_exec(co, cmd))
        res = res[start:end]
        if len(res) == 0:
            continue
        if len(res[0]) == 1:
            res = list(enumerate(x for x, in res))
            if col_is_x:
                res = [(y,x) for x,y in res]
        x = np.array([x for x,_ in res])
        y = np.array([y for _,y in res])
        plt.plot(x, y, '-o', **kw)
    plt.legend()
    plt.show()
    co.close()

def graph(files, y, start=0, end=-1):
    exprs = [
        re.compile(r"(?P<i>\d+) (?P<t>\d+\.\d+) (?P<minflt>\d+) (?P<majflt>\d+) (?P<inblock>\d+) (?P<oublock>\d+)"),
    ]
    row_exprs = exprs[-1:]
    draw(files, exprs, row_exprs,
         [('select i,%s from a where file="%s"' % (y, filename),
           dict(label="%s" % filename))
          for filename in files], # , "minflt", "majflt"
         xlabel="i", ylabel=y, start=start, end=end)

if sys.argv[0].endswith("read_mmap_vis.py"):
    graph(sys.argv[1:], "t")

