# Object files to create for the executable
DLL_OBJS = xxhash.o bloom.o

# Names of tools to use when building
CC = g++
FILENAME = libbloom

# Compiler flags
CFLAGS = -m64 -march=native -mtune=native -mssse3 -Wall -Wextra -Ofast -ftree-vectorize

# Linker flags
LDFLAGS = -shared -s

all: linux windows

linux:
	${CC} ${CFLAGS} -DXXH_INLINE_ALL -c xxhash/xxhash.c -o xxhash.o
	${CC} ${CFLAGS} -DXXH_INLINE_ALL -DADD_EXPORTS -c bloom/bloom.cpp -o bloom.o
	${CC} ${DLL_OBJS} ${LDFLAGS} -fpic -o "${FILENAME}.so"
	
windows:
	${CC} ${CFLAGS} -DXXH_INLINE_ALL -c xxhash/xxhash.c -o xxhash.o
	${CC} ${CFLAGS} -DXXH_INLINE_ALL -DADD_EXPORTS -c bloom/bloom.cpp -o bloom.o
	${CC} ${DLL_OBJS} ${LDFLAGS} -Wl,--subsystem,windows -o "${FILENAME}.dll"