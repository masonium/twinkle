include header.mk

# Used for debugging makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning Building $@$(if $<, (from $<))%(if $?, ($? newer)))$(OLD_SHELL)

ifndef CONFIG
CONFIG=Debug
endif

SRCS = $(wildcard lib/*.cpp)
OBJSTMP = $(SRCS:.cpp=.o)
OBJS = $(OBJSTMP:lib/%=obj/%)
DEPS = $(OBJS:.o=.d)

EXES = twinkle test_twinkle fresnel_test tonemap

CXX = g++

.PHONY: test

all: $(EXES)

-include $(DEPS)

obj/%.o: lib/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LFLAGS)
	$(CXX) -MM $(CXXFLAGS) $(LFLAGS) $< > obj/$*.d
	@mv -f obj/$*.d obj/$*.d.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.d.tmp > obj/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.d
	@rm -f obj/$*.d.tmp

twinkle: $(OBJS) main.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

tonemap: $(OBJS) tonemap_main.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test_twinkle: $(OBJS) test.cpp	
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

fresnel_test: $(OBJS) fresnel_test.cpp	
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test: test_twinkle
	./test_twinkle

clean:
	rm -rf $(EXES) $(OBJS) $(DEPS)

check-syntax:
	$(CXX) $(SFLAGS) $(CHK_SOURCES)
