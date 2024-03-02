/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_SYMBOL_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_SYMBOL_ANIMATION_H

#include <string>
#include <map>
#include <vector>
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"
#include "animation/rs_animation_timing_curve.h"
#include "symbol_animation_config.h"

#include "draw/path.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSSymbolAnimation {
public:
    RSSymbolAnimation();
    virtual ~RSSymbolAnimation();
    bool SetScaleUnitAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    void SetSymbolGeometry(const std::shared_ptr<RSNode>& rsNode, const Vector4f& bounds);
    bool SetVariableColorAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);

    // set symbol animation manager
    bool SetSymbolAnimation(
        const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);

    void SetNode(std::shared_ptr<RSNode>& rsNode)
    {
        rsNode_ = rsNode;
    }

private:
    std::shared_ptr<RSNode> rsNode_ = nullptr;

    std::shared_ptr<RSAnimation> ScaleSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
        const Vector2f& scaleValueBegin = Vector2f{0.f, 0.f},
        const Vector2f& scaleValue = Vector2f{0.f, 0.f},
        const Vector2f& scaleValueEnd = Vector2f{0.f, 0.f},
        const int delay = 0);
    void SetIconProperty(Drawing::Brush& brush, Drawing::Pen& pen, TextEngine::SymbolNode& symbolNode);
    RSAnimationTimingCurve SetScaleSpringTimingCurve();

    bool isEqual(const Vector2f val1, const Vector2f val2);

    Vector4f CalculateOffset(const Drawing::Path &path, const float &offsetX, const float &offsetY);

    template<typename T>
    bool CreateOrSetModifierValue(std::shared_ptr<RSAnimatableProperty<T>>& property, const T& value);

    std::shared_ptr<RSAnimation> VariableColorSymbolAnimationNodeFirst(const std::shared_ptr<RSNode>& rsNode);
    std::shared_ptr<RSAnimation> VariableColorSymbolAnimationNodeSecond(const std::shared_ptr<RSNode>& rsNode);
    std::shared_ptr<RSAnimation> VariableColorSymbolAnimationNodeThird(const std::shared_ptr<RSNode>& rsNode);

    // scale symbol animation
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleStartProperty_;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty_;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleEndProperty_;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> pivotProperty_;

    // none symbol animation
    std::shared_ptr<RSAnimatableProperty<Vector2f>> pivotNone1Property_;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> pivotNone2Property_;

    // variableColor
    std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase1_;
    std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase2_;
    std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase3_;
};
} // namespace Rosen
} // namespace OHOS

#endif
