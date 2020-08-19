TARGET = myGame
VPATH = source
OBJS = main.o
CPPFLAGS = -std=c++2a

all: ${TARGET}

${TARGET}: ${OBJS}
	c++ -o $@ ${OBJS}

main.o:

.PHONY: do clean reset

do: 
	@./${TARGET}

clean: 
	rm ${OBJS}

reset:
	@echo "おはやし 1 50 50 5 3 0 1 0\n\
	-1" > data/player.dat