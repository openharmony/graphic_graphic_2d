/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR3_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR3_H
#include <cmath>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class Vector3 {
public:
    union {
        struct {
            T x_;
            T y_;
            T z_;
        };
        T data_[3];
    };

    Vector3();
    Vector3(T x, T y, T z);
    explicit Vector3(T* v);
    ~Vector3();

    Vector3 Normalized() const;
    T Dot(const Vector3<T>& other) const;
    Vector3 Cross(const Vector3<T>& other) const;
    T GetSqrLength() const;
    T GetLength() const;
    void SetZero();
    void SetValues(T x, T y, T z);
    T Normalize();

    Vector3& operator*=(const Vector3<T>& other);
    Vector3& operator*=(T s);
    Vector3 operator*(T s) const;
    Vector3 operator+(const Vector3<T>& other) const;
    Vector3& operator+=(const Vector3<T>& other);
    Vector3& operator=(const Vector3<T>& other);
    Vector3 operator-(const Vector3<T>& other) const;
    T operator[](int index) const;
    T& operator[](int index);
    bool operator==(const Vector3& other) const;
    bool IsNearEqual(const Vector3& other, T threshold = std::numeric_limits<T>::epsilon()) const;
    T* GetData();

    static const Vector3 ZERO;
};

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;
template<typename T>
const Vector3<T> Vector3<T>::ZERO(0.0, 0.0, 0.0);

template<typename T>
Vector3<T>::Vector3()
{
    data_[0] = 0.0;
    data_[1] = 0.0;
    data_[2] = 0.0;
}

template<typename T>
Vector3<T>::Vector3(T x, T y, T z)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
}

template<typename T>
Vector3<T>::Vector3(T* v)
{
    data_[0] = v[0];
    data_[1] = v[1];
    data_[2] = v[2];
}

template<typename T>
Vector3<T>::~Vector3()
{}

template<typename T>
Vector3<T> Vector3<T>::Normalized() const
{
    Vector3<T> rNormalize(*this);
    rNormalize.Normalize();
    return rNormalize;
}

template<typename T>
T Vector3<T>::Dot(const Vector3<T>& other) const
{
    const T* oData = other.data_;
    T sum = data_[0] * oData[0];
    sum += data_[1] * oData[1];
    sum += data_[2] * oData[2];
    return sum;
}

template<typename T>
Vector3<T> Vector3<T>::Cross(const Vector3<T>& other) const
{
    T x = data_[0];
    T y = data_[1];
    T z = data_[2];
    const T* oData = other.data_;
    T oX = oData[0];
    T oY = oData[1];
    T oZ = oData[2];
    Vector3<T> rCross;
    rCross.SetValues(y * oZ - z * oY, z * oX - x * oZ, x * oY - y * oX);
    return rCross;
}

template<typename T>
T Vector3<T>::GetSqrLength() const
{
    T x = data_[0];
    T y = data_[1];
    T z = data_[2];
    return (x * x) + (y * y) + (z * z);
}

template<typename T>
T Vector3<T>::GetLength() const
{
    return sqrt(GetSqrLength());
}

template<typename T>
void Vector3<T>::SetZero()
{
    *this = ZERO;
}

template<typename T>
void Vector3<T>::SetValues(T x, T y, T z)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
}

template<typename T>
T Vector3<T>::Normalize()
{
    T l = GetLength();
    if (ROSEN_EQ<T>(l, 0.0)) {
        return 0.0;
    }

    const T d = 1.0f / l;
    data_[0] *= d;
    data_[1] *= d;
    data_[2] *= d;
    return l;
}

template<typename T>
Vector3<T>& Vector3<T>::operator*=(const Vector3<T>& other)
{
    const T* oData = other.data_;
    data_[0] *= oData[0];
    data_[1] *= oData[1];
    data_[2] *= oData[2];
    return *this;
}

template<typename T>
Vector3<T>& Vector3<T>::operator*=(T s)
{
    data_[0] *= s;
    data_[1] *= s;
    data_[2] *= s;
    return *this;
}

template<typename T>
Vector3<T> Vector3<T>::operator*(T s) const
{
    Vector3<T> rMulti(*this);
    T* rData = rMulti.data_;

    rData[0] *= s;
    rData[1] *= s;
    rData[2] *= s;
    return rMulti;
}

template<typename T>
Vector3<T> Vector3<T>::operator+(const Vector3<T>& other) const
{
    Vector3<T> rVec = *this;
    rVec += other;
    return rVec;
}

template<typename T>
Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& other)
{
    data_[0] += other.data_[0];
    data_[1] += other.data_[1];
    data_[2] += other.data_[2];
    return *this;
}

template<typename T>
Vector3<T>& Vector3<T>::operator=(const Vector3<T>& other)
{
    data_[0] = other.data_[0];
    data_[1] = other.data_[1];
    data_[2] = other.data_[2];
    return *this;
}

template<typename T>
Vector3<T> Vector3<T>::operator-(const Vector3<T>& other) const
{
    Vector3<T> rSub(*this);
    T* rData = rSub.data_;
    const T* oData = other.data_;
    rData[0] -= oData[0];
    rData[1] -= oData[1];
    rData[2] -= oData[2];
    return rSub;
}

template<typename T>
T Vector3<T>::operator[](int index) const
{
    return data_[index];
}

template<typename T>
T& Vector3<T>::operator[](int index)
{
    return data_[index];
}

template<typename T>
inline bool Vector3<T>::operator==(const Vector3& other) const
{
    const T* oData = other.data_;

    return (ROSEN_EQ<T>(data_[0], oData[0])) && (ROSEN_EQ<T>(data_[1], oData[1])) && (ROSEN_EQ<T>(data_[2], oData[2]));
}

template<typename T>
bool Vector3<T>::IsNearEqual(const Vector3& other, T threshold) const
{
    const T* otherData = other.data_;

    return (ROSEN_EQ<T>(data_[0], otherData[0], threshold)) && (ROSEN_EQ<T>(data_[1], otherData[1], threshold)) &&
           (ROSEN_EQ<T>(data_[2], otherData[2], threshold)); // 2 index data
}

template<typename T>
inline T* Vector3<T>::GetData()
{
    return data_;
}
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR3_H
