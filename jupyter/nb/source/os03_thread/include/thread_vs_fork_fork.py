import os

x = 0

def f():
    global x
    x += 321

def main():
    global x
    x = 123
  
    pid = os.fork()
    if pid == 0:
        f()
    else:
        cid, ws = os.waitpid(pid, 0)
        if os.WIFEXITED(ws):
            print(f"exited, status={os.WEXITSTATUS(ws)}", flush=True)
        elif os.WIFSIGNALED(ws):
            print(f"killed by signal {os.WTERMSIG(ws)}", flush=True)
        print(f"after the child finished, x = {x}")

main()
