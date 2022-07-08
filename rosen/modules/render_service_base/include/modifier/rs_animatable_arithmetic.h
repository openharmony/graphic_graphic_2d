/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_ARITHMETIC_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_ARITHMETIC_H

#include <memory>

#include "common/rs_macros.h"
#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
class RSAnimatableBase {
public:
    RSAnimatableBase() = default;
    virtual ~RSAnimatableBase() = default;

    virtual std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) = 0;
    virtual std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) = 0;
    virtual std::shared_ptr<RSAnimatableBase> Multiply(const float scale) = 0;
    virtual bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) = 0;
};

template<typename T>
class RS_EXPORT RSAnimatableArithmetic : public RSAnimatableBase {
public:
    RSAnimatableArithmetic() = default;
    virtual ~RSAnimatableArithmetic() override = default;
};

inline std::shared_ptr<RSAnimatableBase> operator+(const std::shared_ptr<RSAnimatableBase>& a,
    const std::shared_ptr<RSAnimatableBase>& b)
{
    return a->Add(b);
}
inline std::shared_ptr<RSAnimatableBase> operator-(const std::shared_ptr<RSAnimatableBase>& a,
    const std::shared_ptr<RSAnimatableBase>& b)
{
    return a->Minus(b);
}
inline std::shared_ptr<RSAnimatableBase> operator*(const std::shared_ptr<RSAnimatableBase>& value, const float scale)
{
    return value->Multiply(scale);
}
inline bool operator==(const std::shared_ptr<RSAnimatableBase>& a, const std::shared_ptr<RSAnimatableBase>& b)
{
    return a->IsEqual(b);
}
inline bool operator!=(const std::shared_ptr<RSAnimatableBase>& a, const std::shared_ptr<RSAnimatableBase>& b)
{
    return !a->IsEqual(b);
}

class RSAnimatableFloat : public RSAnimatableArithmetic<RSAnimatableFloat> {
public:
    RSAnimatableFloat(const float value) : value_(value) {}
    ~RSAnimatableFloat() override = default;
    std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Multiply(const float scale) override;
    bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) override;
    float GetValue();
private:
    float value_;
};

class RSAnimatableInt : public RSAnimatableArithmetic<RSAnimatableInt> {
public:
    RSAnimatableInt(const int value) : value_(value) {}
    ~RSAnimatableInt() override = default;
    std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Multiply(const float scale) override;
    bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) override;
    int GetValue();
private:
    int value_;
};

class RSAnimatableColor : public RSAnimatableArithmetic<RSAnimatableColor> {
public:
    RSAnimatableColor(const Color value) : value_(value) {}
    ~RSAnimatableColor() override = default;
    std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Multiply(const float scale) override;
    bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) override;
    Color GetValue();
private:
    Color value_;
};

class RSAnimatableMatrix3f : public RSAnimatableArithmetic<RSAnimatableMatrix3f> {
public:
    RSAnimatableMatrix3f(const Matrix3f value) : value_(value) {}
    ~RSAnimatableMatrix3f() override = default;
    std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Multiply(const float scale) override;
    bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) override;
    Matrix3f GetValue();
private:
    Matrix3f value_;
};

class RSAnimatableQuaternion : public RSAnimatableArithmetic<RSAnimatableQuaternion> {
public:
    RSAnimatableQuaternion(const Quaternion value) : value_(value) {}
    ~RSAnimatableQuaternion() override = default;
    std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Multiply(const float scale) override;
    bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) override;
    Quaternion GetValue();
private:
    Quaternion value_;
};

class RSAnimatableFilter : public RSAnimatableArithmetic<RSAnimatableFilter> {
public:
    RSAnimatableFilter(const std::shared_ptr<RSFilter> value) : value_(value) {}
    ~RSAnimatableFilter() override = default;
    std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Multiply(const float scale) override;
    bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSFilter> GetValue();
private:
    std::shared_ptr<RSFilter> value_;
};

class RSAnimatableVector2f : public RSAnimatableArithmetic<RSAnimatableVector2f> {
public:
    RSAnimatableVector2f(const Vector2f value) : value_(value) {}
    ~RSAnimatableVector2f() override = default;
    std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Multiply(const float scale) override;
    bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) override;
    Vector2f GetValue();
private:
    Vector2f value_;
};

class RSAnimatableVector4f : public RSAnimatableArithmetic<RSAnimatableVector4f> {
public:
    RSAnimatableVector4f(const Vector4f value) : value_(value) {}
    ~RSAnimatableVector4f() override = default;
    std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Multiply(const float scale) override;
    bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) override;
    Vector4f GetValue();
private:
    Vector4f value_;
};

class RSAnimatableVector4Color : public RSAnimatableArithmetic<RSAnimatableVector4Color> {
public:
    RSAnimatableVector4Color(const Vector4<Color> value) : value_(value) {}
    ~RSAnimatableVector4Color() override = default;
    std::shared_ptr<RSAnimatableBase> Add(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Minus(const std::shared_ptr<RSAnimatableBase>& value) override;
    std::shared_ptr<RSAnimatableBase> Multiply(const float scale) override;
    bool IsEqual(const std::shared_ptr<RSAnimatableBase>& value) override;
    Vector4<Color> GetValue();
private:
    Vector4<Color> value_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_ARITHMETIC_H
