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
#include "render/rs_sound_wave_filter.h"

#include "common/rs_color.h"
#include "pipeline/rs_paint_filter_canvas.h"

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {

RSSoundWaveFilter::RSSoundWaveFilter(Vector4f colorA, Vector4f colorB, Vector4f colorC, float colorProgress,
    float soundIntensity, float shockWaveAlphaA, float shockWaveAlphaB, float shockWaveProgressA,
    float shockWaveProgressB, float shockWaveTotalAlpha)
    : colorProgress_(colorProgress), soundIntensity_(soundIntensity), shockWaveAlphaA_(shockWaveAlphaA),
      shockWaveAlphaB_(shockWaveAlphaB), shockWaveProgressA_(shockWaveProgressA),
      shockWaveProgressB_(shockWaveProgressB), shockWaveTotalAlpha_(shockWaveTotalAlpha)
{
    colorA_ = { colorA.x_, colorA.y_, colorA.z_, colorA.w_ };
    colorB_ = { colorB.x_, colorB.y_, colorB.z_, colorB.w_ };
    colorC_ = { colorC.x_, colorC.y_, colorC.z_, colorC.w_ };
    type_ = ShaderFilterType::SOUND_WAVE;
#ifdef USE_M133_SKIA
    hash_ = SkChecksum::Hash32(&colorA_, sizeof(colorA_), hash_);
    hash_ = SkChecksum::Hash32(&colorB_, sizeof(colorB_), hash_);
    hash_ = SkChecksum::Hash32(&colorC_, sizeof(colorC_), hash_);
    hash_ = SkChecksum::Hash32(&colorProgress_, sizeof(colorProgress_), hash_);
    hash_ = SkChecksum::Hash32(&soundIntensity_, sizeof(soundIntensity_), hash_);
    hash_ = SkChecksum::Hash32(&shockWaveAlphaA_, sizeof(shockWaveAlphaA_), hash_);
    hash_ = SkChecksum::Hash32(&shockWaveAlphaB_, sizeof(shockWaveAlphaB_), hash_);
    hash_ = SkChecksum::Hash32(&shockWaveProgressA_, sizeof(shockWaveProgressA_), hash_);
    hash_ = SkChecksum::Hash32(&shockWaveProgressB_, sizeof(shockWaveProgressB_), hash_);
    hash_ = SkChecksum::Hash32(&shockWaveTotalAlpha_, sizeof(shockWaveTotalAlpha_), hash_);
#else
    hash_ = SkOpts::hash(&colorA_, sizeof(colorA_), hash_);
    hash_ = SkOpts::hash(&colorB_, sizeof(colorB_), hash_);
    hash_ = SkOpts::hash(&colorC_, sizeof(colorC_), hash_);
    hash_ = SkOpts::hash(&colorProgress_, sizeof(colorProgress_), hash_);
    hash_ = SkOpts::hash(&soundIntensity_, sizeof(soundIntensity_), hash_);
    hash_ = SkOpts::hash(&shockWaveAlphaA_, sizeof(shockWaveAlphaA_), hash_);
    hash_ = SkOpts::hash(&shockWaveAlphaB_, sizeof(shockWaveAlphaB_), hash_);
    hash_ = SkOpts::hash(&shockWaveProgressA_, sizeof(shockWaveProgressA_), hash_);
    hash_ = SkOpts::hash(&shockWaveProgressB_, sizeof(shockWaveProgressB_), hash_);
    hash_ = SkOpts::hash(&shockWaveTotalAlpha_, sizeof(shockWaveTotalAlpha_), hash_);
#endif
}

RSSoundWaveFilter::~RSSoundWaveFilter() {}

float RSSoundWaveFilter::GetColorProgress() const
{
    return colorProgress_;
}

float RSSoundWaveFilter::GetSoundIntensity() const
{
    return soundIntensity_;
}

float RSSoundWaveFilter::GetShockWaveAlphaA() const
{
    return shockWaveAlphaA_;
}

float RSSoundWaveFilter::GetShockWaveAlphaB() const
{
    return shockWaveAlphaB_;
}

float RSSoundWaveFilter::GetShockWaveProgressA() const
{
    return shockWaveProgressA_;
}

float RSSoundWaveFilter::GetShockWaveProgressB() const
{
    return shockWaveProgressB_;
}

float RSSoundWaveFilter::GetShockWaveTotalAlpha() const
{
    return shockWaveTotalAlpha_;
}

void RSSoundWaveFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto soundWaveFilter = std::make_shared<Drawing::GEVisualEffect>("SOUND_WAVE",
        Drawing::DrawingPaintType::BRUSH);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_COLOR_A, colorA_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_COLOR_B, colorB_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_COLOR_C, colorC_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_COLORPROGRESS, colorProgress_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SOUNDINTENSITY, soundIntensity_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_A, shockWaveAlphaA_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SHOCKWAVEALPHA_B, shockWaveAlphaB_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_A, shockWaveProgressA_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_SHOCKWAVEPROGRESS_B, shockWaveProgressB_);
    soundWaveFilter->SetParam(GE_FILTER_SOUND_WAVE_TOTAL_ALPHA, shockWaveTotalAlpha_);
    visualEffectContainer->AddToChainedFilter(soundWaveFilter);
}
} // namespace Rosen

} // namespace OHOS
