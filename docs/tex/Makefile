
all: dvi

dvi:
	platex manual.tex
	bibtex manual
	platex manual.tex
	platex manual.tex
	platex manual.tex

pdf: dvi
	dvipdfmx manual.dvi
