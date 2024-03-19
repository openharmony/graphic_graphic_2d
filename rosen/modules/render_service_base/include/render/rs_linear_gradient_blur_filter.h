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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_FILTER_H

#include "render/rs_skia_filter.h"
#include "render/rs_gradient_blur_para.h"

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSLinearGradientBlurFilter : public RSDrawingFilter {
public:
    RSLinearGradientBlurFilter(const std::shared_ptr<RSLinearGradientBlurPara>& para, const float geoWidth,
        const float geoHeight);
    RSLinearGradientBlurFilter(const RSLinearGradientBlurFilter&) = delete;
    RSLinearGradientBlurFilter operator=(const RSLinearGradientBlurFilter&) = delete;
    ~RSLinearGradientBlurFilter() override;

    void PostProcess(RSPaintFilterCanvas& canvas) override {};
    std::string GetDescription() override;
    void SetBoundsGeometry(float geoWidth, float geoHeight) override
    {
        geoWidth_ = geoWidth;
        geoHeight_ = geoHeight;
    }

    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& other) const override
    {
        return nullptr;
    }
    void SetCanvasChange(Drawing::Matrix& mat, float surfaceWidth, float surfaceHeight) override
    {
        mat_ = mat;
        surfaceWidth_ = surfaceWidth;
        surfaceHeight_ = surfaceHeight;
    }

private:
    friend class RSMarshallingHelper;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
    inline static float imageScale_ = 1.f;
    inline static float geoWidth_ = 0.f;
    inline static float geoHeight_ = 0.f;
    inline static float surfaceWidth_ = 0.f;
    inline static float surfaceHeight_ = 0.f;

    static Drawing::Rect ComputeRectBeforeClip(const uint8_t directionBias, const Drawing::Rect& dst);
    static uint8_t CalcDirectionBias(const Drawing::Matrix& mat);

    inline static Drawing::Matrix mat_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_FILTER_H