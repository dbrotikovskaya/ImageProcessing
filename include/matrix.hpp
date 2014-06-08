template<typename ValueT>
template<typename T>
inline
T& Matrix<ValueT>::make_rw(const T& val) const
{
    return const_cast<T&>(val);
}

template<typename ValueT>
Matrix<ValueT>::Matrix(uint row_count, uint col_count):
    n_rows{row_count},
    n_cols{col_count},
    stride{n_cols},
    pin_row{0},
    pin_col{0},
    _data{}
{
    auto size = n_cols * n_rows;
    if (size)
        _data.reset(new ValueT[size], std::default_delete<ValueT[]>());
}

template<typename ValueT>
Matrix<ValueT>::Matrix(std::initializer_list<ValueT> lst):
    n_rows{1},
    n_cols(lst.size()), // FIXME: narrowing.
    stride{n_cols},
    pin_row{0},
    pin_col{0},
    _data{}
{
    if (n_cols) {
        _data.reset(new ValueT[n_cols], std::default_delete<ValueT[]>());
        std::copy(lst.begin(), lst.end(), _data.get());
    }
}

template<typename ValueT>
Matrix<ValueT> Matrix<ValueT>::deep_copy() const
{
    Matrix<ValueT> tmp(n_rows, n_cols);
    for (uint i = 0; i < n_rows; ++i)
        for (uint j = 0; j < n_cols; ++j)
            tmp(i, j) = (*this)(i, j);
    return tmp;
}

template<typename ValueT>
const Matrix<ValueT> &Matrix<ValueT>::operator = (const Matrix<ValueT> &m)
{
    make_rw(n_rows) = m.n_rows;
    make_rw(n_cols) = m.n_cols;
    make_rw(stride) = m.stride;
    make_rw(pin_row) = m.pin_row;
    make_rw(pin_col) = m.pin_col;
    _data = m._data;
    return *this;
}
template<typename ValueT>
Matrix<ValueT>::Matrix(std::initializer_list<std::initializer_list<ValueT>> lsts):
    n_rows(lsts.size()), // FIXME: narrowing.
    n_cols{0},
    stride{n_cols},
    pin_row{0},
    pin_col{0},
    _data{}
{
    // check if no action is needed.
    if (n_rows == 0)
        return;

    // initializing columns count using first row.
    make_rw(n_cols) = lsts.begin()->size();
    make_rw(stride) = n_cols;

    // lambda function to check sublist length.
    // local block to invalidate stack variables after it ends.
    {
        auto local_n_cols = n_cols;
        auto chk_length = [local_n_cols](const std::initializer_list<ValueT> &l) {
            return l.size() == local_n_cols;
        };
        // checking that all row sizes are equal.
        if (not std::all_of(lsts.begin(), lsts.end(), chk_length))
            throw std::string("Initialization rows must have equal length");
    }

    if (n_cols == 0)
        return;

    // allocating matrix memory.
    _data.reset(new ValueT[n_cols * n_rows], std::default_delete<ValueT[]>());

    // copying matrix data.
    {
        auto write_ptr = _data.get();
        auto ptr_delta = n_cols;
        auto copier = [&write_ptr, ptr_delta](const std::initializer_list<ValueT> &l) {
            std::copy(l.begin(), l.end(), write_ptr);
            write_ptr += ptr_delta;
        };
        for_each(lsts.begin(), lsts.end(), copier);
    }
}

template<typename ValueT>
Matrix<ValueT>::Matrix(const Matrix &src):
    n_rows{src.n_rows},
    n_cols{src.n_cols},
    stride{src.stride},
    pin_row{src.pin_row},
    pin_col{src.pin_col},
    _data{src._data}
{
}

template<typename ValueT>
Matrix<ValueT>::Matrix(Matrix &&src):
    n_rows{src.n_rows},
    n_cols{src.n_cols},
    stride{src.stride},
    pin_row{src.pin_row},
    pin_col{src.pin_col},
    _data{src._data}
{
    // resetting state of donor object.
    make_rw(src.n_rows) = 0;
    make_rw(src.n_cols) = 0;
    make_rw(src.stride) = 0;
    make_rw(src.pin_row) = 0;
    make_rw(src.pin_col) = 0;
    src._data.reset();
}


template<typename ValueT>
ValueT &Matrix<ValueT>::operator()(uint row, uint col)
{
    if (row >= n_rows or col >= n_cols)
        throw std::string("Out of bounds");
    row += pin_row;
    col += pin_col;
    return _data.get()[row * stride + col];
}

template<typename ValueT>
const ValueT &Matrix<ValueT>::operator()(uint row, uint col) const
{
    if (row >= n_rows or col >= n_cols)
        throw std::string("Out of bounds");
    row += pin_row;
    col += pin_col;
    return _data.get()[row * stride + col];
}

template<typename ValueT>
Matrix<ValueT>::~Matrix()
{ }

template<typename ValueT>
const Matrix<ValueT> Matrix<ValueT>::submatrix(uint prow, uint pcol,
                                               uint rows, uint cols) const
{
    if (prow + rows > n_rows or pcol + cols > n_cols)
        throw std::string("Out of bounds");
    // copying requested data to submatrix.
    Matrix<ValueT> tmp(*this);
    make_rw(tmp.n_rows) = rows;
    make_rw(tmp.n_cols) = cols;
    make_rw(tmp.pin_row) = pin_row + prow;
    make_rw(tmp.pin_col) = pin_col + pcol;
    return tmp;
}

template<typename ValueT>
template<typename UnaryMatrixOperator>
Matrix<typename std::result_of<UnaryMatrixOperator(Matrix<ValueT>)>::type>
Matrix<ValueT>::unary_map(const UnaryMatrixOperator &op) const
{
    // Let's typedef return type of function for ease of usage
    typedef typename std::result_of<UnaryMatrixOperator(Matrix<ValueT>)>::type ReturnT;
    if (n_cols * n_rows == 0)
        return Matrix<ReturnT>(0, 0);

    Matrix<ReturnT> tmp(n_rows, n_cols);

    const auto radius = op.radius;
    const auto size = 2 * radius + 1;

    const auto start_i = radius;
    const auto end_i = n_rows - radius;
    const auto start_j = radius;
    const auto end_j = n_cols - radius;


    for (uint i = start_i; i < end_i; ++i) {
        for (uint j = start_j; j < end_j; ++j) {
            auto neighbourhood = submatrix(i - radius, j - radius, size, size);
            tmp(i, j) = op(neighbourhood);
        }
    }
    return tmp;
}

template<typename ValueT>
template<typename UnaryMatrixOperator>
Matrix<typename std::result_of<UnaryMatrixOperator(Matrix<ValueT>)>::type>
Matrix<ValueT>::unary_map(UnaryMatrixOperator &op) const
{
    typedef typename std::result_of<UnaryMatrixOperator(Matrix<ValueT>)>::type ReturnT;
    if (n_cols * n_rows == 0)
        return Matrix<ReturnT>(0, 0);

    Matrix<ReturnT> tmp(n_rows, n_cols);

    const auto radius = op.radius;
    const auto size = 2 * radius + 1;

    const auto start_i = radius;
    const auto end_i = n_rows - radius;
    const auto start_j = radius;
    const auto end_j = n_cols - radius;


    for (uint i = start_i; i < end_i; ++i) {
        for (uint j = start_j; j < end_j; ++j) {
            auto neighbourhood = submatrix(i - radius, j - radius, size, size);
            tmp(i, j) = op(neighbourhood);
        }
    }
    return tmp;
}
