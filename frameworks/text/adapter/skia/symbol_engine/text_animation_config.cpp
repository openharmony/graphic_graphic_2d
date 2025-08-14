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

#include "text_animation_config.h"
#include "draw/path.h"
#include "hm_symbol_node_build.h"
#include "utils/text_log.h"
#include "utils/text_trace.h"

namespace OHOS::Rosen::SPText {

void TextAnimationConfig::SetUniqueId(uint64_t uniqueId)
{
    uniqueId_ = uniqueId;
}

void TextAnimationConfig::SetAnimation(
    const std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
    animationFunc)
{
    if (animationFunc) {
        animationFunc_ = animationFunc;
    }
}

void TextAnimationConfig::SetEffectConfig(
    const std::vector<std::vector<Drawing::DrawingPiecewiseParameter>>& parameters)
{
    parameters_ = parameters;
    currentAnimationHasPlayed_ = false;
}

void TextAnimationConfig::SetColor(const RSColor& color)
{
    color_ = color.CastToColorQuad();
}

void TextAnimationConfig::SetAnimationStart(bool animationStart)
{
    animationStart_ = animationStart;
}

void TextAnimationConfig::SetEffectStrategy(RSEffectStrategy effectStrategy)
{
    effectStrategy_ = effectStrategy;
    currentAnimationHasPlayed_ = false;
}

bool TextAnimationConfig::DrawTextEffect(RSCanvas* canvas,
    const std::vector<TextEngine::TextEffectElement>& effectElements)
{
    TEXT_TRACE_FUNC();
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

bool TextAnimationConfig::TextAnimationRun(const std::vector<TextEngine::TextEffectElement>& effectElements)
{
    TEXT_TRACE_FUNC();
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

void TextAnimationConfig::OnDrawTextEffect(RSCanvas* canvas,
    const std::vector<TextEngine::TextEffectElement>& effectElements)
{
    TEXT_TRACE_FUNC();
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(color_.CastToColorQuad());
    canvas->AttachBrush(brush);
    for (const auto& effectElement : effectElements) {
        ClearTextAnimation(effectElement.uniqueId);
        auto path = effectElement.path;
        path.Offset(effectElement.offset.GetX(), effectElement.offset.GetY());
        canvas->DrawPath(path);
    }
    canvas->DetachBrush();
}

void TextAnimationConfig::ClearTextAnimation(uint64_t uniqueId)
{
    if (animationFunc_ == nullptr) {
        return;
    }
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::NONE;
    symbolAnimationConfig->symbolSpanId = uniqueId;
    animationFunc_(symbolAnimationConfig);
}

void TextAnimationConfig::AnimationUnchange(bool isUnchange)
{
    currentAnimationHasPlayed_ = isUnchange;
}

void TextAnimationConfig::ClearAllTextAnimation()
{
    TEXT_TRACE_FUNC();
    if (animationFunc_ == nullptr) {
        return;
    }
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::TEXT_FLIP;
    symbolAnimationConfig->animationStart = false;
    animationFunc_(symbolAnimationConfig);
}
}