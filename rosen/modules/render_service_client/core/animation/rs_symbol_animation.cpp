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

#include "animation/rs_symbol_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "draw/paint.h"
#include "platform/common/rs_log.h"
#include "utils/point.h"

namespace OHOS {
namespace Rosen {
static const Vector2f CENTER_NODE_COORDINATE = { 0.5f, 0.5f }; // scale center node
static const std::string SCALE_PROP_X = "sx";
static const std::string SCALE_PROP_Y = "sy";
static const std::string ALPHA_PROP = "alpha";
static const unsigned int PROPERTIES = 2; // symbol animation property contains two values, change from one to the other
static const unsigned int PROP_START = 0; // symbol animation property contains two values, change from START to the END
static const unsigned int PROP_END = 1;   // symbol animation property contains two values, change from START to the END
static const unsigned int WIDTH = 2;
static const unsigned int HEIGHT = 3;
static const unsigned int INVALID_STATUS = -1;  // invalid status label
static const unsigned int APPEAR_STATUS = 1 ;   // appear status label

namespace SymbolAnimation {
template<typename T>
bool CreateOrSetModifierValue(std::shared_ptr<RSAnimatableProperty<T>>& property, const T& value)
{
    if (property == nullptr) {
        property = std::make_shared<RSAnimatableProperty<T>>(value);
        return true;
    }
    property->Set(value);
    return false;
}

template<typename T>
bool ElementInMap(const std::string& curElement, const std::map<std::string, T>& curMap)
{
    if (curMap.empty()) {
        return false;
    }
    auto element = curMap.find(curElement);
    return (element != curMap.end());
}

float CurveArgsInMap(const std::string& curElement, const std::map<std::string, double>& curMap)
{
    if (curMap.empty()) {
        return 0.0;
    }
    auto element = curMap.find(curElement);
    if (element == curMap.end()) {
        return 0.0;
    }
    return static_cast<float>(element->second);
}

void CreateAnimationTimingCurve(const OHOS::Rosen::Drawing::DrawingCurveType type,
    const std::map<std::string, double>& curveArgs, RSAnimationTimingCurve& curve)
{
    curve = RSAnimationTimingCurve();
    if (type == OHOS::Rosen::Drawing::DrawingCurveType::LINEAR) {
        curve = RSAnimationTimingCurve::LINEAR;
    } else if (type == OHOS::Rosen::Drawing::DrawingCurveType::SPRING) {
        float scaleVelocity = CurveArgsInMap("velocity", curveArgs);
        float scaleMass = CurveArgsInMap("mass", curveArgs);
        float scaleStiffness = CurveArgsInMap("stiffness", curveArgs);
        float scaleDamping = CurveArgsInMap("damping", curveArgs);
        curve = RSAnimationTimingCurve::CreateInterpolatingSpring(scaleMass, scaleStiffness, scaleDamping,
                                                                  scaleVelocity);
    } else if (type == OHOS::Rosen::Drawing::DrawingCurveType::FRICTION ||
        type == OHOS::Rosen::Drawing::DrawingCurveType::SHARP) {
        float ctrlX1 = CurveArgsInMap("ctrlX1", curveArgs);
        float ctrlY1 = CurveArgsInMap("ctrlY1", curveArgs);
        float ctrlX2 = CurveArgsInMap("ctrlX2", curveArgs);
        float ctrlY2 = CurveArgsInMap("ctrlY2", curveArgs);
        curve = RSAnimationTimingCurve::CreateCubicCurve(ctrlX1, ctrlY1, ctrlX2, ctrlY2);
    } else {
        return;
    }
}

void CalcOneTimePercent(std::vector<float>& timePercents, const uint32_t totalDuration, const uint32_t duration)
{
    if (totalDuration == 0) {
        return;
    }
    float timePercent = static_cast<float>(duration) / static_cast<float>(totalDuration);
    timePercent = timePercent > 1 ? 1.0 : timePercent;
    timePercents.push_back(timePercent);
}
} // namespace SymbolAnimation

RSSymbolAnimation::RSSymbolAnimation() {}

RSSymbolAnimation::~RSSymbolAnimation() {}

bool RSSymbolAnimation::SetSymbolAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (rsNode_ == nullptr || symbolAnimationConfig == nullptr) {
        ROSEN_LOGD("HmSymbol RSSymbolAnimation::getNode or get symbolAnimationConfig:failed");
        return false;
    }

    NodeProcessBeforeAnimation(symbolAnimationConfig);
    if (symbolAnimationConfig->effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_NONE) {
        return true; // pre code already clear nodes.
    }
    InitSupportAnimationTable();

    if (symbolAnimationConfig->effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_REPLACE_APPEAR) {
        return SetReplaceAnimation(symbolAnimationConfig);
    }

    return SetPublicAnimation(symbolAnimationConfig);
}

void RSSymbolAnimation::NodeProcessBeforeAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig->effectStrategy ==
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_REPLACE_APPEAR) {
        PopNodeFromReplaceList(symbolAnimationConfig->symbolSpanId);
        return;
    }

    if (rsNode_->canvasNodesListMap.count(symbolAnimationConfig->symbolSpanId) > 0) {
        std::lock_guard<std::mutex> lock(rsNode_->childrenNodeLock_);
        rsNode_->canvasNodesListMap.erase(symbolAnimationConfig->symbolSpanId);
    }
    return;
}

