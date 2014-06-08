#pragma once

#include <initializer_list>
#include <algorithm>
#include <tuple>
#include <memory>
#include <iostream>
#include <string>
#include <type_traits>

typedef unsigned int uint;

template<typename ValueT>
class Matrix
{
public:
    // Number of rows
    const uint n_rows;
    // Number of cols
    const uint n_cols;

    // Construct matrix with row_count of rows and col_count of columns
    Matrix(uint row_count=0, uint col_count=0);

    // Construct and initialize matrix which consists of one row.
    //
    // Example:
    // Matrix<int> binomial = {1, 4, 6, 4, 1};
    Matrix(std::initializer_list<ValueT>);

    // Construct and initialize matrix with 2d array
    //
    // Example:
    // Matrix<double> growing = { {0.1, 0.2, 0.3, 0.4},
    //                            {0.5, 0.6, 0.7, 0.8} };
    Matrix(std::initializer_list<std::initializer_list<ValueT>>);

    // Shallow copy. Be careful, this function just copies the pointer
    // to data and doesn't allocate any memory for data!
    Matrix(const Matrix&);
    // Deep copy. Allocates memory and copies all values
    Matrix<ValueT> deep_copy() const;

    // Assignment operator
    const Matrix<ValueT> &operator = (const Matrix<ValueT> &);

    // Move copy constructor. Needed when copy temporary object.
    // It is from c++ 11 standard.
    Matrix(Matrix&&);

    // Desctructor, yeah.
    ~Matrix();

    // Matrix indexing. You give row and col, it returns element.
    // Be careful, indexing starts at 0!
    //
    // Const for just taking value:
    // Matrix<int> a = {9, 8, 7};
    // int i = a(0, 2); // i = 7;
    const ValueT &operator() (uint row, uint col) const;
    // Non-const for assignment
    // a(0, 1) = 3;
    // cout << a; // 9 3 7
    ValueT &operator() (uint row, uint col);

    // Matrix convolution.
    //
    // You give this function a unary operator. Operator _must_
    // have radius field and function
    // operator()(const Matrix<ValueT> neighbourhood).
    // For every pixel of that matrix this function takes
    // neighbourhood of that pixel of size
    // (2 * radius + 1) x (2 * radius + 1), applies operator to that
    // neighbourhood and writes result in a new matrix of the same size
    // Minimum radius is 0, operator will process only one pixel every time
    template<typename UnaryMatrixOperator>
    // Function unary map returns a matrix of
    Matrix<
    // type which is returned
        typename std::result_of<
    // by operator applied to neighbourhood of pixel
            UnaryMatrixOperator(Matrix<ValueT>)
        >::type
    >
    unary_map(const UnaryMatrixOperator &op) const;

    // Same, but unary operator is mutable.
    // If you take operator with mutable fields, you can
    // make statistic computations using unary map
    // (statistics like sum of pixel values or histograms of pixel values)
    template<typename UnaryMatrixOperator>
    Matrix<typename std::result_of<UnaryMatrixOperator(Matrix<ValueT>)>::type>
    unary_map(UnaryMatrixOperator &op) const;

    // binary_map has the same idea as unary_map,
    // but now operator takes two neighbourhoods. For example,
    // if radius = 0, you can make operator, which make elementwise
    // product of two matrices.
    //
    // This function isn't implemented, if you understood unary_map, using its
    // code you can easily implement binary_map.
    template<typename BinaryMatrixOperator>
    friend Matrix<ValueT> binary_map(const BinaryMatrixOperator&,
                                     const Matrix<ValueT>&,
                                     const Matrix<ValueT>&);

    // Get sumbmatrix of matrix
    // Remember that indexing starts at 0!
    //
    // prow, pcol - row and column of point, where to start slicing
    // rows, cols - length of submatrix in rows and columns
    //
    // Matrix<int> a = { {1, 2, 3},
    //                   {4, 5, 6} };
    // cout << a.submatrix(1, 1, 1, 2); // 5 6
    const Matrix<ValueT> submatrix(uint prow, uint pcol,
                                   uint rows, uint cols) const;

private:
    // Stride - number of elements between two rows (needed for efficient
    // submatrix function without memory copy)
    const uint stride;
    // First row and col, useful for taking submatrices. By default is (0, 0).
    const uint pin_row, pin_col;
    // shared_ptr still has no support of c-style arrays and
    // <type[]> partial specialization like unique_ptr has.
    // works: unique_ptr<int[]>; doesn't: shared_ptr<int[]>.
    // so, for now we use shared_ptr just for counting links,
    // and work with raw pointer through get().
    std::shared_ptr<ValueT> _data;

    // Const cast for writing public const fields.
    template<typename T> inline T& make_rw(const T& val) const;
};

// Output for matrix. Useful for debugging
template<typename ValueT>
std::ostream &operator << (std::ostream &out, const Matrix<ValueT> &m)
{
    if (m.n_rows * m.n_cols == 0) {
        out << "empty matrix" << std::endl;
        return out;
    }
    for (uint i = 0; i < m.n_rows; ++i) {
        for (uint j = 0; j < m.n_cols; ++j) {
            out << m(i, j) << " ";
        }
        out << std::endl;
    }
    return out;
}

// Implementation of Matrix class
#include "matrix.hpp"
