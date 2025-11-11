#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <stdexcept>

class Matrix {
private:
    int N;        // dimension (N x N)
    int* data;    // 1D array storing elements row-major
    int idx(int r, int c) const { return r * N + c; } //helper which gives the array index number given position

public:
    Matrix() : N(0), data(nullptr) {} //constructor with no size, empty

    Matrix(int n) : N(n) { //Matrix constructor, fail if negative
        if (n < 0) throw std::invalid_argument("Matrix size cannot be negative");
        data = (N > 0) ? new int[N * N]() : nullptr; //Allocate the memory with N*N 0's as 1d array
    }

    Matrix(const Matrix& other) : N(other.N) { //Constructor to make a copy of another matrix
        if (N > 0) {
            data = new int[N * N]; //allocate
            for (int i = 0; i < N * N; ++i) data[i] = other.data[i]; //copy values
        } else {
            data = nullptr; //if negative or 0 nullptr
        }
    }

    // assignment operator
    Matrix& operator=(const Matrix& other) { //Assigns an already existing matrix to values of different matrix
        if (this != &other) { //if they are different
            delete[] data; //delete the data
            N = other.N;
            if (N > 0) { //replace the data
                data = new int[N * N];
                for (int i = 0; i < N * N; ++i) data[i] = other.data[i];
            } else {
                data = nullptr;
            }
        }
        return *this;
    }

    ~Matrix() { delete[] data; } //destructor, tells program how to delete a matrix object, which is just deleting the data

    int size() const { return N; } //ask for size return N

    //this is very important as it overloads the object calling to allow the 1d array to be accessed like a 2d array
    int& operator()(int r, int c) { //when you call Matrix(row, col) either say out of range or return the data since it's 1d array
        if (r < 0 || r >= N || c < 0 || c >= N) throw std::out_of_range("Index out of range");
        return data[idx(r, c)];
    }
    int operator()(int r, int c) const { //allows previous function to work on const matrices as well
        if (r < 0 || r >= N || c < 0 || c >= N) throw std::out_of_range("Index out of range");
        return data[idx(r, c)];
    }

    Matrix operator+(const Matrix& other) const { //addition overload. If adding check for size equality then add the 1d array values
        if (N != other.N) throw std::invalid_argument("Matrix sizes must match for addition");
        Matrix result(N);
        for (int i = 0; i < N * N; ++i) //add each value to counterpart in other matrix
            result.data[i] = data[i] + other.data[i];
        return result;
    }

    Matrix operator*(const Matrix& other) const { //multiplication overload
        if (N != other.N) throw std::invalid_argument("Matrix sizes must match for multiplication"); //since all matrices NxN only need to check if equal
        Matrix result(N);
        for (int i = 0; i < N; ++i) //iterate over rows, cols then values
            for (int j = 0; j < N; ++j) {
                int sum = 0; //set sum to 0 after each loop of individual elements
                for (int k = 0; k < N; ++k)
                    sum += (*this)(i, k) * other(k, j); //multiply the values, add to sum
                result(i, j) = sum; //result matrix data for current row,column is sum
            }
        return result; //return Matrix
    }


    void print(const std::string& label = "") const { //print object overload
        if (!label.empty()) std::cout << label << " (" << N << "x" << N << "):\n"; //say the size
        if (N == 0) { //if size is 0 matrix is empty
            std::cout << "(empty)\n";
            return;
        }
        int width = 0;
        for (int i = 0; i < N * N; ++i) {
            int len = std::to_string(data[i]).size();
            if (len > width) width = len; //make the spaces in matrix width as big as the most digit number is long
        }
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c)
                std::cout << std::setw(width) << (*this)(r, c) << (c == N - 1 ? '\n' : ' '); //for each element print it with the width, if on last column print newline else space
        }
    }

    // compute main and secondary diagonal sums
    void diagonalSums(long long& mainSum, long long& secSum) const { //in lab9 I had to add the matrix odd checker, had to add it again in this
    mainSum = 0;
    secSum = 0;
    for (int i = 0; i < N; ++i) { //iterate up to N
        mainSum += (*this)(i, i); //mainSum is top left to bottom right
        secSum += (*this)(i, N - 1 - i); //secSum is top right to bottom left
    }
    if (N % 2 == 1) { //if the matrix is odd
        int mid = N / 2; 
        secSum -= (*this)(mid, mid); //subtract the center number to not count it twice
    }
}
};

