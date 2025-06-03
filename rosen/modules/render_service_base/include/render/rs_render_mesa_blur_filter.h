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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MESA_BLUR_SHADER_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MESA_BLUR_SHADER_FILTER_H

#include "rs_pixel_stretch_params.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_render_filter_base.h"
#include <mutex>

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSMESABlurShaderFilter : public RSRenderFilterParaBase {
public:
    RSMESABlurShaderFilter(int radius);
    RSMESABlurShaderFilter(int radius, float greyCoefLow, float greyCoefHigh);
    ~RSMESABlurShaderFilter() override;
    int GetRadius() const;
    void CalculateHash();
    void SetPixelStretchParams(std::shared_ptr<RSPixelStretchParams>& param);
    void SetRadius(int radius);
    std::string GetDetailedDescription() const;
    std::shared_ptr<RSPixelStretchParams> GetPixelStretchParams() const
    {
        std::lock_guard<std::mutex> lock(pixelStretchParamsMutex_);
        return pixelStretchParam_;
    }
    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;

private:
    int radius_ = 0;
    float greyCoefLow_ = 0.f;
    float greyCoefHigh_ = 0.f;
    mutable std::mutex pixelStretchParamsMutex_;
    std::shared_ptr<RSPixelStretchParams> pixelStretchParam_ = nullptr;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_MESA_BLUR_SHADER_FILTER_H
