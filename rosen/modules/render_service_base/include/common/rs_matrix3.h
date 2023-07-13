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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MATRIX3_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MATRIX3_H

#define USE_MATH_DEFINES
#include <cmath>

#include "common/rs_macros.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"

// column-major order
namespace OHOS {
namespace Rosen {
template<typename T>
class Matrix3 {
public:
    static const Matrix3 ZERO;
    static const Matrix3 IDENTITY;
    Matrix3();
    Matrix3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22);

    Matrix3(const Matrix3& matrix) noexcept = default;

    explicit Matrix3(const T* v);

    ~Matrix3();
    T Trace() const;
    static int Index(int row, int col);
    void SetIdentity();
    void SetZero();
    bool IsIdentity() const;
    Matrix3 Inverse() const;
    Matrix3 Multiply(const Matrix3& other) const;

    Matrix3 operator+(const Matrix3& other) const;
    Matrix3 operator-(const Matrix3& other) const;
    Matrix3 operator-() const;
    Matrix3 operator*(const Matrix3& other) const;
    Vector3<T> operator*(const Vector3<T>& other) const;
    Matrix3 operator*(T scale) const;
    T* operator[](int col);
    Matrix3& operator=(const Matrix3& other);
    Matrix3& operator+=(const Matrix3& other);
    Matrix3& operator-=(const Matrix3& other);
    Matrix3& operator*=(const Matrix3& other);
    Matrix3& operator*=(T scale);
    bool operator==(const Matrix3& other) const;
    bool operator!=(const Matrix3& other) const;
    T* GetData();
    const T* GetConstData() const;
    T Determinant() const;
    Matrix3 Transpose() const;
    Matrix3 Translate(const Vector2<T>& vec) const;
    Matrix3 Rotate(T angle) const;
    Matrix3 Rotate(T angle, T pivotx, T pivoty) const;
    Matrix3 Scale(const Vector2<T>& vec) const;
    Matrix3 Scale(const Vector2<T>& vec, T pivotx, T pivoty) const;
    Matrix3 ShearX(T y) const;
    Matrix3 ShearY(T x) const;

protected:
    T data_[9] = { 0 };
};

typedef Matrix3<float> Matrix3f;
typedef Matrix3<double> Matrix3d;

template<typename T>
const Matrix3<T> Matrix3<T>::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0);

template<typename T>
const Matrix3<T> Matrix3<T>::IDENTITY(1, 0, 0, 0, 1, 0, 0, 0, 1);

template<typename T>
Matrix3<T>::Matrix3()
{}

template<typename T>
Matrix3<T>::Matrix3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22)
{
    data_[0] = m00;
    data_[1] = m01;
    data_[2] = m02;

    data_[3] = m10;
    data_[4] = m11;
    data_[5] = m12;

    data_[6] = m20;
    data_[7] = m21;
    data_[8] = m22;
}

template<typename T>
Matrix3<T>::Matrix3(const T* v)
{
    std::copy_n(v, std::size(data_), data_);
}

template<typename T>
Matrix3<T>::~Matrix3()
{}

template<typename T>
T Matrix3<T>::Trace() const
{
    T rTrace = 0.0;
    rTrace += data_[0];
    rTrace += data_[4];
    rTrace += data_[8];
    return rTrace;
}

template<typename T>
int Matrix3<T>::Index(int row, int col)
{
    return (col * 3) + row;
}

template<typename T>
void Matrix3<T>::SetIdentity()
{
    *this = IDENTITY;
}

template<typename T>
void Matrix3<T>::SetZero()
{
    *this = ZERO;
}

template<typename T>
bool Matrix3<T>::IsIdentity() const
{
    return (ROSEN_EQ<T>(data_[0], 1.0)) && (ROSEN_EQ<T>(data_[1], 0.0)) && (ROSEN_EQ<T>(data_[2], 0.0)) &&
           (ROSEN_EQ<T>(data_[3], 0.0)) && (ROSEN_EQ<T>(data_[4], 1.0)) && (ROSEN_EQ<T>(data_[5], 0.0)) &&
           (ROSEN_EQ<T>(data_[6], 0.0)) && (ROSEN_EQ<T>(data_[7], 0.0)) && (ROSEN_EQ<T>(data_[8], 1.0));
}