void RSSymbolAnimation::PopNodeFromReplaceList(uint64_t symbolSpanId)
{
    std::lock_guard<std::mutex> lock(rsNode_->childrenNodeLock_);
    if (rsNode_->canvasNodesListMap.count(symbolSpanId) < 1) {
        rsNode_->canvasNodesListMap[symbolSpanId] = {};
    }

    if (rsNode_->replaceNodesSwapMap.find(INVALID_STATUS) == rsNode_->replaceNodesSwapMap.end()) {
        rsNode_->replaceNodesSwapMap[INVALID_STATUS] = {};
    } else {
        std::unordered_map<NodeId,
            OHOS::Rosen::AnimationNodeConfig> invalidNodes = rsNode_->replaceNodesSwapMap[INVALID_STATUS];
        for (const auto& [id, config] : invalidNodes) {
            if (rsNode_->canvasNodesListMap[symbolSpanId].count(id) > 0) {
                rsNode_->canvasNodesListMap[symbolSpanId].erase(id);
            }
        }
        rsNode_->replaceNodesSwapMap[INVALID_STATUS].clear();
    }

    if (rsNode_->replaceNodesSwapMap.find(APPEAR_STATUS) == rsNode_->replaceNodesSwapMap.end()) {
        rsNode_->replaceNodesSwapMap[APPEAR_STATUS] = {};
    }
}

/**
 * @brief Set Disappear config of replace animation
 * @param symbolAnimationConfig is the symbol animation config of new symbol
 * @param disappearConfig set the symbol animation config of old symbol
 */
bool RSSymbolAnimation::SetDisappearConfig(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    std::shared_ptr<TextEngine::SymbolAnimationConfig>& disappearConfig)
{
    if (symbolAnimationConfig == nullptr || disappearConfig == nullptr) {
        ROSEN_LOGD("[%{public}s]: symbolAnimationConfig or disappearConfig is nullptr \n", __func__);
        return false;
    }

    auto disappearNodes = rsNode_->replaceNodesSwapMap[APPEAR_STATUS];
    disappearConfig->repeatCount = symbolAnimationConfig->repeatCount;
    disappearConfig->animationMode = symbolAnimationConfig->animationMode;
    disappearConfig->animationStart = symbolAnimationConfig->animationStart;
    disappearConfig->symbolSpanId = symbolAnimationConfig->symbolSpanId;
    disappearConfig->commonSubType = symbolAnimationConfig->commonSubType;
    disappearConfig->effectStrategy = symbolAnimationConfig->effectStrategy;

    // count node levels and animation levels
    uint32_t numNodes = 0;
    int animationLevelNum = -1; // -1 is initial value, that is no animation levels
    for (const auto& [id, config] : disappearNodes) {
        TextEngine::SymbolNode symbolNode;
        symbolNode.animationIndex = config.symbolNode.animationIndex;
        disappearConfig->SymbolNodes.push_back(symbolNode);
        animationLevelNum =
            animationLevelNum < symbolNode.animationIndex ? symbolNode.animationIndex : animationLevelNum;
        numNodes++;
    }
    disappearConfig->numNodes = numNodes;
    // 0 is the byLayer effect and 1 is the wholeSymbol effect
    disappearConfig->animationMode = animationLevelNum > 0 ? 0 : 1;
    return true;
}

bool RSSymbolAnimation::SetReplaceAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    auto disappearConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    if (SetDisappearConfig(symbolAnimationConfig, disappearConfig)) {
        SetReplaceDisappear(disappearConfig);
    }
    SetReplaceAppear(symbolAnimationConfig,
        rsNode_->replaceNodesSwapMap[INVALID_STATUS].size() > 0);
    return true;
}

bool RSSymbolAnimation::SetReplaceDisappear(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig->numNodes <= 0) {
        ROSEN_LOGD("[%{public}s]::getNode or get symbolAnimationConfig:failed \n", __func__);
        return false;
    }

    std::unordered_map<NodeId,
        OHOS::Rosen::AnimationNodeConfig> disappearNodes = rsNode_->replaceNodesSwapMap[APPEAR_STATUS];
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    TextEngine::SymbolAnimationEffectStrategy effectStrategy =
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_REPLACE_DISAPPEAR;
    bool res = GetAnimationGroupParameters(symbolAnimationConfig, parameters, effectStrategy);
    for (const auto& [id, config] : disappearNodes) {
        rsNode_->childrenNodeLock_.lock();
        rsNode_->replaceNodesSwapMap[INVALID_STATUS][id] = config;
        rsNode_->replaceNodesSwapMap[APPEAR_STATUS].erase(id);
        rsNode_->childrenNodeLock_.unlock();
        if (!res || (config.symbolNode.animationIndex < 0)) {
            ROSEN_LOGD("[%{public}s] invalid parameter \n", __func__);
            continue;
        }
        auto canvasNode = rsNode_->canvasNodesListMap[symbolAnimationConfig->symbolSpanId][id];

        if (static_cast<int>(parameters.size()) <= config.symbolNode.animationIndex ||
            parameters.at(config.symbolNode.animationIndex).empty()) {
            ROSEN_LOGD("[%{public}s] invalid parameter \n", __func__);
            continue;
        }
        auto oneGroupParas = parameters[config.symbolNode.animationIndex];
        if (oneGroupParas.empty()) {
            ROSEN_LOGD("[%{public}s] invalid parameter \n", __func__);
            continue;
        }
        SpliceAnimation(canvasNode, oneGroupParas, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_DISAPPEAR);
    }
    return true;
}


