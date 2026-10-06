import sys
import threading
import time

def cur_time():
    return int(time.time() * 1e9)

def do_nothing():
    return

def main():
    n = int(sys.argv[1]) if 1 < len(sys.argv) else 5
    t0 = cur_time()
### if VER % 2 == 1

  
    ここにプログラムを書く

  
### else
    for i in range(n):
        th = threading.Thread(target=do_nothing)
        th.start()
        th.join()
### endif
    t1 = cur_time()
    dt = t1 - t0
    print(f"{dt} nsec to thread_create-and-join {n} threads ({dt/n} nsec/thread)")

main()
