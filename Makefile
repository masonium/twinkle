# Used for debugging makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning Building $@$(if $<, (from $<))%(if $?, ($? newer)))$(OLD_SHELL)

.PHONY: test clean check-syntax debug release

ifndef CONFIG
CONFIG = Release
endif

BINDIR = $(CONFIG)/bin
OBJDIR = $(CONFIG)/obj
LIBDIR = $(CONFIG)/lib

CXX = g++
CXX_VERSION = -std=c++1y
COMMON_FLAGS = -Wall -Wextra -Wno-unused-parameter $(CXX_VERSION)
CXXFLAGS := $(COMMON_FLAGS) -Isrc/ -Iextlib/
SFLAGS =  -fsyntax-only $(CXXFLAGS)
LFLAGS = -pthread -lm  -lUnitTest++ -L$(LIBDIR) -ltwinkle

ifeq (${CONFIG}, Debug)
CXXFLAGS += -g
else ifeq (${CONFIG}, Release)
CXXFLAGS += -O3
else
echo "CONFIG must be one of (Debug, Release)"
endif

SRCS := $(wildcard src/*.cpp)
-include src/shapes/Makefile
-include src/textures/Makefile
-include src/tests/Makefile

OBJSTMP := $(SRCS:.cpp=.o)
OBJS := $(OBJSTMP:src/%=$(OBJDIR)/%)

EXE_NAMES = twinkle test_twinkle tonemap model_check texture_check
EXES = $(addprefix $(BINDIR)/,$(EXE_NAMES))

STATIC_LIBS = $(LIBDIR)/libtwinkle.a

all: $(EXES)

release:
	make CONFIG=Release all ${OPTIONS}

debug:
	make CONFIG=Debug all ${OPTIONS}
cleandebug debugclean:
	make CONFIG=Debug clean ${OPTIONS}

DEPS := $(OBJS:.o=.dep)
-include $(DEPS)

VPATH = .:src:src/shapes:src/textures:src/tests

$(LIBDIR)/libtwinkle.a: $(OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	$(CXX) -MM $(CXXFLAGS) $< > $(OBJDIR)/$*.dep
	@mv -f $(OBJDIR)/$*.dep $(OBJDIR)/$*.dep.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.dep.tmp > $(OBJDIR)/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.dep
	@rm -f $(OBJDIR)/$*.dep.tmp

$(BINDIR)/twinkle: $(OBJDIR)/main.o $(LIBDIR)/libtwinkle.a
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/tonemap: $(OBJDIR)/tonemap_main.o $(LIBDIR)/libtwinkle.a
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/test_twinkle: $(OBJDIR)/test.o $(LIBDIR)/libtwinkle.a
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/model_check: $(OBJDIR)/model_check.o $(LIBDIR)/libtwinkle.a
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/texture_check: $(OBJDIR)/texture_check.o $(LIBDIR)/libtwinkle.a
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(LFLAGS)

test: $(BINDIR)/test_twinkle
	$(BINDIR)/test_twinkle

clean:
	rm -rf $(CONFIG)
