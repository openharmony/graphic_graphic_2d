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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H

#include "render/rs_gradient_blur_para.h"
#include "render/rs_shader_filter.h"


namespace OHOS {
namespace Rosen {
class RSLinearGradientBlurShaderFilter : public RSShaderFilter {
public:
    RSLinearGradientBlurShaderFilter(const std::shared_ptr<RSLinearGradientBlurPara>& para,
        const float geoWidth, const float geoHeight);
    RSLinearGradientBlurShaderFilter(const RSLinearGradientBlurShaderFilter&) = delete;
    RSLinearGradientBlurShaderFilter operator=(const RSLinearGradientBlurShaderFilter&) = delete;
    ~RSLinearGradientBlurShaderFilter() override;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;
    void SetGeometry(Drawing::Canvas& canvas, float geoWidth, float geoHeight)
    {
        auto dst = canvas.GetDeviceClipBounds();
        tranX_ = dst.GetLeft();
        tranY_ = dst.GetTop();
        mat_ = canvas.GetTotalMatrix();
        geoWidth_ = std::ceil(geoWidth);
        geoHeight_ = std::ceil(geoHeight);
    }
    
    void IsOffscreenCanvas(bool isOffscreenCanvas)
    {
        isOffscreenCanvas_ = isOffscreenCanvas;
    }
    float GetLinearGradientBlurRadius() const;
private:
    friend class RSMarshallingHelper;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
    inline static float imageScale_ = 1.f;
    inline static float geoWidth_ = 0.f;
    inline static float geoHeight_ = 0.f;
    inline static float tranX_ = 0.f;
    inline static float tranY_ = 0.f;
    inline static bool isOffscreenCanvas_ = true;
    inline static Drawing::Matrix mat_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H