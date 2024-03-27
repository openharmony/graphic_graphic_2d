/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "include/text/hm_symbol_config_ohos.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSSymbolAnimation {
public:
    RSSymbolAnimation();
    virtual ~RSSymbolAnimation();
    void SetNode(std::shared_ptr<RSNode>& rsNode)
    {
        rsNode_ = rsNode;
    }
    bool SetSymbolGeometry(const std::shared_ptr<RSNode>& rsNode, const Vector4f& bounds);

    // set symbol animation manager
    bool SetSymbolAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);

    bool SetScaleUnitAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    bool SetVariableColorAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);

private:
    void SetIconProperty(Drawing::Brush& brush, Drawing::Pen& pen, TextEngine::SymbolNode& symbolNode);

    Vector4f CalculateOffset(const Drawing::Path& path, const float& offsetX, const float& offsetY);
    void DrawSymbolOnCanvas(ExtendRecordingCanvas* recordingCanvas,
        TextEngine::SymbolNode& symbolNode, const Vector4f& offsets);
    void DrawPathOnCanvas(ExtendRecordingCanvas* recordingCanvas,
        TextEngine::SymbolNode& symbolNode, const Vector4f& offsets);
    bool CalcTimePercents(std::vector<float>& timePercents, const float totalDuration,
        const std::vector<Drawing::DrawingPiecewiseParameter>& oneGroupParas);

    std::shared_ptr<RSAnimation> ScaleSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
        const Drawing::DrawingPiecewiseParameter& scaleUnitParas,
        const Vector2f& scaleValueBegin = Vector2f{0.f, 0.f},
        const Vector2f& scaleValue = Vector2f{0.f, 0.f},
        const Vector2f& scaleValueEnd = Vector2f{0.f, 0.f});
    bool GetScaleUnitAnimationParas(Drawing::DrawingPiecewiseParameter& scaleUnitParas,
        Vector2f& scaleValueBegin, Vector2f& scaleValue);
    RSAnimationTimingCurve SetScaleSpringTimingCurve(const std::map<std::string, double_t>& curveArgs);

    std::shared_ptr<RSAnimation> VariableColorSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
        const uint32_t& duration, const int& delay, const std::vector<float>& timePercents);
    bool GetVariableColorAnimationParas(const uint32_t index, uint32_t& totalDuration, int& delay,
        std::vector<float>& timePercents);

    std::shared_ptr<RSNode> rsNode_ = nullptr;

    // scale symbol animation
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleStartProperty_ = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty_ = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleEndProperty_ = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> pivotProperty_ = nullptr;

    // variableColor symbol animation
    std::vector<std::shared_ptr<RSAnimatableProperty<float>>> alphaPropertyPhases_;
};
} // namespace Rosen
} // namespace OHOS

#endif
