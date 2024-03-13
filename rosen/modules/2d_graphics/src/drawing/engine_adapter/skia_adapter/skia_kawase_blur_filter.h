/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef SKIA_KAWASE_BLUR_FILTER_H
#define SKIA_KAWASE_BLUR_FILTER_H

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "image/image.h"
#include "utils/kawase_blur_utils.h"
#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaKawaseBlurFilter {
public:
    bool ApplyKawaseBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const KawaseParameters& param, const KawaseProperties& properties);

    static SkiaKawaseBlurFilter* GetKawaseBlurFilter()
    {
        static thread_local SkiaKawaseBlurFilter* filter;
        if (filter == nullptr) {
            filter = new SkiaKawaseBlurFilter();
        }
        return filter;
    }

private:
    struct BlurParams {
        int numberOfPasses = 1;
        int width = 0;
        int height = 0;
        float radiusByPass = 0.f;
    };

    SkiaKawaseBlurFilter();
    ~SkiaKawaseBlurFilter();
    SkiaKawaseBlurFilter(const SkiaKawaseBlurFilter& filter);
    const SkiaKawaseBlurFilter &operator=(const SkiaKawaseBlurFilter& filter);

    void SetupBlurEffect();
    void SetupMixEffect();
    static Drawing::Matrix GetShaderTransform(const Drawing::Canvas* canvas, const Drawing::Rect& blurRect,
        float scaleW = 1.0f, float scaleH = 1.0f);
    void CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const KawaseParameters& param, std::shared_ptr<Drawing::Image>& checkedImage);
    void OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const KawaseParameters& param);
    std::shared_ptr<Drawing::Image> ExecutePingPongBlur(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const KawaseParameters& kParam, const BlurParams& bParam,
        const KawaseProperties& properties) const;
    bool ApplyBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const std::shared_ptr<Drawing::Image>& blurImage, const KawaseParameters& param,
        const KawaseProperties& properties) const;
    void ComputeRadiusAndScale(int radius);
    void AdjustRadiusAndScale();
    // Advanced Filter
    void SetupBlurEffectAdvancedFilter();

    static constexpr float baseBlurScale = 0.5f; // base downSample radio
    static constexpr uint32_t kMaxPasses = 4; // Maximum number of render passes
    static constexpr uint32_t kMaxPassesLargeRadius = 7;
    static constexpr float kDilatedConvolution = 2.0f;
    static constexpr float kDilatedConvolutionLargeRadius = 4.6f;
    // To avoid downscaling artifacts, interpolate the blurred fbo with the full composited image, up to this radius
    static constexpr float kMaxCrossFadeRadius = 10.0f;
    static constexpr bool supportLargeRadius = true;

    std::shared_ptr<Drawing::RuntimeEffect> blurEffect_;
    std::shared_ptr<Drawing::RuntimeEffect> mixEffect_;
    // Advanced Filter
    std::shared_ptr<Drawing::RuntimeEffect> blurEffectAF_;

    float blurRadius_ = 0.f;
    float blurScale_ = 0.25f;
    int numberOfPasses_ = 1;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_KAWASE_BLUR_FILTER_H