#include "matop.h"

namespace rtw {
Matrix::Matrix() :m_data(nullptr), m_width(0), m_height(0) {}

 Matrix::~Matrix() {
    delete[] m_data;
    m_width = m_height = 0;
    m_data = nullptr;
}

Matrix::Matrix(float* data, int width, int height) : m_width(width), m_height(height) {
    m_data = new float[m_width * m_height];
    memcpy(m_data, data, m_width * m_height * sizeof(float));
}

Matrix::Matrix(const Matrix& other) {
    fmt::print("copy\n");
    m_data = new float[other.size()];
    memcpy(m_data, other.m_data, other.size() * sizeof(float));
    m_width = other.m_width;
    m_height = other.m_height;
}

Matrix::Matrix(Matrix&& other) {
    fmt::print("move\n");
    m_data = std::exchange(other.m_data, nullptr);
    m_width = std::exchange(other.m_width, 0);
    m_height = std::exchange(other.m_height, 0); 
}

Matrix& Matrix::operator=(const Matrix& other) {
    fmt::print("copy\n");
    if(this == &other) {
        return *this;
    }

    if (this->size() != other.size()) {
        float* temp = new float[other.size()];
        delete[] m_data;
        m_data = temp;
        temp = nullptr;
    }

    memcpy(m_data, other.m_data, other.size() * sizeof(float));
    m_width = other.m_width;
    m_height = other.m_height;
    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) {
    fmt::print("move\n");
    if (this == &other) return *this;

    delete[] m_data;
    m_data = std::exchange(other.m_data, nullptr);
    m_width = std::exchange(other.m_width, 0);
    m_height = std::exchange(other.m_height, 0); 
    return *this;
}

void Matrix::print() {
    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            float num =  (this->operator()(i, j));
            fmt::print("{:f} ", num);
        }
        fmt::print("\n");
    }
}

Matrix operator*(const Matrix& mat1, const Matrix& mat2) {
    assert(mat1.width() == mat2.height() && "wrong input");
    float* a = new float[mat1.height() * mat2.width()];
    for (int i = 0; i < mat1.height(); ++i) {
        for (int j = 0; j < mat2.width(); ++j) {
            float sum = 0;
            for (int m = 0; m < mat1.width(); ++m) {
                sum += mat1(i, m)*mat2(m, j);
            }
            a[i * mat2.width() + j] = sum;
        }
    }
    Matrix ret(a, mat2.width(), mat1.height());
    delete[] a;
    return ret;
}

Matrix operator*(Matrix&& mat1, const Matrix& mat2) {
    assert(mat1.width() == mat2.height() && "wrong input");
    float* a = new float[mat1.height() * mat2.width()];
    for (int i = 0; i < mat1.height(); ++i) {
        for (int j = 0; j < mat2.width(); ++j) {
            float sum = 0;
            for (int m = 0; m < mat1.width(); ++m) {
                sum += mat1(i, m)*mat2(m, j);
            }
            a[i * mat2.width() + j] = sum;
        }
    }
    Matrix ret(a, mat2.width(), mat1.height());
    delete[] a;
    return ret;
}

Matrix operator*(Matrix&& mat1, Matrix&& mat2) {
    assert(mat1.width() == mat2.height() && "wrong input");
    float* a = new float[mat1.height() * mat2.width()];
    for (int i = 0; i < mat1.height(); ++i) {
        for (int j = 0; j < mat2.width(); ++j) {
            float sum = 0;
            for (int m = 0; m < mat1.width(); ++m) {
                sum += mat1(i, m)*mat2(m, j);
            }
            a[i * mat2.width() + j] = sum;
        }
    }
    Matrix ret(a, mat2.width(), mat1.height());
    delete[] a;
    return ret;
}

Matrix operator*(const Matrix& mat1, Matrix&& mat2) {
    assert(mat1.width() == mat2.height() && "wrong input");
    float* a = new float[mat1.height() * mat2.width()];
    for (int i = 0; i < mat1.height(); ++i) {
        for (int j = 0; j < mat2.width(); ++j) {
            float sum = 0;
            for (int m = 0; m < mat1.width(); ++m) {
                sum += mat1(i, m)*mat2(m, j);
            }
            a[i * mat2.width() + j] = sum;
        }
    }
    Matrix ret(a, mat2.width(), mat1.height());
    delete[] a;
    return ret;
}

