# 316443902 Nir Koren

CXX = g++
CXXFLAGS = -Wall -O2

EXEC = pagerank

SOURCES = parallel_pagerank.cpp main.c graph.c threadpool.c

OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)

HEADERS = parallel_pagerank.h graph.h threadpool.h

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)

.PHONY: all clean


