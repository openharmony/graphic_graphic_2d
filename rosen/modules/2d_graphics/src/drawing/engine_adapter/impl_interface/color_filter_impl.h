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

#ifndef COLOR_FILTER_IMPL_H
#define COLOR_FILTER_IMPL_H

#include "base_impl.h"

#include "draw/blend_mode.h"
#include "draw/color.h"
#include "effect/color_matrix.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;
class ColorFilter;
class ColorFilterImpl : public BaseImpl {
public:
    // Color matrix is a 4x5 float type matrix.
    constexpr static int MATRIX_SIZE = 20;
    constexpr static int OVER_DRAW_COLOR_NUM = 6;
    ColorFilterImpl() noexcept {}
    ~ColorFilterImpl() override {}

    virtual void InitWithBlendMode(ColorQuad c, BlendMode mode) = 0;
    virtual void InitWithColorMatrix(const ColorMatrix& m) = 0;
    virtual void InitWithColorFloat(const float f[20]) = 0;
    virtual void InitWithLinearToSrgbGamma() = 0;
    virtual void InitWithSrgbGammaToLinear() = 0;
    virtual void InitWithCompose(const ColorFilter& f1, const ColorFilter& f2) = 0;
    virtual void InitWithCompose(const float f1[MATRIX_SIZE], const float f2[MATRIX_SIZE]) = 0;
    virtual void InitWithOverDrawColor(const ColorQuad colors[OVER_DRAW_COLOR_NUM]) = 0;
    virtual void Compose(const ColorFilter& f) = 0;
    virtual void InitWithLuma() = 0;
    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;
    virtual bool AsAColorMatrix(scalar matrix[MATRIX_SIZE]) const = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif