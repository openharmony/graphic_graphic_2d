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

#include "render/rs_render_filter_base.h"
 
namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSWaterRippleShaderFilter : public RSRenderFilterParaBase {
public:
    RSWaterRippleShaderFilter(const float progress, const uint32_t waveCount, const float rippleCenterX,
        const float rippleCenterY, const uint32_t rippleMode);
    ~RSWaterRippleShaderFilter() override = default;
    float GetProgress() const;
    uint32_t GetWaveCount() const;
    float GetRippleCenterX() const;
    float GetRippleCenterY() const;
    uint32_t GetRippleMode() const;
    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;

private:
    static constexpr char RS_FILTER_WATER_RIPPLE_PROGRESS[] = "PROGRESS";
    static constexpr char RS_FILTER_WATER_RIPPLE_WAVE_NUM[] = "WAVE_NUM";
    static constexpr char RS_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X[] = "RIPPLE_CENTER_X";
    static constexpr char RS_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y[] = "RIPPLE_CENTER_Y";
    static constexpr char RS_FILTER_WATER_RIPPLE_RIPPLE_MODE[] = "RIPPLE_MODE";
    float progress_ = 0.0f;
    uint32_t waveCount_ = 0.0f;
    float rippleCenterX_ = 0.0f;
    float rippleCenterY_ = 0.0f;
    uint32_t rippleMode_ = 0.0f;
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS
 
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_WATER_RIPPLE_SHADER_FILTER_H