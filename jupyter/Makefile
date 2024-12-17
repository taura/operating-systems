#
# compile
#
#mk_nb_flags := --dbg 2

nb_srcs :=
nb_srcs += $(wildcard nb/source/os13_*/*.sos)
#nb_srcs += $(wildcard nb/source/os12_*/*.py)
#nb_srcs += $(wildcard nb/source/os14_*/*.sos)
# nb/source/os12/*.py

aux_srcs :=

aux_srcs += $(wildcard nb/source/os08_addr/addrs_vis.py)
aux_srcs += $(wildcard nb/source/os09_vm/page_fault_vis.py)
aux_srcs += $(wildcard nb/source/os09_vm/mincore_vis.py)
aux_srcs += $(wildcard nb/source/os10_cache/read_file_vis.py)
aux_srcs += $(wildcard nb/source/os11_prefetch/read_file_vis.py)
aux_srcs += $(wildcard nb/source/os12_mmap/read_mmap_vis.py)
#aux_srcs += $(wildcard nb/source/os07_atomic/lock_vis.py)
#aux_srcs += $(wildcard nb/source/os2023_exam/include/vers/2/buy2.h)
#aux_srcs += $(wildcard nb/source/os2023_exam/include/vers/2/check_buy2.c)
#aux_srcs += $(wildcard nb/source/os2022_exam/test?.txt)
#aux_srcs += $(wildcard nb/source/*/img/*.png)
#aux_srcs += $(wildcard nb/source/*/img/*.svg)
#aux_srcs += $(wildcard nb/source/*/misc/*.html)

users_csv := users.csv

feedback_class := os
include ../../jupyter_tools/authoring/jupyter_tools.mk
