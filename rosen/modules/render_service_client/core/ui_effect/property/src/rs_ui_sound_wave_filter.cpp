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

#include "ui_effect/property/include/rs_ui_sound_wave_filter.h"
#include "render/rs_render_sound_wave_filter.h"
namespace OHOS {
namespace Rosen {

bool RSUISoundWaveFilterPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::SOUND_WAVE) {
        ROSEN_LOGW("RSUISoundWaveFilterPara::Equals NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }
    return true;
}

void RSUISoundWaveFilterPara::Dump(std::string& out) const
{
    out += "RSUISoundWaveFilterPara: [";
    char buffer[UINT8_MAX] = { 0 };
    auto iter = properties_.find(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS);
    if (iter != properties_.end()) {
        auto soundProgress = std::static_pointer_cast<RSProperty<float>>(iter->second);
        if (soundProgress) {
            sprintf_s(buffer, UINT8_MAX, "[soundProgress: %f]", soundProgress->Get());
            out.append(buffer);
        } else {
            out += "nullptr";
        }
    }
}

void RSUISoundWaveFilterPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (!other || other->GetType() != RSUIFilterType::SOUND_WAVE) {
        ROSEN_LOGW("RSUISoundWaveFilterPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }
    SetStagingEnableHdrEffect(other->GetEnableHdrEffect());

    auto soundWaveProperty = std::static_pointer_cast<RSUISoundWaveFilterPara>(other);
    if (!soundWaveProperty) {
        ROSEN_LOGW("RSUISoundWaveFilterPara::SetProperty soundWaveProperty NG!");
        return;
    }
    auto colorA = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_A);
    auto colorB = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_B);
    auto colorC = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_C);
    auto colorProgress = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS);
    auto soundIntensity = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_INTENSITY);
    auto shockWaveAlphaA = soundWaveProperty->GetRSProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_A);
    auto shockWaveAlphaB = soundWaveProperty->GetRSProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_B);
    auto shockWaveProgressA = soundWaveProperty->GetRSProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_A);
    auto shockWaveProgressB = soundWaveProperty->GetRSProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_B);
    auto shockWaveTotalAlpha = soundWaveProperty->GetRSProperty(RSUIFilterType::SHOCK_WAVE_TOTAL_ALPHA);

    auto colorAProperty = std::static_pointer_cast<RSProperty<Vector4f>>(colorA);
    auto colorBProperty = std::static_pointer_cast<RSProperty<Vector4f>>(colorB);
    auto colorCProperty = std::static_pointer_cast<RSProperty<Vector4f>>(colorC);
    auto colorProgressAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(colorProgress);
    auto soundIntensityAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(soundIntensity);
    auto shockWaveAlphaAAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(shockWaveAlphaA);
    auto shockWaveAlphaBAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(shockWaveAlphaB);
    auto shockWaveProgressAAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(shockWaveProgressA);
    auto shockWaveProgressBAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(shockWaveProgressB);
    auto shockWaveTotalAlphaAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(shockWaveTotalAlpha);
    if (!colorAProperty || !colorBProperty || !colorCProperty || !colorProgressAnimation || !soundIntensityAnimation ||
        !shockWaveAlphaAAnimation || !shockWaveAlphaBAnimation || !shockWaveProgressAAnimation ||
        !shockWaveProgressBAnimation || !shockWaveTotalAlphaAnimation) {
        ROSEN_LOGW("RSUISoundWaveFilterPara::SetProperty RSAnimatableProperty null NG!");
        return;
    }

    SetColors(colorAProperty->Get(), colorBProperty->Get(), colorCProperty->Get());
    SetColorProgress(colorProgressAnimation->Get());
    SetSoundIntensity(soundIntensityAnimation->Get());
    SetShockWaveAlphaA(shockWaveAlphaAAnimation->Get());
    SetShockWaveAlphaB(shockWaveAlphaBAnimation->Get());
    SetShockWaveProgressA(shockWaveProgressAAnimation->Get());
    SetShockWaveProgressB(shockWaveProgressBAnimation->Get());
    SetShockWaveTotalAlpha(shockWaveTotalAlphaAnimation->Get());
}

void RSUISoundWaveFilterPara::SetColors(Vector4f colorA, Vector4f colorB, Vector4f colorC)
{
    Setter<RSProperty<Vector4f>>(RSUIFilterType::SOUND_WAVE_COLOR_A, colorA);
    Setter<RSProperty<Vector4f>>(RSUIFilterType::SOUND_WAVE_COLOR_B, colorB);
    Setter<RSProperty<Vector4f>>(RSUIFilterType::SOUND_WAVE_COLOR_C, colorC);
}

void RSUISoundWaveFilterPara::SetColorProgress(float progress)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS, progress);
}

void RSUISoundWaveFilterPara::SetSoundIntensity(float intensity)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SOUND_INTENSITY, intensity);
}

void RSUISoundWaveFilterPara::SetShockWaveAlphaA(float alpha)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SHOCK_WAVE_ALPHA_A, alpha);
}

void RSUISoundWaveFilterPara::SetShockWaveAlphaB(float alpha)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SHOCK_WAVE_ALPHA_B, alpha);
}

void RSUISoundWaveFilterPara::SetShockWaveProgressA(float progress)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SHOCK_WAVE_PROGRESS_A, progress);
}