bool RSSymbolAnimation::SetReplaceAppear(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    bool isStartAnimation)
{
    auto nodeNum = symbolAnimationConfig->numNodes;
    if (nodeNum <= 0) {
        return false;
    }
    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    auto& symbolFirstNode = symbolAnimationConfig->SymbolNodes[0]; // calculate offset by the first node
    Vector4f offsets = CalculateOffset(symbolFirstNode.symbolData.path_,
        symbolFirstNode.nodeBoundary[0], symbolFirstNode.nodeBoundary[1]); // index 0 offsetX and 1 offsetY of layout
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    TextEngine::SymbolAnimationEffectStrategy effectStrategy =
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_REPLACE_APPEAR;
    bool res = GetAnimationGroupParameters(symbolAnimationConfig, parameters,
        effectStrategy);
    for (uint32_t n = 0; n < nodeNum; n++) {
        auto& symbolNode = symbolAnimationConfig->SymbolNodes[n];
        auto canvasNode = RSCanvasNode::Create();
        rsNode_->childrenNodeLock_.lock();
        if (rsNode_->canvasNodesListMap.count(symbolSpanId) < 1) {
            rsNode_->canvasNodesListMap.insert({symbolSpanId, {}}) ;
        }
        rsNode_->canvasNodesListMap[symbolSpanId].insert((std::make_pair(canvasNode->GetId(), canvasNode)));
        AnimationNodeConfig appearNodeConfig = {.symbolNode = symbolNode, .animationIndex = symbolNode.animationIndex};
        rsNode_->replaceNodesSwapMap[APPEAR_STATUS].insert((std::make_pair(canvasNode->GetId(), appearNodeConfig)));
        rsNode_->childrenNodeLock_.unlock();
        if (!SetSymbolGeometry(canvasNode, Vector4f(offsets[0], offsets[1], // 0: offsetX of newNode 1: offsetY
            symbolNode.nodeBoundary[WIDTH], symbolNode.nodeBoundary[HEIGHT]))) {
            continue;
        }
        rsNode_->AddChild(canvasNode, -1);
        GroupDrawing(canvasNode, symbolNode, offsets, nodeNum > 1);
        if (!isStartAnimation || !res || (symbolNode.animationIndex < 0)) {
            continue;
        }
        if (static_cast<int>(parameters.size()) <= symbolNode.animationIndex ||
            parameters[symbolNode.animationIndex].empty()) {
            ROSEN_LOGD("[%{public}s] invalid parameter \n", __func__);
            continue;
        }
        auto oneGroupParas = parameters[symbolNode.animationIndex];
        if (oneGroupParas.empty()) {
            ROSEN_LOGD("[%{public}s] invalid parameter \n", __func__);
            continue;
        }
        SpliceAnimation(canvasNode, oneGroupParas, TextEngine::SymbolAnimationEffectStrategy::SYMBOL_APPEAR);
    }
    return true;
}

void RSSymbolAnimation::InitSupportAnimationTable()
{
    // Init public animation list
    publicSupportAnimations_ = { TextEngine::SymbolAnimationEffectStrategy::SYMBOL_BOUNCE,
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_APPEAR,
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_DISAPPEAR,
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_SCALE};
    upAndDownSupportAnimations_ = {TextEngine::SymbolAnimationEffectStrategy::SYMBOL_BOUNCE,
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_SCALE};
}

bool RSSymbolAnimation::GetAnimationGroupParameters(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>>& parameters,
    TextEngine::SymbolAnimationEffectStrategy& effectStrategy)
{
    // count animation levels
    int animationLevelNum = -1;
    auto nodeNum = symbolAnimationConfig->numNodes;
    for (uint32_t n = 0; n < nodeNum; n++) {
        const auto& symbolNode = symbolAnimationConfig->SymbolNodes[n];
        animationLevelNum =
            animationLevelNum < symbolNode.animationIndex ? symbolNode.animationIndex : animationLevelNum;
    }

    if (animationLevelNum < 0) {
        return false;
    }
    animationLevelNum = animationLevelNum + 1;

    // get animation group paramaters
    if (std::count(upAndDownSupportAnimations_.begin(), upAndDownSupportAnimations_.end(),
        effectStrategy) != 0) {
        parameters = Drawing::HmSymbolConfigOhos::GetGroupParameters(
            Drawing::DrawingAnimationType(effectStrategy),
            static_cast<uint16_t>(animationLevelNum),
            symbolAnimationConfig->animationMode, symbolAnimationConfig->commonSubType);
    } else {
        parameters = Drawing::HmSymbolConfigOhos::GetGroupParameters(
            Drawing::DrawingAnimationType(effectStrategy),
            static_cast<uint16_t>(animationLevelNum),
            symbolAnimationConfig->animationMode);
    }
    if (parameters.empty()) {
        return false;
    }
    return true;
}

