all: pdf txt html

pdf: Using.md
	pandoc -o Using.pdf $<

html: Using.md
	pandoc -o Using.html $<

txt: Using.md
	pandoc -o Using.txt $<

clean:
	\rm -f *.html *.pdf *.txt

clobber: clean
	\rm -f *~

