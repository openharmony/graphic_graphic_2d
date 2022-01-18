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

#ifndef COLOR_FILTER_IMPL_H
#define COLOR_FILTER_IMPL_H

#include "base_impl.h"

#include "draw/blend_mode.h"
#include "draw/color.h"
#include "effect/color_matrix.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorFilter;
class ColorFilterImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    ColorFilterImpl() noexcept {}
    virtual ~ColorFilterImpl() {}
    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }

    virtual void InitWithBlendMode(ColorQuad c, BlendMode mode) = 0;
    virtual void InitWithColorMatrix(ColorMatrix& m) = 0;
    virtual void InitWithLinearToSrgbGamma() = 0;
    virtual void InitWithSrgbGammaToLinear() = 0;
    virtual void InitWithCompose(const ColorFilter& f1, const ColorFilter& f2) = 0;
    virtual void Compose(const ColorFilter& f) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif