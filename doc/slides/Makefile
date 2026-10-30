MAIN = slides
BUILD = build

LATEXMK = latexmk

.PHONY: all clean watch

all:
	mkdir -p $(BUILD)
	$(LATEXMK) -pdf -outdir=$(BUILD) $(MAIN).tex
	cp $(BUILD)/$(MAIN).pdf .

watch:
	mkdir -p $(BUILD)
	$(LATEXMK) -pdf -pvc -outdir=$(BUILD) $(MAIN).tex

clean:
	$(LATEXMK) -C -outdir=$(BUILD)

deep-clean: clean
	$(RM) -r $(BUILD)
	find . -name "*~" -delete
