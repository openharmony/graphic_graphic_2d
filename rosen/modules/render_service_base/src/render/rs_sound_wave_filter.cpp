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
#include "rs_sound_wave_filter.h"

#include "common/rs_log.h"
#include "pipeline/rs_paint_filter_canvas.h"


namespace OHOS {
namespace Rosen {

RSSoundWaveFilter::RSSoundWaveFilter(RSColor colorOne, RSColor colorTwo, RSColor colorThree,
                                     float colorProgress, float centerBrightness, float soundIntensity,
                                     float shockWaveAlphaOne, float shockWaveAlphaTwo,
                                     float shockWaveProgressOne, float shockWaveProgressTwo)
    : colorOne_(colorOne), colorTwo_(colorTwo), colorThree_(colorThree),
      colorProgress_(colorProgress), centerBrightness_(centerBrightness), soundIntensity_(soundIntensity),
      shockWaveAlphaOne_(shockWaveAlphaOne), shockWaveAlphaTwo_(shockWaveAlphaTwo),
      shockWaveProgressOne_(shockWaveProgressOne), shockWaveProgressTwo_(shockWaveProgressTwo)
{
    type_ = ShaderFilterType::SOUND_WAVE;
    hash = SkOpts::hash(&colorOne_, sizeof(colorOne_), hash_);
    hash = SkOpts::hash(&colorTwo_, sizeof(colorTwo_), hash_);
    hash = SkOpts::hash(&colorThree_, sizeof(colorThree_), hash_);
    hash = SkOpts::hash(&colorProgress_, sizeof(colorProgress_), hash_);
    hash = SkOpts::hash(&centerBrightness_, sizeof(centerBrightness_), hash_);
    hash = SkOpts::hash(&soundIntensity_, sizeof(soundIntensity_), hash_);
    hash = SkOpts::hash(&shockWaveAlphaOne_, sizeof(shockWaveAlphaOne_), hash_);
    hash = SkOpts::hash(&shockWaveAlphaTwo_, sizeof(shockWaveAlphaTwo_), hash_);
    hash = SkOpts::hash(&shockWaveProgressOne_, sizeof(shockWaveProgressOne_), hash_);
    hash = SkOpts::hash(&shockWaveProgressTwo_, sizeof(shockWaveProgressTwo_), hash_);
}

RSSoundWaveFilter::~RSSoundWaveFilter() {}

float RSSoundWaveFilter::GetColorProgress() const
{
    return colorProgress_;
}

float RSSoundWaveFilter::GetCenterBrightness() const
{
    return centerBrightness_;
}

float RSSoundWaveFilter::GetSoundIntensity() const
{
    return soundIntensity_;
}

float RSSoundWaveFilter::GetShockWaveAlphaOne() const
{
    return shockWaveAlphaOne_;
}

float RSSoundWaveFilter::GetShockWaveAlphaTwo() const
{
    return shockWaveAlphaTwo_;
}

float RSSoundWaveFilter::GetShockWaveProgressOne() const
{
    return shockWaveProgressOne_;
}

float RSSoundWaveFilter::GetShockWaveProgressTwo() const
{
    return shockWaveProgressTwo_;
}

void RSSoundWaveFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto soundWaveFilter = std::make_shared<Drawing::GEVisualEffect>("SOUND_WAVE",
        Drawing::DrawingPaintType::BRUSH);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_COLORONE, colorOne_.AsRgbaInt());
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_COLORTWO, colorTwo_.AsRgbaInt());
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_COLORTHREE, colorThree_.AsRgbaInt());
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_COLORPROGRESS, colorProgress_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_CENTERBRIGHTNESS, centerBrightness_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SOUNDINTENSITY, soundIntensity_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHAONE, shockWaveAlphaOne_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHATWO, shockWaveAlphaTwo_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESSONE, shockWaveProgressOne_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESSTWO, shockWaveProgressTwo_);
    visualEffectContainer->AddVisualEffect(soundWaveFilter);
}
} // namespace Rosen
} // namespace OHOS
