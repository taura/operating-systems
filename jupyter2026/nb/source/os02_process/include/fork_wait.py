import os
import time

pid = os.fork()
if pid == 0:
    # 新しいプロセス(子プロセス)
    for i in range(5):
        print(f"child  {os.getpid()}: {i}", flush=True)
        time.sleep(0.1)
else:
    # 元いたプロセス(親プロセス) forkの返り値は子プロセスのプロセスID
    print(f"parent: wait for child (pid = {pid}) to finish", flush=True)
    cid, ws = os.waitpid(pid, 0)
    if os.WIFEXITED(ws):
        print(f"exited, status={os.WEXITSTATUS(ws)}", flush=True)
    elif os.WIFSIGNALED(ws):
        print(f"killed by signal {os.WTERMSIG(ws)}", flush=True)