bool RSSymbolAnimation::ChooseAnimation(const std::shared_ptr<RSNode>& rsNode,
    std::vector<Drawing::DrawingPiecewiseParameter>& parameters,
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (std::count(publicSupportAnimations_.begin(),
        publicSupportAnimations_.end(), symbolAnimationConfig->effectStrategy) != 0) {
        SpliceAnimation(rsNode, parameters, symbolAnimationConfig->effectStrategy);
        return true;
    }

    switch (symbolAnimationConfig->effectStrategy) {
        case TextEngine::SymbolAnimationEffectStrategy::SYMBOL_VARIABLE_COLOR:
            return SetKeyframeAlphaAnimation(rsNode, parameters, symbolAnimationConfig);
        case TextEngine::SymbolAnimationEffectStrategy::SYMBOL_PULSE:
            return SetKeyframeAlphaAnimation(rsNode, parameters, symbolAnimationConfig);
        default:
            ROSEN_LOGD("[%{public}s] not support input animation type \n", __func__);
            return false;
    }
}

bool RSSymbolAnimation::SetPublicAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    auto nodeNum = symbolAnimationConfig->numNodes;
    if (nodeNum <= 0) {
        ROSEN_LOGD("HmSymbol SetDisappearAnimation::getNode or get symbolAnimationConfig:failed");
        return false;
    }

    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    auto& symbolFirstNode = symbolAnimationConfig->SymbolNodes[0]; // calculate offset by the first node

    Vector4f offsets = CalculateOffset(symbolFirstNode.symbolData.path_, symbolFirstNode.nodeBoundary[0],
        symbolFirstNode.nodeBoundary[1]); // index 0 offsetX and 1 offsetY of layout

    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    bool res = GetAnimationGroupParameters(symbolAnimationConfig, parameters, symbolAnimationConfig->effectStrategy);

    for (uint32_t n = 0; n < nodeNum; n++) {
        auto& symbolNode = symbolAnimationConfig->SymbolNodes[n];
        auto canvasNode = RSCanvasNode::Create();
        rsNode_->childrenNodeLock_.lock();
        if (!rsNode_->canvasNodesListMap.count(symbolSpanId)) {
            rsNode_->canvasNodesListMap[symbolSpanId] = {};
        }
        rsNode_->canvasNodesListMap[symbolSpanId][canvasNode->GetId()] = canvasNode;
        rsNode_->childrenNodeLock_.unlock();
        if (!SetSymbolGeometry(canvasNode, Vector4f(offsets[0], offsets[1], // 0: offsetX of newNode 1: offsetY
            symbolNode.nodeBoundary[WIDTH], symbolNode.nodeBoundary[HEIGHT]))) {
            return false;
        }
        rsNode_->AddChild(canvasNode, -1);
    
        // if there is mask layer, set the blendmode on the original node rsNode_
        if (symbolNode.isMask) {
            rsNode_->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
            rsNode_->SetColorBlendApplyType(RSColorBlendApplyType::SAVE_LAYER);
        }
        GroupDrawing(canvasNode, symbolNode, offsets, nodeNum > 1);

        if (!res || (symbolNode.animationIndex < 0)) {
            continue;
        }

        if (static_cast<int>(parameters.size()) <= symbolNode.animationIndex ||
            parameters[symbolNode.animationIndex].empty()) {
            ROSEN_LOGD("[%{public}s] invalid parameter \n", __func__);
            continue;
        }
        auto oneGroupParas = parameters[symbolNode.animationIndex];
        if (oneGroupParas.empty()) {
            ROSEN_LOGD("[%{public}s] invalid parameter \n", __func__);
            continue;
        }
        ChooseAnimation(canvasNode, oneGroupParas, symbolAnimationConfig);
    }
    return true;
}

void RSSymbolAnimation::GroupAnimationStart(
    const std::shared_ptr<RSNode>& rsNode, std::vector<std::shared_ptr<RSAnimation>>& animations)
{
    if (rsNode == nullptr || animations.empty()) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return;
    }

    for (int i = 0; i < static_cast<int>(animations.size()); i++) {
        if (animations[i]) {
            animations[i]->Start(rsNode);
        }
    }
}

void RSSymbolAnimation::SetNodePivot(const std::shared_ptr<RSNode>& rsNode)
{
    // Set Node Center Offset
    Vector2f curNodePivot = rsNode->GetStagingProperties().GetPivot();
    pivotProperty_ = nullptr; // reset
    if (!(curNodePivot.x_ == CENTER_NODE_COORDINATE.x_ && curNodePivot.y_ == CENTER_NODE_COORDINATE.y_)) {
        bool isCreate = SymbolAnimation::CreateOrSetModifierValue(pivotProperty_, CENTER_NODE_COORDINATE);
        if (isCreate) {
            auto pivotModifier = std::make_shared<RSPivotModifier>(pivotProperty_);
            rsNode->AddModifier(pivotModifier);
        }
    }
}

void RSSymbolAnimation::SpliceAnimation(const std::shared_ptr<RSNode>& rsNode,
    std::vector<Drawing::DrawingPiecewiseParameter>& parameters,
    const TextEngine::SymbolAnimationEffectStrategy& effectStrategy)
{
    if (effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_DISAPPEAR ||
        effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_APPEAR) {
        AppearAnimation(rsNode, parameters);
    } else if (effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_BOUNCE ||
        effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_SCALE) {
        BounceAnimation(rsNode, parameters);
    } else {
        return;
    }
}

