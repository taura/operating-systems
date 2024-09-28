# open_in_new_tab.py
from panflute import run_filter, Link, Span, Str

def set_target_blank(elem, doc):
    if isinstance(elem, Link):
        elem.attributes['target'] = '_blank'
        elem.attributes['rel'] = 'noopener'
    if isinstance(elem, Span) and 'blue' in elem.classes:
        elem.attributes['style'] = 'color: blue;'
    return elem

def main(doc=None):
    return run_filter(set_target_blank, doc=doc)

if __name__ == "__main__":
    main()
