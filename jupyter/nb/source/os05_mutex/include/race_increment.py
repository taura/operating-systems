import random
import sys
import threading
g = 0

### if VER >= 2
def plus_one(x):
    return x + 1

### endif
def parallel(f, nthreads):
    """
    #pragma omp parallel に似たもの

    f(0), f(1), ..., f(nthreads - 1) の各々をスレッドで実行
    """
    threads = [threading.Thread(target=f, args=(i, ))
               for i in range(nthreads)]
    for th in threads:
        th.start()
    for th in threads:
        th.join()

### if VER >= 3
def parallel_for(f, a, b, nthreads):
    """
    #pragma omp parallel for に似たもの
    f(a), f(a+1), ..., f(b-1) を nthreads で分割して実行
    """
    def thread_fun(i):
        ai = (a * (nthreads - i)     + b * i) // nthreads
        bi = (a * (nthreads - i - 1) + b * (i + 1)) // nthreads
        for i in range(ai, bi):
            f(i)
    parallel(thread_fun, nthreads)

### endif
def main():
    global g
    argv = sys.argv
    argc = len(argv)
    i = 1
    n = int(argv[i]) if i < argc else 1000000
    i += 1
    nthreads = int(argv[i]) if i < argc else 2
    i += 1

    g = 0
### if VER == 1
    def thread_fun(idx):
        global g
        for i in range(n):
            g += 1
    parallel(thread_fun, nthreads)
### elif VER == 2
    def thread_fun(idx):
        global g
        for i in range(n):
            g = plus_one(g)
    parallel(thread_fun, nthreads)
### else
    # 1 iteration分の処理
### if VER == 4
    m = threading.Lock()
### endif    
    def iter_fun(i):
        global g
### if VER == 4
        m.acquire()
### endif    
        g = plus_one(g)
### if VER == 4
        m.release()
### endif    
    parallel_for(iter_fun, 0, n, nthreads)
### endif
    print(f"g = {g}")
    return 0

sys.exit(main())
