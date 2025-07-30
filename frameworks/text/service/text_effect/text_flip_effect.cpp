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

#include "text_flip_effect.h"

#include <algorithm>

#include "symbol_engine/text_animation_config.h"
#include "text/hm_symbol.h"
#include "text_effect.h"
#include "text_effect_macro.h"

namespace OHOS::Rosen {
static std::string g_scaleX = "sx";
static std::string g_scaleY = "sy";
static std::string g_alphaProp = "alpha";
static std::string g_blurProp = "blur";
static std::string g_translateX = "tx";
static std::string g_translateY = "ty";

TextFlipEffect::TextFlipEffect()
{
    strategy_ = TextEffectStrategy::FLIP;
}

TextFlipEffect::~TextFlipEffect()
{
    typographyConfig_.typography = nullptr;
    typographyConfig_.rawTextRange = {0, 0};
}

bool TextFlipEffect::CheckInputParams(const std::unordered_map<TextEffectAttribute, std::string>& config)
{
    for (const auto& [key, value] : config) {
        auto iter = std::find_if(supportAttributes_.begin(), supportAttributes_.end(),
            [key = key](const FlipAttributeFunction& func) { return func.attribute == key; });
        if (iter == supportAttributes_.end()) {
            continue;
        }
        if (!iter->checkFunc(this, value)) {
            return false;
        }
    }
    return true;
}

bool TextFlipEffect::CheckDirection(const std::string& direction)
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

bool TextFlipEffect::CheckBlurEnable(const std::string& enable)
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
            [key = key](const FlipAttributeFunction& func) { return func.attribute == key; });
        if (iter == supportAttributes_.end()) {
            continue;
        }
        iter->setFunc(this, value);
    }
    return TEXT_EFFECT_SUCCESS;
}

int TextFlipEffect::AppendTypography(const std::vector<TypographyConfig>& typographyConfigs)
{
    if (typographyConfigs.empty() || typographyConfigs[0].typography == nullptr) {
        return TEXT_EFFECT_INVALID_INPUT;
    }

    if (typographyConfig_.typography != nullptr) {
        return TEXT_EFFECT_UNKNOWN;
    }

    typographyConfig_.typography = typographyConfigs[0].typography;
    typographyConfig_.rawTextRange = typographyConfigs[0].rawTextRange;
    typographyConfig_.typography->SetTextEffectAssociation(true);
    return TEXT_EFFECT_SUCCESS;
}

int TextFlipEffect::UpdateTypography(std::vector<std::pair<TypographyConfig, TypographyConfig>>& typographyConfigs)
{
    if (typographyConfigs.empty() || typographyConfigs[0].first.typography != typographyConfig_.typography ||
        typographyConfigs[0].second.typography == nullptr) {
        return TEXT_EFFECT_INVALID_INPUT;
    }
    if (typographyConfig_.typography == nullptr) {
        return TEXT_EFFECT_UNKNOWN;
    }
    typographyConfig_.typography->SetTextEffectAssociation(false);
    typographyConfig_.typography = typographyConfigs[0].second.typography;
    typographyConfig_.rawTextRange = typographyConfigs[0].second.rawTextRange;
    typographyConfig_.typography->SetTextEffectAssociation(true);
    return TEXT_EFFECT_SUCCESS;
}

void TextFlipEffect::RemoveTypography(const std::vector<TypographyConfig>& typographyConfigs)
{
    if (typographyConfigs.empty()) {
        ClearTypography();
        return;
    }
    if (typographyConfig_.typography != typographyConfigs[0].typography) {
        return;
    }
    ClearTypography();
}

void TextFlipEffect::StartEffect(Drawing::Canvas* canvas, double x, double y)
{
    if (typographyConfig_.typography == nullptr) {
        return;
    }
    typographyConfig_.typography->SetSkipTextBlobDrawing(true);
    typographyConfig_.typography->Paint(canvas, x, y);
    std::vector<TextBlobRecordInfo> textBlobRecordInfos = typographyConfig_.typography->GetTextBlobRecordInfo();
    DrawTextFlip(textBlobRecordInfos, canvas, x, y);

    lastAllBlobGlyphIds_.swap(currentBlobGlyphIds_);
    std::vector<uint16_t>().swap(currentBlobGlyphIds_);
    currentGlyphIndex_ = 0;
    changeNumber_ = 0;
}

void TextFlipEffect::StopEffect()
{
    if (typographyConfig_.typography == nullptr) {
        return;
    }

    typographyConfig_.typography->SetSkipTextBlobDrawing(false);
    SPText::TextAnimationConfig textEffectConfig;
    auto animationFunc = typographyConfig_.typography->GetAnimation();
    textEffectConfig.SetAnimation(animationFunc);
    textEffectConfig.ClearAllTextAnimation();
    std::vector<uint16_t>().swap(lastAllBlobGlyphIds_);
}

