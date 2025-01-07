mds   := index.md $(wildcard html/*.md)
htmls := $(patsubst %.md,%.html,$(mds))

all : $(htmls)

$(htmls) : %.html : %.md
	pandoc --filter scripts/filter.py --number-sections $< -o $@ 

.DELETE_ON_ERROR:
