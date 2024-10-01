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
    for i in range(5):
        print(f"parent {os.getpid()}: {i}", flush=True)
        time.sleep(0.1)
