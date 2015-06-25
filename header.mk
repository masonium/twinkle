CXX_VERSION = -std=c++1y
COMMON_FLAGS = -Wall -Wextra -Wno-unused-parameter $(CXX_VERSION)
SFLAGS =  -fsyntax-only $(COMMON_FLAGS)
LFLAGS = -pthread -lm  -lUnitTest++
CXXFLAGS = $(COMMON_FLAGS) -g -Isrc/ -Itests/ -Iextlib/
#CXXFLAGS = -Wall $(CXX_VERSION) -O3 -Isrc/ -Itests/ -Iextlib/
