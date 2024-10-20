CC := clang
CFLAGS := $(shell pkg-config sdl3-ttf --cflags)
TARGET_NAME := scriba
SOURCE := main.c vector.c clock.c cursor.c text.c 
LDFLAGS := -I. $(shell pkg-config sdl3-ttf --libs)



run: clean build
	chmod +x scriba
	./scriba
clean: 
	rm -f $(TARGET_NAME) > /dev/null
build:
	$(CC) -g $(SOURCE) $(CFLAGS) $(LDFLAGS) -o $(TARGET_NAME)

