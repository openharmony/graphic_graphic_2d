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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H

#include "include/effects/SkRuntimeEffect.h"
#include "common/rs_macros.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
class RSB_EXPORT RSBlurFilter : public RSSkiaFilter {
#else
class RSB_EXPORT RSBlurFilter : public RSDrawingFilter {
#endif
public:
    RSBlurFilter(float blurRadiusX, float blurRadiusY);
    RSBlurFilter(const RSBlurFilter&) = delete;
    RSBlurFilter operator=(const RSBlurFilter&) = delete;
    ~RSBlurFilter() override;
    float GetBlurRadiusX();
    float GetBlurRadiusY();
    bool IsValid() const override;
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<RSSkiaFilter> Compose(const std::shared_ptr<RSSkiaFilter>& other) const override;
#else
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& other) const override;
#endif
    std::string GetDescription() override;

    std::shared_ptr<RSFilter> Add(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Sub(const std::shared_ptr<RSFilter>& rhs) override;
    std::shared_ptr<RSFilter> Multiply(float rhs) override;
    std::shared_ptr<RSFilter> Negate() override;
    bool IsNearEqual(
        const std::shared_ptr<RSFilter>& other, float threshold = std::numeric_limits<float>::epsilon()) const override;
    bool IsNearZero(float threshold = std::numeric_limits<float>::epsilon()) const override;
    
    void DrawImageRect(
        SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const override;

private:
    float blurRadiusX_;
    float blurRadiusY_;
    bool useKawase = true;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H