void RSUISoundWaveFilterPara::SetShockWaveProgressB(float progress)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SHOCK_WAVE_PROGRESS_B, progress);
}

void RSUISoundWaveFilterPara::SetShockWaveTotalAlpha(float alpha)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SHOCK_WAVE_TOTAL_ALPHA, alpha);
}

std::shared_ptr<RSRenderFilterParaBase> RSUISoundWaveFilterPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderSoundWaveFilterPara>(id_);

    auto colorA = std::static_pointer_cast<RSProperty<Color>>(
        GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_A));
    auto colorAProperty =
        std::make_shared<RSRenderProperty<Color>>(colorA->Get(),
        colorA->GetId());
    frProperty->Setter(RSUIFilterType::SOUND_WAVE_COLOR_A, colorAProperty);

    auto colorB = std::static_pointer_cast<RSProperty<Color>>(
        GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_B));
    auto colorBProperty =
        std::make_shared<RSRenderProperty<Color>>(colorB->Get(),
        colorB->GetId());
    frProperty->Setter(RSUIFilterType::SOUND_WAVE_COLOR_B, colorBProperty);

    auto colorC = std::static_pointer_cast<RSProperty<Color>>(
        GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_C));
    auto colorCProperty =
        std::make_shared<RSRenderProperty<Color>>(colorC->Get(),
        colorC->GetId());
    frProperty->Setter(RSUIFilterType::SOUND_WAVE_COLOR_C, colorCProperty);

    auto colorProgressAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS));
    auto colorProgressProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(colorProgressAnimation->Get(),
        colorProgressAnimation->GetId());
    frProperty->Setter(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS, colorProgressProperty);

    auto soundIntensityAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SOUND_INTENSITY));
    auto soundIntensityProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(soundIntensityAnimation->Get(),
        soundIntensityAnimation->GetId());
    frProperty->Setter(RSUIFilterType::SOUND_INTENSITY, soundIntensityProperty);

    auto shockWaveAlphaAAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_A));
    auto shockWaveAlphaAProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(shockWaveAlphaAAnimation->Get(),
        shockWaveAlphaAAnimation->GetId());
    frProperty->Setter(RSUIFilterType::SHOCK_WAVE_ALPHA_A, shockWaveAlphaAProperty);

    auto shockWaveAlphaBAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_B));
    auto shockWaveAlphaBProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(shockWaveAlphaBAnimation->Get(),
        shockWaveAlphaBAnimation->GetId());
    frProperty->Setter(RSUIFilterType::SHOCK_WAVE_ALPHA_B, shockWaveAlphaBProperty);

    auto shockWaveProgressAAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_A));
    auto shockWaveProgressAProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(shockWaveProgressAAnimation->Get(),
        shockWaveProgressAAnimation->GetId());
    frProperty->Setter(RSUIFilterType::SHOCK_WAVE_PROGRESS_A, shockWaveProgressAProperty);

    auto shockWaveProgressBAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_B));
    auto shockWaveProgressBProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(shockWaveProgressBAnimation->Get(),
        shockWaveProgressBAnimation->GetId());
    frProperty->Setter(RSUIFilterType::SHOCK_WAVE_PROGRESS_B, shockWaveProgressBProperty);

    auto shockWaveTotalAlphaAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SHOCK_WAVE_TOTAL_ALPHA));
    auto shockWaveTotalProperty =
        std::make_shared<RSRenderProperty<float>>(shockWaveTotalAlphaAnimation->Get(),
        shockWaveTotalAlphaAnimation->GetId());
    frProperty->Setter(RSUIFilterType::SHOCK_WAVE_TOTAL_ALPHA, shockWaveTotalProperty);

    return frProperty;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUISoundWaveFilterPara::GetLeafProperties()
{
    std::vector<std::shared_ptr<RSPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}

bool RSUISoundWaveFilterPara::CheckEnableHdrEffect()
{
    auto colorAProperty =
        std::static_pointer_cast<RSProperty<Vector4f>>(GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_A));
    auto colorBProperty =
        std::static_pointer_cast<RSProperty<Vector4f>>(GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_B));
    auto colorCProperty =
        std::static_pointer_cast<RSProperty<Vector4f>>(GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_C));
    bool isValid = colorAProperty && colorBProperty && colorCProperty;
    if (!isValid) {
        return false;
    }
 
    Vector4f colorA = colorAProperty->Get();
    Vector4f colorB = colorBProperty->Get();
    Vector4f colorC = colorCProperty->Get();
    enableHdrEffect_ = ROSEN_GNE(colorA.x_, 1.0f) || ROSEN_GNE(colorA.y_, 1.0f) || ROSEN_GNE(colorA.z_, 1.0f) ||
                       ROSEN_GNE(colorB.x_, 1.0f) || ROSEN_GNE(colorB.y_, 1.0f) || ROSEN_GNE(colorB.z_, 1.0f) ||
                       ROSEN_GNE(colorC.x_, 1.0f) || ROSEN_GNE(colorC.y_, 1.0f) || ROSEN_GNE(colorC.z_, 1.0f);
    return enableHdrEffect_ || stagingEnableHdrEffect_;
}
} // namespace ROSEN
} // namespace OHOS