void RSSymbolAnimation::BounceAnimation(
    const std::shared_ptr<RSNode>& rsNode, std::vector<Drawing::DrawingPiecewiseParameter>& parameters)
{
    unsigned int animationStageNum = 2; // the count of atomizated animations
    if (rsNode == nullptr || parameters.empty() || parameters.size() < animationStageNum) {
        ROSEN_LOGD("[%{public}s] : invalid input\n", __func__);
        return;
    }

    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    bool isAdd = AddScaleBaseModifier(rsNode, parameters[0], scaleProperty);
    if (!isAdd) {
        ROSEN_LOGD("[%{public}s] : add scale modifier failed\n", __func__);
        return;
    }

    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    ScaleAnimationBase(scaleProperty, parameters[0], groupAnimation);
    ScaleAnimationBase(scaleProperty, parameters[1], groupAnimation);
    GroupAnimationStart(rsNode, groupAnimation);
}

void RSSymbolAnimation::AppearAnimation(
    const std::shared_ptr<RSNode>& rsNode, std::vector<Drawing::DrawingPiecewiseParameter>& parameters)
{
    unsigned int animationStageNum = 2; // the count of atomizated animations
    if (rsNode == nullptr || parameters.empty() || parameters.size() < animationStageNum) {
        ROSEN_LOGD("[%{public}s] : invalid input\n", __func__);
        return;
    }

    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    bool isAdd = AddScaleBaseModifier(rsNode, parameters[0], scaleProperty);
    if (!isAdd) {
        ROSEN_LOGD("[%{public}s] : add scale modifier failed\n", __func__);
        return;
    }

    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    ScaleAnimationBase(scaleProperty, parameters[0], groupAnimation);
    AlphaAnimationBase(rsNode, parameters[1], groupAnimation);
    GroupAnimationStart(rsNode, groupAnimation);
}

Vector4f RSSymbolAnimation::CalculateOffset(const Drawing::Path& path, const float offsetX, const float offsetY)
{
    auto rect = path.GetBounds();
    float left = rect.GetLeft();
    float top = rect.GetTop();
    // the nodeTranslation is offset of new node to the father node;
    // the newOffset is offset of path on new node;
    Vector2f nodeTranslation = { offsetX + left, offsetY + top };
    Vector2f newOffset = { -left, -top };
    return Vector4f(nodeTranslation[0], nodeTranslation[1], newOffset[0], newOffset[1]);
}

void RSSymbolAnimation::GroupDrawing(const std::shared_ptr<RSCanvasNode>& canvasNode,
    TextEngine::SymbolNode& symbolNode, const Vector4f& offsets, bool isMultiLayer)
{
    // drawing a symbol or a path group
    auto recordingCanvas = canvasNode->BeginRecording(symbolNode.nodeBoundary[WIDTH], symbolNode.nodeBoundary[HEIGHT]);
    if (isMultiLayer) {
        DrawPathOnCanvas(recordingCanvas, symbolNode, offsets);
    } else {
        DrawSymbolOnCanvas(recordingCanvas, symbolNode, offsets);
    }
    canvasNode->FinishRecording();
}

void RSSymbolAnimation::DrawSymbolOnCanvas(
    ExtendRecordingCanvas* recordingCanvas, TextEngine::SymbolNode& symbolNode, const Vector4f& offsets)
{
    if (recordingCanvas == nullptr) {
        return;
    }
    Drawing::Brush brush;
    Drawing::Pen pen;
    SetIconProperty(brush, pen, symbolNode);
    Drawing::Point offsetLocal = Drawing::Point { offsets[2], offsets[3] }; // index 2 offsetX 3 offsetY
    recordingCanvas->AttachBrush(brush);
    recordingCanvas->AttachPen(pen);
    recordingCanvas->DrawSymbol(symbolNode.symbolData, offsetLocal);
    recordingCanvas->DetachBrush();
    recordingCanvas->DetachPen();
}

void RSSymbolAnimation::DrawPathOnCanvas(
    ExtendRecordingCanvas* recordingCanvas, TextEngine::SymbolNode& symbolNode, const Vector4f& offsets)
{
    if (recordingCanvas == nullptr) {
        return;
    }
    Drawing::Brush brush;
    Drawing::Pen pen;
    if (symbolNode.isMask) {
        brush.SetBlendMode(Drawing::BlendMode::CLEAR);
        pen.SetBlendMode(Drawing::BlendMode::CLEAR);
    }
    SetIconProperty(brush, pen, symbolNode);
    symbolNode.path.Offset(offsets[2], offsets[3]); // index 2 offsetX 3 offsetY
    recordingCanvas->AttachBrush(brush);
    recordingCanvas->AttachPen(pen);
    recordingCanvas->DrawPath(symbolNode.path);
    recordingCanvas->DetachBrush();
    recordingCanvas->DetachPen();
}

