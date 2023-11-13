CC = gcc
CFLAGS = -g
TARGET = ss
SRCS = ss.c create_dir.c create_file.c delete_file.c delete_folder.c ss1_copy.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

.PHONY: clean

clean:
	rm -f $(TARGET)
