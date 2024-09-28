#!/usr/bin/python
import sys

def Ws(s):
    sys.stdout.write(s)

def Es(s):
    sys.stderr.write(s)

def log2svg(fp, wp, w0, dw):
    log = []
    for line in fp:
        # 22040 7 enter_lock 1446099464.310180 
        #         return_from_lock 1446099464.310189 
        #         enter_unlock 1446099464.310189
        [ x,tid,_,call,_,locked,_,unlocked ] = line.strip().split()
        log.append((int(tid), int(x), float(call), float(locked), float(unlocked)))
    # get minimum time and tid
    min_t   = min([ call for tid,i,call,locked,unlocked in log ])
    min_tid = min([ tid for tid,i,call,locked,unlocked in log ])
    max_tid = max([ tid for tid,i,call,locked,unlocked in log ]) + 1
    # make everything relative
    log = [ (tid-min_tid,i,call-min_t,locked-min_t,unlocked-min_t) for tid,i,call,locked,unlocked in log ]
    # filter entries overlapping with the window [w0,w0+dw]
    # and again make everything relative to w0
    Ws("original log size = %d\n" % len(log))
    log = [ (tid,i,call-w0,locked-w0,unlocked-w0) for tid,i,call,locked,unlocked in log if (call <= w0 + dw and unlocked >= w0) ]
    Ws("log truncated to = %d\n" % len(log))
    tscale = 100000             # 1 sec = 100000 svg pixels
    aspect_ratio = 0.003
    x0 = 0
    y0 = 0
    x1 = int(dw * tscale)
    y1 = int(aspect_ratio * x1)
    yscale = y1 / (max_tid - min_tid)
    wp.write('''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <svg 
    xmlns="http://www.w3.org/2000/svg"
    width="%(width)s" height="%(height)s"
    viewBox="%(x0)s %(y0)s %(x1)s %(y1)s" 
    >\n''' % { "x0" : x0, "y0" : y0, "x1" : x1, "y1" : y1 ,
           "width"  : x1 - x0, "height" : y1 - y0 })
    colors = [ "red", "blue", "green", "purple" ]
    for tid,i,call,locked,unlocked in log:
        call     = min(dw, max(0, call))
        locked   = min(dw, max(0, locked))
        unlocked = min(dw, max(0, unlocked))
        x0 = int(call     * tscale)
        x1 = int(locked   * tscale)
        x2 = int(unlocked * tscale)
        y0 = int(tid       * yscale)
        y1 = int((tid + 1) * yscale)
        s0 = ('''<rect x="%(x)s" y="%(y)s" 
        width="%(width)s" height="%(height)s"
        fill="%(color)s" opacity="0.5" />
        '''
             % { "x" : x0, "y" : y0, 
                 "width" : max(1,x1 - x0), "height" : max(1,y1 - y0),
                 "color" : colors[tid % len(colors)] })
        s1 = ('''<rect x="%(x)s" y="%(y)s" 
        width="%(width)s" height="%(height)s"
        fill="%(color)s" opacity="1.0" />
        '''
             % { "x" : x1, "y" : y0, 
                 "width" : max(1, x2 - x1), "height" : max(1, y1 - y0),
                 "color" : colors[tid % len(colors)] })
        wp.write("%s\n" % s0)
        wp.write("%s\n" % s1)
    wp.write("</svg>\n")

def main():
    if len(sys.argv) <= 2:
        Es("usage: %s log_txt log_svg\n" % sys.argv[0])
        Es("example:\n  %s log.txt log.svg\n" % sys.argv[0])
        sys.exit(1)
    fp = open(sys.argv[1], "rb")
    wp = open(sys.argv[2], "wb")
    log2svg(fp, wp, 0.0, 10.0)
    wp.close()
    fp.close()

main()
