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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_WATER_RIPPLE_SHADER_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_WATER_RIPPLE_SHADER_FILTER_H
 
#include "render/rs_drawing_filter.h"
#include "property/rs_properties.h"
 
namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSWaterRippleShaderFilter : public RSShaderFilter {
public:
    RSWaterRippleShaderFilter(const float progress, const float waveCount, const float rippleCenterX,
        const float rippleCenterY);
    ~RSWaterRippleShaderFilter() override;
    float GetProgress() const;
    float GetwaveCount() const;
    float GetRippleCenterX() const;
    float GetRippleCenterY() const;
    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;
 
private:
    inline static float progress_ = 0.0f;
    inline static float waveCount_ = 0.0f;
    inline static float rippleCenterX_ = 0.0f;
    inline static float rippleCenterY_ = 0.0f;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS
 
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_WATER_RIPPLE_SHADER_FILTER_H