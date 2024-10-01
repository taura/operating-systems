import os

pid = os.fork()
if pid == 0:                    # 新しいプロセス(子プロセス)
    argv = [ "/bin/ls", "-l" ]
    os.execv(argv[0], argv)
else:
    cid, ws = os.waitpid(pid, 0)
    if os.WIFEXITED(ws):
        print(f"exited, status={os.WEXITSTATUS(ws)}", flush=True)
    elif os.WIFSIGNALED(ws):
        print(f"killed by signal {os.WTERMSIG(ws)}", flush=True)
