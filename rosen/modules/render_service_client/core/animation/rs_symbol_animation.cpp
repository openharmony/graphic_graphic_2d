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


#include "animation/rs_symbol_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "platform/common/rs_log.h"
#include "draw/paint.h"
#include "utils/point.h"


namespace OHOS {
namespace Rosen {
static const Vector2f CENTER_NODE_COORDINATE = {0.5f, 0.5f}; //scale center node

RSSymbolAnimation::RSSymbolAnimation()
{
}

RSSymbolAnimation::~RSSymbolAnimation()
{
}

bool RSSymbolAnimation::SetSymbolAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (rsNode_ == nullptr || symbolAnimationConfig == nullptr) {
        ROSEN_LOGE("HmSymbol RSSymbolAnimation::getNode or get symbolAnimationConfig:failed");
        return false;
    }

    if (rsNode_->canvasNodesListMap.count(symbolAnimationConfig->symbolSpanId) > 0) {
        rsNode_->canvasNodesListMap.erase(symbolAnimationConfig->symbolSpanId);
    }

    if (symbolAnimationConfig->effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_NONE) {
        return true; // pre code already clear nodes.
    }

    if (symbolAnimationConfig->effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_SCALE) {
        return SetScaleUnitAnimation(symbolAnimationConfig);
    } else if (symbolAnimationConfig->effectStrategy ==
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_HIERARCHICAL) {
        return SetVariableColorAnimation(symbolAnimationConfig);
    }
    return false;
}

template<typename T>
bool RSSymbolAnimation::CreateOrSetModifierValue(std::shared_ptr<RSAnimatableProperty<T>>& property, const T& value)
{
    if (property == nullptr) {
        property = std::make_shared<RSAnimatableProperty<T>>(value);
        return true;
    }
    property->Set(value);
    return false;
}

bool RSSymbolAnimation::isEqual(const Vector2f val1, const Vector2f val2)
{
    return(val1.x_ == val2.x_ && val1.y_ == val2.y_);
}

Vector4f RSSymbolAnimation::CalculateOffset(const Drawing::Path &path, const float &offsetX, const float &offsetY)
{
    auto rect = path.GetBounds();
    float left = rect.GetLeft();
    float top = rect.GetTop();
    // the nodeTranslation is offset of new node to the father node;
    // the newOffset is offset of path on new node;
    Vector2f nodeTranslation = {offsetX + left, offsetY + top};
    Vector2f newOffset = {-left, -top};

    return Vector4f(nodeTranslation[0], nodeTranslation[1], newOffset[0], newOffset[1]);
}

bool RSSymbolAnimation::SetScaleUnitAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&
    symbolAnimationConfig)
{
    auto nodeNum = symbolAnimationConfig->numNodes;
    if (nodeNum <= 0) {
        return false;
    }
    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    auto canvasNode = RSCanvasNode::Create();
    if (rsNode_->canvasNodesListMap.count(symbolSpanId) > 0) {
        rsNode_->canvasNodesListMap[symbolSpanId].emplace_back(canvasNode);
    } else {
        rsNode_->canvasNodesListMap[symbolSpanId] = {canvasNode};
    }

    auto& symbolNode = symbolAnimationConfig->SymbolNodes[0];
    Vector4f offsets = CalculateOffset(symbolNode.symbolData.path_,
        symbolNode.nodeBoundary[0], symbolNode.nodeBoundary[1]); // index 0 offsetX of layout, 1 offsetY of layout
    SetSymbolGeometry(canvasNode, Vector4f(offsets[0], offsets[1], // index 0 offsetX of newNode, 1 offsetY of newNode
        symbolNode.nodeBoundary[2], symbolNode.nodeBoundary[3])); // index 2 width 3 height
    const Vector2f scaleValueBegin = {1.0f, 1.0f}; // 1.0 scale
    const Vector2f scaleValue = {1.15f, 1.15f}; // 1.5 scale
    const Vector2f scaleValueEnd = scaleValueBegin;
    auto animation = ScaleSymbolAnimation(canvasNode, scaleValueBegin, scaleValue, scaleValueEnd);
    if (!animation) {
        return false;
    }
    animation->Start(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(symbolNode.nodeBoundary[2], symbolNode.nodeBoundary[3]);

        Drawing::Brush brush;
        Drawing::Pen pen;
        SetIconProperty(brush, pen, symbolNode);
        Drawing::Point offsetLocal = Drawing::Point{offsets[2], offsets[3]}; // index 2 offsetX 3 offsetY
        recordingCanvas->AttachBrush(brush);
        recordingCanvas->DrawSymbol(symbolNode.symbolData, offsetLocal);
        recordingCanvas->DetachBrush();
        recordingCanvas->AttachPen(pen);
        recordingCanvas->DrawSymbol(symbolNode.symbolData, offsetLocal);
        recordingCanvas->DetachPen();
    canvasNode->FinishRecording();
    rsNode_->AddChild(canvasNode, -1);
    return true;
}

void RSSymbolAnimation::SetSymbolGeometry(const std::shared_ptr<RSNode>& rsNode, const Vector4f& bounds)
{
    if (rsNode == nullptr) {
        return;
    }
    std::shared_ptr<RSAnimatableProperty<Vector4f>> frameProperty = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector4f>> boundsProperty = nullptr;

    bool isFrameCreate = CreateOrSetModifierValue(frameProperty, bounds);
    if (isFrameCreate) {
        auto frameModifier = std::make_shared<RSFrameModifier>(frameProperty);
        rsNode->AddModifier(frameModifier);
    }
    bool isBoundsCreate = CreateOrSetModifierValue(boundsProperty, bounds);
    if (isBoundsCreate) {
        auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
        rsNode->AddModifier(boundsModifier);
    }
}

std::shared_ptr<RSAnimation> RSSymbolAnimation::ScaleSymbolAnimation(
    const std::shared_ptr<RSNode>& rsNode, const Vector2f& scaleValueBegin,
    const Vector2f& scaleValue, const Vector2f& scaleValueEnd, int delay)
{
    bool isCreate = CreateOrSetModifierValue(scaleStartProperty_, scaleValueBegin);
    if (isCreate) {
        auto scaleModifier = std::make_shared<RSScaleModifier>(scaleStartProperty_);
        rsNode->AddModifier(scaleModifier);
    }
    CreateOrSetModifierValue(scaleProperty_, scaleValue);
    CreateOrSetModifierValue(scaleEndProperty_, scaleValueEnd);
    Vector2f curNodePivot = rsNode->GetStagingProperties().GetPivot();
    if (!isEqual(curNodePivot, CENTER_NODE_COORDINATE)) {
        bool isCreate = CreateOrSetModifierValue(pivotProperty_, CENTER_NODE_COORDINATE);
        if (isCreate) {
            auto pivotModifier = std::make_shared<RSPivotModifier>(pivotProperty_);
            rsNode->AddModifier(pivotModifier);
        }
    }

    RSAnimationTimingCurve scaleCurve = SetScaleSpringTimingCurve();

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(scaleStartProperty_);
    keyframeAnimation->SetDuration(2000); // duration is 2000ms
    keyframeAnimation->AddKeyFrame(0.25f, scaleProperty_, scaleCurve);
    keyframeAnimation->AddKeyFrame(0.75f, scaleProperty_, scaleCurve);
    keyframeAnimation->AddKeyFrame(1.f, scaleEndProperty_, scaleCurve);
    return keyframeAnimation;
}

RSAnimationTimingCurve RSSymbolAnimation::SetScaleSpringTimingCurve()
{
    float velocity = 0;
    float mass = 1;
    float stiffness = 228;
    float damping = 22;
    RSAnimationTimingCurve scaleCurve = RSAnimationTimingCurve::CreateSpringCurve(
        velocity, mass, stiffness, damping);
    return scaleCurve;
}

bool RSSymbolAnimation::SetVariableColorAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&
    symbolAnimationConfig)
{
    auto nodeNum = symbolAnimationConfig->numNodes;
    if (nodeNum <= 0) {
        RS_LOGD("SetScaleUnitAnimation numNodes <= 0");
        return false;
    }

    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    auto& symbolFistNode = symbolAnimationConfig->SymbolNodes[0];
    Vector4f offsets = CalculateOffset(symbolFistNode.symbolData.path_,
        symbolFistNode.nodeBoundary[0], symbolFistNode.nodeBoundary[1]); // index 0 offsetX and 1 offsetY of layout
    for (uint32_t n = 0; n < nodeNum; n++) {
        auto& symbolNode = symbolAnimationConfig->SymbolNodes[n];
        auto canvasNode = RSCanvasNode::Create();
        if (rsNode_->canvasNodesListMap.count(symbolSpanId) > 0) {
            rsNode_->canvasNodesListMap[symbolSpanId].emplace_back(canvasNode);
        } else {
            rsNode_->canvasNodesListMap[symbolSpanId] = {canvasNode};
        }
        SetSymbolGeometry(canvasNode, Vector4f(offsets[0], offsets[1], //0: offsetX and 1: offsetY of newNode
            symbolNode.nodeBoundary[2], symbolNode.nodeBoundary[3])); // 2: width 3: height
        CreateOrSetModifierValue(alphaPropertyPhase1_, 0.4f); // 0.4 means 40% alpha
        CreateOrSetModifierValue(alphaPropertyPhase2_, 0.6f); // 0.6 means 60% alpha
        CreateOrSetModifierValue(alphaPropertyPhase3_, 1.0f); // 1.0 means 100% alpha

        std::shared_ptr<RSAnimation> animation;
        auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaPropertyPhase1_);
        canvasNode->AddModifier(alphaModifier);
        if (symbolNode.animationIndex == 0) { // 0 first layer
            animation = VariableColorSymbolAnimationNodeThird(canvasNode);
        } else if (symbolNode.animationIndex == 1) { // 1 second layer
            animation = VariableColorSymbolAnimationNodeSecond(canvasNode);
        } else if (symbolNode.animationIndex == 2) { // 2 first layer
            animation = VariableColorSymbolAnimationNodeFirst(canvasNode);
        }
        if (animation == nullptr) {
            return false;
        }
        animation->Start(canvasNode);
        auto recordingCanvas = canvasNode->BeginRecording(symbolNode.nodeBoundary[2], symbolNode.nodeBoundary[3]);
            Drawing::Brush brush;
            Drawing::Pen pen;
            SetIconProperty(brush, pen, symbolNode);
            symbolNode.path.Offset(offsets[2], offsets[3]); // index 2 offsetX 3 offsetY
            recordingCanvas->AttachBrush(brush);
            recordingCanvas->AttachPen(pen);
            recordingCanvas->DrawPath(symbolNode.path);
            recordingCanvas->DetachBrush();
            recordingCanvas->DetachPen();
        canvasNode->FinishRecording();
        rsNode_->AddChild(canvasNode, -1);
    }
    return true;
}

