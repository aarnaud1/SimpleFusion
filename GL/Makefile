CC := gcc
CFLAGS := -O2 -march=native
LDFLAGS := -ldl -lm
IFLAGS := -I./include

all: libOpenGL.so

lib/libOpenGL.so: src/glad.c
	$(CC) $(IFLAGS) -fPIC -shared -o $@ $^ $(LDFLAGS)

clean:
	rm -f lib/libOpenGL.so
