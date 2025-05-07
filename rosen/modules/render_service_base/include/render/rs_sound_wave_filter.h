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
#ifndef RENDER_SERVICE_BASE_RENDER_RS_SOUND_WAVE_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RS_SOUND_WAVE_FILTER_H

#include <memory>
#include "common/rs_color_palette.h"
#include "render/rs_shader_filter.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSSoundWaveFilter : public RSShaderFilter {
public:
    RSSoundWaveFilter(RSColor colorOne, RSColor colorTwo, RSColor colorThree,
                      float colorProgress, float centerBrightness, float soundIntensity,
                      float shockWaveAlphaOne, float shockWaveAlphaTwo,
                      float shockWaveProgressOne, float shockWaveProgressTwo);
    RSSoundWaveFilter(const RSSoundWaveFilter&) = delete;
    RSSoundWaveFilter operator=(const RSSoundWaveFilter&) = delete;
    ~RSSoundWaveFilter() override = default;

    float GetColorProgress() const;
    float GetCenterBrightness() const;
    float GetSoundIntensity() const;
    float GetShockWaveAlphaOne() const;
    float GetShockWaveAlphaTwo() const;
    float GetShockWaveProgressOne() const;
    float GetShockWaveProgressTwo() const;

    void GenerateGEVisualEffect(std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer) override;

private:
    static constexpr char GE_FILTER_SOUND_WAVE_COLORONE[] = "COLORONE";
    static constexpr char GE_FILTER_SOUND_WAVE_COLORTWO[] = "COLORTWO";
    static constexpr char GE_FILTER_SOUND_WAVE_COLORTHREE[] = "COLORTHREE";
    static constexpr char GE_FILTER_SOUND_WAVE_COLORPROGRESS[] = "COLORPROGRESS";
    static constexpr char GE_FILTER_SOUND_WAVE_CENTERBRIGHTNESS[] = "CENTERBRIGHTNESS";
    static constexpr char GE_FILTER_SOUND_WAVE_SOUNDINTENSITY[] = "SOUNDINTENSITY";
    static constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHAONE[] = "SHOCKWAVEALPHAONE";
    static constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHATWO[] = "SHOCKWAVEALPHATWO";
    static constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESSONE[] = "SHOCKWAVEPROGRESSONE";
    static constexpr char GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESSTWO[] = "SHOCKWAVEPROGRESSTWO";
    //sound wave
    RSColor colorOne_ = RSColor(0xFFFFFFFF);
    RSColor colorTwo_ = RSColor(0xFFFFFFFF);
    RSColor colorThree_ = RSColor(0xFFFFFFFF);

    float colorProgress_ = 0.0f;
    float centerBrightness_ = 1.0f;
    float soundIntensity_ = 0.0f;

    //shock wave
    float shockWaveAlphaOne_ = 1.0f;
    float shockWaveAlphaTwo_ = 1.0f;
    float shockWaveProgressOne_ = 0.0f;
    float shockWaveProgressTwo_ = 0.0f;
};
} // Rosen
} // OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RS_SOUND_WAVE_FILTER_H