SRC=echo-server.c
EXE=echo-server

all: ${EXE}

${EXE}: ${SRC}
	gcc -o $@ $< -luv

.PHONY: clean

clean:
	rm -f ${EXE}
