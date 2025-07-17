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
#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H

#include "render/rs_gradient_blur_para.h"
#include "render/rs_render_filter_base.h"


namespace OHOS {
namespace Rosen {
class RSLinearGradientBlurShaderFilter : public RSRenderFilterParaBase {
public:
    RSLinearGradientBlurShaderFilter(const std::shared_ptr<RSLinearGradientBlurPara>& para,
        const float geoWidth, const float geoHeight);
    RSLinearGradientBlurShaderFilter(const RSLinearGradientBlurShaderFilter&) = delete;
    RSLinearGradientBlurShaderFilter operator=(const RSLinearGradientBlurShaderFilter&) = delete;
    ~RSLinearGradientBlurShaderFilter() override = default;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;
    
    void IsOffscreenCanvas(bool isOffscreenCanvas)
    {
        isOffscreenCanvas_ = isOffscreenCanvas;
    }
    float GetLinearGradientBlurRadius() const;
private:
    friend class RSMarshallingHelper;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
    inline static float imageScale_ = 1.f;
    inline static bool isOffscreenCanvas_ = true;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H