/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UTILS_MATRIX_EXPORT_MATRIX_H
#define UTILS_MATRIX_EXPORT_MATRIX_H

#include <math.h>
#include <memory>
#include <ostream>

template<class T = float>
class Matrix {
public:
    static inline constexpr int32_t rowcol = 4;
    static inline constexpr int32_t units = rowcol * rowcol;

    static Matrix<T> UnitMatrix()
    {
        Matrix<T> retval;
        for (int32_t i = 0; i < units; i++) {
            retval.ptr[i] = 0;
        }
        retval.ele(0x0, 0x0) = 0x1;
        retval.ele(0x1, 0x1) = 0x1;
        retval.ele(0x2, 0x2) = 0x1;
        retval.ele(0x3, 0x3) = 0x1;
        return retval;
    }

    static Matrix<T> TranslateMatrix(T x, T y, T z = 0)
    {
        Matrix<T> retval = UnitMatrix();
        retval.ele(0x0, 0x3) = x;
        retval.ele(0x1, 0x3) = y;
        retval.ele(0x2, 0x3) = z;
        return retval;
    }

    static Matrix<T> ScaleMatrix(T x, T y, T z = 1)
    {
        Matrix<T> retval = UnitMatrix();
        retval.ele(0x0, 0x0) *= x;
        retval.ele(0x1, 0x1) *= y;
        retval.ele(0x2, 0x2) *= z;
        return retval;
    }

    static Matrix<T> RotateMatrixZ(double angle)
    {
        Matrix<T> retval = UnitMatrix();
        double rad = angle * acos(-1) / 180.0;
        retval.ele(0x0, 0x0) = cos(rad);
        retval.ele(0x0, 0x1) = -sin(rad);
        retval.ele(0x1, 0x0) = sin(rad);
        retval.ele(0x1, 0x1) = cos(rad);
        return retval;
    }

    Matrix() = default;

    Matrix(const Matrix<T> &m)
    {
        for (int32_t i = 0; i < units; i++) {
            ptr[i] = m.ptr[i];
        }
    }

    Matrix(Matrix<T> &&m)
    {
        ptr = std::move(m.ptr);
    }

    Matrix<T> &operator =(const Matrix<T> &m)
    {
        for (int32_t i = 0; i < units; i++) {
            ptr[i] = m.ptr[i];
        }
        return *this;
    }

    Matrix<T> operator *(const Matrix<T> other)
    {
        Matrix<T> retval;
        for (int i = 0; i < rowcol; i++) {
            for (int j = 0; j < rowcol; j++) {
                retval.ele(i, j) = 0;
                for (int k = 0; k < rowcol; k++) {
                    retval.ele(i, j) += ele(i, k) * other.ele(k, j);
                }
            }
        }
        return retval;
    }

    operator T*() const
    {
        return ptr.get();
    }

    friend std::ostream &operator <<(std::ostream &os, const Matrix<T> &m)
    {
        os << std::endl;
        os << "Matrix {" << std::endl;
        os << "  [" << m.ele(0x0, 0x0) << "] ["
                    << m.ele(0x0, 0x1) << "] ["
                    << m.ele(0x0, 0x2) << "] ["
                    << m.ele(0x0, 0x3) << "]"
                    << std::endl;
        os << "  [" << m.ele(0x1, 0x0) << "] ["
                    << m.ele(0x1, 0x1) << "] ["
                    << m.ele(0x1, 0x2) << "] ["
                    << m.ele(0x1, 0x3) << "]"
                    << std::endl;
        os << "  [" << m.ele(0x2, 0x0) << "] ["
                    << m.ele(0x2, 0x1) << "] ["
                    << m.ele(0x2, 0x2) << "] ["
                    << m.ele(0x2, 0x3) << "]"
                    << std::endl;
        os << "  [" << m.ele(0x3, 0x0) << "] ["
                    << m.ele(0x3, 0x1) << "] ["
                    << m.ele(0x3, 0x2) << "] ["
                    << m.ele(0x3, 0x3) << "]"
                    << std::endl;
        os << "}" << std::endl;
        return os;
    }

private:
    inline T &ele(int i, int j) const
    {
        return ptr[i + j * rowcol];
    }

    std::unique_ptr<T[]> ptr = std::make_unique<T[]>(units);
};

#endif // UTILS_MATRIX_EXPORT_MATRIX_H