void RSSymbolAnimation::SetIconProperty(Drawing::Brush& brush, Drawing::Pen& pen,
    TextEngine::SymbolNode& symbolNode)
{
    brush.SetColor(Drawing::Color::ColorQuadSetARGB(0xFF, symbolNode.color.r,
        symbolNode.color.g, symbolNode.color.b));
    brush.SetAlphaF(symbolNode.color.a);
    brush.SetAntiAlias(true);

    pen.SetColor(Drawing::Color::ColorQuadSetARGB(0xFF, symbolNode.color.r,
        symbolNode.color.g, symbolNode.color.b));
    pen.SetAlphaF(symbolNode.color.a);
    pen.SetAntiAlias(true);
    return;
}

std::shared_ptr<RSAnimation> RSSymbolAnimation::VariableColorSymbolAnimationNodeFirst(const std::shared_ptr<RSNode>&
    rsNode)
{
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(alphaPropertyPhase1_);
    if (!keyframeAnimation) {
        RS_LOGD("VariableColorSymbolAnimationNodeFirst nullptr");
        return nullptr;
    }
    keyframeAnimation->SetDuration(1300); // duration is 100+150+200+250+600=1300ms
    // at different moments, the alpha value is a different value
    // e.g. 0.08 = 100/1300, at this moment of 100ms, the alpha value is the value of alphaPropertyPhase2_
    // e.g. 0.19 = (100+150)/1300, at this moment of 250ms, the alpha value is the value of alphaPropertyPhase3_
    keyframeAnimation->AddKeyFrame(0.08f, alphaPropertyPhase2_, RSAnimationTimingCurve::LINEAR); // 0.08 is 8% of time
    keyframeAnimation->AddKeyFrame(0.19f, alphaPropertyPhase3_, RSAnimationTimingCurve::LINEAR); // 0.19 is 19% of time
    keyframeAnimation->AddKeyFrame(0.35f, alphaPropertyPhase2_, RSAnimationTimingCurve::LINEAR); // 0.35 is 35% of time
    keyframeAnimation->AddKeyFrame(0.54f, alphaPropertyPhase1_, RSAnimationTimingCurve::LINEAR); // 0.54 is 54% of time
    keyframeAnimation->AddKeyFrame(1.f, alphaPropertyPhase1_, RSAnimationTimingCurve::LINEAR); // 1. is 100% of time
    return keyframeAnimation;
}

std::shared_ptr<RSAnimation> RSSymbolAnimation::VariableColorSymbolAnimationNodeSecond(const std::shared_ptr<RSNode>&
    rsNode)
{
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(alphaPropertyPhase1_);
    if (!keyframeAnimation) {
        RS_LOGD("VariableColorSymbolAnimationNodeSecond nullptr");
        return nullptr;
    }
    keyframeAnimation->SetStartDelay(100); // 100ms is the time gap between first layer and second layer
    keyframeAnimation->SetDuration(1200); // duration is 150+200+250+200+400=1200ms
    keyframeAnimation->AddKeyFrame(0.125f, alphaPropertyPhase2_, RSAnimationTimingCurve::LINEAR); // 0.125 time percent
    keyframeAnimation->AddKeyFrame(0.29f, alphaPropertyPhase3_, RSAnimationTimingCurve::LINEAR); // 0.29 is 29% of time
    keyframeAnimation->AddKeyFrame(0.5f, alphaPropertyPhase2_, RSAnimationTimingCurve::LINEAR); // 0.5 is 50% of time
    keyframeAnimation->AddKeyFrame(0.67f, alphaPropertyPhase1_, RSAnimationTimingCurve::LINEAR); // 0.67 is 67% of time
    keyframeAnimation->AddKeyFrame(1.f, alphaPropertyPhase1_, RSAnimationTimingCurve::LINEAR); // 1. is 100% of time
    return keyframeAnimation;
}

