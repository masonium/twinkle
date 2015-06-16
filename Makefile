include header.mk

# Used for debugging makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning Building $@$(if $<, (from $<))%(if $?, ($? newer)))$(OLD_SHELL)

ifndef CONFIG
CONFIG=Debug
endif

SRCS = $(wildcard src/*.cpp)
OBJSTMP = $(SRCS:.cpp=.o)
OBJS = $(OBJSTMP:src/%=obj/%)
DEPS = $(OBJS:.o=.dep)

EXES = twinkle test_twinkle fresnel_test tonemap model_check

CXX = g++

.PHONY: test

all: $(EXES)

-include $(DEPS)

obj/%.o: src/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LFLAGS)
	$(CXX) -MM $(CXXFLAGS) $(LFLAGS) $< > obj/$*.dep
	@mv -f obj/$*.dep obj/$*.dep.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.dep.tmp > obj/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.dep
	@rm -f obj/$*.dep.tmp

twinkle: $(OBJS) main.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

tonemap: $(OBJS) tonemap_main.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test_twinkle: $(OBJS) test.cpp	
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

fresnel_test: $(OBJS) fresnel_test.cpp	
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

model_check: $(OBJS) model_check.cpp	
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test: test_twinkle
	./test_twinkle

clean:
	rm -rf $(EXES) $(OBJS) $(DEPS)

check-syntax:
	$(CXX) $(SFLAGS) $(CHK_SOURCES)
