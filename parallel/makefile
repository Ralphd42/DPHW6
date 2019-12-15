CC = mpicc
CFLAGS =   -std=c99
LDFLAGS =
OBJFILES = matmul.o timing.o
TARGET = part_1_parallel.out
all: $(TARGET)
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
