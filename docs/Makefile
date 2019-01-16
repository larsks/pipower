%.pdf: %.ps
	ps2pdf $< $@

default: html

.PHONY: default html

html:
	doxygen

clean:
	rm -rf html
