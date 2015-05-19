SFLAGS = -Wall -fsyntax-only -std=c++0x -I/home/mason/include
SRCS = $(wildcard lib/*.cpp)
OBJSTMP = $(SRCS:.cpp=.o)
OBJS = $(OBJSTMP:lib/%=obj/%)

EXES = twinkle test_twinkle

LFLAGS =  -pthread -lm  -I/home/mason/include -L/home/mason/lib -lUnitTest++ -Ilib/
CXXFLAGS = -Wall -std=c++0x -g

.PHONY: test

all: $(EXES)

obj:
	mkdir -p obj

obj/%.o: lib/%.cpp obj
	g++ -c -o $@ $< $(CXXFLAGS) $(LFLAGS)

twinkle: $(OBJS) main.cpp
	g++ -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test_twinkle: $(OBJS) test.cpp	
	g++ -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test: test_twinkle
	./test_twinkle

clean:
	rm -rf $(EXES) obj

check-syntax:
	$(CXX) $(SFLAGS) $(CHK_SOURCES)
