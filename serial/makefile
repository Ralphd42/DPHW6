CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS =
OBJFILES = matmul.o timing.o
TARGET = part_1_serial.out
all: $(TARGET)
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
