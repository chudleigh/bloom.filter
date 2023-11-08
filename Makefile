DLL_OBJS = bloom.o
CC = g++ -std=c++20
FILENAME = libbloom
CFLAGS = -m64 -march=native -mtune=native -mssse3 -Wall -Wextra -Ofast -ftree-vectorize -static
LDFLAGS = -shared -s

all: linux windows

linux:
	${CC} ${CFLAGS} -DXXH_INLINE_ALL -DADD_EXPORTS -c bloom/bloom.cpp -o bloom.o
	${CC} ${DLL_OBJS} ${LDFLAGS} -fpic -o "${FILENAME}.so"
	
windows:
	${CC} ${CFLAGS} -DXXH_INLINE_ALL -DADD_EXPORTS -c bloom/bloom.cpp -o bloom.o
	${CC} ${DLL_OBJS} ${LDFLAGS} -Wl,--subsystem,windows -o "${FILENAME}.dll"