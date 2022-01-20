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

#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR4_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR4_H

#include <algorithm>
#include <cmath>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class Vector4 {
public:
    union {
        struct {
            T x_;
            T y_;
            T z_;
            T w_;
        };
        T data_[4];
    };

    Vector4();
    Vector4(T x, T y, T z, T w);
    explicit Vector4(const T* array);
    ~Vector4();

    Vector4 Normalized() const;
    T Dot(const Vector4<T>& other) const;
    T GetSqrLength() const;
    T GetLength() const;
    T Normalize();
    void Identity();
    bool IsInfinite() const;
    bool IsIdentity() const;
    void SetValues(T x, T y, T z, T w);
    void SetZero();
    Vector4 operator-() const;
    Vector4 operator-(const Vector4<T>& other) const;
    Vector4 operator+(const Vector4<T>& other) const;
    Vector4 operator/(T scale) const;
    Vector4 operator*(T scale) const;
    Vector4 operator*(const Vector4<T>& other) const;
    Vector4& operator*=(const Vector4<T>& other);
    Vector4& operator=(const Vector4<T>& other);
    bool operator==(const Vector4& other) const;
    bool operator!=(const Vector4& other) const;

    T operator[](int index) const;
    T& operator[](int index);
    T* GetData();

    void Scale(T arg);
    void Sub(const Vector4<T>& arg);
    void Add(const Vector4<T>& arg);
    void Multiply(const Vector4<T>& arg);
    void Div(const Vector4<T>& arg);
    void Negate();
    void Absolute();
    static void Min(const Vector4<T>& a, const Vector4<T>& b, Vector4<T>& result);
    static void Max(const Vector4<T>& a, const Vector4<T>& b, Vector4<T>& result);
    static void Mix(const Vector4<T>& min, const Vector4<T>& max, T a, Vector4<T>& result);
};

typedef Vector4<float> Vector4f;
typedef Vector4<double> Vector4d;

class Quaternion : public Vector4f {
public:
    Quaternion() : Vector4f() {}
    Quaternion(float x, float y, float z, float w) : Vector4f(x, y, z, w) {}
    Quaternion(const Vector4f& other) : Vector4f(other) {}
    Quaternion(const Vector4f&& other) : Vector4f(other) {}
    Quaternion Slerp(const Quaternion& to, float t);
    Quaternion Flip() const;
};

template<typename T>
Vector4<T>::Vector4()
{
    Identity();
}

template<typename T>
Vector4<T>::Vector4(T x, T y, T z, T w)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
    data_[3] = w;
}

template<typename T>
Vector4<T>::Vector4(const T* array)
{
    std::copy_n(array, std::size(data_), data_);
}

template<typename T>
Vector4<T>::~Vector4()
{}

inline Quaternion Quaternion::Flip() const
{
    return { -data_[0], -data_[1], -data_[2], -data_[3] };
}

inline Quaternion Quaternion::Slerp(const Quaternion& to, float t)
{
    constexpr double SLERP_EPSILON = 1e-5;
    if (t < 0.0 || t > 1.0) {
        return *this;
    }

    auto from = *this;

    double cosHalfAngle = from.x_ * to.x_ + from.y_ * to.y_ + from.z_ * to.z_ + from.w_ * to.w_;
    if (cosHalfAngle < 0.0) {
        // Since the half angle is > 90 degrees, the full rotation angle would
        // exceed 180 degrees. The quaternions (x, y, z, w) and (-x, -y, -z, -w)
        // represent the same rotation. Flipping the orientation of either
        // quaternion ensures that the half angle is less than 90 and that we are
        // taking the shortest path.
        from = from.Flip();
        cosHalfAngle = -cosHalfAngle;
    }

    // Ensure that acos is well behaved at the boundary.
    if (cosHalfAngle > 1.0) {
        cosHalfAngle = 1.0;
    }

    double sinHalfAngle = std::sqrt(1.0 - cosHalfAngle * cosHalfAngle);
    if (sinHalfAngle < SLERP_EPSILON) {
        // Quaternions share common axis and angle.
        return *this;
    }

    double half_angle = std::acos(cosHalfAngle);

    float scaleA = std::sin((1.0 - t) * half_angle) / sinHalfAngle;
    float scaleB = std::sin(t * half_angle) / sinHalfAngle;

    return (from * scaleA) + (to * scaleB);
}

