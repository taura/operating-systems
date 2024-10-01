import os
import threading

x = 0

def f():
    global x
    x += 321

def main():
    global x
    x = 123
    th = threading.Thread(target=f)
    th.start()
    th.join()
    print(f"after the child finished, x = {x}")

main()
