#ifndef _MATOP_H_
#define _MATOP_H_

#include <cstddef>
#include <cstring>
#include <fmt/core.h>
//rtw = reinventing the wheel lmao
#include <vector>
namespace rtw {

class Matrix {
public:
    Matrix();
    Matrix(float* data, int width, int height);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other);
    ~Matrix();

    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other);

    constexpr int height() const {return m_height;} 
    constexpr int width() const {return m_width;}
    constexpr int size() const {return m_width * m_height;}

    constexpr float& operator()(int row, int col) {return m_data[row * m_width + col];}
    constexpr float operator()(int row, int col) const {return m_data[row * m_width + col];}

    constexpr float& operator[](int index) {return m_data[index];}
    constexpr float operator[](int index) const {return m_data[index];}
    void print();
private:
    float* m_data;
    int m_width;
    int m_height;
};

void transpose(Matrix& mat);

//Overloads for "*"

Matrix operator*(const Matrix& mat1, const Matrix& mat2);
Matrix operator*(Matrix&& mat1, const Matrix& mat2);
Matrix operator*(Matrix&& mat1, Matrix&& mat2);
Matrix operator*(const Matrix& mat1, Matrix&& mat2);
Matrix operator*(const Matrix& mat, float alpha);
Matrix operator*(Matrix&& mat, float alpha);
Matrix operator*(float alpha, const Matrix& mat);
Matrix operator*(float alpha, Matrix&& mat);

///Hadamart "*"

Matrix hadamart(const Matrix& mat1, const Matrix& mat2);
Matrix hadamart(Matrix&& mat1, const Matrix& mat2);
Matrix hadamart(Matrix&& mat1, Matrix&& mat2);
Matrix hadamart(const Matrix& mat1, Matrix&& mat2);

///Smash 2 matrices together

Matrix concat(const Matrix& mat1, const Matrix& mat2);
Matrix concat(Matrix&& mat1, const Matrix& mat2);
Matrix concat(Matrix&& mat1, Matrix&& mat2);
Matrix concat(const Matrix& mat1, Matrix&& mat2);

///Overloads for "+"

Matrix operator+(const Matrix& mat1, const Matrix& mat2);
Matrix operator+(Matrix&& mat1, const Matrix& mat2);
Matrix operator+(const Matrix& mat1, Matrix&& mat2);

}


#endif