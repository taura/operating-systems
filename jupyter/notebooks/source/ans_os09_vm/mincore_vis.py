#!/usr/bin/python3
import math
import sys
import time
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as anm
from IPython import display 

def put_circle(img, cy, cx, r):
    h,w = img.shape
    for dx in range(-r, r + 1):
        for dy in range(-r, r + 1):
            if 0 <= cx + dx < w and 0 <= cy + dy < h and dx * dx + dy * dy <= r * r:
                img[cy+dy,cx+dx] = 1.0

def make_img(bm, mru):
    n_bytes, = bm.shape
    width = 256
    height = (n_bytes + width - 1) // width
    img = np.zeros((width * height), dtype=float)
    img[:n_bytes] = 0.4 * bm
    for idx in mru:
        p = int(idx) // 4096
        img[p] = 1.0
    img = img.reshape((height, width))
    for idx in mru:
        p = int(idx) // 4096
        cy = p // width
        cx = p  % width
        assert(img[cy,cx] == 1.0)
        put_circle(img, cy, cx, 1)
    X = np.array(range(width))
    Y = np.array(range(height))
    X,Y = np.meshgrid(X,Y)
    assert(X.shape == (height, width))
    return X, Y, img
        
def shrink(z):
    # z : 2次元配列 を縦横1ずつ縮めて, かつ無理やり1次元の配列に直す
    # (以前は set_array に必要だったが不要になた)
    m,n = z.shape
    return z[:m-1,:n-1].reshape((m - 1) * (n - 1))
    
def generate_mincore_frames(writer, fig, mincore_dat, animation_speed):
    # mincore_dat : filename
    # animation_speed :
    # change this value to change animation speed
    # 1.0 means animation runs as fast as the real execution.
    # smaller value means slower animation
    t0 = time.time()
    t = 0
    ax = fig.add_subplot(111)
    with open(mincore_dat, "rb") as fp:
        im = None
        txt = None
        frames = 0
        while 1:
            print(".", end="", flush=True)
            line = fp.readline()
            if line == b"":
                break
            if im is None:
                record_time0 = float(line)
                record_time = 0
            else:
                record_time = float(line) - record_time0
            n_pages = int(fp.readline())
            bm = np.fromfile(fp, dtype=np.uint8, count=n_pages)
            assert(bm.shape == (n_pages,)), (bm.shape, n_pages)
            line = fp.readline()
            n_mrus = int(line)
            mru = np.fromfile(fp, dtype=np.uint64, count=n_mrus)
            assert(mru.shape == (n_mrus,)), (mru.shape, n_mrus)
            t = (time.time() - t0) * animation_speed
            if im is None or t < record_time:
                X, Y, img = make_img(bm, mru)
                if im is None:
                    # show the first frame
                    height, _ = img.shape
                    cmap = plt.cm.get_cmap('bwr')
                    im = ax.pcolor(X, Y, img, cmap=cmap, vmin=0.0, vmax=1.0)
                    txt = ax.text(160, -height/7.5, "",
                                  horizontalalignment="right")
                    print("*", end="", flush=True)
                    writer.grab_frame()
                    # yield im,
                im.set_array(img)
                while t < record_time:
                    txt.set_text("real time : %.3f\nrecorded time: %.3f"
                                 % (t, record_time))
                    print("*", end="", flush=True)
                    writer.grab_frame()
                    # yield im,
                    t = (time.time() - t0) * animation_speed
            frames += 1
        # last image
        if im:
            X, Y, img = make_img(bm, mru)
            im.set_array(img)
            txt.set_text("real time : %.3f\nrecorded time: %.3f"
                         % (t, record_time))
            print("*", end="", flush=True)
            writer.grab_frame()
            # yield im,
        print("done")

def animate_mincore(mincore_dat, out_mp4, animation_speed):
    writer = anm.FFMpegWriter(fps=15) # fps=15, metadata={"artist": "Me"}, bitrate=1800)
    fig = plt.figure()
    with writer.saving(fig, out_mp4, dpi=100):
        generate_mincore_frames(writer, fig, mincore_dat, animation_speed)
        
def main():
    if len(sys.argv) <= 2:
        sys.stderr.write("usage: %s mincore_dat out_mp4\n" % sys.argv[0])
        sys.stderr.write(" file is a log file written by record mincore\n")
        sys.exit(1)
    micore_dat = sys.argv[1]
    out_mp4 = sys.argv[2]
    return animate_mincore(micore_dat, out_mp4, 0.05)

if sys.argv[0].endswith("mincore_vis.py"):
    main()
