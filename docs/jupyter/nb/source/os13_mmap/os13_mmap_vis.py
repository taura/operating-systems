""" md
# 対話的な可視化用
"""

""" code w kernel=python """
%matplotlib widget
""" include nb_src/source/include/os13_mmap/read_mmap_vis.py """
""" """

""" md 
* 経過時間
"""

""" code w kernel=python """
graph(["mmap.dat"], "t")
""" """

""" md 
* マイナーフォルト (getrusageで取得)
"""

""" code w kernel=python """
graph(["mmap.dat"], "minflt")
""" """

""" md 
* メジャーフォルト (getrusageで取得)
"""

""" code w kernel=python """
graph(["mmap.dat"], "majflt")
""" """

""" md
* 読み込みブロック数 (getrusageで取得)
"""

""" code w kernel=python """
graph(["mmap.dat"], "inblock")
""" """

""" md
# mmap vs. read
"""

""" md 
* 経過時間
"""

""" code w kernel=python """
graph(["mmap.dat", "read.dat"], "t")
""" """

""" md
* マイナーフォルト (getrusageで取得)
"""

""" code w kernel=python """
graph(["mmap.dat", "read.dat"], "minflt")
""" """

""" md 
* メジャーフォルト (getrusageで取得)
"""

""" code w kernel=python """
graph(["mmap.dat", "read.dat"], "majflt")
""" """

""" md
* 読み込みブロック数 (getrusageで取得)
"""

""" code w kernel=python """
graph(["mmap.dat", "read.dat"], "inblock")
""" """

""" md
* メモリ消費量の違い
"""

""" code w kernel=python """
graph(["mmap_mem_limit.dat", "read_mem_limit.dat"], "t")
""" """

