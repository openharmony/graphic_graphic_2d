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

#ifndef SYMBOL_ENGINE_TEXT_ANIMATION_CONFIG_H
#define SYMBOL_ENGINE_TEXT_ANIMATION_CONFIG_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "drawing.h"
#include "rosen_text/symbol_animation_config.h"

namespace OHOS::Rosen::SPText {
class TextAnimationConfig {
public:
    ~TextAnimationConfig() = default;

    TextAnimationConfig() = default;

    void SetUniqueId(uint64_t uniqueId);

    void SetEffectConfig(const std::vector<std::vector<Drawing::DrawingPiecewiseParameter>>& parameters);

    void SetColor(const RSColor& color);

    bool DrawTextEffect(RSCanvas* canvas, const std::vector<TextEngine::TextEffectElement>& effectElements);

    void SetAnimationStart(bool animationStart);

    void SetEffectStrategy(RSEffectStrategy effectStrategy);

    void SetAnimation(
        const std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
        animationFunc);

private:
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc_ = nullptr;

    uint64_t uniqueId_ = 0;
    RSColor color_;
    bool animationStart_ = false;
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters_;
    RSEffectStrategy effectStrategy_ = RSEffectStrategy::NONE;
    bool currentAnimationHasPlayed_ = false;

    void ClearTextAnimation(uint64_t uniqueId);

    bool TextAnimationRun(const std::vector<TextEngine::TextEffectElement>& effectElements);

    void OnDrawTextEffect(RSCanvas* canvas, const std::vector<TextEngine::TextEffectElement>& effectElements);
};
}
#endif