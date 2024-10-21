import os
import time
import sys

# Structure to store the time period and the CPU it ran on
class Rec:
    def __init__(self, begin, end, proc):
        self.begin = begin
        self.end = end
        self.proc = proc

# Get the current time
def cur_time():
    return time.time()

def get_current_cpu():
    with open("/proc/self/stat", "r") as f:
        stat = f.read().split()
        return int(stat[38])  # 39th field in /proc/self/stat is the CPU number

# Function that simulates continuous running for T seconds and records the CPU it ran on
def run(T, n):
    pid = os.getpid()  # Get process ID
    limit = cur_time() + T
    records = [Rec(cur_time(), cur_time(), get_current_cpu()) for _ in range(n)]
    i = 0
    records[i].begin = records[i].end = cur_time()

    while records[i].end < limit and i < n:
        t = cur_time()
        proc = get_current_cpu()  # Simulate sched_getcpu with affinity
        if t - records[i].end < 1.0E-3 and proc == records[i].proc:
            records[i].end = t
        else:
            if i + 1 >= n:
                break
            i += 1
            records[i].proc = proc
            records[i].begin = records[i].end = cur_time()
    assert i < n
    for j in range(i + 1):
        print(f'{pid} {records[j].begin:.6f} {records[j].end:.6f} {records[j].proc} {records[j].end - records[j].begin:.6f}')
    return 0

def main():
    # Get command line arguments
    T = float(sys.argv[1]) if len(sys.argv) > 1 else 10.0
    n = int(sys.argv[2]) if len(sys.argv) > 2 else 100000
    run(T, n)

if __name__ == "__main__":
    main()
