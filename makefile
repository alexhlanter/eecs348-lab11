CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: matrix_operations

calculator: matrix_operations.cpp
	$(CXX) $(CXXFLAGS) matrix_operations.cpp -o matrix_operations

clean:
	rm -f matrix_operations
