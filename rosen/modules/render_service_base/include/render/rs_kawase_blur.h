/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#include "include/core/SkData.h"
#include "include/core/SkPaint.h"
#include "include/core/SkSize.h"
#include "include/core/SkString.h"
#include "include/core/SkSurface.h"
#ifdef NEW_SKIA
#include "include/effects/SkRuntimeEffect.h"
#endif
#include "tools/Resources.h"
#else
#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"
#endif

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
struct KawaseParameter {
    SkRect src;
    SkRect dst;
    int radius;
    sk_sp<SkColorFilter> colorFilter;
    float alpha;

    KawaseParameter(const SkRect& s, const SkRect& d, int r, sk_sp<SkColorFilter> color = nullptr, float a = 0.f)
        : src(s), dst(d), radius(r), colorFilter(color), alpha(a) {}
};
#else
struct KawaseParameter {
    Drawing::Rect src;
    Drawing::Rect dst;
    int radius;
    std::shared_ptr<Drawing::ColorFilter> colorFilter;
    float alpha = 0.f;

    KawaseParameter(const Drawing::Rect& s, const Drawing::Rect& d, int r,
        std::shared_ptr<Drawing::ColorFilter> color = nullptr, float a = 0.f)
        : src(s), dst(d), radius(r), colorFilter(color), alpha(a) {}
};
#endif
class KawaseBlurFilter {
public:
#ifndef USE_ROSEN_DRAWING
    bool ApplyKawaseBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param);
#else
    bool ApplyKawaseBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const KawaseParameter& param);
#endif
    static KawaseBlurFilter* GetKawaseBlurFilter()
    {
        static thread_local KawaseBlurFilter* filter;
        if (filter == nullptr) {
            filter = new KawaseBlurFilter();
        }
        return filter;
    }

private:
    KawaseBlurFilter();
    ~KawaseBlurFilter();
    KawaseBlurFilter(const KawaseBlurFilter& filter);
    const KawaseBlurFilter &operator=(const KawaseBlurFilter& filter);
#ifndef USE_ROSEN_DRAWING
    static SkMatrix GetShaderTransform(const SkCanvas* canvas, const SkRect& blurRect, float scale = 1.0f);
    void CheckInputImage(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param,
        sk_sp<SkImage>& checkedImage);
    void OutputOriginalImage(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param);
    bool ApplyBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const sk_sp<SkImage>& blurImage,
        const KawaseParameter& param) const;
#else
    static Drawing::Matrix GetShaderTransform(const Drawing::Canvas* canvas, const Drawing::Rect& blurRect,
        float scaleW = 1.0f, float scaleH = 1.0f);
    void CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const KawaseParameter& param, std::shared_ptr<Drawing::Image>& checkedImage);
    void OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const KawaseParameter& param);
    bool ApplyBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const std::shared_ptr<Drawing::Image>& blurImage, const KawaseParameter& param) const;
#endif
    void ComputeRadiusAndScale(int radius);
    void AdjustRadiusAndScale();
    std::string GetDescription() const;

    static constexpr float baseBlurScale = 0.5f; // base downSample radio
    static constexpr uint32_t kMaxPasses = 4; // Maximum number of render passes
    static constexpr uint32_t kMaxPassesLargeRadius = 7;
    static constexpr float kDilatedConvolution = 2.0f;
    static constexpr float kDilatedConvolutionLargeRadius = 4.6f;
    // To avoid downscaling artifacts, interpolate the blurred fbo with the full composited image, up to this radius
    static constexpr float kMaxCrossFadeRadius = 10.0f;
    static constexpr bool supportLargeRadius = true;

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    sk_sp<SkRuntimeEffect> blurEffect_;
    sk_sp<SkRuntimeEffect> mixEffect_;
#endif
#else
    std::shared_ptr<Drawing::RuntimeEffect> blurEffect_;
    std::shared_ptr<Drawing::RuntimeEffect> mixEffect_;
#endif
    float blurRadius_ = 0.f;
    float blurScale_ = 0.25f;

    // Advanced Filter
    void setupBlurEffectAdvancedFilter();
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkRuntimeEffect> blurEffectAF_;
#else
    std::shared_ptr<Drawing::RuntimeEffect> blurEffectAF_;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H
