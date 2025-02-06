CC = gcc
FILES = mainloop.c aliases.c functions.c pipes.c redirection.c wild_characters.c
TARGET = mysh

all: $(TARGET)

$(TARGET): $(FILES)
	$(CC) -o $(TARGET) $(FILES)

clean:
	rm -f $(TARGET)