void TextFlipEffect::NoEffect(Drawing::Canvas* canvas, double x, double y)
{
    if (typographyConfig_.typography == nullptr) {
        return;
    }
    bool lastSkipState = typographyConfig_.typography->HasSkipTextBlobDrawing();
    typographyConfig_.typography->SetSkipTextBlobDrawing(false);
    typographyConfig_.typography->Paint(canvas, x, y);
    typographyConfig_.typography->SetSkipTextBlobDrawing(lastSkipState);
}

void TextFlipEffect::DrawTextFlip(std::vector<TextBlobRecordInfo>& infos, Drawing::Canvas* canvas, double x, double y)
{
    if (canvas == nullptr) {
        return;
    }
    auto animationFunc = typographyConfig_.typography->GetAnimation();
    double height = 0;
    for (auto& info : infos) {
        std::vector<std::vector<TextEngine::TextEffectElement>> effectElements = GenerateChangeElements(info.blob,
            x + info.offset.fX, y + info.offset.fY);
        if (effectElements.empty()) {
            Drawing::Brush brush;
            brush.SetColor(info.color);
            canvas->AttachBrush(brush);
            canvas->DrawTextBlob(info.blob.get(), x + info.offset.fX, y + info.offset.fY);
            canvas->DetachBrush();
            continue;
        }
        if (info.blob == nullptr || info.blob->Bounds() == nullptr) {
            continue;
        }
        height = info.blob->Bounds()->GetHeight();
        DrawTextFlipElements(canvas, height, info.color, animationFunc, effectElements);
    }
    DrawResidualText(canvas, height, animationFunc);
}

void TextFlipEffect::DrawResidualText(Drawing::Canvas* canvas, double height,
    const std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& func)
{
    if (currentGlyphIndex_ >= lastAllBlobGlyphIds_.size()) {
        return;
    }

    std::vector<std::vector<TextEngine::TextEffectElement>> effectElements;
    std::vector<TextEngine::TextEffectElement> unEffectElements;
    std::vector<TextEngine::TextEffectElement> inEffectElements;
    for (size_t index = currentGlyphIndex_; index < lastAllBlobGlyphIds_.size(); index++) {
        TextEngine::TextEffectElement effectElement;
        effectElement.uniqueId = index;
        effectElement.delay = 0;
        inEffectElements.emplace_back(effectElement);
    }
    effectElements.emplace_back(unEffectElements); // index 0 is unchanged
    effectElements.emplace_back(inEffectElements); // index 1 is changed
    DrawTextFlipElements(canvas, height, Drawing::Color::COLOR_BLACK, func, effectElements);
}

void TextFlipEffect::DrawTextFlipElements(Drawing::Canvas* canvas, double height, const Drawing::Color& color,
    const std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& func,
    const std::vector<std::vector<TextEngine::TextEffectElement>>& effectElements)
{
    const size_t unchangeIndex = 0;
    const size_t changeIndex = 1;
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters = GenerateFlipConfig(height);
    SPText::TextAnimationConfig textEffectConfig;
    textEffectConfig.SetAnimation(func);
    textEffectConfig.SetAnimationStart(true);
    textEffectConfig.SetColor(color);
    textEffectConfig.SetEffectStrategy(Drawing::DrawingEffectStrategy::TEXT_FLIP);
    textEffectConfig.SetEffectConfig(parameters);
    textEffectConfig.AnimationUnchange(true);
    textEffectConfig.DrawTextEffect(canvas, effectElements[unchangeIndex]);
    textEffectConfig.AnimationUnchange(false);
    textEffectConfig.DrawTextEffect(canvas, effectElements[changeIndex]);
}

void TextFlipEffect::ClearTypography()
{
    if (typographyConfig_.typography == nullptr) {
        return;
    }
    typographyConfig_.typography->SetTextEffectAssociation(false);
    typographyConfig_.typography = nullptr;
    typographyConfig_.rawTextRange = {0, 0};
}

std::vector<std::vector<TextEngine::TextEffectElement>> TextFlipEffect::GenerateChangeElements(
    const std::shared_ptr<Drawing::TextBlob>& blob, double x, double y)
{
    const int delayTimeMs = 100;
    std::vector<std::vector<TextEngine::TextEffectElement>> effectElements;
    std::vector<TextEngine::TextEffectElement> unEffectElements;
    std::vector<TextEngine::TextEffectElement> inEffectElements;
    std::vector<Drawing::Point> points;
    Drawing::TextBlob::GetDrawingPointsForTextBlob(blob.get(), points);
    std::vector<uint16_t> glyphIds;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(blob.get(), glyphIds);
    currentBlobGlyphIds_.insert(currentBlobGlyphIds_.end(), glyphIds.begin(), glyphIds.end());

    for (size_t index = 0; index < glyphIds.size(); index++) {
        TextEngine::TextEffectElement effectElement;
        effectElement.path = Drawing::TextBlob::GetDrawingPathforTextBlob(glyphIds[index], blob.get());
        if (!effectElement.path.IsValid()) {
            return {};
        }
        effectElement.offset = Drawing::Point{x + points[index].GetX(), y + points[index].GetY()};
        auto rect = effectElement.path.GetBounds();
        effectElement.width = rect.GetWidth();
        effectElement.height = rect.GetHeight();
        effectElement.uniqueId = currentGlyphIndex_ + index;
        effectElement.delay = changeNumber_ * delayTimeMs;
        if (currentGlyphIndex_ + index < lastAllBlobGlyphIds_.size() &&
            lastAllBlobGlyphIds_[currentGlyphIndex_ + index] == glyphIds[index]) {
            unEffectElements.emplace_back(effectElement);
        } else {
            inEffectElements.emplace_back(effectElement);
            changeNumber_++;
        }
    }
    effectElements.emplace_back(unEffectElements); // index 0 is unchanged
    effectElements.emplace_back(inEffectElements); // index 1 is changed
    currentGlyphIndex_ += glyphIds.size();
    return effectElements;
}

