ifdef MESSAGE
MESSAGEFLAG = -m "$(MESSAGE)"
endif

ifdef PUSH
PUSHFLAG = -p
endif

all:
	doxygen

publish: all
	ghp-import -nf $(PUSHFLAG) $(MESSAGEFLAG) html

clean:
	rm -rf html