template<typename T>
Vector4<T> Vector4<T>::Normalized() const
{
    Vector4<T> rNormalize(*this);
    rNormalize.Normalize();
    return rNormalize;
}

template<typename T>
T Vector4<T>::Dot(const Vector4<T>& other) const
{
    const T* oData = other.data_;
    T sum = data_[0] * oData[0];
    sum += data_[1] * oData[1];
    sum += data_[2] * oData[2];
    sum += data_[3] * oData[3];
    return sum;
}

template<typename T>
T Vector4<T>::GetSqrLength() const
{
    T sum = data_[0] * data_[0];
    sum += data_[1] * data_[1];
    sum += data_[2] * data_[2];
    sum += data_[3] * data_[3];
    return sum;
}

template<typename T>
T Vector4<T>::GetLength() const
{
    return sqrt(GetSqrLength());
}

template<typename T>
T Vector4<T>::Normalize()
{
    T l = GetLength();
    if (ROSEN_EQ<T>(l, 0.0)) {
        return (T)0.0;
    }

    const T d = 1.0f / l;
    data_[0] *= d;
    data_[1] *= d;
    data_[2] *= d;
    data_[3] *= d;
    return l;
}

template<typename T>
void Vector4<T>::Min(const Vector4<T>& a, const Vector4<T>& b, Vector4<T>& result)
{
    T* resultData = result.data_;
    const T* aData = a.data_;
    const T* bData = b.data_;
    resultData[3] = std::min(aData[3], bData[3]);
    resultData[2] = std::min(aData[2], bData[2]);
    resultData[1] = std::min(aData[1], bData[1]);
    resultData[0] = std::min(aData[0], bData[0]);
}

template<typename T>
void Vector4<T>::Max(const Vector4<T>& a, const Vector4<T>& b, Vector4<T>& result)
{
    T* resultData = result.data_;
    const T* aData = a.data_;
    const T* bData = b.data_;
    resultData[3] = std::max(aData[3], bData[3]);
    resultData[2] = std::max(aData[2], bData[2]);
    resultData[1] = std::max(aData[1], bData[1]);
    resultData[0] = std::max(aData[0], bData[0]);
}

template<typename T>
void Vector4<T>::Mix(const Vector4<T>& min, const Vector4<T>& max, T a, Vector4<T>& result)
{
    T* resultData = result.data_;
    const T* minData = min.data_;
    const T* maxData = max.data_;
    resultData[3] = minData[3] + a * (maxData[3] - minData[3]);
    resultData[2] = minData[2] + a * (maxData[2] - minData[2]);
    resultData[1] = minData[1] + a * (maxData[1] - minData[1]);
    resultData[0] = minData[0] + a * (maxData[0] - minData[0]);
}

template<typename T>
inline T* Vector4<T>::GetData()
{
    return data_;
}

template<typename T>
void Vector4<T>::Identity()
{
    SetValues(0.f, 0.f, 0.f, 1.f);
}

template<typename T>
bool Vector4<T>::IsIdentity() const
{
    return operator==(Vector4<T>(0.f, 0.f, 0.f, 1.f));
}

template<typename T>
void Vector4<T>::SetValues(T x, T y, T z, T w)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
    data_[3] = w;
}

template<typename T>
void Vector4<T>::SetZero()
{
    SetValues(0.f, 0.f, 0.f, 0.f);
}

template<typename T>
Vector4<T> Vector4<T>::operator-(const Vector4<T>& other) const
{
    const T* otherData = other.data_;

    return Vector4<T>(
        data_[0] - otherData[0], data_[1] - otherData[1], data_[2] - otherData[2], data_[3] - otherData[3]);
}

template<typename T>
Vector4<T> Vector4<T>::operator+(const Vector4<T>& other) const
{
    const T* thisData = data_;
    const T* otherData = other.data_;

    return Vector4<T>(
        thisData[0] + otherData[0], thisData[1] + otherData[1], thisData[2] + otherData[2], thisData[3] + otherData[3]);
}

