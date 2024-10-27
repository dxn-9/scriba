CC := clang
CFLAGS := -g $(shell pkg-config sdl3-ttf --cflags)
TARGET_NAME := scriba
SOURCE := main.c utils.c vector.c clock.c cursor.c text.c 
LDFLAGS := -I. $(shell pkg-config sdl3-ttf --libs)
FILE ?= 


run: clean build
	chmod +x scriba
	./scriba $(FILE)
clean: 
	rm -f $(TARGET_NAME) > /dev/null
build:
	$(CC) $(SOURCE) $(CFLAGS) $(LDFLAGS) -o $(TARGET_NAME)

