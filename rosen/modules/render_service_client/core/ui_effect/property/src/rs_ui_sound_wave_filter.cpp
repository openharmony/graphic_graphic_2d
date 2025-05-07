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
    auto soundWaveProperty = std::static_pointer_cast<RSUISoundWaveFilterPara>(other);
    if (!soundWaveProperty) {
        ROSEN_LOGW("RSUISoundWaveFilterPara::SetProperty soundWaveProperty NG!");
        return;
    }
    auto colorOne = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_ONE);
    auto colorTwo = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_TWO);
    auto colorThree = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_THREE);
    auto colorProgress = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS);
    auto colorBrightness = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_WAVE_CENTER_BRIGHTNESS);
    auto soundIntensity = soundWaveProperty->GetRSProperty(RSUIFilterType::SOUND_INTENSITY);
    auto shockWaveAlphaOne = soundWaveProperty->GetRSProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_ONE);
    auto shockWaveAlphaTwo = soundWaveProperty->GetRSProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_TWO);
    auto shockWaveProgressOne = soundWaveProperty->GetRSProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_ONE);
    auto shockWaveProgressTwo = soundWaveProperty->GetRSProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_TWO);
    if (!colorProgress || !colorBrightness || !soundIntensity
        || !shockWaveAlphaOne || !shockWaveAlphaTwo || !shockWaveProgressOne || !shockWaveProgressTwo) {
        ROSEN_LOGW("RSUISoundWaveFilterPara::SetProperty RSProperty null NG!");
        return;
    }

    auto colorOneProperty = std::static_pointer_cast<RSProperty<Color>>(colorOne);
    auto colorTwoProperty = std::static_pointer_cast<RSProperty<Color>>(colorTwo);
    auto colorThreeProperty = std::static_pointer_cast<RSProperty<Color>>(colorThree);
    auto colorProgressAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(colorProgress);
    auto colorBrightnessProperty = std::static_pointer_cast<RSProperty<float>>(colorBrightness);
    auto soundIntensityAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(soundIntensity);
    auto shockWaveAlphaOneAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(shockWaveAlphaOne);
    auto shockWaveAlphaTwoAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(shockWaveAlphaTwo);
    auto shockWaveProgressOneAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(shockWaveProgressOne);
    auto shockWaveProgressTwoAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(shockWaveProgressTwo);
    if (!colorProgressAnimation || !soundIntensityAnimation || !shockWaveAlphaOneAnimation ||
        !shockWaveAlphaTwoAnimation || !shockWaveProgressOneAnimation || !shockWaveProgressTwoAnimation) {
        ROSEN_LOGW("RSUISoundWaveFilterPara::SetProperty RSAnimatableProperty null NG!");
        return;
    }

    SetColors(colorOneProperty->Get(), colorTwoProperty->Get(), colorThreeProperty->Get());
    SetColorProgress(colorProgressAnimation->Get());
    SetCenterBrightness(colorBrightnessProperty->Get());
    SetSoundIntensity(soundIntensityAnimation->Get());
    SetShockWaveAlphaOne(shockWaveAlphaOneAnimation->Get());
    SetShockWaveAlphaTwo(shockWaveAlphaTwoAnimation->Get());
    SetShockWaveProgressOne(shockWaveProgressOneAnimation->Get());
    SetShockWaveProgressTwo(shockWaveProgressTwoAnimation->Get());
}

void RSUISoundWaveFilterPara::SetColors(RSColor colorOne, RSColor colorTwo, RSColor colorThree)
{
    Setter<RSProperty<Color>>(RSUIFilterType::SOUND_WAVE_COLOR_ONE, colorOne);
    Setter<RSProperty<Color>>(RSUIFilterType::SOUND_WAVE_COLOR_TWO, colorTwo);
    Setter<RSProperty<Color>>(RSUIFilterType::SOUND_WAVE_COLOR_THREE, colorThree);
}

void RSUISoundWaveFilterPara::SetColorProgress(float progress)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS, progress);
}

void RSUISoundWaveFilterPara::SetCenterBrightness(float value)
{
    Setter<RSProperty<float>>(RSUIFilterType::SOUND_WAVE_CENTER_BRIGHTNESS, value);
}