template<typename T>
Vector4<T> Vector4<T>::operator/(T scale) const
{
    if (AGP_EQ(scale, 0)) {
        return *this;
    }
    Vector4<T> clone(data_);
    clone.Scale(1.0f / scale);
    return clone;
}

template<typename T>
Vector4<T> Vector4<T>::operator*(T scale) const
{
    Vector4<T> clone(data_);
    clone.Scale(scale);
    return clone;
}

template<typename T>
Vector4<T> Vector4<T>::operator*(const Vector4<T>& other) const
{
    Vector4<T> rMult;
    T* rData = rMult.data_;
    const T* oData = other.data_;
    rData[0] = data_[0] * oData[0];
    rData[1] = data_[1] * oData[1];
    rData[2] = data_[2] * oData[2];
    rData[3] = data_[3] * oData[3];
    return rMult;
}

template<typename T>
Vector4<T>& Vector4<T>::operator*=(const Vector4<T>& other)
{
    const T* oData = other.data_;
    data_[0] *= oData[0];
    data_[1] *= oData[1];
    data_[2] *= oData[2];
    data_[3] *= oData[3];
    return *this;
}

template<typename T>
Vector4<T>& Vector4<T>::operator=(const Vector4<T>& other)
{
    const T* oData = other.data_;
    data_[0] = oData[0];
    data_[1] = oData[1];
    data_[2] = oData[2];
    data_[3] = oData[3];
    return *this;
}

template<typename T>
inline bool Vector4<T>::operator==(const Vector4& other) const
{
    const T* oData = other.data_;

    return (ROSEN_EQ<T>(data_[0], oData[0])) && (ROSEN_EQ<T>(data_[1], oData[1])) &&
           (ROSEN_EQ<T>(data_[2], oData[2])) && (ROSEN_EQ<T>(data_[3], oData[3]));
}

template<typename T>
inline bool Vector4<T>::operator!=(const Vector4& other) const
{
    return !operator==(other);
}

template<typename T>
Vector4<T> Vector4<T>::operator-() const
{
    return Vector4<T>(-data_[0], -data_[1], -data_[2], -data_[3]);
}

template<typename T>
T Vector4<T>::operator[](int index) const
{
    return data_[index];
}

template<typename T>
T& Vector4<T>::operator[](int index)
{
    return data_[index];
}

template<typename T>
void Vector4<T>::Scale(T arg)
{
    data_[3] *= arg;
    data_[2] *= arg;
    data_[1] *= arg;
    data_[0] *= arg;
}

template<typename T>
void Vector4<T>::Sub(const Vector4<T>& arg)
{
    const T* argData = arg.data_;
    data_[3] -= argData[3];
    data_[2] -= argData[2];
    data_[1] -= argData[1];
    data_[0] -= argData[0];
}

template<typename T>
void Vector4<T>::Add(const Vector4<T>& arg)
{
    const T* argData = arg.data_;
    data_[3] += argData[3];
    data_[2] += argData[2];
    data_[1] += argData[1];
    data_[0] += argData[0];
}

template<typename T>
void Vector4<T>::Multiply(const Vector4<T>& arg)
{
    const T* argData = arg.data_;
    data_[3] *= argData[3];
    data_[2] *= argData[2];
    data_[1] *= argData[1];
    data_[0] *= argData[0];
}

template<typename T>
void Vector4<T>::Div(const Vector4<T>& arg)
{
    const T* argData = arg.data_;
    data_[3] /= argData[3];
    data_[2] /= argData[2];
    data_[1] /= argData[1];
    data_[0] /= argData[0];
}

template<typename T>
void Vector4<T>::Negate()
{
    data_[3] = -data_[3];
    data_[2] = -data_[2];
    data_[1] = -data_[1];
    data_[0] = -data_[0];
}

template<typename T>
void Vector4<T>::Absolute()
{
    data_[3] = abs(data_[3]);
    data_[2] = abs(data_[2]);
    data_[1] = abs(data_[1]);
    data_[0] = abs(data_[0]);
}

template<typename T>
bool Vector4<T>::IsInfinite() const
{
    return std::isinf(data_[0]) || std::isinf(data_[1]) ||
        std::isinf(data_[2]) || std::isinf(data_[3]);
}
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_VECTOR4_H
