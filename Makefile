# Makefile for Bignum project

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Source files
SRCS = Bignum.cpp 

# Executable name
EXEC = bignum_exe


all: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(EXEC)

# Clean command
clean:
	rm -f $(EXEC)
