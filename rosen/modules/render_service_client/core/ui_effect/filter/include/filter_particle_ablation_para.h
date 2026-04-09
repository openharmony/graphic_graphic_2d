/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_FILTER_PARTICLE_ABLATION_PARA_H
#define UIEFFECT_FILTER_PARTICLE_ABLATION_PARA_H

#include "pixel_map.h"
#include "common/rs_macros.h"
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT ParticleAblationPara : public FilterPara {
public:
    ParticleAblationPara()
    {
        this->type_ = FilterPara::ParaType::PARTICLE_ABLATION;
    }
    ~ParticleAblationPara() override = default;

    void SetProgress(float progress)
    {
        progress_ = progress;
    }

    float GetProgress() const
    {
        return progress_;
    }

    void SetAblationRate(float ablationRate)
    {
        ablationRate_ = ablationRate;
    }

    float GetAblationRate() const
    {
        return ablationRate_;
    }

    void SetCenters0(Vector2f& centers0)
    {
        centers0_ = centers0;
    }

    const Vector2f& GetCenters0() const
    {
        return centers0_;
    }

    void SetCenters1(Vector2f& centers1)
    {
        centers1_ = centers1;
    }

    const Vector2f& GetCenters1() const
    {
        return centers1_;
    }

    void SetCenters2(Vector2f& centers2)
    {
        centers2_ = centers2;
    }

    const Vector2f& GetCenters2() const
    {
        return centers2_;
    }

    void SetCenters3(Vector2f& centers3)
    {
        centers3_ = centers3;
    }

    const Vector2f& GetCenters3() const
    {
        return centers3_;
    }

    void SetGlowLevel(float glowLevel)
    {
        glowLevel_ = glowLevel;
    }

    float GetGlowLevel() const
    {
        return glowLevel_;
    }

    void SetGlowBrightness(float glowBrightness)
    {
        glowBrightness_ = glowBrightness;
    }

    float GetGlowBrightness() const
    {
        return glowBrightness_;
    }

    void SetMaxParticleCount(int32_t maxParticleCount)
    {
        maxParticleCount_ = maxParticleCount;
    }

    int32_t GetMaxParticleCount() const
    {
        return maxParticleCount_;
    }

    void SetWind(Vector2f& wind)
    {
        wind_ = wind;
    }

    const Vector2f& GetWind() const
    {
        return wind_;
    }

    void SetTurbScale(int32_t turbScale)
    {
        turbScale_ = turbScale;
    }

    int32_t GetTurbScale() const
    {
        return turbScale_;
    }

    void SetTurbEvo(int32_t turbEvo)
    {
        turbEvo_ = turbEvo;
    }

    int32_t GetTurbEvo() const
    {
        return turbEvo_;
    }

    void SetTurbAmp(int32_t turbAmp)
    {
        turbAmp_ = turbAmp;
    }

    int32_t GetTurbAmp() const
    {
        return turbAmp_;
    }

    void SetScaleSize(Vector2f& scaleSize)
    {
        scaleSize_ = scaleSize;
    }

    const Vector2f& GetScaleSize() const
    {
        return scaleSize_;
    }

private:
    float progress_ = 0.0f;
    float ablationRate_ = 0.0f;
    Vector2f centers0_ = { 0.0f, 0.0f };
    Vector2f centers1_ = { 0.0f, 0.0f };
    Vector2f centers2_ = { 0.0f, 0.0f };
    Vector2f centers3_ = { 0.0f, 0.0f };
    float glowLevel_ = 0.0f;
    float glowBrightness_ = 0.0f;
    int32_t maxParticleCount_ = 0;
    Vector2f wind_ = { 0.0f, 0.0f };
    int32_t turbScale_ = 0;
    int32_t turbEvo_ = 0;
    int32_t turbAmp_ = 0;
    Vector2f scaleSize_ = { 1.0f, 1.0f };
};

} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_PARTICLE_ABLATION_PARA_H
