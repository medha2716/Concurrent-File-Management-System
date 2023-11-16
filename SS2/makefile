CC = gcc
CFLAGS = -g
TARGET = ss
SRCS = storage_server.c create_dir.c create_file.c delete_file.c delete_folder.c ss1_copy.c copy_self.c file_details.c readfile.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

.PHONY: clean

clean:
	rm -f $(TARGET)