// read two matrices from file and set them to A and B
bool readMatrices(const std::string& filename, Matrix& A, Matrix& B) { 
    std::ifstream fin(filename.c_str()); //tries to open file
    if (!fin) { //failure
        std::cerr << "Could not open file.\n";
        return false;
    }

    int N; 
    if (!(fin >> N) || N <= 0) { //if cant find token or N is negative invalid size
        std::cerr << "Invalid size.\n";
        return false;
    }

    A = Matrix(N);
    B = Matrix(N);
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            if (!(fin >> A(r, c))) return false; //Assign each datapoint for A, if fail return False

    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            if (!(fin >> B(r, c))) return false; //Assign each datapoint for A, if fail return False

    return true; //Return true if N, datapoints get assigned
    //This function does not close the file but after it is doesn running fin leaves scope and closes on destructor
}

// I debated whether to move these 3 functions into the object as members but I ended up not doing it since
// They are not designed to actually modify the matrix. swapRows was specified in instructions to not modify 
// so cols was assumed to not modify. Update currently does not modify, if it did I would've moved it inside

void swapRows(Matrix m, int r1 = 0, int r2 = 1) { //swap rows, by default row 0 and 1
    int N = m.size();
    if (r1 < 0 || r1 >= N || r2 < 0 || r2 >= N) { //if try to swap outside range fail
        std::cout << "Invalid row indices.\n";
        return;
    }
    for (int c = 0; c < N; ++c) { //use a temp variable to swap the values 1 at a time
        int temp = m(r1, c);
        m(r1, c) = m(r2, c);
        m(r2, c) = temp;
    }
    m.print("Matrix after row swap"); //print the result matrix
}


void swapCols(Matrix m, int c1 = 0, int c2 = 1) { //swap columns, by default 0 and 1
    int N = m.size();
    if (c1 < 0 || c1 >= N || c2 < 0 || c2 >= N) { //everything is the same as swap rows except what gets swapped
        std::cout << "Invalid column indices.\n";
        return;
    }
    for (int r = 0; r < N; ++r) {
        int temp = m(r, c1);
        m(r, c1) = m(r, c2);
        m(r, c2) = temp;
    }
    m.print("Matrix after column swap");
}

// update a single element (by value)
void updateElement(Matrix m, int r = 0, int c = 0, int val = 100) { //update an element, by default the top left element changes to 100
    int N = m.size();
    if (r < 0 || r >= N || c < 0 || c >= N) { //if out of range fail
        std::cout << "Invalid indices.\n";
        return;
    }
    m(r, c) = val; //set position of element to be changed to val passed in or 100
    m.print("Matrix after update"); //print the result
}

int main() {
    std::cout << "Enter input filename: "; //get filename
    std::string filename;
    std::cin >> filename; //declare filename then take in what user gave

    Matrix A, B; 
    if (!readMatrices(filename, A, B)) { //if readMatrices fail exit with 1 for fail
        std::cerr << "Error reading file.\n";
        return 1;
    }

    std::cout << "\nMatrix A:\n"; //print received matrices
    A.print();
    std::cout << "\nMatrix B:\n";
    B.print();

    // A Series of test cases to test the functions

    std::cout << "\nA + B:\n"; //test addition
    (A + B).print();

    std::cout << "\nA * B:\n"; //test multiplication
    (A * B).print();

    long long mainSum, secSum; // 64-bit ints instead of 32 for some reason, will probably never be needed
    A.diagonalSums(mainSum, secSum);
    std::cout << "\nMain diagonal sum: " << mainSum
              << "\nSecondary diagonal sum (minus repeat center): " << secSum
              << "\nTotal X sum: " << (mainSum + secSum) << "\n";

    std::cout << "\nSwapping rows:\n"; //test the default cases for swap row, col, element
    swapRows(A);

    std::cout << "\nSwapping columns:\n";
    swapCols(A);

    std::cout << "\nUpdating element:\n";
    updateElement(A);

    std::cout << "\nDone.\n"; //program done
    return 0; //success exit
}
