
all: shell

shell: shell.c shellhead.h
	gcc -g -o shell shell.c shellhead.h
	
clean:
	rm -rf *.o *.a shell