bool RSSymbolAnimation::GetScaleUnitAnimationParas(
    Drawing::DrawingPiecewiseParameter& scaleUnitParas, Vector2f& scaleValueBegin, Vector2f& scaleValueEnd)
{
    auto& scaleProperties = scaleUnitParas.properties;
    if (!SymbolAnimation::ElementInMap(SCALE_PROP_X, scaleProperties) ||
        !SymbolAnimation::ElementInMap(SCALE_PROP_Y, scaleProperties)) {
        ROSEN_LOGD("[%{public}s] scaleProperties is null \n", __func__);
        return false;
    }

    if (scaleProperties[SCALE_PROP_X].size() != PROPERTIES || scaleProperties[SCALE_PROP_Y].size() != PROPERTIES) {
        ROSEN_LOGD("[%{public}s] scaleProperties have wrong number \n", __func__);
        return false;
    }

    scaleValueBegin = { scaleProperties[SCALE_PROP_X][PROP_START], scaleProperties[SCALE_PROP_Y][PROP_START] };
    scaleValueEnd = { scaleProperties[SCALE_PROP_X][PROP_END], scaleProperties[SCALE_PROP_Y][PROP_END] };
    return true;
}

bool RSSymbolAnimation::SetScaleUnitAnimation(const std::shared_ptr<RSNode>& rsNode,
    std::vector<Drawing::DrawingPiecewiseParameter>& parameters)
{
    unsigned int validSize = 2; // 2 means this animation type has two animation stages
    if (rsNode_ == nullptr || parameters.size() != validSize) {
        ROSEN_LOGD("HmSymbol SetScaleUnitAnimation::getNode or get parameters:failed");
        return false;
    }
    Vector2f scaleValueBegin;
    Vector2f scaleValueEnd;
    // 0 means first stage of this animation
    if (!GetScaleUnitAnimationParas(parameters[0], scaleValueBegin, scaleValueEnd)) {
        return false;
    }
    scaleProperty_ = nullptr; // reset
    bool isCreate = SymbolAnimation::CreateOrSetModifierValue(scaleProperty_, scaleValueBegin);
    if (isCreate) {
        auto scaleModifier = std::make_shared<RSScaleModifier>(scaleProperty_);
        rsNode->AddModifier(scaleModifier);
    }
    auto animation0 = ScaleSymbolAnimation(rsNode, parameters[0], scaleValueEnd);
    if (animation0 == nullptr) {
        return false;
    }
    // 1 means second stage of this animation
    if (!GetScaleUnitAnimationParas(parameters[1], scaleValueBegin, scaleValueEnd)) {
        return false;
    }
    auto animation1 = ScaleSymbolAnimation(rsNode, parameters[1], scaleValueEnd);
    if (animation1 == nullptr) {
        return false;
    }
    animation0->Start(rsNode);
    animation1->Start(rsNode);
    return true;
}

bool RSSymbolAnimation::SetSymbolGeometry(const std::shared_ptr<RSNode>& rsNode, const Vector4f& bounds)
{
    if (rsNode == nullptr) {
        return false;
    }
    std::shared_ptr<RSAnimatableProperty<Vector4f>> frameProperty = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector4f>> boundsProperty = nullptr;

    bool isFrameCreate = SymbolAnimation::CreateOrSetModifierValue(frameProperty, bounds);
    if (isFrameCreate) {
        auto frameModifier = std::make_shared<RSFrameModifier>(frameProperty);
        rsNode->AddModifier(frameModifier);
    }
    bool isBoundsCreate = SymbolAnimation::CreateOrSetModifierValue(boundsProperty, bounds);
    if (isBoundsCreate) {
        auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
        rsNode->AddModifier(boundsModifier);
    }
    rsNode_->SetClipToBounds(false);
    rsNode_->SetClipToFrame(false);
    return true;
}

std::shared_ptr<RSAnimation> RSSymbolAnimation::ScaleSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
    const Drawing::DrawingPiecewiseParameter& scaleUnitParas, const Vector2f& scaleValueEnd)
{
    if (rsNode == nullptr) {
        return nullptr;
    }
    SetNodePivot(rsNode);
    RSAnimationTimingCurve scaleCurve;
    SymbolAnimation::CreateAnimationTimingCurve(scaleUnitParas.curveType, scaleUnitParas.curveArgs, scaleCurve);

    RSAnimationTimingProtocol protocol;
    protocol.SetStartDelay(scaleUnitParas.delay);

    auto animations = RSNode::Animate(protocol, scaleCurve, [this, scaleValueEnd]() {
        scaleProperty_->Set(scaleValueEnd);
    });
    if (animations.size() <= 0) {
        return nullptr;
    }
    return animations[0];
}

bool RSSymbolAnimation::SetKeyframeAlphaAnimation(const std::shared_ptr<RSNode>& rsNode,
    std::vector<Drawing::DrawingPiecewiseParameter>& parameters,
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (rsNode_ == nullptr || symbolAnimationConfig == nullptr) {
        ROSEN_LOGD("HmSymbol SetVariableColorAnimation::getNode or get symbolAnimationConfig:failed");
    }
    alphaPropertyStages_.clear();
    uint32_t duration = 0;
    std::vector<float> timePercents;
    if (!GetKeyframeAlphaAnimationParas(parameters, duration, timePercents)) {
        return false;
    }

    // 0 means the first stage of a node
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaPropertyStages_[0]);
    rsNode->AddModifier(alphaModifier);
    std::shared_ptr<RSAnimation> animation = nullptr;
    animation = KeyframeAlphaSymbolAnimation(rsNode, parameters[0], duration, timePercents);
    if (animation == nullptr) {
        return false;
    }

    if (symbolAnimationConfig->effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_VARIABLE_COLOR &&
        symbolAnimationConfig->animationMode == 0) {
        animation->SetRepeatCount(1);
    } else {
        animation->SetRepeatCount(-1); // -1 means loop playback
    }
    animation->Start(rsNode);
    return true;
}

bool RSSymbolAnimation::GetKeyframeAlphaAnimationParas(
    std::vector<Drawing::DrawingPiecewiseParameter>& oneGroupParas,
    uint32_t& totalDuration, std::vector<float>& timePercents)
{
    if (oneGroupParas.empty()) {
        return false;
    }
    totalDuration = 0;
    int interval = 0;
    // traverse all time stages
    for (unsigned int i = 0; i < oneGroupParas.size(); i++) {
        if (i + 1 < oneGroupParas.size()) {
            interval = oneGroupParas[i + 1].delay -
                       (static_cast<int>(oneGroupParas[i].duration) + oneGroupParas[i].delay);
        } else {
            interval = 0;
        }
        if (interval < 0) {
            return false;
        }
        totalDuration = oneGroupParas[i].duration + totalDuration + static_cast<uint32_t>(interval);
        if (!SymbolAnimation::ElementInMap(ALPHA_PROP, oneGroupParas[i].properties) ||
            oneGroupParas[i].properties[ALPHA_PROP].size() != PROPERTIES) {
            return false;
        }
        // the value of the key frame needs
        float alphaValueStart = oneGroupParas[i].properties[ALPHA_PROP][PROP_START];
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyStart = nullptr;
        SymbolAnimation::CreateOrSetModifierValue(alphaPropertyStart, alphaValueStart);
        alphaPropertyStages_.push_back(alphaPropertyStart);

        float alphaValueEnd = oneGroupParas[i].properties[ALPHA_PROP][PROP_END];
        std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyEnd = nullptr;
        SymbolAnimation::CreateOrSetModifierValue(alphaPropertyEnd, alphaValueEnd);
        alphaPropertyStages_.push_back(alphaPropertyEnd);
    }
    return CalcTimePercents(timePercents, totalDuration, oneGroupParas);
}

bool RSSymbolAnimation::CalcTimePercents(std::vector<float>& timePercents, const uint32_t totalDuration,
    const std::vector<Drawing::DrawingPiecewiseParameter>& oneGroupParas)
{
    if (totalDuration == 0) {
        return false;
    }
    uint32_t duration = 0;
    int interval = 0;
    timePercents.push_back(0); // the first property of timePercent
    for (int i = 0; i < static_cast<int>(oneGroupParas.size()) - 1; i++) {
        duration = duration + oneGroupParas[i].duration;
        SymbolAnimation::CalcOneTimePercent(timePercents, totalDuration, duration);
        interval = oneGroupParas[i + 1].delay -
                   (static_cast<int>(oneGroupParas[i].duration) + oneGroupParas[i].delay);
        if (interval < 0) {
            return false;
        }
        duration = duration + static_cast<uint32_t>(interval);
        SymbolAnimation::CalcOneTimePercent(timePercents, totalDuration, duration);
    }
    duration = duration + oneGroupParas.back().duration;
    SymbolAnimation::CalcOneTimePercent(timePercents, totalDuration, duration);
    return true;
}

void RSSymbolAnimation::SetIconProperty(Drawing::Brush& brush, Drawing::Pen& pen, TextEngine::SymbolNode& symbolNode)
{
    brush.SetColor(Drawing::Color::ColorQuadSetARGB(0xFF, symbolNode.color.r, symbolNode.color.g, symbolNode.color.b));
    brush.SetAlphaF(symbolNode.color.a);
    brush.SetAntiAlias(true);

    pen.SetColor(Drawing::Color::ColorQuadSetARGB(0xFF, symbolNode.color.r, symbolNode.color.g, symbolNode.color.b));
    pen.SetAlphaF(symbolNode.color.a);
    pen.SetAntiAlias(true);
    return;
}

std::shared_ptr<RSAnimation> RSSymbolAnimation::KeyframeAlphaSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
    const Drawing::DrawingPiecewiseParameter& oneStageParas, const uint32_t duration,
    const std::vector<float>& timePercents)
{
    if (alphaPropertyStages_.size() == 0 || timePercents.size() != alphaPropertyStages_.size()) {
        return nullptr;
    }
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(alphaPropertyStages_[0]); // initial the alpha status
    if (keyframeAnimation == nullptr || rsNode == nullptr) {
        return nullptr;
    }
    keyframeAnimation->SetStartDelay(oneStageParas.delay);
    keyframeAnimation->SetDuration(duration);
    RSAnimationTimingCurve timingCurve;
    SymbolAnimation::CreateAnimationTimingCurve(oneStageParas.curveType, oneStageParas.curveArgs, timingCurve);
    std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> keyframes;
    for (unsigned int i = 1; i < alphaPropertyStages_.size(); i++) {
        keyframes.push_back(std::make_tuple(timePercents[i], alphaPropertyStages_[i], timingCurve));
    }
    keyframeAnimation->AddKeyFrames(keyframes);
    return keyframeAnimation;
}

