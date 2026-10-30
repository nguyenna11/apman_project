MAIN = report
TEX = pdflatex
BIB = biber

.PHONY: all clean

all: $(MAIN).pdf

$(MAIN).pdf: $(MAIN).tex biblio.bib
	$(TEX) $(MAIN)
	$(BIB) $(MAIN)
	$(TEX) $(MAIN)
	$(TEX) $(MAIN)

clean:
	$(RM) $(MAIN).aux $(MAIN).bbl $(MAIN).blg $(MAIN).log $(MAIN).out $(MAIN).toc $(MAIN).lof $(MAIN).lop $(MAIN).bcf $(MAIN).run.xml *~
