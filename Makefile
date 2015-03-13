#  Assembler Makefile
#  Author: Sai Koppula
#  Created: January 2014
#  Updated: January 2014

#Name of Executable
EXES = bfs

# Flags to use when compiling, preprocessing, assembling, and linking
CXX = g++ -std=c++11 -pthread
WARNINGS = -Wchar-subscripts -Wparentheses -Wreturn-type -Wmissing-braces -Wundef -Wshadow
CXX_OPTS = -c -g -O0 -Wfatal-errors -Werror $(WARNINGS)

# This generates the list of source files
SRC =  $(wildcard *.cpp)

#This generates the corresponding list of .o files
OBJS=$(SRC:.cpp =.o)

#Actual Compile Instructions
all: $(SRC) $(EXES)

$(EXES): $(OBJS)
	$(CXX) $(OBJS) -o $@

.cpp.o:
	$(CXX) $(CXX_OPTS) $< -o $@

#Other useful options
.PHONY: all clean

all: $(EXES)

clean:
	-rm *.o $(EXES) 2>/dev/null
