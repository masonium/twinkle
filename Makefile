CXX = g++
CXX_VERSION = -std=c++1y
COMMON_FLAGS = -Wall -Wextra -Wno-unused-parameter $(CXX_VERSION)
SFLAGS =  -fsyntax-only $(COMMON_FLAGS)
LFLAGS = -pthread -lm  -lUnitTest++ #-lc++
CXXFLAGS = $(COMMON_FLAGS) -g -Isrc/ -Itests/ -Iextlib/

# Used for debugging makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning Building $@$(if $<, (from $<))%(if $?, ($? newer)))$(OLD_SHELL)

ifndef CONFIG
CONFIG=Debug
endif

SRCS := $(wildcard src/*.cpp)
-include tests/makefile
-include src/shapes/Makefile

OBJSTMP := $(SRCS:.cpp=.o)
OBJS := $(OBJSTMP:src/%=obj/%)
OBJS := $(OBJS:tests/%=obj/%)

DEPS := $(OBJS:.o=.dep)

EXE_NAMES = twinkle test_twinkle fresnel_test tonemap model_check
EXES = $(addprefix bin/,$(EXE_NAMES))

.PHONY: test clean

all: $(EXES)

-include $(DEPS)

obj/%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	$(CXX) -MM $(CXXFLAGS) $< > obj/$*.dep
	@mv -f obj/$*.dep obj/$*.dep.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.dep.tmp > obj/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.dep
	@rm -f obj/$*.dep.tmp

obj/%.o: src/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	$(CXX) -MM $(CXXFLAGS) $< > obj/$*.dep
	@mv -f obj/$*.dep obj/$*.dep.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.dep.tmp > obj/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.dep
	@rm -f obj/$*.dep.tmp

obj/%.o: tests/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	$(CXX) -MM $(CXXFLAGS) $< > obj/$*.dep
	@mv -f obj/$*.dep obj/$*.dep.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.dep.tmp > obj/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.dep
	@rm -f obj/$*.dep.tmp

bin/twinkle: $(OBJS) obj/main.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

bin/tonemap: $(OBJS) obj/tonemap_main.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

bin/test_twinkle: $(OBJS) $(TESTOBJS) obj/test.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

bin/fresnel_test: $(OBJS) obj/fresnel_test.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

bin/model_check: $(OBJS) obj/model_check.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test: bin/test_twinkle
	bin/test_twinkle

clean:
	rm -rf obj/*.dep obj/*.o obj/shapes/*.o obj/shapes/*.dep bin/*

check-syntax:
	$(CXX) $(SFLAGS) $(CHK_SOURCES)
