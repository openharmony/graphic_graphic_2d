/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "common/rs_color.h"
#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RSB_EXPORT RSArithmetic {
public:
    RSArithmetic() = default;
    virtual ~RSArithmetic() = default;

    virtual bool IsEqual(const T& value) const = 0;

    bool operator==(const T& value) const
    {
        return IsEqual(value);
    }

    bool operator!=(const T& value) const
    {
        return !IsEqual(value);
    }
};

template<typename T>
class RSB_EXPORT RSAnimatableArithmetic : public RSArithmetic<T> {
public:
    RSAnimatableArithmetic() = default;
    virtual ~RSAnimatableArithmetic() = default;

    virtual T Add(const T& value) const = 0;
    virtual T Minus(const T& value) const = 0;
    virtual T Multiply(const float scale) const = 0;

    T operator+(const T& value) const
    {
        return Add(value);
    }
    T operator+=(const T& value) const
    {
        return Add(value);
    }
    T operator-(const T& value) const
    {
        return Minus(value);
    }
    T operator-=(const T& value) const
    {
        return Minus(value);
    }
    T operator*(const float scale) const
    {
        return Multiply(scale);
    }
    T operator*=(const float scale) const
    {
        return Multiply(scale);
    }
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_ARITHMETIC_H
