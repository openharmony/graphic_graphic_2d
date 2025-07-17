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
#ifndef UIEFFECT_FILTER_WATER_RIPPPLE_PARA_H
#define UIEFFECT_FILTER_WATER_RIPPPLE_PARA_H
#include <iostream>
#include "filter_para.h"
#include "common/rs_vector2.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT WaterRipplePara : public FilterPara {
public:
    WaterRipplePara()
    {
        this->type_ = FilterPara::ParaType::WATER_RIPPLE;
    }
    ~WaterRipplePara() override = default;

    WaterRipplePara(const WaterRipplePara& other);

    void SetProgress(float progress)
    {
        progress_ = progress;
    }

    float GetProgress() const
    {
        return progress_;
    }

    void SetWaveCount(uint32_t waveCount)
    {
        waveCount_ = waveCount;
    }

    uint32_t GetWaveCount () const
    {
        return waveCount_;
    }

    void SetRippleCenterX(float rippleCenterX)
    {
        rippleCenterX_ = rippleCenterX;
    }

    float GetRippleCenterX() const
    {
        return rippleCenterX_;
    }

    void SetRippleCenterY(float rippleCenterY)
    {
        rippleCenterY_ = rippleCenterY;
    }

    float GetRippleCenterY() const
    {
        return rippleCenterY_;
    }

    void SetRippleMode(uint32_t rippleMode)
    {
        rippleMode_ = rippleMode;
    }

    uint32_t GetRippleMode() const
    {
        return rippleMode_;
    }

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val);

    std::shared_ptr<FilterPara> Clone() const override;

private:
    float rippleCenterX_ = 0.0f;
    float rippleCenterY_ = 0.0f;
    float progress_ = 0.0f;
    uint32_t waveCount_ = 0;
    uint32_t rippleMode_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_WATER_RIPPPLE_PARA_H
