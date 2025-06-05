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

#include "include/core/SkColorFilter.h"
#ifdef NEW_SKIA
#include "include/effects/SkRuntimeEffect.h"
#endif

#include "property/rs_properties_painter.h"
#include "common/rs_macros.h"
#include "render/rs_skia_filter.h"
#include "render/rs_kawase_blur.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSBlurFilter : public RSDrawingFilterOriginal {
public:
    RSBlurFilter(float blurRadiusX, float blurRadiusY, bool disableSystemAdaptation = true);
    RSBlurFilter(const RSBlurFilter&) = delete;
    RSBlurFilter operator=(const RSBlurFilter&) = delete;
    ~RSBlurFilter() override;
    float GetBlurRadiusX();
    float GetBlurRadiusY();
    bool GetDisableSystemAdaptation();
    bool IsValid() const override;
    std::shared_ptr<RSDrawingFilterOriginal> Compose(
        const std::shared_ptr<RSDrawingFilterOriginal>& other) const override;
    std::string GetDescription() override;
    std::string GetDetailedDescription() override;

    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
    void SetGreyCoef(const std::optional<Vector2f>& greyCoef) override;

    bool CanSkipFrame() const override;

private:
    float blurRadiusX_;
    float blurRadiusY_;
    std::optional<Vector2f> greyCoef_;
    bool disableSystemAdaptation_ {true};
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H