# Used for debugging makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning Building $@$(if $<, (from $<))%(if $?, ($? newer)))$(OLD_SHELL)

ifndef CONFIG
CONFIG=Debug
endif

SFLAGS = -Wall -fsyntax-only -std=c++0x
SRCS = $(wildcard lib/*.cpp)
OBJSTMP = $(SRCS:.cpp=.o)
OBJS = $(OBJSTMP:lib/%=obj/%)

EXES = twinkle test_twinkle

LFLAGS =  -pthread -lm  -lUnitTest++ -Ilib/
CXXFLAGS = -Wall -std=c++0x -g -O3


all: twinkle

obj/%.o: lib/%.cpp
	g++ -c -o $@ $< $(CXXFLAGS) $(LFLAGS)

twinkle: $(OBJS) main.cpp
	g++ -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test_twinkle: $(OBJS) test.cpp	
	g++ -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test: test_twinkle
	./test_twinkle

clean:
	rm -rf $(EXES) $(OBJS)

check-syntax:
	$(CXX) $(SFLAGS) $(CHK_SOURCES)
