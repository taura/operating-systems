import os

n = 5
for i in range(5):
    cid = os.fork()
    print(f"{os.getpid()} -> {cid}", flush=True)
