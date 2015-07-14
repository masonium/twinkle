ifndef CONFIG
CONFIG = Debug
endif

CXX = g++
CXX_VERSION = -std=c++1y
COMMON_FLAGS = -Wall -Wextra -Wno-unused-parameter $(CXX_VERSION)
SFLAGS =  -fsyntax-only $(COMMON_FLAGS)
LFLAGS = -pthread -lm  -lUnitTest++ `pkg-config guile-2.0 --libs`
CXXFLAGS := $(COMMON_FLAGS) -Isrc/ -Itests/ -Iextlib/ `pkg-config guile-2.0 --cflags`

ifeq (${CONFIG}, Debug)
CXXFLAGS += -g
else ifeq (${CONFIG}, Release)
CXXFLAGS += -O3
else
echo "CONFIG must be one of (Debug, Release)"
endif

BINDIR = $(CONFIG)/bin
OBJDIR = $(CONFIG)/obj
LIBDIR = $(CONFIG)/lib

# Used for debugging makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning Building $@$(if $<, (from $<))%(if $?, ($? newer)))$(OLD_SHELL)

SRC :=

SRCS := $(wildcard src/*.cpp)
-include tests/Makefile
-include src/shapes/Makefile

OBJSTMP := $(SRCS:.cpp=.o)
OBJSTMP := $(OBJSTMP:src/%=$(OBJDIR)/%)
OBJS := $(OBJSTMP:tests/%=$(OBJDIR)/%)

GUILE_SRCS = $(wildcard src/guile/*.cpp)
GUILE_OBJS = $(GUILE_SRCS:.cpp=.o)

DEPS := $(OBJS:.o=.dep)

EXE_NAMES = twinkle test_twinkle fresnel_test tonemap model_check texture_check
EXES = $(addprefix $(BINDIR)/,$(EXE_NAMES))

.PHONY: test clean

SHARED_LIBS = $(LIBDIR)/libtg.so

all: $(EXES) $(SHARED_LIBS)

-include $(DEPS)

VPATH = .:src:src/shapes:tests

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	$(CXX) -MM $(CXXFLAGS) $< > $(OBJDIR)/$*.dep
	@mv -f $(OBJDIR)/$*.dep $(OBJDIR)/$*.dep.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.dep.tmp > $(OBJDIR)/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.dep
	@rm -f $(OBJDIR)/$*.dep.tmp

$(LIBDIR)/libtg.so: $(GUILE_SRCS)
	@mkdir -p $(dir $@)
	$(CXX) -shared -o $@ $^ $(CXXFLAGS) -fPIC

$(BINDIR)/twinkle: $(OBJS) $(OBJDIR)/main.o
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/tonemap: $(OBJS) $(OBJDIR)/tonemap_main.o
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/test_twinkle: $(OBJS) $(TESTOBJS) $(OBJDIR)/test.o
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/fresnel_test: $(OBJS) $(OBJDIR)/fresnel_test.o
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/model_check: $(OBJS) $(OBJDIR)/model_check.o
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/texture_check: $(OBJS) $(OBJDIR)/texture_check.o
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

test: $(BINDIR)/test_twinkle
	$(BINDIR)/test_twinkle

clean:
	rm -rf $(CONFIG)

check-syntax:
	$(CXX) $(SFLAGS) $(CHK_SOURCES)
