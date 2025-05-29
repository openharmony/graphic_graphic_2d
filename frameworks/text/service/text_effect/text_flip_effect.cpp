/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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

#include <algorithm>
#include <tuple>

#include "text_effect.h"
#include "text_effect_marco.h"

namespace OHOS::Rosen {

TextFlipEffect::TextFlipEffect()
{
    strategy_ = TextEffectStrategy::FLIP;
}

TextFlipEffect::~TextFlipEffect()
{
    typographyConfig_.typography = nullptr;
    typographyConfig_.rawTextRange = {0, 0};
}

bool TextFlipEffect::CheckInputParams(const std::unordered_map<TextEffectAttribute, std::string>& config) const
{
    for (const auto& [key, value] : config) {
        auto iter = std::find_if(supportAttributes_.begin(), supportAttributes_.end(),
            [&key](const FlipAttributeFunction& func) { return func.attribute == key; });
        if (iter == supportAttributes_.end()) {
            continue;
        }
        if (!iter->checkFunc(this, value)) {
            return false;
        }
    }
    return true;
}

bool TextFlipEffect::CheckDirection(const std::string& direction) const
{
    return direction == "up" || direction == "down";
}

void TextFlipEffect::SetDirection(const std::string& direction)
{
    if (direction == "up") {
        direction_ = TextEffectFlipDirection::UP;
    } else if (direction == "down") {
        direction_ = TextEffectFlipDirection::DOWN;
    }
}

bool TextFlipEffect::CheckBlurEnable(const std::string& enable) const
{
    return enable == "true" || enable == "false";
}

void TextFlipEffect::SetBlurEnable(const std::string& enable)
{
    blurEnable_ = enable == "true";
}

int TextFlipEffect::UpdateEffectConfig(const std::unordered_map<TextEffectAttribute, std::string>& config)
{
    if (config.empty()) {
        return TEXT_EFFECT_SUCCESS;
    }

    if (!CheckInputParams(config)) {
        return TEXT_EFFECT_INVALID_INPUT;
    }

    for (const auto& [key, value] : config) {
        auto iter = std::find_if(supportAttributes_.begin(), supportAttributes_.end(),
            [&key](const FlipAttributeFunction& func) { return func.attribute == key; });
        if (iter == supportAttributes_.end()) {
            continue;
        }
        iter->setFunc(this, value);
    }
    return TEXT_EFFECT_SUCCESS;
}

int TextFlipEffect::AppendTypography(const std::vector<TypographyConfig>& typographyConfigs)
{
    if (typographyConfigs.empty()) {
        return TEXT_EFFECT_INVALID_INPUT;
    }

    if (typographyConfig_.typography != nullptr) {
        return TEXT_EFFECT_UNKNOWN;
    }
    
    typographyConfig_ = typographyConfigs[0];
    return TEXT_EFFECT_SUCCESS;
}

int TextFlipEffect::UpdateTypography(std::shared_ptr<TypographyConfig> target,
    const std::vector<TypographyConfig>& typographyConfigs)
{
    if (typographyConfig_.typography != target.typography || typographyConfigs.empty()) {
        return TEXT_EFFECT_INVALID_INPUT;
    }
    typographyConfig_ = typographyConfigs[0];
    return TEXT_EFFECT_SUCCESS;
}

void TextFlipEffect::RemoveTypography(const std::vector<TypographyConfig>& typographyConfigs)
{
    if (typographyConfig_.typography != typographyConfigs[0].typography) {
        return;
    }
    typographyConfig_.typography = nullptr; 
    typographyConfig_.rawTextRange = {0, 0};
}

void TextFlipEffect::StartEffect(Drawing::Canvas* canvas, double x, double y)
{
    if (typographyConfig_.typography == nullptr) {
        return;
    }
    typographyConfig_.typography->Paint(canvas, x, y);
    std::vector<TextBlobRecordInfo> textBlobRecordInfos = typographyConfig_.typography->GetTextBlobRecordInfo();
    lastTextBlobRecordInfos_.swap(textBlobRecordInfos);
}

void TextFlipEffect::StopEffect(Drawing::Canvas* canvas, double x, double y)
{
    if (typographyConfig_.typography == nullptr) {
        return;
    }
    typographyConfig_.typography->Paint(canvas, x, y);
}

REGISTER_TEXT_EFFECT_FACTORY_IMPL(Flip, TextEffectStrategy::FLIP);
} // namespace OHOS::Rosen