std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> TextFlipEffect::GenerateFlipConfig(double height)
{
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    size_t paramLength = 2; // index 0 is out, index 1 is in
    parameters.resize(paramLength);
    std::map<std::string, float> inFrictionCurve = {{"ctrlX1", 0.2}, {"ctrlY1", 0}, {"ctrlX2", 0.2}, {"ctrlY2", 1}};
    std::map<std::string, float> inFrictionCurve2 = {{"ctrlX1", 0}, {"ctrlY1", 0}, {"ctrlX2", 0}, {"ctrlY2", 1}};
    std::map<std::string, float> sharpCurve = {{"ctrlX1", 0.33}, {"ctrlY1", 0}, {"ctrlX2", 0.67}, {"ctrlY2", 1}};
    std::map<std::string, float> outFrictionCurve = {
        {"ctrlX1", 0.83}, {"ctrlY1", 0}, {"ctrlX2", 0.95}, {"ctrlY2", 0.77}
    };
    int factor = direction_ == TextEffectFlipDirection::DOWN ? 1 : -1;

    Drawing::DrawingPiecewiseParameter outTranslateParameter = { Drawing::DrawingCurveType::FRICTION,
        outFrictionCurve, 100, 0, {{g_translateX, {0, 0}}, {g_translateY, {0, 0.5 * height * factor}}}};
    parameters[0].emplace_back(outTranslateParameter);

    Drawing::DrawingPiecewiseParameter outScaleParameter = { Drawing::DrawingCurveType::FRICTION,
        outFrictionCurve, 100, 0, {{g_scaleX, {1, 0.6}}, {g_scaleY, {1, 0.6}}}};
    parameters[0].emplace_back(outScaleParameter);

    Drawing::DrawingPiecewiseParameter outAlphaParameter = { Drawing::DrawingCurveType::LINEAR,
        sharpCurve, 100, 0, {{g_alphaProp, {1, 0}}}};
    parameters[0].emplace_back(outAlphaParameter);

    Drawing::DrawingPiecewiseParameter inTranslateParameter1 = {
        Drawing::DrawingCurveType::FRICTION, inFrictionCurve, 175, 0,
        {{g_translateX, {0, 0}}, {g_translateY, {-0.6 * height * factor, 0.1 * height * factor}}}
    };
    parameters[1].emplace_back(inTranslateParameter1);

    Drawing::DrawingPiecewiseParameter inTranslateParameter2 = { Drawing::DrawingCurveType::FRICTION,
        inFrictionCurve2, 175, 175, {{g_translateX, {0, 0}}, {g_translateY, {0.1 * height * factor, 0}}}};
    parameters[1].emplace_back(inTranslateParameter2);

    Drawing::DrawingPiecewiseParameter inScaleParameter = { Drawing::DrawingCurveType::FRICTION,
        inFrictionCurve, 350, 0, {{g_scaleX, {0.8, 1.0}}, {g_scaleY, {0.8, 1.0}}}};
    parameters[1].emplace_back(inScaleParameter);

    Drawing::DrawingPiecewiseParameter inAlphaParameter = { Drawing::DrawingCurveType::SHARP,
        sharpCurve, 150, 0, {{g_alphaProp, {0, 1}}}};
    parameters[1].emplace_back(inAlphaParameter);

    if (blurEnable_) {
        Drawing::DrawingPiecewiseParameter outBlurParameter = {
            Drawing::DrawingCurveType::LINEAR, sharpCurve, 100, 0, {{g_blurProp, {0, 25}}}
        };
        parameters[0].emplace_back(outBlurParameter);

        Drawing::DrawingPiecewiseParameter inBlurParameter = {
            Drawing::DrawingCurveType::SHARP, sharpCurve, 150, 0, {{g_blurProp, {25, 0}}}
        };
        parameters[1].emplace_back(inBlurParameter);
    }
    return parameters;
}

REGISTER_TEXT_EFFECT_FACTORY_IMPL(Flip, TextEffectStrategy::FLIP);
} // namespace OHOS::Rosen