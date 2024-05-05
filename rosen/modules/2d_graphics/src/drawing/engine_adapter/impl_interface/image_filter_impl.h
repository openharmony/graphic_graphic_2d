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

#ifndef IMAGE_FILTER_IMPL_H
#define IMAGE_FILTER_IMPL_H

#include <memory>
#include <vector>

#include "base_impl.h"

#include "effect/shader_effect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;
class ImageFilter;
class ColorFilter;
enum class GradientDir : uint8_t {
    LEFT = 0,
    TOP,
    RIGHT,
    BOTTOM,
    LEFT_TOP,
    LEFT_BOTTOM,
    RIGHT_TOP,
    RIGHT_BOTTOM,
};

enum class GradientBlurType : uint8_t {
    AlPHA_BLEND = 0,
    RADIUS_GRADIENT,
};

enum class ImageBlurType : uint8_t {
    KAWASE = 0,
    GAUSS,
};

class ImageFilterImpl : public BaseImpl {
public:
    ImageFilterImpl() noexcept {}
    ~ImageFilterImpl() override {}

    virtual void InitWithBlur(scalar sigmaX, scalar sigmaY, TileMode mode, const std::shared_ptr<ImageFilter> f,
        ImageBlurType blurType) = 0;
    virtual void InitWithColor(const ColorFilter& colorFilter, const std::shared_ptr<ImageFilter> f) = 0;
    virtual void InitWithColorBlur(const ColorFilter& colorFilter, scalar sigmaX, scalar sigmaY,
        ImageBlurType blurType) = 0;
    virtual void InitWithOffset(scalar dx, scalar dy, const std::shared_ptr<ImageFilter> f) = 0;
    virtual void InitWithArithmetic(const std::vector<scalar>& coefficients, bool enforcePMColor,
        const std::shared_ptr<ImageFilter> f1, const std::shared_ptr<ImageFilter> f2) = 0;
    virtual void InitWithCompose(const std::shared_ptr<ImageFilter> f1, const std::shared_ptr<ImageFilter> f2) = 0;
    virtual void InitWithGradientBlur(float radius, const std::vector<std::pair<float, float>>& fractionStops,
        GradientDir direction, GradientBlurType blurType,
        const std::shared_ptr<ImageFilter> f) = 0;
    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;
    virtual void InitWithBlend(BlendMode mode, std::shared_ptr<ImageFilter> background,
        std::shared_ptr<ImageFilter> foreground = nullptr) = 0;
    virtual void InitWithShader(std::shared_ptr<ShaderEffect> shader, const Rect& rect) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif