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

#include <map>
#include <string>
#include <vector>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_symbol_node_config.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "draw/path.h"
#include "include/text/hm_symbol_config_ohos.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"
#include "symbol_animation_config.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"

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

private:
    // SetPublicAnimation is interface for animation that can be spliced by atomizated animations
    bool SetPublicAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    bool GetAnimationGroupParameters(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
        std::vector<std::vector<Drawing::DrawingPiecewiseParameter>>& parameters,
        TextEngine::SymbolAnimationEffectStrategy& effectStrategy);
    // choose the animation is a public animation or special animation
    bool ChooseAnimation(const std::shared_ptr<RSNode>& rsNode,
        std::vector<Drawing::DrawingPiecewiseParameter>& parameters,
        const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    bool SetKeyframeAlphaAnimation(const std::shared_ptr<RSNode>& rsNode,
        std::vector<Drawing::DrawingPiecewiseParameter>& parameters,
        const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    bool SetScaleUnitAnimation(const std::shared_ptr<RSNode>& rsNode,
        std::vector<Drawing::DrawingPiecewiseParameter>& parameters);

    void InitSupportAnimationTable();

    // to start animations for one path group
    void GroupAnimationStart(
        const std::shared_ptr<RSNode>& rsNode, std::vector<std::shared_ptr<RSAnimation>>& animations);

    // Set Node Center Offset
    void SetNodePivot(const std::shared_ptr<RSNode>& rsNode);

    // splice atomizated animation construct
    void SpliceAnimation(const std::shared_ptr<RSNode>& rsNode,
        std::vector<Drawing::DrawingPiecewiseParameter>& parameters,
        const TextEngine::SymbolAnimationEffectStrategy& effectStrategy);

    void BounceAnimation(
        const std::shared_ptr<RSNode>& rsNode, std::vector<Drawing::DrawingPiecewiseParameter>& parameters);
    void AppearAnimation(
        const std::shared_ptr<RSNode>& rsNode, std::vector<Drawing::DrawingPiecewiseParameter>& parameters);

    // atomizated animation construct
    void ScaleAnimationBase(const std::shared_ptr<RSNode>& rsNode, Drawing::DrawingPiecewiseParameter& scaleParamter,
        std::vector<std::shared_ptr<RSAnimation>>& animations);
    void AlphaAnimationBase(const std::shared_ptr<RSNode>& rsNode, Drawing::DrawingPiecewiseParameter& alphaParamter,
        std::vector<std::shared_ptr<RSAnimation>>& animations);

    // drawing a path group : symbol drawing or path drawing
    void GroupDrawing(const std::shared_ptr<RSCanvasNode>& canvasNode, TextEngine::SymbolNode& symbolNode,
        const Vector4f& offsets, bool isMultiLayer);

    void SetIconProperty(Drawing::Brush& brush, Drawing::Pen& pen, TextEngine::SymbolNode& symbolNode);

    Vector4f CalculateOffset(const Drawing::Path& path, const float offsetX, const float offsetY);
    void DrawSymbolOnCanvas(
        ExtendRecordingCanvas* recordingCanvas, TextEngine::SymbolNode& symbolNode, const Vector4f& offsets);
    void DrawPathOnCanvas(
        ExtendRecordingCanvas* recordingCanvas, TextEngine::SymbolNode& symbolNode, const Vector4f& offsets);
    bool CalcTimePercents(std::vector<float>& timePercents, const float totalDuration,
        const std::vector<Drawing::DrawingPiecewiseParameter>& oneGroupParas);

    std::shared_ptr<RSAnimation> ScaleSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
        const Drawing::DrawingPiecewiseParameter& scaleUnitParas,
        const Vector2f& scaleValueEnd = Vector2f { 0.f, 0.f });
    bool GetScaleUnitAnimationParas(
        Drawing::DrawingPiecewiseParameter& scaleUnitParas, Vector2f& scaleValueBegin, Vector2f& scaleValueEnd);

    std::shared_ptr<RSAnimation> KeyframeAlphaSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
        const Drawing::DrawingPiecewiseParameter& oneStageParas,
        const uint32_t duration, const std::vector<float>& timePercents);
    bool GetKeyframeAlphaAnimationParas(std::vector<Drawing::DrawingPiecewiseParameter>& oneGroupParas,
        uint32_t& totalDuration, std::vector<float>& timePercents);

    // Set Replace Animation which include disappear stage and appear stage
    bool SetReplaceAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    // Set Disappear stage of replace animation
    bool SetReplaceDisappear(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    // Set appear stage of replace animation
    bool SetReplaceAppear(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
        bool isStartAnimation=true);

    // process node before animation include clean invalid node and config info
    void NodeProcessBeforeAnimation(
        const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    // pop invalid node before replace animation, replace animation have special rsnode lifecycle.
    void PopNodeFromReplaceList(uint64_t symbolSpanId);

    std::shared_ptr<RSNode> rsNode_ = nullptr;
    // scale symbol animation
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty_ = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> pivotProperty_ = nullptr;

    // variableColor symbol animation
    std::vector<std::shared_ptr<RSAnimatableProperty<float>>> alphaPropertyStages_;

    // animation support splice base animation
    std::vector<TextEngine::SymbolAnimationEffectStrategy> publicSupportAnimations_ = {};
    // animation support up&down interface
    std::vector<TextEngine::SymbolAnimationEffectStrategy> upAndDownSupportAnimations_ = {};
};
} // namespace Rosen
} // namespace OHOS

#endif
