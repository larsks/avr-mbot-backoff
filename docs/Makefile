ifdef MESSAGE
MESSAGEFLAG = -m "$(MESSAGE)"
endif

ifdef PUSH
PUSHFLAG = -p
endif

all: docs

docs:
	doxygen

clean:
	rm -rf html

publish: all
	ghp-import -nf $(PUSHFLAG) $(MESSAGEFLAG) html

.PHONY: all docs clean publish
