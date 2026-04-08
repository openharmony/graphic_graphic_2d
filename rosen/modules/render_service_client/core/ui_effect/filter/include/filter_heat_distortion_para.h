/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_FILTER_HEAT_DISTORTION_PARA_H
#define UIEFFECT_FILTER_HEAT_DISTORTION_PARA_H

#include "filter_para.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT HeatDistortionPara : public FilterPara {
public:
    HeatDistortionPara()
    {
        this->type_ = FilterPara::ParaType::HEAT_DISTORTION;
    }
    ~HeatDistortionPara() override = default;

    void SetIntensity(float intensity)
    {
        intensity_ = intensity;
    }

    float GetIntensity() const
    {
        return intensity_;
    }

    void SetRiseSpeed(float riseSpeed)
    {
        riseSpeed_ = riseSpeed;
    }

    float GetRiseSpeed() const
    {
        return riseSpeed_;
    }

    void SetNoiseScale(float noiseScale)
    {
        noiseScale_ = noiseScale;
    }

    float GetNoiseScale() const
    {
        return noiseScale_;
    }

    void SetNoiseSpeed(float noiseSpeed)
    {
        noiseSpeed_ = noiseSpeed;
    }

    float GetNoiseSpeed() const
    {
        return noiseSpeed_;
    }

    void SetRiseWeight(float riseWeight)
    {
        riseWeight_ = riseWeight;
    }

    float GetRiseWeight() const
    {
        return riseWeight_;
    }

private:
    float intensity_ = 1.0f;
    float riseSpeed_ = 1.0f;
    float noiseScale_ = 1.0f;
    float noiseSpeed_ = 0.4f;
    float riseWeight_ = 0.2f;
};
} // namespace Rosen
} // namespace OHOS

#endif // UIEFFECT_FILTER_HEAT_DISTORTION_PARA_H