Matrix operator*(const Matrix& mat, float alpha) {
    float* a = new float[mat.size()];
    for (int i = 0; i < mat.size(); ++i) {
        a[i] = alpha * mat[i];
    } 
    Matrix ret(a, mat.width(), mat.height());
    delete[] a;
    return ret;
}

Matrix operator*(Matrix&& mat, float alpha) {
    float* a = new float[mat.size()];
    for (int i = 0; i < mat.size(); ++i) {
        a[i] = alpha * mat[i];
    } 
    Matrix ret(a, mat.width(), mat.height());
    delete[] a;
    return ret;
}

Matrix operator*(float alpha, const Matrix& mat) {
    float* a = new float[mat.size()];
    for (int i = 0; i < mat.size(); ++i) {
        a[i] = alpha * mat[i];
    } 
    Matrix ret(a, mat.width(), mat.height());
    delete[] a;
    return ret;
}

Matrix operator*(float alpha, Matrix&& mat) {
    float* a = new float[mat.size()];
    for (int i = 0; i < mat.size(); ++i) {
        a[i] = alpha * mat[i];
    } 
    Matrix ret(a, mat.width(), mat.height());
    delete[] a;
    return ret;
}

//Hadamart *

Matrix hadamart(const Matrix& mat1, const Matrix& mat2) {
    assert(mat1.width() == mat2.width() 
        && mat1.height() == mat2.height() 
        && "wrong dimensions");
    float* a = new float[mat1.size()];
    for (int i = 0; i < mat1.size(); ++i) {
        a[i] = mat1[i] + mat2[i];
    }
    Matrix ret(a, mat1.width(), mat1.height());
    delete[] a;
    return ret;
}
Matrix hadamart(Matrix&& mat1, const Matrix& mat2) {
    assert(mat1.width() == mat2.width() 
        && mat1.height() == mat2.height() 
        && "wrong dimensions");
    float* a = new float[mat1.size()];
    for (int i = 0; i < mat1.size(); ++i) {
        a[i] = mat1[i] + mat2[i];
    }
    Matrix ret(a, mat1.width(), mat1.height());
    delete[] a;
    return ret;
}
Matrix hadamart(Matrix&& mat1, Matrix&& mat2) {
    assert(mat1.width() == mat2.width() 
        && mat1.height() == mat2.height() 
        && "wrong dimensions");
    float* a = new float[mat1.size()];
    for (int i = 0; i < mat1.size(); ++i) {
        a[i] = mat1[i] + mat2[i];
    }
    Matrix ret(a, mat1.width(), mat1.height());
    delete[] a;
    return ret;
}
Matrix hadamart(const Matrix& mat1, Matrix&& mat2) {
    assert(mat1.width() == mat2.width() 
        && mat1.height() == mat2.height() 
        && "wrong dimensions");
    float* a = new float[mat1.size()];
    for (int i = 0; i < mat1.size(); ++i) {
        a[i] = mat1[i] + mat2[i];
    }
    Matrix ret(a, mat1.width(), mat1.height());
    delete[] a;
    return ret;
}

///Overloads for "+"

Matrix operator+(const Matrix& mat1, const Matrix& mat2) {
    float* a = new float[mat1.size()];
    for (int i = 0; i < mat1.size(); ++i) {
        a[i] = mat1[i] + mat2[i];
    }
    Matrix ret(a, mat1.width(), mat2.height());
    delete[] a;
    return ret;
}

Matrix operator+(Matrix&& mat1, const Matrix& mat2) {
    float* a = new float[mat1.size()];
    for (int i = 0; i < mat1.size(); ++i) {
        a[i] = mat1[i] + mat2[i];
    }
    Matrix ret(a, mat1.width(), mat2.height());
    delete[] a;
    return ret;
}

Matrix operator+(const Matrix& mat1, Matrix&& mat2) {
    float* a = new float[mat1.size()];
    for (int i = 0; i < mat1.size(); ++i) {
        a[i] = mat1[i] + mat2[i];
    }
    Matrix ret(a, mat1.width(), mat2.height());
    delete[] a;
    return ret;
}

}