template<typename T>
Matrix3<T> Matrix3<T>::Inverse() const
{
    T det = Determinant();
    if (ROSEN_EQ<T>(det, 0.0)) {
        return Matrix3<T>(*this);
    }

    const T invDet = 1.0f / det;
    const T* data = data_;

    T iX = invDet * (data[4] * data[8] - data[5] * data[7]);
    T iY = invDet * (data[2] * data[7] - data[1] * data[8]);
    T iZ = invDet * (data[1] * data[5] - data[2] * data[4]);
    T jX = invDet * (data[5] * data[6] - data[3] * data[8]);
    T jY = invDet * (data[0] * data[8] - data[2] * data[6]);
    T jZ = invDet * (data[2] * data[3] - data[0] * data[5]);
    T kX = invDet * (data[3] * data[7] - data[4] * data[6]);
    T kY = invDet * (data[1] * data[6] - data[0] * data[7]);
    T kZ = invDet * (data[0] * data[4] - data[1] * data[3]);

    return Matrix3<T>(iX, iY, iZ, jX, jY, jZ, kX, kY, kZ);
}

template<typename T>
Matrix3<T> Matrix3<T>::Multiply(const Matrix3<T>& other) const
{
    Matrix3<T> rMulti;
    T* rData = rMulti.data_;
    const T* oData = other.data_;

    rData[0] = data_[0] * oData[0] + data_[3] * oData[1] + data_[6] * oData[2];
    rData[3] = data_[0] * oData[3] + data_[3] * oData[4] + data_[6] * oData[5];
    rData[6] = data_[0] * oData[6] + data_[3] * oData[7] + data_[6] * oData[8];

    rData[1] = data_[1] * oData[0] + data_[4] * oData[1] + data_[7] * oData[2];
    rData[4] = data_[1] * oData[3] + data_[4] * oData[4] + data_[7] * oData[5];
    rData[7] = data_[1] * oData[6] + data_[4] * oData[7] + data_[7] * oData[8];

    rData[2] = data_[2] * oData[0] + data_[5] * oData[1] + data_[8] * oData[2];
    rData[5] = data_[2] * oData[3] + data_[5] * oData[4] + data_[8] * oData[5];
    rData[8] = data_[2] * oData[6] + data_[5] * oData[7] + data_[8] * oData[8];
    return rMulti;
}

template<typename T>
Matrix3<T> Matrix3<T>::operator+(const Matrix3<T>& other) const
{
    Matrix3<T> rMat3Add;
    T* rMat3Data = rMat3Add.data_;
    const T* oData = other.data_;

    rMat3Data[0] = data_[0] + oData[0];
    rMat3Data[1] = data_[1] + oData[1];
    rMat3Data[2] = data_[2] + oData[2];
    rMat3Data[3] = data_[3] + oData[3];
    rMat3Data[4] = data_[4] + oData[4];
    rMat3Data[5] = data_[5] + oData[5];
    rMat3Data[6] = data_[6] + oData[6];
    rMat3Data[7] = data_[7] + oData[7];
    rMat3Data[8] = data_[8] + oData[8];

    return rMat3Add;
}

template<typename T>
Matrix3<T> Matrix3<T>::operator-(const Matrix3<T>& other) const
{
    return *this + (-other);
}

template<typename T>
Matrix3<T> Matrix3<T>::operator-() const
{
    Matrix3<T> rMat3Sub;
    T* rMat3Data = rMat3Sub.data_;

    rMat3Data[0] = -data_[0];
    rMat3Data[1] = -data_[1];
    rMat3Data[2] = -data_[2];
    rMat3Data[3] = -data_[3];
    rMat3Data[4] = -data_[4];
    rMat3Data[5] = -data_[5];
    rMat3Data[6] = -data_[6];
    rMat3Data[7] = -data_[7];
    rMat3Data[8] = -data_[8];

    return rMat3Sub;
}

template<typename T>
Matrix3<T> Matrix3<T>::operator*(const Matrix3<T>& other) const
{
    return Multiply(other);
}

template<typename T>
Vector3<T> Matrix3<T>::operator*(const Vector3<T>& other) const
{
    Vector3<T> rMulti;
    T* rData = rMulti.data_;
    const T* oData = other.data_;
    rData[0] = data_[0] * oData[0] + data_[3] * oData[1] + data_[6] * oData[2];

    rData[1] = data_[1] * oData[0] + data_[4] * oData[1] + data_[7] * oData[2];

    rData[2] = data_[2] * oData[0] + data_[5] * oData[1] + data_[8] * oData[2];
    return rMulti;
}

