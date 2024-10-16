CC := clang
TARGET_NAME := scriba
SOURCE := text.c main.c
LDFLAGS := -lSDL2 -lSDL2_ttf



run: clean build
	chmod +x scriba
	./scriba
clean: 
	rm -f $(TARGET_NAME) > /dev/null
build:
	$(CC) -g $(SOURCE) $(LDFLAGS) -o $(TARGET_NAME)

