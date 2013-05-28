CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

TARGET := rle2bmp
OBJS   := rle2bmp.o

CFLAGS = -O0 -Wall -lstdc++

all: $(TARGET)

clean: 
	rm -f *.o $(TARGET) *~

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

rle2bmp.o: rle2bmp.cpp
