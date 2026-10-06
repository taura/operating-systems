import threading
import time

def f(arg):
    th = threading.current_thread()
    slp_n = 5
    for i in range(slp_n):
        print(f"child[{th.native_id}]: ({i}/{slp_n})", flush=True)
    time.sleep(0.1)

def main():
    my_th = threading.current_thread()
    child_th = threading.Thread(target=f, args=(0,))
    child_th.start()
    slp_n = 5
    for i in range(slp_n):
        print(f"parent[{my_th.native_id}]: ({i}/{slp_n})", flush=True)
        time.sleep(0.1)
    child_th.join()

main()