/**
 * @brief creates a scaleModifier by scaleParamter and adds it to rsNode
 * @param rsNode is the node of symbol animation
 * @param scaleParamter is the parameter of the scale effect
 * @param scaleProperty property of the scale effect
 * @return true if add scale modifer success
 */
bool RSSymbolAnimation::AddScaleBaseModifier(const std::shared_ptr<RSNode>& rsNode,
    Drawing::DrawingPiecewiseParameter& scaleParameter,
    std::shared_ptr<RSAnimatableProperty<Vector2f>>& scaleProperty)
{
    // validation input
    if (rsNode == nullptr) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return false;
    }
    unsigned int validSize = 2;
    if (scaleParameter.properties.count(SCALE_PROP_X) <= 0 || scaleParameter.properties.count(SCALE_PROP_Y) <= 0 ||
        scaleParameter.properties[SCALE_PROP_X].size() < validSize ||
        scaleParameter.properties[SCALE_PROP_Y].size() < validSize) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return false;
    }

    SetNodePivot(rsNode);

    const Vector2f scaleValueBegin = {scaleParameter.properties[SCALE_PROP_X][0],
        scaleParameter.properties[SCALE_PROP_Y][0]};

    bool isCreate = SymbolAnimation::CreateOrSetModifierValue(scaleProperty, scaleValueBegin);
    if (!isCreate) {
        ROSEN_LOGD("[%{public}s] : invalid parameter \n", __func__);
        return false;
    }

    auto scaleModifier = std::make_shared<Rosen::RSScaleModifier>(scaleProperty);
    rsNode->AddModifier(scaleModifier);
    return true;
}

// base atomizated animation
void RSSymbolAnimation::ScaleAnimationBase(std::shared_ptr<RSAnimatableProperty<Vector2f>>& scaleProperty,
    Drawing::DrawingPiecewiseParameter& scaleParameter, std::vector<std::shared_ptr<RSAnimation>>& animations)
{
    if (scaleProperty == nullptr) {
        ROSEN_LOGD("[%{public}s] : scaleProperty is nullptr \n", __func__);
        return;
    }

    unsigned int validSize = 2;
    if (scaleParameter.properties.count(SCALE_PROP_X) <= 0 || scaleParameter.properties.count(SCALE_PROP_Y) <= 0 ||
        scaleParameter.properties[SCALE_PROP_X].size() < validSize ||
        scaleParameter.properties[SCALE_PROP_Y].size() < validSize) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return;
    }

    const Vector2f scaleValueEnd = {scaleParameter.properties[SCALE_PROP_X][1],
        scaleParameter.properties[SCALE_PROP_Y][1]};

    // set animation curve and protocol
    RSAnimationTimingCurve scaleCurve;
    SymbolAnimation::CreateAnimationTimingCurve(scaleParameter.curveType, scaleParameter.curveArgs, scaleCurve);

    RSAnimationTimingProtocol scaleprotocol;
    scaleprotocol.SetStartDelay(scaleParameter.delay);
    if (scaleParameter.duration > 0) {
        scaleprotocol.SetDuration(scaleParameter.duration);
    }

    // set animation
    std::vector<std::shared_ptr<RSAnimation>> animations1 = RSNode::Animate(
        scaleprotocol, scaleCurve, [&scaleProperty, &scaleValueEnd]() { scaleProperty->Set(scaleValueEnd); });

    if (animations1.size() > 0 && animations1[0] != nullptr) {
        animations.emplace_back(animations1[0]);
    }
}

void RSSymbolAnimation::AlphaAnimationBase(const std::shared_ptr<RSNode>& rsNode,
    Drawing::DrawingPiecewiseParameter& alphaParameter, std::vector<std::shared_ptr<RSAnimation>>& animations)
{
    // validation input
    if (rsNode == nullptr) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return;
    }
    unsigned int validSize = 2;
    if (alphaParameter.properties.count("alpha") <= 0 || alphaParameter.properties["alpha"].size() < validSize) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return;
    }

    float alphaBegin = static_cast<float>(alphaParameter.properties["alpha"][0]);
    float alphaValueEnd = static_cast<float>(alphaParameter.properties["alpha"][1]);

    std::shared_ptr<RSAnimatableProperty<float>> alphaProperty;

    if (!SymbolAnimation::CreateOrSetModifierValue(alphaProperty, alphaBegin)) {
        return;
    }
    auto alphaModifier = std::make_shared<Rosen::RSAlphaModifier>(alphaProperty);

    rsNode->AddModifier(alphaModifier);

    RSAnimationTimingProtocol alphaProtocol;
    alphaProtocol.SetStartDelay(alphaParameter.delay);
    alphaProtocol.SetDuration(alphaParameter.duration);
    RSAnimationTimingCurve alphaCurve;
    SymbolAnimation::CreateAnimationTimingCurve(alphaParameter.curveType, alphaParameter.curveArgs, alphaCurve);

    std::vector<std::shared_ptr<RSAnimation>> animations1 = RSNode::Animate(
        alphaProtocol, alphaCurve, [&alphaProperty, &alphaValueEnd]() { alphaProperty->Set(alphaValueEnd); });

    if (animations1.size() > 0 && animations1[0] != nullptr) {
        animations.emplace_back(animations1[0]);
    }
}
} // namespace Rosen
} // namespace OHOS