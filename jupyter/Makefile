#
# compile
#
#mk_nb_flags := --dbg 2

nb_srcs :=
#nb_srcs += $(wildcard nb/source/os16_*/*.sos)
nb_srcs += $(wildcard nb/source/os2025_exam_practice/*.sos)
#nb_srcs += $(wildcard nb/source/os14_*/*.sos)
# nb/source/os12/*.py

aux_srcs :=

aux_srcs += $(wildcard nb/source/os2025_exam/svg/*.svg)
aux_srcs += $(wildcard nb/source/os2025_exam_practice/misc/wikipedia-operating-system.html)

# aux_srcs += $(wildcard nb/source/os08_addr/addrs_vis.py)
# aux_srcs += $(wildcard nb/source/os09_vm/page_fault_vis.py)
# aux_srcs += $(wildcard nb/source/os09_vm/mincore_vis.py)
# aux_srcs += $(wildcard nb/source/os10_cache/read_file_vis.py)
# aux_srcs += $(wildcard nb/source/os11_prefetch/read_file_vis.py)
# aux_srcs += $(wildcard nb/source/os12_mmap/read_mmap_vis.py)
# aux_srcs += $(wildcard nb/source/os14_shell/samples/*.*)
#aux_srcs += $(wildcard nb/source/os07_atomic/lock_vis.py)
#aux_srcs += $(wildcard nb/source/os2023_exam/include/vers/2/buy2.h)
#aux_srcs += $(wildcard nb/source/os2023_exam/include/vers/2/check_buy2.c)
#aux_srcs += $(wildcard nb/source/os2022_exam/test?.txt)
#aux_srcs += $(wildcard nb/source/*/img/*.png)
#aux_srcs += $(wildcard nb/source/*/img/*.svg)
#aux_srcs += $(wildcard nb/source/*/misc/*.html)

aux_gpg_srcs :=
aux_gpg_srcs += nb/source/os2025_exam_practice/count_char.c
# aux_gpg_srcs += ../exams/2024/problems/program/prob2/i.pgm
# aux_gpg_srcs += ../exams/2024/problems/program/prob2/idx.txt
# aux_gpg_srcs += ../exams/2024/problems/program/prob2/pgm.c
# aux_gpg_srcs += ../exams/2024/problems/program/prob3/gate_test_void.c
# aux_gpg_srcs += ../exams/2024/problems/program/prob3/gate_test_int.c
aux_gpg_dsts := $(patsubst nb/%,notebooks/%.gpg,$(aux_gpg_srcs))
#aux_gpg_dsts := $(patsubst ../exams/2025/problems/program/%,notebooks/source/os2025_exam/%.gpg,$(aux_gpg_srcs))

users_csv := users.csv

feedback_class := os
include ../../jupyter_tools/authoring/jupyter_tools.mk

# $(aux_gpg_dsts) : notebooks/%.gpg : nb/%
# 	mkdir -p $(dir $@)
# 	cat $< | gpg --batch --passphrase-file ../exams/2025/problems/os2025_exam_pw.txt --symmetric --cipher-algo AES256 > $@

# $(aux_gpg_dsts) : notebooks/source/os2025_exam/%.gpg : ../exams/2025/problems/program/%
# 	mkdir -p $(dir $@)
# 	cat $< | gpg --batch --passphrase-file ../exams/2025/problems/os2025_exam_pw.txt --symmetric --cipher-algo AES256 > $@

notebooks/source/os2025_exam_practice/count_char.c.gpg : notebooks/source/%.c.gpg : nb/source/%.c
	mkdir -p $(dir $@)
	./mk_version.py -D VER=1 $< | gpg --batch --passphrase-file ../exams/2025/problems/os2025_exam_practice_pw.txt --symmetric --cipher-algo AES256 -o $@

notebooks/source/ans_os2025_exam_practice/count_char.c.gpg : notebooks/source/ans_%.c.gpg : nb/source/%.c
	mkdir -p $(dir $@)
	./mk_version.py -D VER=1 $< | gpg --batch --passphrase-file ../exams/2025/problems/os2025_exam_practice_pw.txt --symmetric --cipher-algo AES256 -o $@

compile : $(aux_gpg_dsts)

# compile : notebooks/source/os2025_exam_practice/count_char.c.gpg
# compile : notebooks/source/ans_os2025_exam_practice/count_char.c.gpg


