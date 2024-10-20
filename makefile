CC := clang
TARGET_NAME := scriba
SOURCE := main.c vector.c clock.c cursor.c text.c 
LDFLAGS := -I. -lSDL2 -lSDL2_ttf



run: clean build
	chmod +x scriba
	./scriba
clean: 
	rm -f $(TARGET_NAME) > /dev/null
build:
	$(CC) -g $(SOURCE) $(LDFLAGS) -o $(TARGET_NAME)

