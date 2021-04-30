CC	:= gcc
CFLAGS	:= -Wall -g
OBJS	:= main.o raid.o
M_OBJS	:= main.c raid.h
R_OBJS	:= raid.c raid.h debug.h bitops.h
PROGS	:= lrc_sim

all: $(PROGS)

.PHONY	: all

$(PROGS): $(OBJS)
	$(CC) -o $@ $(OBJS)

main.o: $(M_OBJS)
	$(CC) $(CFLAGS) -c $<

raid.o: $(R_OBJS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o $(PROGS) *.h.gch