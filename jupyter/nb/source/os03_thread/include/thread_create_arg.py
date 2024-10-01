import sys
import threading
import time

def f(slp_n, slp_usec, idx):
    th = threading.current_thread()
    for i in range(slp_n):
        print(f"child[{idx}/{th.native_id}] ({i}/{slp_n}): sleep {slp_usec} usec",
               flush=True)
        time.sleep(slp_usec * 1e-6)

def main():
    nthreads = int(sys.argv[1]) if 1 < len(sys.argv) else 3
    # 指定された数のスレッドを作る
    threads = []
    for i in range(nthreads):
        slp_n = i + 2
        th = threading.Thread(target=f, args=(slp_n, 1e6 / slp_n, i))
        th.start()
        threads.append(th)
    # 終了待ち
    for th in threads:
        th.join()

main()
