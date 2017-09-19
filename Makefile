default:
	cc -Wall -o farg farg.c

install: default
	cp farg ~/bin/

clean:
	rm -f farg
