index.html : lecture.md scripts/style.css scripts/filter.py
	pandoc --filter scripts/filter.py --number-sections $< -o $@ 
