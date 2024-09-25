NAME=echo-server
SRC=${NAME}.c

ifeq ($(OS),Windows)
    EXE_EXT=.exe
endif

EXE=${NAME}${EXE_EXT}

all: ${EXE}

ifneq ($(shell command -v cl),)

${EXE}: ${SRC}
	cl /std:clatest $< /Fe:$@ libuv.lib ws2_32.lib advapi32.lib user32.lib iphlpapi.lib userenv.lib dbghelp.lib ole32.lib shell32.lib

else

${EXE}: ${SRC}
	gcc -o $@ $< -luv

endif

.PHONY: clean

clean:
	rm -f ${EXE} *.o *.obj
