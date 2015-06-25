include header.mk

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

EXES = twinkle test_twinkle fresnel_test tonemap model_check

CXX = g++

.PHONY: test clean

all: $(EXES)

-include $(DEPS)

obj/%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LFLAGS)
	$(CXX) -MM $(CXXFLAGS) $(LFLAGS) $< > obj/$*.dep
	@mv -f obj/$*.dep obj/$*.dep.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.dep.tmp > obj/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.dep
	@rm -f obj/$*.dep.tmp

obj/%.o: src/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LFLAGS)
	$(CXX) -MM $(CXXFLAGS) $(LFLAGS) $< > obj/$*.dep
	@mv -f obj/$*.dep obj/$*.dep.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.dep.tmp > obj/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.dep
	@rm -f obj/$*.dep.tmp

obj/%.o: tests/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LFLAGS)
	$(CXX) -MM $(CXXFLAGS) $(LFLAGS) $< > obj/$*.dep
	@mv -f obj/$*.dep obj/$*.dep.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.dep.tmp > obj/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.dep
	@rm -f obj/$*.dep.tmp

twinkle: $(OBJS) obj/main.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

tonemap: $(OBJS) obj/tonemap_main.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test_twinkle: $(OBJS) $(TESTOBJS) obj/test.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

fresnel_test: $(OBJS) obj/fresnel_test.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

model_check: $(OBJS) obj/model_check.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test: test_twinkle
	./test_twinkle

clean:
	rm -rf obj/*.dep obj/*.o obj/shapes/*.o obj/shapes/*.dep

check-syntax:
	$(CXX) $(SFLAGS) $(CHK_SOURCES)
