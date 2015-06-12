CXX_VERSION = -std=c++11
SFLAGS = -Wall -fsyntax-only $(CXX_VERSION)
LFLAGS =  -pthread -lm  -lUnitTest++
CXXFLAGS = -Wall $(CXX_VERSION) -g -Og -Isrc/ -Iextlib/
