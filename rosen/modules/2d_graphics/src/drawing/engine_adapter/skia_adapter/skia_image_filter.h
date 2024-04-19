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

#ifndef SKIA_IMAGE_FILTER_H
#define SKIA_IMAGE_FILTER_H

#include "include/core/SkColorFilter.h"
#include "include/core/SkImageFilter.h"

#include "impl_interface/image_filter_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImageFilter : public ImageFilterImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaImageFilter() noexcept;
    ~SkiaImageFilter() override {};

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void InitWithBlur(scalar sigmaX, scalar sigmaY, TileMode mode, const std::shared_ptr<ImageFilter> f,
        ImageBlurType blurType) override;
    void InitWithColor(const ColorFilter& colorFilter, const std::shared_ptr<ImageFilter> f) override;
    void InitWithColorBlur(const ColorFilter& colorFilter, scalar sigmaX, scalar sigmaY,
        ImageBlurType blurType) override;
    void InitWithOffset(scalar dx, scalar dy, const std::shared_ptr<ImageFilter> f) override;
    void InitWithArithmetic(const std::vector<scalar>& coefficients, bool enforcePMColor,
        const std::shared_ptr<ImageFilter> f1, const std::shared_ptr<ImageFilter> f2) override;
    void InitWithCompose(const std::shared_ptr<ImageFilter> f1, const std::shared_ptr<ImageFilter> f2) override;
    void InitWithGradientBlur(float radius, const std::vector<std::pair<float, float>>& fractionStops,
        GradientDir direction, GradientBlurType blurType,
        const std::shared_ptr<ImageFilter> f) override;
    void InitWithBlend(BlendMode mode, std::shared_ptr<ImageFilter> background,
        std::shared_ptr<ImageFilter> foreground = nullptr) override;
    void InitWithShader(std::shared_ptr<ShaderEffect> shader, const Rect& rect) override;
    sk_sp<SkImageFilter> GetImageFilter() const;
    /*
     * @brief  Update the member variable to filter, adaptation layer calls.
     */
    void SetSkImageFilter(const sk_sp<SkImageFilter>& filter);

    std::shared_ptr<Data> Serialize() const override;
    bool Deserialize(std::shared_ptr<Data> data) override;
private:
    sk_sp<SkImageFilter> filter_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif