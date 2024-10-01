import os

print(f"{os.getpid()} : before fork", flush=True)
os.fork()
print(f"{os.getpid()} : after fork")