template<typename T>
Matrix3<T> Matrix3<T>::operator*(T scale) const
{
    Matrix3<T> rMulti;
    T* rData = rMulti.data_;
    rData[0] = data_[0] * scale;
    rData[1] = data_[1] * scale;
    rData[2] = data_[2] * scale;
    rData[3] = data_[3] * scale;
    rData[4] = data_[4] * scale;
    rData[5] = data_[5] * scale;
    rData[6] = data_[6] * scale;
    rData[7] = data_[7] * scale;
    rData[8] = data_[8] * scale;

    return rMulti;
}

template<typename T>
T* Matrix3<T>::operator[](int col)
{
    return &data_[col * 3];
}

template<typename T>
Matrix3<T>& Matrix3<T>::operator=(const Matrix3<T>& other)
{
    const T* oMat3Data = other.data_;
    data_[0] = oMat3Data[0];
    data_[1] = oMat3Data[1];
    data_[2] = oMat3Data[2];
    data_[3] = oMat3Data[3];
    data_[4] = oMat3Data[4];
    data_[5] = oMat3Data[5];
    data_[6] = oMat3Data[6];
    data_[7] = oMat3Data[7];
    data_[8] = oMat3Data[8];

    return *this;
}

template<typename T>
Matrix3<T>& Matrix3<T>::operator+=(const Matrix3<T>& other)
{
    const T* oData = other.data_;

    data_[0] += oData[0];
    data_[1] += oData[1];
    data_[2] += oData[2];
    data_[3] += oData[3];
    data_[4] += oData[4];
    data_[5] += oData[5];
    data_[6] += oData[6];
    data_[7] += oData[7];
    data_[8] += oData[8];

    return *this;
}

template<typename T>
Matrix3<T>& Matrix3<T>::operator-=(const Matrix3<T>& other)
{
    const T* oData = other.data_;

    data_[0] -= oData[0];
    data_[1] -= oData[1];
    data_[2] -= oData[2];
    data_[3] -= oData[3];
    data_[4] -= oData[4];
    data_[5] -= oData[5];
    data_[6] -= oData[6];
    data_[7] -= oData[7];
    data_[8] -= oData[8];

    return *this;
}

template<typename T>
Matrix3<T>& Matrix3<T>::operator*=(const Matrix3<T>& other)
{
    return (*this = *this * other);
}

template<typename T>
Matrix3<T>& Matrix3<T>::operator*=(T scale)
{
    data_[0] *= scale;
    data_[1] *= scale;
    data_[2] *= scale;
    data_[3] *= scale;
    data_[4] *= scale;
    data_[5] *= scale;
    data_[6] *= scale;
    data_[7] *= scale;
    data_[8] *= scale;
    return *this;
}

template<typename T>
bool Matrix3<T>::operator==(const Matrix3& other) const
{
    const T* oData = other.data_;

    return (ROSEN_EQ<T>(data_[0], oData[0])) && (ROSEN_EQ<T>(data_[1], oData[1])) &&
           (ROSEN_EQ<T>(data_[2], oData[2])) && (ROSEN_EQ<T>(data_[3], oData[3])) &&
           (ROSEN_EQ<T>(data_[4], oData[4])) && (ROSEN_EQ<T>(data_[5], oData[5])) &&
           (ROSEN_EQ<T>(data_[6], oData[6])) && (ROSEN_EQ<T>(data_[7], oData[7])) && (ROSEN_EQ<T>(data_[8], oData[8]));
}

template<typename T>
bool Matrix3<T>::operator!=(const Matrix3& other) const
{
    return !operator==(other);
}

template<typename T>
inline T* Matrix3<T>::GetData()
{
    return data_;
}

template<typename T>
const T* Matrix3<T>::GetConstData() const
{
    return data_;
}

template<typename T>
T Matrix3<T>::Determinant() const
{
    T x = data_[0] * ((data_[4] * data_[8]) - (data_[5] * data_[7]));
    T y = data_[1] * ((data_[3] * data_[8]) - (data_[5] * data_[6]));
    T z = data_[2] * ((data_[3] * data_[7]) - (data_[4] * data_[6]));
    return x - y + z;
}

