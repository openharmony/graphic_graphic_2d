/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef UIEFFECT_WAVE_DISTURBANCE_MASK_PARA_H
#define UIEFFECT_WAVE_DISTURBANCE_MASK_PARA_H
#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "ui_effect/utils.h"
 
namespace OHOS {
namespace Rosen {
// limits for wave disturbance mask animation progress parameters
constexpr std::pair<float, float> WAVE_DISTURBANCE_MASK_PROGRESS_LIMITS { 0.0f, 1.0f };
// limits for wave disturbance mask reflect ratio parameters
constexpr std::pair<float, float> WAVE_DISTURBANCE_MASK_RATIO_LIMITS { 1.0f, 20.0f };
// limits for wave disturbance mask down parameters
constexpr std::pair<float, float> WAVE_DISTURBANCE_MASK_DOWN_LIMITS { 0.f, 1.0f };
// limits for wave disturbance mask length parameters
constexpr std::pair<float, float> WAVE_DISTURBANCE_MASK_LENGTH_LIMITS { 0.0f, 800.0f };
// limits for wave disturbance mask width parameters
constexpr std::pair<float, float> WAVE_DISTURBANCE_MASK_WIDTH_LIMITS { 0.0f, 300.0f };
// limits for wave disturbance mask height parameters
constexpr std::pair<float, float> WAVE_DISTURBANCE_MASK_HEIGHT_LIMITS { 0.0f, 150.0f };

class WaveDisturbanceMaskPara : public MaskPara {
public:
    WaveDisturbanceMaskPara()
    {
        type_ = MaskPara::Type::WAVE_DISTURBANCE_MASK;
    }
    ~WaveDisturbanceMaskPara() override = default;
 
    void SetProgress(float progress)
    {
        progress_ = UIEffect::GetLimitedPara(progress, WAVE_DISTURBANCE_MASK_PROGRESS_LIMITS);
    }
 
    const float& GetProgress() const
    {
        return progress_;
    }
 
    void SetClickPos(Vector2f& clickPos)
    {
        clickPos_ = clickPos; // no invalid value
    }
 
    const Vector2f& GetClickPos() const
    {
        return clickPos_;
    }
 
    void SetWaveRD(Vector2f& waveRD)
    {
        waveRD_[0] = UIEffect::GetLimitedPara(waveRD[0], WAVE_DISTURBANCE_MASK_RATIO_LIMITS);
        waveRD_[1] = UIEffect::GetLimitedPara(waveRD[1], WAVE_DISTURBANCE_MASK_DOWN_LIMITS);
    }
 
    const Vector2f& GetWaveRD() const
    {
        return waveRD_;
    }
 
    void SetWaveLWH(Vector3f& waveLWH)
    {
        waveLWH_[0] = UIEffect::GetLimitedPara(waveLWH[0], WAVE_DISTURBANCE_MASK_LENGTH_LIMITS);
        waveLWH_[1] = UIEffect::GetLimitedPara(waveLWH[1], WAVE_DISTURBANCE_MASK_WIDTH_LIMITS);
        waveLWH_[2] = UIEffect::GetLimitedPara(waveLWH[2], WAVE_DISTURBANCE_MASK_HEIGHT_LIMITS); // 2: the third param
    }
 
    const Vector3f& GetWaveLWH() const
    {
        return waveLWH_;
    }
 
private:
    float progress_ = 0.5;
    Vector2f clickPos_ = { 0.0f, 0.0f };
    Vector2f waveRD_ = { 1.0f, 0.0f };
    Vector3f waveLWH_ = { 100.0f, 0.0f, 0.0f };
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_WAVE_DISTURBANCE_MASK_PARA_H