#include "matrix.h"

template<typename T>
Matrix<T>::Matrix(int rows, int cols)
    :rows(rows), cols(cols)
{
    data = new T[rows*cols]();
}

template<typename T>
Matrix<T>::Matrix(int rows, int cols, const T& fill)
    :rows(rows), cols(cols)
{
    data = new T[rows*cols] ;
    for(int i = 0; i < rows*cols; i++)
        data[i] = fill;
}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& other)
    :Matrix<T>::Matrix(other.rows, other.cols)
{
    for(int i = 0; i < rows*cols; i++)
            data[i] = other.data[i];
}

template<typename T>
Matrix<T>::~Matrix()
{
    delete [] data;
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other)
{
    if(this == &other) return *this;
    rows = other.rows; cols = other.cols;
    T* old = data;
    data = new T[rows*cols];
    delete [] old;
    for(int i = 0; i < rows*cols; i++) data[i] = other.data[i];
    return *this;
}

template<typename T>
T& Matrix<T>::operator()(int r, int c)
{
    return data[r*cols+c];
}

template<typename T>
const T& Matrix<T>::operator()(int r, int c) const
{
    return data[r*cols+c];
}

template<typename T>
T* Matrix<T>::operator[](int r)
{
    if(r<0 || r >= rows) return nullptr;
    return data + r*cols;
}

template<typename T>
const T* Matrix<T>::operator[](int r) const
{
    if(r<0 || r >= rows) return nullptr;
    return data+r*cols;
}

template<typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& other)
{
    for(int i = 0; i < rows*cols; i++) data[i] += other.data[i];
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const T& scalar)
{
    for(int i = 0; i < rows*cols; i++) data[i] *= scalar;
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator-() const
{
    Matrix m(rows, cols);
    for(int i = 0; i < rows*cols; i++) m.data[i] = -data[i];
    return m;
}

template<typename T>
Matrix<T> operator+(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    Matrix m(lhs);
    m += rhs;
    return m;
}

template<typename T>
Matrix<T> operator*(const Matrix<T>& matrix, const T& scalar)
{
    Matrix m(matrix);
    m *= scalar;
    return m;
}

template<typename T>
Matrix<T> operator*(const T& scalar, const Matrix<T>& matrix)
{
    Matrix m(matrix);
    m *= scalar;
    return m;
}


template <typename T>
bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    if (lhs.get_num_row() != rhs.get_num_row() ||
        lhs.get_num_col() != rhs.get_num_col())
        return false;
    
    for (int r = 0; r < lhs.get_num_row(); ++r)
        for (int c = 0; c < lhs.get_num_col(); ++c)
            if (!(lhs(r, c) == rhs(r, c)))
                return false;
    return true;
}

template <typename T>
bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs)
{
    return !(lhs==rhs);
}

template <>
bool operator==(const Matrix<float>& lhs, const Matrix<float>& rhs)
{
    if (lhs.get_num_row() != rhs.get_num_row() ||
        lhs.get_num_col() != rhs.get_num_col())
        return false;
    
    for (int r = 0; r < lhs.get_num_row(); ++r)
        for (int c = 0; c < lhs.get_num_col(); ++c)
            if (std::fabs(lhs(r, c) - rhs(r, c)) > epsilon)
                return false;
    return true;
}

template <>
bool operator==(const Matrix<double>& lhs, const Matrix<double>& rhs)
{

    if (lhs.get_num_row() != rhs.get_num_row() ||
        lhs.get_num_col() != rhs.get_num_col())
        return false;
    
    for (int r = 0; r < lhs.get_num_row(); ++r)
        for (int c = 0; c < lhs.get_num_col(); ++c)
            if (std::fabs(lhs(r, c) - rhs(r, c)) > epsilon)
                return false;
    return true;
}


template <>
bool operator!=(const Matrix<float>& lhs, const Matrix<float>& rhs)
{
    return !(lhs==rhs);
}

template <>
bool operator!=(const Matrix<double>& lhs, const Matrix<double>& rhs)
{
    return !(lhs==rhs);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix)
{
    for (int r = 0; r < matrix.get_num_row(); ++r)
    {
        for (int c = 0; c < matrix.get_num_col(); ++c)
        {
            os << matrix(r,c);
            if(c != matrix.get_num_col()-1) os << " ";
        }
        if(r != matrix.get_num_row()-1) os << std::endl;
    }
    return os;
}