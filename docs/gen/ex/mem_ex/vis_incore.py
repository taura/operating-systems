#!/usr/bin/python
import math,sys,time
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# change this value to change animation speed
# 1.0 means animation runs as fast as the real execution.
# smaller value means slower animation
animation_speed = 0.1

def decode_run_length(X, last_accessed_page):
    """
    X : list of integers that encode bitmaps. e.g.,
    if the original data was 000110000111
    X = [3, 2, 4, 3]
    """
    # decode X into the bitmap of 1/0s
    n_bits = sum(X)
    # make a rectangle of roughly n_bits pixels
    width = int(math.sqrt(n_bits))
    height = (n_bits + width - 1) / width
    assert(width * height >= n_bits), (n_bits, width, height)
    # bitmap of all pages
    img = np.zeros((width * height))
    c = 0
    i = 0
    for x in X:
        img[i:i+x] = c
        c = 1 - c
        i += x
    img[i:] = 0
    img[last_accessed_page-20:last_accessed_page] = 0.5
    return img.reshape((width, height))
    

def vis_incore(fp):
    def load_data():
        im = None
        # animation time. incremented whenever
        # update gets called
        t0 = time.time()
        t = record_time = 0.0
        while 1:
            # print "t = %f, record_time = %f" % (t, record_time)
            line = fp.readline()
            if line == "": break
            line_split = line.split()
            # time when the record was taken
            record_time = float(line_split[0])
            if animation_speed * t >= record_time: 
                # print " --> continue"
                continue
            # idx of the last accessed page
            last_accessed_page = int(line_split[1])
            # run length encoding of in/out pages
            X = [ int(x) for x in line_split[2:] ]
            img = decode_run_length(X, last_accessed_page)
            if im:
                im.set_data(img)
            else:
                im = plt.imshow(img)
            while animation_speed * t < record_time:
                # print " --> yield t = %f, record_time = %f, img = %s" % (t, record_time, img)
                yield im,
                t = time.time() - t0

    fig = plt.figure()
    it = load_data()
    im, = it.next()
    def update(*args):
        try:
            return it.next()
        except StopIteration:
            return im,
    ani = animation.FuncAnimation(fig, update, 
                                  interval=30, 
                                  blit=True)
    plt.show()
        
def main():
    if len(sys.argv) <= 1:
        sys.stderr.write("usage: %s FILE\n" % sys.argv[0])
        sys.stderr.write(" FILE is a log file written by record_mincore\n")
        sys.exit(1)
    log = sys.argv[1]
    fp = open(log)
    vis_incore(fp)
    print "done"

if __name__ == "__main__":
    main()
print "OK"
