/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR2_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR2_H
#include <cmath>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class Vector2 {
public:
    union {
        struct {
            T x_;
            T y_;
        };
        T data_[2];
    };

    Vector2();
    Vector2(T x, T y);
    explicit Vector2(const T* v);
    virtual ~Vector2();

    Vector2 Normalized() const;
    T Dot(const Vector2<T>& other) const;
    T Cross(const Vector2<T>& other) const;
    Vector2 operator-() const;
    Vector2 operator-(const Vector2<T>& other) const;
    Vector2 operator+(const Vector2<T>& other) const;
    Vector2 operator/(T scale) const;
    Vector2 operator*(T scale) const;
    Vector2 operator*(const Vector2<T>& other) const;
    Vector2& operator*=(const Vector2<T>& other);
    Vector2& operator+=(const Vector2<T>& other);
    Vector2& operator-=(const Vector2<T>& other);
    Vector2& operator=(const Vector2& other);
    T operator[](int index) const;
    T& operator[](int index);
    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;
    bool IsNearEqual(const Vector2& other, T threshold = std::numeric_limits<T>::epsilon()) const;

    T* GetData();

    T GetLength() const;
    T GetSqrLength() const;
    T Normalize();
    bool IsInfinite() const;
    bool IsNaN() const;
};

typedef Vector2<int> UIPoint;
typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;
template<typename T>
Vector2<T>::Vector2()
{}

template<typename T>
Vector2<T>::Vector2(T x, T y)
{
    data_[0] = x;
    data_[1] = y;
}

template<typename T>
Vector2<T>::Vector2(const T* v)
{
    data_[0] = v[0];
    data_[1] = v[1];
}

template<typename T>
Vector2<T>::~Vector2()
{}

template<typename T>
Vector2<T> Vector2<T>::Normalized() const
{
    Vector2<T> rNormalize(*this);
    rNormalize.Normalize();
    return rNormalize;
}

template<typename T>
T Vector2<T>::Dot(const Vector2<T>& other) const
{
    const T* oData = other.data_;
    T sum = data_[0] * oData[0];
    sum += data_[1] * oData[1];
    return sum;
}

template<typename T>
T Vector2<T>::Cross(const Vector2<T>& other) const
{
    const T* oData = other.data_;

    return data_[0] * oData[1] - data_[1] * oData[0];
}

template<typename T>
Vector2<T> Vector2<T>::operator-() const
{
    Vector2<T> rNeg;
    T* rData = rNeg.data_;
    rData[0] = -data_[0];
    rData[1] = -data_[1];
    return rNeg;
}

template<typename T>
Vector2<T> Vector2<T>::operator-(const Vector2<T>& other) const
{
    Vector2<T> rSub(*this);
    T* rData = rSub.data_;
    const T* oData = other.data_;
    rData[0] -= oData[0];
    rData[1] -= oData[1];
    return rSub;
}

template<typename T>
Vector2<T> Vector2<T>::operator+(const Vector2<T>& other) const
{
    Vector2<T> rAdd(*this);
    return rAdd += other;
}

template<typename T>
Vector2<T> Vector2<T>::operator/(T scale) const
{
    if (ROSEN_EQ<T>(scale, 0)) {
        return *this;
    }
    const T invScale = 1.0f / scale;
    return (*this) * invScale;
}

template<typename T>
Vector2<T> Vector2<T>::operator*(T scale) const
{
    Vector2<T> rMult(*this);
    T* rData = rMult.data_;

    rData[0] *= scale;
    rData[1] *= scale;
    return rMult;
}

template<typename T>
Vector2<T> Vector2<T>::operator*(const Vector2<T>& other) const
{
    Vector2<T> rMult(*this);
    return rMult *= other;
}

template<typename T>
Vector2<T>& Vector2<T>::operator*=(const Vector2<T>& other)
{
    const T* oData = other.data_;
    data_[0] *= oData[0];
    data_[1] *= oData[1];
    return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& other)
{
    data_[0] += other.data_[0];
    data_[1] += other.data_[1];
    return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& other)
{
    data_[0] -= other.data_[0];
    data_[1] -= other.data_[1];
    return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator=(const Vector2<T>& other)
{
    const T* oData = other.data_;
    data_[0] = oData[0];
    data_[1] = oData[1];
    return *this;
}

template<typename T>
T Vector2<T>::operator[](int index) const
{
    return data_[index];
}

template<typename T>
inline T& Vector2<T>::operator[](int index)
{
    return data_[index];
}

template<typename T>
inline bool Vector2<T>::operator==(const Vector2& other) const
{
    const T* oData = other.data_;

    return (ROSEN_EQ<T>(data_[0], oData[0])) && (ROSEN_EQ<T>(data_[1], oData[1]));
}

template<typename T>
inline bool Vector2<T>::operator!=(const Vector2& other) const
{
    const T* oData = other.data_;

    return (!ROSEN_EQ<T>(data_[0], oData[0])) || (!ROSEN_EQ<T>(data_[1], oData[1]));
}

template<typename T>
bool Vector2<T>::IsNearEqual(const Vector2& other, T threshold) const
{
    const T* otherData = other.data_;

    return (ROSEN_EQ<T>(data_[0], otherData[0], threshold)) && (ROSEN_EQ<T>(data_[1], otherData[1], threshold));
}

template<typename T>
inline T* Vector2<T>::GetData()
{
    return data_;
}

template<typename T>
T Vector2<T>::GetLength() const
{
    return sqrt(GetSqrLength());
}

template<typename T>
T Vector2<T>::GetSqrLength() const
{
    T sum = data_[0] * data_[0];
    sum += data_[1] * data_[1];
    return sum;
}

template<typename T>
T Vector2<T>::Normalize()
{
    T l = GetLength();
    if (ROSEN_EQ<T>(l, 0.0)) {
        return 0.0f;
    }

    const T invLen = 1.0f / l;

    data_[0] *= invLen;
    data_[1] *= invLen;
    return l;
}

template<typename T>
bool Vector2<T>::IsInfinite() const
{
    return std::isinf(data_[0]) || std::isinf(data_[1]);
}

template<typename T>
bool Vector2<T>::IsNaN() const
{
    return IsNan(data_[0]) || IsNan(data_[1]);
}
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR2_H
