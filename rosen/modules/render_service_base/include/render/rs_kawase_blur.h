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
class KawaseBlurFilter {
public:
    explicit KawaseBlurFilter();
    ~KawaseBlurFilter();
    bool ApplyKawaseBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const KawaseParameter& param);

private:
    static SkMatrix GetShaderTransform(const SkCanvas* canvas, const SkRect& blurRect, float scale = 1.0f);
    bool ApplyBlur(SkCanvas& canvas, const sk_sp<SkImage>& image, const sk_sp<SkImage>& blurImage,
        const KawaseParameter& param) const;
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

#ifdef NEW_SKIA
    sk_sp<SkRuntimeEffect> blurEffect_;
    sk_sp<SkRuntimeEffect> mixEffect_;
#endif
    float blurRadius_ = 0.f;
    float blurScale_ = 0.25f;
};
#endif
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H
