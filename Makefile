all: build
clean: docs_clean

build:
	g++ -g -O2 -o main main.cpp -lsimlib -lm

run: build
	./$(EXECUTABLE)

docs:
	pdflatex -output-directory=$(DOCS_DIR) $(DOCS_NAME).tex
	pdflatex -output-directory=$(DOCS_DIR) $(DOCS_NAME).tex

docs_clean:
	rm -f $(DOCS_NAME).dvi $(DOCS_NAME).toc $(DOCS_NAME).log $(DOCS_NAME).fdb_latexmk docs/$(DOCS_NAME).fls $(DOCS_NAME).fdb_out $(DOCS_NAME).out $(DOCS_NAME).aux $(DOCS_NAME).ps $(DOCS_NAME).pdf
