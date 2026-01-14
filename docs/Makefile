mds :=
mds += index.md
mds += $(wildcard html/*.md)
mds += $(wildcard past_exams/*.md)
mds += $(wildcard html/exam2024/*.md)
htmls := $(patsubst %.md,%.html,$(mds))

all : $(htmls)

$(htmls) : %.html : %.md
	pandoc --filter scripts/filter.py --number-sections $< -o $@ 

.DELETE_ON_ERROR:
