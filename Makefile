.PHONY: all clean
.DEFAULT: all

CXX=g++
LD=g++

CXXFLAGS=-O2
LDLIBS=-lallegro -lallegro_font -lallegro_image -lallegro_primitives -ljson-c

SRCS=$(wildcard *.cc)
OBJS=$(patsubst %.cc, %.o, $(SRCS))

all: game

game: $(OBJS)
	$(LD) $(OBJS) $(LDLIBS) -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) $< -c -o $@

clean:
	rm -f $(OBJS)
	rm -f game
