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

#include "render/rs_render_sound_wave_filter.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "common/rs_color_palette.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

    std::shared_ptr<RSRenderFilterParaBase> RSRenderSoundWaveFilterPara::DeepCopy() const
    {
        auto copyFilter = std::make_shared<RSRenderSoundWaveFilterPara>(id_);
        copyFilter->type_ = type_;
        copyFilter->colorA_ = colorA_;
        copyFilter->colorB_ = colorB_;
        copyFilter->colorC_ = colorC_;
        copyFilter->colorProgress_ = colorProgress_;
        copyFilter->soundIntensity_ = soundIntensity_;
        copyFilter->shockWaveAlphaA_ = shockWaveAlphaA_;
        copyFilter->shockWaveAlphaB_ = shockWaveAlphaB_;
        copyFilter->shockWaveProgressA_ = shockWaveProgressA_;
        copyFilter->shockWaveProgressB_ = shockWaveProgressB_;
        copyFilter->shockWaveTotalAlpha_ = shockWaveTotalAlpha_;
        copyFilter->CalculateHash();
        return copyFilter;
    }

    void RSRenderSoundWaveFilterPara::GetDescription(std::string& out) const
    {
        out += "RSRenderSoundWaveFilterPara";
    }

    bool RSRenderSoundWaveFilterPara::WriteToParcel(Parcel& parcel)
    {
        if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
            !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(type_)) ||
            !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(modifierType_))) {
            ROSEN_LOGE("RSRenderSoundWaveFilterPara::WriteToParcel typr Error");
            return false;
        }
        if (!parcel.WriteUint32(properties_.size())) {
            ROSEN_LOGE("RSRenderSoundWaveFilterPara::WriteToParcel size Error");
            return false;
        }
        for (const auto& [key, value] : properties_) {
            if (!RSMarshallingHelper::Marshalling(parcel, key) ||
                !RSMarshallingHelper::Marshalling(parcel, value)) {
                return false;
            }
            ROSEN_LOGD("RSRenderSoundWaveFilterPara::WriteToParcel type %{public}d", static_cast<int>(key));
        }
        return true;
    }

    bool RSRenderSoundWaveFilterPara::ReadFromParcel(Parcel& parcel)
    {
        int16_t type = 0;
        int16_t modifierType = 0;
        if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, id_) ||
            !RSMarshallingHelper::Unmarshalling(parcel, type) ||
            !RSMarshallingHelper::Unmarshalling(parcel, modifierType)) {
            ROSEN_LOGE("RSRenderSoundWaveFilterPara::ReadFromParcel type Error");
            return false;
        }
        type_ = static_cast<RSUIFilterType>(type);
        modifierType_ = static_cast<RSModifierType>(modifierType);

        uint32_t size = 0;
        if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
            ROSEN_LOGE("RSRenderSoundWaveFilterPara::ReadFromParcel size Error");
            return false;
        }
        if (size > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
            ROSEN_LOGE("RSRenderSoundWaveFilterPara::ReadFromParcel size large Error");
            return false;
        }
        properties_.clear();
        for (uint32_t i = 0; i < size; ++i) {
            RSUIFilterType key;
            if (!RSMarshallingHelper::Unmarshalling(parcel, key)) {
                ROSEN_LOGE("RSRenderSoundWaveFilterPara::ReadFromParcel type %{public}d", static_cast<int>(key));
                return false;
            }
            std::shared_ptr<RSRenderPropertyBase> value = CreateRenderProperty(key);
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                ROSEN_LOGE("RSRenderSoundWaveFilterPara::ReadFromParcel value %{public}d", static_cast<int>(key));
                return false;
            }
            Setter(key, value);
        }
        return true;
    }

    std::shared_ptr<RSRenderPropertyBase> RSRenderSoundWaveFilterPara::CreateRenderProperty(RSUIFilterType type)
    {
        switch (type) {
            case RSUIFilterType::SOUND_WAVE_COLOR_A: {
                return std::make_shared<RSRenderProperty<Vector4f>>(Vector4f(), 0);
            }
            case RSUIFilterType::SOUND_WAVE_COLOR_B: {
                return std::make_shared<RSRenderProperty<Vector4f>>(Vector4f(), 0);
            }
            case RSUIFilterType::SOUND_WAVE_COLOR_C: {
                return std::make_shared<RSRenderProperty<Vector4f>>(Vector4f(), 0);
            }
            case RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS: {
                return std::make_shared<RSRenderAnimatableProperty<float>>(0.f, 0);
            }
            case RSUIFilterType::SOUND_INTENSITY: {
                return std::make_shared<RSRenderAnimatableProperty<float>>(0.f, 0);
            }
            case RSUIFilterType::SHOCK_WAVE_ALPHA_A: {
                return std::make_shared<RSRenderAnimatableProperty<float>>(1.f, 0);
            }
            case RSUIFilterType::SHOCK_WAVE_ALPHA_B: {
                return std::make_shared<RSRenderAnimatableProperty<float>>(1.f, 0);
            }
            case RSUIFilterType::SHOCK_WAVE_PROGRESS_A: {
                return std::make_shared<RSRenderAnimatableProperty<float>>(0.f, 0);
            }
            case RSUIFilterType::SHOCK_WAVE_PROGRESS_B: {
                return std::make_shared<RSRenderAnimatableProperty<float>>(0.f, 0);
            }
            case RSUIFilterType::SHOCK_WAVE_TOTAL_ALPHA: {
                return std::make_shared<RSRenderAnimatableProperty<float>>(1.f, 0);
            }
            default: {
                ROSEN_LOGD("RSRenderSoundWaveFilterPara::CreateRenderProperty mask nullptr");
                return nullptr;
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderSoundWaveFilterPara::GetLeafRenderProperties()
    {
        std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
        for (auto& [k, v] : properties_) {
            out.emplace_back(v);
        }
        return out;
    }

    bool RSRenderSoundWaveFilterPara::ParseFilterValues()
    {
        auto waveColorA =
            std::static_pointer_cast<RSRenderProperty<Vector4f>>(GetRenderProperty(RSUIFilterType::SOUND_WAVE_COLOR_A));
        auto waveColorB =
            std::static_pointer_cast<RSRenderProperty<Vector4f>>(GetRenderProperty(RSUIFilterType::SOUND_WAVE_COLOR_B));
        auto waveColorC =
            std::static_pointer_cast<RSRenderProperty<Vector4f>>(GetRenderProperty(RSUIFilterType::SOUND_WAVE_COLOR_C));
        auto waveColorProgress = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
            GetRenderProperty(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS));
        auto soundIntensity = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
            GetRenderProperty(RSUIFilterType::SOUND_INTENSITY));
        auto shockWaveAlphaA = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
            GetRenderProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_A));
        auto shockWaveAlphaB = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
            GetRenderProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_B));
        auto shockWaveProgressA = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
            GetRenderProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_A));
        auto shockWaveProgressB = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
            GetRenderProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_B));
        auto shockWaveTotalAlpha = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(
            GetRenderProperty(RSUIFilterType::SHOCK_WAVE_TOTAL_ALPHA));
        bool soundWavePropertyIsValid = waveColorA && waveColorB && waveColorC && waveColorProgress && soundIntensity &&
                                        shockWaveAlphaA && shockWaveAlphaB && shockWaveProgressA &&
                                        shockWaveProgressB && shockWaveTotalAlpha;
        if (!soundWavePropertyIsValid) {
            ROSEN_LOGE("RSRenderSoundWaveFilterPara::ParseFilterValues get soundWaveRenderProperty nullptr.");
            return false;
        }
        auto colorA = waveColorA->Get();
        auto colorB = waveColorB->Get();
        auto colorC = waveColorC->Get();
        colorA_ = { colorA.x_, colorA.y_, colorA.z_, colorA.w_ };
        colorB_ = { colorB.x_, colorB.y_, colorB.z_, colorB.w_ };
        colorC_ = { colorC.x_, colorC.y_, colorC.z_, colorC.w_ };
        colorProgress_ = waveColorProgress->Get();
        soundIntensity_ = soundIntensity->Get();
        shockWaveAlphaA_ = shockWaveAlphaA->Get();
        shockWaveAlphaB_ = shockWaveAlphaB->Get();
        shockWaveProgressA_ = shockWaveProgressA->Get();
        shockWaveProgressB_ = shockWaveProgressB->Get();
        shockWaveTotalAlpha_ = shockWaveTotalAlpha->Get();
        return true;
    }

    void RSRenderSoundWaveFilterPara::CalculateHash()
    {
#ifdef USE_M133_SKIA
        const auto hashFunc = SkChecksum::Hash32;
#else
        const auto hashFunc = SkOpts::hash;
#endif
        hash_ = hashFunc(&colorA_, sizeof(colorA_), hash_);
        hash_ = hashFunc(&colorB_, sizeof(colorB_), hash_);
        hash_ = hashFunc(&colorC_, sizeof(colorC_), hash_);
        hash_ = hashFunc(&colorProgress_, sizeof(colorProgress_), hash_);
        hash_ = hashFunc(&soundIntensity_, sizeof(soundIntensity_), hash_);
        hash_ = hashFunc(&shockWaveAlphaA_, sizeof(shockWaveAlphaA_), hash_);
        hash_ = hashFunc(&shockWaveAlphaB_, sizeof(shockWaveAlphaB_), hash_);
        hash_ = hashFunc(&shockWaveProgressA_, sizeof(shockWaveProgressA_), hash_);
        hash_ = hashFunc(&shockWaveProgressB_, sizeof(shockWaveProgressB_), hash_);
        hash_ = hashFunc(&shockWaveTotalAlpha_, sizeof(shockWaveTotalAlpha_), hash_);
        hash_ = hashFunc(&geoWidth_, sizeof(geoWidth_), hash_);
        hash_ = hashFunc(&geoHeight_, sizeof(geoHeight_), hash_);
    }

    float RSRenderSoundWaveFilterPara::GetColorProgress() const
    {
        return colorProgress_;
    }

    float RSRenderSoundWaveFilterPara::GetSoundIntensity() const
    {
        return soundIntensity_;
    }

    float RSRenderSoundWaveFilterPara::GetShockWaveAlphaA() const
    {
        return shockWaveAlphaA_;
    }

    float RSRenderSoundWaveFilterPara::GetShockWaveAlphaB() const
    {
        return shockWaveAlphaB_;
    }

    float RSRenderSoundWaveFilterPara::GetShockWaveProgressA() const
    {
        return shockWaveProgressA_;
    }

    float RSRenderSoundWaveFilterPara::GetShockWaveProgressB() const
    {
        return shockWaveProgressB_;
    }

    float RSRenderSoundWaveFilterPara::GetShockWaveTotalAlpha() const
    {
        return shockWaveTotalAlpha_;
    }

    void RSRenderSoundWaveFilterPara::GenerateGEVisualEffect(
        std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
    {
        auto soundWaveFilter = std::make_shared<Drawing::GEVisualEffect>(
            "SOUND_WAVE", Drawing::DrawingPaintType::BRUSH, GetFilterCanvasInfo());
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