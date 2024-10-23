CC := clang
CFLAGS := -g $(shell pkg-config sdl3-ttf --cflags)
TARGET_NAME := scriba
SOURCE := main.c utils.c vector.c clock.c cursor.c text.c 
LDFLAGS := -I. $(shell pkg-config sdl3-ttf --libs)



run: clean build
	chmod +x scriba
	./scriba
clean: 
	rm -f $(TARGET_NAME) > /dev/null
build:
	$(CC) $(SOURCE) $(CFLAGS) $(LDFLAGS) -o $(TARGET_NAME)

