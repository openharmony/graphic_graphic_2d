/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
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
#include "include/effects/SkRuntimeEffect.h"
#include "tools/Resources.h"

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
class KawaseBlur {
public:
    explicit KawaseBlur();
    ~KawaseBlur();
    void ApplyKawaseBlur(
        SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst, const int radius);
    const sk_sp<SkImage> GetBlurImage(SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& dst) const;

private:
    static SkMatrix GetShaderTransform(const SkCanvas* canvas, const SkRect& blurRect, float scale);
    void ApplyBlur(
        SkCanvas& canvas, const sk_sp<SkImage>& image, const sk_sp<SkImage>& blurImage, const SkRect& dst) const;
    int GetDecelerateRadius(int radius);

    static constexpr float kBlurScale = 0.03f;
    // Maximum number of render passes
    static constexpr uint32_t kMaxPasses = 4;
    static constexpr uint32_t kMaxPassesLargeRadius = 7;
    static constexpr float kDilatedConvolution = 2.0f;
    static constexpr float kDilatedConvolutionLargeRadius = 4.0f;
    // To avoid downscaling artifacts, we interpolate the blurred fbo with the full composited
    // image, up to this radius.
    static constexpr float kMaxCrossFadeRadius = 10.0f;
    static constexpr int kMaxGaussRadius = 100;
    static constexpr int kMaxKawaseRadius = 300;
    static constexpr bool supporteLargeRadius = true;

    sk_sp<SkRuntimeEffect> blurEffect_;
    sk_sp<SkRuntimeEffect> mixEffect_;
    float blurRadius_;
    float blurScale_;
};
#endif
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_KAWASE_BLUR_H