CXX = gcc
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -g
#CFLAGS += -O3
CFLAGS += -std=gnu99
LIBS    = -lmraa

APP = rfm12

OBJS = main.o swspi.o cio.o cmd.o cli.o rfm12bs.o
FILES =  Makefile

.PHONY: all clean

all: $(APP)

$(APP): $(OBJS)
	$(CXX) $(CFLAGS) -o $(APP) $(OBJS) $(LIBS)

clean:
	rm -f *.o $(APP)

%.o: %.c %.h $(FILES)
	$(CXX) -c $(CFLAGS) -DIO_TARGET=IO_EDISON $< -o $@


