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

#include "text_effect.h"
#include "draw/path.h"
#include "hm_symbol_node_build.h"
#include "utils/text_log.h"

namespace OHOS::Rosen::SPText {

void TextEffect::SetUniqueId(uint64_t uniqueId)
{
    uniqueId_ = uniqueId;
}

void TextEffect::SetAnimation(
    const std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
    animationFunc)
{
    if (animationFunc) {
        animationFunc_ = animationFunc;
    }
}

void TextEffect::SetEffectConfig(const std::vector<std::vector<Drawing::DrawingPiecewiseParameter>>& parameters)
{
    parameters_ = parameters;
    currentAnimationHasPlayed_ = false;
}

void TextEffect::SetColor(const RSColor& color)
{
    color_ = color.CastToColorQuad();
}

void TextEffect::SetAnimationStart(bool animationStart)
{
    animationStart_ = animationStart;
}

void TextEffect::SetEffectStrategy(RSEffectStrategy effectStrategy)
{
    effectStrategy_ = effectStrategy;
    currentAnimationHasPlayed_ = false;
}

bool TextEffect::DrawTextEffect(RSCanvas* canvas, const std::vector<TextEngine::TextEffectElement>& effectElements)
{
    if (canvas == nullptr) {
        TEXT_LOGE("Invalid input, canvas is nullptr.");
        return false;
    }

    if (effectElements.empty()) {
        TEXT_LOGE("Invalid input, effectElements is empty.");
        return false;
    }

    if (animationStart_) {
        if (TextAnimationRun(effectElements)) {
            currentAnimationHasPlayed_ = true;
            return true;
        }
    }
    currentAnimationHasPlayed_ = false;
    OnDrawTextEffect(canvas, effectElements);
    return false;
}

bool TextEffect::TextAnimationRun(const std::vector<TextEngine::TextEffectElement>& effectElements)
{
    if (animationFunc_ == nullptr) {
        return false;
    }
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->effectStrategy = effectStrategy_;
    symbolAnimationConfig->animationStart = animationStart_;
    symbolAnimationConfig->currentAnimationHasPlayed = currentAnimationHasPlayed_;
    symbolAnimationConfig->parameters = parameters_;
    symbolAnimationConfig->color = color_.CastToColorQuad();
    for (const auto& element: effectElements) {
        symbolAnimationConfig->effectElement = element;
        symbolAnimationConfig->symbolSpanId = element.uniqueId;
        if (!animationFunc_(symbolAnimationConfig)) {
            return false;
        }
    }

    return true;
}

void TextEffect::OnDrawTextEffect(RSCanvas* canvas,
    const std::vector<TextEngine::TextEffectElement>& effectElements)
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(color_.CastToColorQuad());
    for (const auto& effectElement : effectElements) {
        ClearTextAnimation(effectElement.uniqueId);
        auto path = effectElement.path;
        path.Offset(effectElement.offset.GetX(), effectElement.offset.GetY());
        canvas->AttachBrush(brush);
        canvas->DrawPath(path);
        canvas->DetachBrush();
    }
}

void TextEffect::ClearTextAnimation(uint64_t uniqueId)
{
    if (animationFunc_ == nullptr) {
        return;
    }
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::NONE;
    symbolAnimationConfig->symbolSpanId = uniqueId;
    animationFunc_(symbolAnimationConfig);
}
}