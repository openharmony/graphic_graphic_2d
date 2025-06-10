/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SOUND_WAVE_FILTER_RENDER_PROPERTY_H
#define RENDER_SOUND_WAVE_FILTER_RENDER_PROPERTY_H

#include "render/rs_render_filter_base.h"
namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSRenderSoundWaveFilterPara : public RSRenderFilterParaBase {

public:
    RSRenderSoundWaveFilterPara(PropertyId id)
    :RSRenderFilterParaBase(RSUIFilterType::SOUND_WAVE)
    {
        id_ = id;
    }

    virtual ~RSRenderSoundWaveFilterPara() = default;

    void GetDescription(std::string& out) const override;

    virtual bool WriteToParcel(Parcel& parcel) override;

    virtual bool ReadFromParcel(Parcel& parcel) override;

    static std::shared_ptr<RSRenderPropertyBase> CreateRenderProperty(RSUIFilterType type);

    virtual std::vector<std::shared_ptr<RSRenderPropertyBase>> GetLeafRenderProperties() override;
    float GetColorProgress() const;
    float GetSoundIntensity() const;
    float GetShockWaveAlphaA() const;
    float GetShockWaveAlphaB() const;
    float GetShockWaveProgressA() const;
    float GetShockWaveProgressB() const;
    float GetShockWaveTotalAlpha() const;

    bool ParseFilterValues() override;
    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;

private:
    void CalculateHash();
    static constexpr char GE_FILTER_SOUND_WAVE_COLOR_A[] = "COLORA";
    static constexpr char GE_FILTER_SOUND_WAVE_COLOR_B[] = "COLORB";
    static constexpr char GE_FILTER_SOUND_WAVE_COLOR_C[] = "COLORC";
    static constexpr char GE_FILTER_SOUND_WAVE_COLORPROGRESS[] = "COLORPROGRESS";
    static constexpr char GE_FILTER_SOUND_WAVE_SOUNDINTENSITY[] = "SOUNDINTENSITY";
    static constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_A[] = "SHOCKWAVEALPHAA";
    static constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_B[] = "SHOCKWAVEALPHAB";
    static constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_A[] = "SHOCKWAVEPROGRESSA";
    static constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_B[] = "SHOCKWAVEPROGRESSB";
    static constexpr char GE_FILTER_SOUND_WAVE_TOTAL_ALPHA[] = "SHOCKWAVETOTALALPHA";
    // sound wave
    Drawing::Color4f colorA_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    Drawing::Color4f colorB_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    Drawing::Color4f colorC_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    float colorProgress_ = 0.0f;
    float soundIntensity_ = 0.0f;

    // shock wave
    float shockWaveAlphaA_ = 1.0f;
    float shockWaveAlphaB_ = 1.0f;
    float shockWaveProgressA_ = 0.0f;
    float shockWaveProgressB_ = 0.0f;
    float shockWaveTotalAlpha_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SOUND_WAVE_FILTER_RENDER_PROPERTY_H