template<typename T>
Matrix3<T> Matrix3<T>::Transpose() const
{
    Matrix3<T> rTrans;
    T* rData = rTrans.data_;
    rData[0] = data_[0];
    rData[1] = data_[3];
    rData[2] = data_[6];
    rData[3] = data_[1];
    rData[4] = data_[4];
    rData[5] = data_[7];
    rData[6] = data_[2];
    rData[7] = data_[5];
    rData[8] = data_[8];
    return rTrans;
}

template<typename T>
Matrix3<T> Matrix3<T>::Translate(const Vector2<T>& vec) const
{
    Matrix3<T> rTrans(*this);
    T* rData = rTrans.data_;

    rData[6] = data_[0] * vec[0] + data_[3] * vec[1] + data_[6];
    rData[7] = data_[1] * vec[0] + data_[4] * vec[1] + data_[7];
    rData[8] = data_[2] * vec[0] + data_[5] * vec[1] + data_[8];
    return rTrans;
}

template<typename T>
Matrix3<T> Matrix3<T>::Rotate(T angle) const
{
    T a = angle;
    T c = cos(a);
    T s = sin(a);

    Matrix3<T> rRotate(*this);
    T* rData = rRotate.data_;
    rData[0] = data_[0] * c + data_[3] * s;
    rData[1] = data_[1] * c + data_[4] * s;
    rData[2] = data_[2] * c + data_[5] * s;

    rData[3] = data_[0] * -s + data_[3] * c;
    rData[4] = data_[1] * -s + data_[4] * c;
    rData[5] = data_[2] * -s + data_[5] * c;
    return rRotate;
}

template<typename T>
Matrix3<T> Matrix3<T>::Rotate(T angle, T pivotx, T pivoty) const
{
    T a = angle;
    T c = cos(a);
    T s = sin(a);
    T dx = s * pivoty + (1 - c) * pivotx;
    T dy = -s * pivotx + (1 - c) * pivoty;

    Matrix3<T> rRotate(*this);
    T* rData = rRotate.data_;
    rData[0] = data_[0] * c + data_[3] * s;
    rData[1] = data_[1] * c + data_[4] * s;
    rData[2] = data_[2] * c + data_[5] * s;

    rData[3] = data_[0] * -s + data_[3] * c;
    rData[4] = data_[1] * -s + data_[4] * c;
    rData[5] = data_[2] * -s + data_[5] * c;

    rData[6] = data_[0] * dx + data_[3] * dy + data_[6];
    rData[7] = data_[1] * dx + data_[4] * dy + data_[7];
    rData[8] = data_[2] * dx + data_[5] * dy + data_[8];
    return rRotate;
}

template<typename T>
Matrix3<T> Matrix3<T>::Scale(const Vector2<T>& vec) const
{
    Matrix3<T> rScale(*this);
    T* rData = rScale.data_;
    rData[0] = data_[0] * vec[0];
    rData[1] = data_[1] * vec[0];
    rData[2] = data_[2] * vec[0];

    rData[3] = data_[3] * vec[1];
    rData[4] = data_[4] * vec[1];
    rData[5] = data_[5] * vec[1];
    return rScale;
}

template<typename T>
Matrix3<T> Matrix3<T>::Scale(const Vector2<T>& vec, T pivotx, T pivoty) const
{
    T dx = pivotx - vec[0] * pivotx;
    T dy = pivoty - vec[1] * pivoty;

    Matrix3<T> rScale(*this);
    T* rData = rScale.data_;
    rData[0] = data_[0] * vec[0];
    rData[1] = data_[1] * vec[0];
    rData[2] = data_[2] * vec[0];

    rData[3] = data_[3] * vec[1];
    rData[4] = data_[4] * vec[1];
    rData[5] = data_[5] * vec[1];

    rData[6] = data_[0] * dx + data_[3] * dy + data_[6];
    rData[7] = data_[1] * dx + data_[4] * dy + data_[7];
    rData[8] = data_[2] * dx + data_[5] * dy + data_[8];
    return rScale;
}

template<typename T>
Matrix3<T> Matrix3<T>::ShearX(T y) const
{
    Matrix3<T> rShear(Matrix3<T>::IDENTITY);
    rShear.data_[1] = y;
    return (*this) * rShear;
}

template<typename T>
Matrix3<T> Matrix3<T>::ShearY(T x) const
{
    Matrix3<T> rShear(Matrix3<T>::IDENTITY);
    rShear.data_[3] = x;
    return (*this) * rShear;
}
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MATRIX3_H
