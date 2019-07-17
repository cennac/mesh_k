#compiler
CC = msdk-linux-gcc

#external lib
APMIB_DIR = ../boa/apmib
CFLAGS := -s -Wall -Os
LDFLAGS := -s -Wall
IFLAGS := -I. -I$(APMIB_DIR)

SCHEDULE_DIR=$(shell pwd)
TARGET=schedule
$(TARGET):timer.o thread.o task.o etherAddr.o
	$(CC) timer.o thread.o task.o etherAddr.o -lrt  $(LDFLAGS) -L$(APMIB_DIR) -lapmib -o $(TARGET)
timer.o:timer.c task.o thread.o 
	$(CC) -c timer.c -o timer.o
thread.o: thread.c
	$(CC) -c thread.c -o thread.o
task.o: task.c etherAddr.o
	$(CC) -c task.c -o task.o
etherAddr.o: etherAddr.c
	$(CC) -c etherAddr.c -o etherAddr.o
romfs:
	$(ROMFSINST) $(SCHEDULE_DIR)/$(TARGET) /bin/

.PHONY:clean	
	
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o $(TARGET)
