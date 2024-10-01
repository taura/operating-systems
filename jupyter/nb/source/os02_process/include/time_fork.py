import os
import sys
import time

def cur_time():
    return int(time.time() * 1.0e9)

def main():
    n = int(sys.argv[1]) if 1 < len(sys.argv) else 5
    t0 = cur_time()
### if VER % 2 == 1

  
    ここにプログラムを書く

  
### else
    for i in range(n):
        pid = os.fork()             # 現プロセスをコピー
        if pid == 0:                #  新しいプロセス(子プロセス)
### if VER == 2
            argv = [ "./do_nothing" ]
            os.execvp(argv[0], argv)
### elif VER == 4
            os._exit(0)
### endif
        else:
            cid, ws = os.waitpid(pid, 0)
### endif
    t1 = cur_time()
    dt = t1 - t0
### if VER <= 2
    print(f"{dt} nsec to fork-exec-wait {n} processes ({dt / n} nsec/proc)")
### elif VER <= 4
    print(f"{dt} nsec to fork-wait {n} processes ({dt / n} nsec/proc)")
### endif

main()
