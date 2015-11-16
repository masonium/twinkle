# Used for debugging makefile
#OLD_SHELL := $(SHELL)
#SHELL = $(warning Building $@$(if $<, (from $<))%(if $?, ($? newer)))$(OLD_SHELL)

.PHONY: test clean check-syntax debug release

ifndef CONFIG
CONFIG = Release
endif

ifndef MULTITHREADED
MULTITHREADED = 1
endif

BINDIR = $(CONFIG)/bin
OBJDIR = $(CONFIG)/obj
LIBDIR = $(CONFIG)/lib

CXX = ccache g++
CXX_VERSION = -std=c++1y
COMMON_FLAGS = -Wall -Wextra -Wno-unused-parameter $(CXX_VERSION)
CXXFLAGS := $(COMMON_FLAGS) -Isrc/ -Iextlib/ -ggdb -I/usr/include/luajit-2.0 -fno-omit-frame-pointer
CXXFLAGS += -DFEATURE_MULTITHREADED=${MULTITHREADED}
NOTESTFLAGS := -fno-exceptions -fno-rtti
SFLAGS = -fsyntax-only $(CXXFLAGS)
LFLAGS := -lUnitTest++ -L$(LIBDIR) -ltwinkle -lcpp-optparse -lluajit-5.1

ifeq (${MULTITHREADED}, 1)
LFLAGS += -pthread -lm
endif

ifeq (${CONFIG}, Debug)
CXXFLAGS += -O0
else ifeq (${CONFIG}, Release)
CXXFLAGS += -O3 # -Ofast
else
echo "CONFIG must be one of (Debug, Release)"
endif

-include src/Makefile

TWINKLE_OBJSTMP := $(SRCS:.cpp=.o)
TWINKLE_OBJS := $(TWINKLE_OBJSTMP:src/%=$(OBJDIR)/%)

TEST_SRCS = $(wildcard src/tests/*.cpp)
TEST_OBJS := $(TEST_SRCS:.cpp=.o)
TEST_OBJS := $(TEST_OBJS:src/%=$(OBJDIR)/%)

CPPPARSE_OBJS := $(OBJDIR)/cpp-optparse/OptionParser.o

EXE_NAMES = twinkle test_twinkle tonemap model_check texture_check material_check
EXES = $(addprefix $(BINDIR)/,$(EXE_NAMES))

STATIC_LIBS = $(LIBDIR)/libtwinkle.a $(LIBDIR)/libcpp-optparse.a

all: $(EXES)

release:
	make CONFIG=Release all ${OPTIONS}

debug:
	make CONFIG=Debug all ${OPTIONS}
cleandebug debugclean:
	make CONFIG=Debug clean ${OPTIONS}

DEPS := $(TWINKLE_OBJS:.o=.dep)
-include $(DEPS)

VPATH = .:src:src/shapes:src/textures:src/tests:src/geometry:src/materials

$(LIBDIR)/libcpp-optparse.a: $(CPPPARSE_OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(LIBDIR)/libtwinkle.a: $(TWINKLE_OBJS)
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

$(OBJDIR)/%.o: extlib/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	$(CXX) -MM $(CXXFLAGS) $< > $(OBJDIR)/$*.dep
	@mv -f $(OBJDIR)/$*.dep $(OBJDIR)/$*.dep.tmp
	@sed -e 's|.*:|$(OBJDIR)/$*.o:|' < $(OBJDIR)/$*.dep.tmp > $(OBJDIR)/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJDIR)/$*.dep.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJDIR)/$*.dep
	@rm -f $(OBJDIR)/$*.dep.tmp

$(BINDIR)/twinkle: $(OBJDIR)/main.o $(STATIC_LIBS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(NOTESTFLAGS) $(LFLAGS)

$(BINDIR)/tonemap: $(OBJDIR)/tonemap_main.o $(STATIC_LIBS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(NOTESTFLAGS) $(LFLAGS)

$(BINDIR)/test_twinkle: $(OBJDIR)/test.o $(TEST_OBJS) $(STATIC_LIBS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LFLAGS)

$(BINDIR)/model_check: $(OBJDIR)/model_check.o $(STATIC_LIBS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(NOTESTFLAGS) $(LFLAGS)

$(BINDIR)/texture_check: $(OBJDIR)/texture_check.o $(STATIC_LIBS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(NOTESTFLAGS) $(LFLAGS)

$(BINDIR)/material_check: $(OBJDIR)/material_check.o $(STATIC_LIBS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< $(CXXFLAGS) $(NOTESTFLAGS) $(LFLAGS)

test: $(BINDIR)/test_twinkle
	$(BINDIR)/test_twinkle

clean:
	rm -rf $(CONFIG)
