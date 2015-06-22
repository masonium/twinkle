CXX_VERSION = -std=c++1y
SFLAGS = -Wall -fsyntax-only $(CXX_VERSION)
LFLAGS =  -pthread -lm  -lUnitTest++
CXXFLAGS = -Wall $(CXX_VERSION) -g -Isrc/ -Itests/ -Iextlib/
#CXXFLAGS = -Wall $(CXX_VERSION) -O3 -Isrc/ -Itests/ -Iextlib/