std::shared_ptr<RSAnimation> RSSymbolAnimation::VariableColorSymbolAnimationNodeThird(const std::shared_ptr<RSNode>&
    rsNode)
{
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(alphaPropertyPhase1_);
    if (!keyframeAnimation) {
        RS_LOGD("VariableColorSymbolAnimationNodeThird nullptr");
        return nullptr;
    }
    keyframeAnimation->SetStartDelay(250); // 250ms is the time gap between first layer and third layer
    keyframeAnimation->SetDuration(1050); // duration is 200+250+200+150+250=1050ms
    keyframeAnimation->AddKeyFrame(0.19f, alphaPropertyPhase2_, RSAnimationTimingCurve::LINEAR); // 0.19 is 19% of time
    keyframeAnimation->AddKeyFrame(0.43f, alphaPropertyPhase3_, RSAnimationTimingCurve::LINEAR); // 0.43 is 43% of time
    keyframeAnimation->AddKeyFrame(0.62f, alphaPropertyPhase2_, RSAnimationTimingCurve::LINEAR); // 0.62 is 62% of time
    keyframeAnimation->AddKeyFrame(0.76f, alphaPropertyPhase1_, RSAnimationTimingCurve::LINEAR); // 0.76 is 76% of time
    keyframeAnimation->AddKeyFrame(1.f, alphaPropertyPhase1_, RSAnimationTimingCurve::LINEAR); // 1. is 100% of time
    return keyframeAnimation;
}
} // namespace Rosen
} // namespace OHOS