void RSUISoundWaveFilterPara::SetSoundIntensity(float intensity)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SOUND_INTENSITY, intensity);
}

void RSUISoundWaveFilterPara::SetShockWaveAlphaOne(float alpha)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SHOCK_WAVE_ALPHA_ONE, alpha);
}

void RSUISoundWaveFilterPara::SetShockWaveAlphaTwo(float alpha)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SHOCK_WAVE_ALPHA_TWO, alpha);
}

void RSUISoundWaveFilterPara::SetShockWaveProgressOne(float progress)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SHOCK_WAVE_PROGRESS_ONE, progress);
}

void RSUISoundWaveFilterPara::SetShockWaveProgressTwo(float progress)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::SHOCK_WAVE_PROGRESS_TWO, progress);
}

std::shared_ptr<RSRenderFilterParaBase> RSUISoundWaveFilterPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderSoundWaveFilterPara>(id_);

    auto colorOne = std::static_pointer_cast<RSProperty<Color>>(
        GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_ONE));
    auto colorOneProperty =
        std::make_shared<RSRenderProperty<Color>>(colorOne->Get(),
        colorOne->GetId(), RSRenderPropertyType::PROPERTY_COLOR);
    frProperty->Setter(RSUIFilterType::SOUND_WAVE_COLOR_ONE, colorOneProperty);

    auto colorTwo = std::static_pointer_cast<RSProperty<Color>>(
        GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_TWO));
    auto colorTwoProperty =
        std::make_shared<RSRenderProperty<Color>>(colorTwo->Get(),
        colorTwo->GetId(), RSRenderPropertyType::PROPERTY_COLOR);
    frProperty->Setter(RSUIFilterType::SOUND_WAVE_COLOR_TWO, colorTwoProperty);

    auto colorThree = std::static_pointer_cast<RSProperty<Color>>(
        GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_THREE));
    auto colorThreeProperty =
        std::make_shared<RSRenderProperty<Color>>(colorThree->Get(),
        colorThree->GetId(), RSRenderPropertyType::PROPERTY_COLOR);
    frProperty->Setter(RSUIFilterType::SOUND_WAVE_COLOR_THREE, colorThreeProperty);

    auto colorProgressAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS));
    auto colorProgressProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(colorProgressAnimation->Get(),
        colorProgressAnimation->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS, colorProgressProperty);

    auto colorBrightness = std::static_pointer_cast<RSProperty<float>>(
        GetRSProperty(RSUIFilterType::SOUND_WAVE_CENTER_BRIGHTNESS));
    auto colorBrightnessProperty =
        std::make_shared<RSRenderProperty<float>>(colorBrightness->Get(),
        colorBrightness->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::SOUND_WAVE_CENTER_BRIGHTNESS, colorBrightnessProperty);

    auto soundIntensityAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SOUND_INTENSITY));
    auto soundIntensityProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(soundIntensityAnimation->Get(),
        soundIntensityAnimation->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::SOUND_INTENSITY, soundIntensityProperty);

    auto shockWaveAlphaOneAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_ONE));
    auto shockWaveAlphaOneProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(shockWaveAlphaOneAnimation->Get(),
        shockWaveAlphaOneAnimation->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::SHOCK_WAVE_ALPHA_ONE, shockWaveAlphaOneProperty);

    auto shockWaveAlphaTwoAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_TWO));
    auto shockWaveAlphaTwoProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(shockWaveAlphaTwoAnimation->Get(),
        shockWaveAlphaTwoAnimation->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::SHOCK_WAVE_ALPHA_TWO, shockWaveAlphaTwoProperty);

    auto shockWaveProgressOneAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_ONE));
    auto shockWaveProgressOneProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(shockWaveProgressOneAnimation->Get(),
        shockWaveProgressOneAnimation->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::SHOCK_WAVE_PROGRESS_ONE, shockWaveProgressOneProperty);

    auto shockWaveProgressTwoAnimation = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_TWO));
    auto shockWaveProgressTwoProperty =
        std::make_shared<RSRenderAnimatableProperty<float>>(shockWaveProgressTwoAnimation->Get(),
        shockWaveProgressTwoAnimation->GetId(), RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::SHOCK_WAVE_PROGRESS_TWO, shockWaveProgressTwoProperty);

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
} // namespace ROSEN
} // namespace OHOS