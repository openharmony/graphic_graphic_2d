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
static const unsigned int UNIT_GROUP = 0;                      // AnimationSubType is UNIT
static const unsigned int UNIT_PERIOD = 0;                     // AnimationSubType is UNIT
static const unsigned int UNIT_NODE = 0;                       // AnimationSubType is UNIT
static const std::string SCALE_PROP_X = "sx";
static const std::string SCALE_PROP_Y = "sy";
static const std::string ALPHA_PROP = "alpha";
static const unsigned int PROPERTIES = 2; // symbol animation property contains two values, change from one to the other
static const unsigned int PROP_START = 0; // symbol animation property contains two values, change from START to the END
static const unsigned int PROP_END = 1;   // symbol animation property contains two values, change from START to the END
static const unsigned int WIDTH = 2;
static const unsigned int HEIGHT = 3;

bool IsEqual(const Vector2f& val1, const Vector2f& val2)
{
    return (val1.x_ == val2.x_ && val1.y_ == val2.y_);
}

void CreateAnimationTimingCurve(OHOS::Rosen::Drawing::DrawingCurveType type, std::map<std::string, double>& curveArgs,
    RSAnimationTimingCurve& curve)
{
    curve = RSAnimationTimingCurve();
    if (type == OHOS::Rosen::Drawing::DrawingCurveType::LINEAR) {
        curve = RSAnimationTimingCurve::LINEAR;
    } else if (type == OHOS::Rosen::Drawing::DrawingCurveType::SPRING) {
        double scaleVelocity = curveArgs.count("velocity") > 0 ? curveArgs["velocity"] : 0;
        double scaleMass = curveArgs.count("mass") > 0 ? curveArgs["mass"] : 0;
        double scaleStiffness = curveArgs.count("stiffness") > 0 ? curveArgs["stiffness"] : 0;
        double scaleDamping = curveArgs.count("damping") > 0 ? curveArgs["damping"] : 0;
        curve = RSAnimationTimingCurve::CreateInterpolatingSpring(static_cast<float>(scaleMass),
            static_cast<float>(scaleStiffness), static_cast<float>(scaleDamping), static_cast<float>(scaleVelocity));
    } else {
        return;
    }
}

bool GetAnimationGroupParameters(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>>& parameters)
{
    // count animation levels
    int animationLevelNum = -1;
    auto nodeNum = symbolAnimationConfig->numNodes;
    for (uint32_t n = 0; n < nodeNum; n++) {
        auto& symbolNode = symbolAnimationConfig->SymbolNodes[n];
        animationLevelNum =
            animationLevelNum < symbolNode.animationIndex ? symbolNode.animationIndex : animationLevelNum;
    }

    if (animationLevelNum < 0) {
        return false;
    }
    animationLevelNum = animationLevelNum + 1;
    // unit animation : 1, multiple animation : 0
    int animationMode = animationLevelNum > 1 ? 0 : 1;
    // get animation group paramaters
    parameters = Drawing::HmSymbolConfigOhos::GetGroupParameters(
        Drawing::DrawingAnimationType(symbolAnimationConfig->effectStrategy), uint16_t(animationLevelNum),
        uint16_t(animationMode));
    if (parameters.empty()) {
        return false;
    }
    return true;
}

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
bool ElementInMap(const std::string curElement, const std::map<std::string, T>& curMap)
{
    if (curMap.empty()) {
        return false;
    }
    auto element = curMap.find(curElement);
    return (element != curMap.end());
}

template<typename T>
bool ElementInMap(const std::string curElement, const std::map<std::string, T>& curMap, T& value)
{
    if (curMap.empty()) {
        return false;
    }
    auto element = curMap.find(curElement);
    if (element == curMap.end()) {
        return false;
    }
    value = curMap.at(curElement);
    return true;
}

RSSymbolAnimation::RSSymbolAnimation() {}

RSSymbolAnimation::~RSSymbolAnimation() {}

bool RSSymbolAnimation::SetSymbolAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (rsNode_ == nullptr || symbolAnimationConfig == nullptr) {
        ROSEN_LOGD("HmSymbol RSSymbolAnimation::getNode or get symbolAnimationConfig:failed");
        return false;
    }

    if (rsNode_->canvasNodesListMap.count(symbolAnimationConfig->symbolSpanId) > 0) {
        rsNode_->canvasNodesListMap.erase(symbolAnimationConfig->symbolSpanId);
    }

    if (symbolAnimationConfig->effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_NONE) {
        return true; // pre code already clear nodes.
    }
    InitSupportAnimationTable();
    return ChooseAnimation(symbolAnimationConfig);
}

void RSSymbolAnimation::InitSupportAnimationTable()
{
    // Init public animation list
    publicSupportAnimations_ = { TextEngine::SymbolAnimationEffectStrategy::SYMBOL_BOUNCE,
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_APPEAR,
        TextEngine::SymbolAnimationEffectStrategy::SYMBOL_DISAPPEAR };
}

bool RSSymbolAnimation::ChooseAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (std::count(publicSupportAnimations_.begin(),
        publicSupportAnimations_.end(), symbolAnimationConfig->effectStrategy)) {
        return SetPublicAnimation(symbolAnimationConfig);
    }

    switch (symbolAnimationConfig->effectStrategy) {
        case TextEngine::SymbolAnimationEffectStrategy::SYMBOL_SCALE:
            return SetScaleUnitAnimation(symbolAnimationConfig);
        case TextEngine::SymbolAnimationEffectStrategy::SYMBOL_VARIABLE_COLOR:
            return SetVariableColorAnimation(symbolAnimationConfig);
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
    bool res = GetAnimationGroupParameters(symbolAnimationConfig, parameters);

    for (uint32_t n = 0; n < nodeNum; n++) {
        auto& symbolNode = symbolAnimationConfig->SymbolNodes[n];
        auto canvasNode = RSCanvasNode::Create();

        if (!rsNode_->canvasNodesListMap.count(symbolSpanId)) {
            rsNode_->canvasNodesListMap[symbolSpanId] = {};
        }
        rsNode_->canvasNodesListMap[symbolSpanId].emplace_back(canvasNode);

        Vector4f offsets =
            CalculateOffset(symbolNode.symbolData.path_, symbolNode.nodeBoundary[0], symbolNode.nodeBoundary[1]);
        if (!SetSymbolGeometry(canvasNode, Vector4f(offsets[0], offsets[1], // 0: offsetX of newNode 1: offsetY
            symbolNode.nodeBoundary[WIDTH], symbolNode.nodeBoundary[HEIGHT]))) {
            return false;
        }
        rsNode_->AddChild(canvasNode, -1);
        GroupDrawing(canvasNode, symbolNode, offsets, nodeNum > 1);

        if (!res || (symbolNode.animationIndex < 0)) {
            continue;
        }

        if (parameters.size() <= symbolNode.animationIndex || parameters[symbolNode.animationIndex].empty()) {
            ROSEN_LOGD("[%{public}s] invalid parameter \n", __func__);
            continue;
        }
        auto oneGroupParas = parameters[int(symbolNode.animationIndex)];
        if (oneGroupParas.empty()) {
            ROSEN_LOGD("[%{public}s] invalid parameter \n", __func__);
            continue;
        }
        SpliceAnimation(canvasNode, oneGroupParas, symbolAnimationConfig->effectStrategy);
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
    if (!IsEqual(curNodePivot, CENTER_NODE_COORDINATE)) {
        bool isCreate = CreateOrSetModifierValue(pivotProperty_, CENTER_NODE_COORDINATE);
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
    } else if (effectStrategy == TextEngine::SymbolAnimationEffectStrategy::SYMBOL_BOUNCE) {
        BounceAnimation(rsNode, parameters);
    } else {
        return;
    }
}

void RSSymbolAnimation::BounceAnimation(
    const std::shared_ptr<RSNode>& rsNode, std::vector<Drawing::DrawingPiecewiseParameter>& parameters)
{
    int animationStageNum = 2; // the count of atomizated animations
    if (rsNode == nullptr && parameters.empty() && parameters.size() < animationStageNum) {
        ROSEN_LOGD("[%{public}s] : invalid input\n", __func__);
        return;
    }

    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    ScaleAnimationBase(rsNode, parameters[0], groupAnimation);
    ScaleAnimationBase(rsNode, parameters[1], groupAnimation);
    GroupAnimationStart(rsNode, groupAnimation);
}

void RSSymbolAnimation::AppearAnimation(
    const std::shared_ptr<RSNode>& rsNode, std::vector<Drawing::DrawingPiecewiseParameter>& parameters)
{
    int animationStageNum = 2; // the count of atomizated animations
    if (rsNode == nullptr && parameters.empty() && parameters.size() < animationStageNum) {
        ROSEN_LOGD("[%{public}s] : invalid input\n", __func__);
        return;
    }

    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    ScaleAnimationBase(rsNode, parameters[0], groupAnimation);
    AlphaAnimationBase(rsNode, parameters[1], groupAnimation);
    GroupAnimationStart(rsNode, groupAnimation);
}

Vector4f RSSymbolAnimation::CalculateOffset(const Drawing::Path& path, const float& offsetX, const float& offsetY)
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
    recordingCanvas->DrawSymbol(symbolNode.symbolData, offsetLocal);
    recordingCanvas->DetachBrush();
    recordingCanvas->AttachPen(pen);
    recordingCanvas->DrawSymbol(symbolNode.symbolData, offsetLocal);
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
    SetIconProperty(brush, pen, symbolNode);
    symbolNode.path.Offset(offsets[2], offsets[3]); // index 2 offsetX 3 offsetY
    recordingCanvas->AttachBrush(brush);
    recordingCanvas->AttachPen(pen);
    recordingCanvas->DrawPath(symbolNode.path);
    recordingCanvas->DetachBrush();
    recordingCanvas->DetachPen();
}

bool RSSymbolAnimation::GetScaleUnitAnimationParas(
    Drawing::DrawingPiecewiseParameter& scaleUnitParas, Vector2f& scaleValueBegin, Vector2f& scaleValue)
{
    // AnimationType, Animation groups, animation_mode; animation_mode is 1 when Animation groups is 1
    auto scaleParas = Drawing::HmSymbolConfigOhos::GetGroupParameters(Drawing::SCALE_TYPE, 1, 1);
    if (scaleParas.empty() || scaleParas[UNIT_GROUP].empty()) {
        ROSEN_LOGD("[%{public}s] can not get scaleParas \n", __func__);
        return false;
    }
    scaleUnitParas = scaleParas[UNIT_GROUP][UNIT_PERIOD];

    auto scaleProperties = scaleUnitParas.properties;
    if (!ElementInMap(SCALE_PROP_X, scaleProperties) || !ElementInMap(SCALE_PROP_Y, scaleProperties)) {
        ROSEN_LOGD("[%{public}s] scaleProperties is null \n", __func__);
        return false;
    }

    if (scaleProperties[SCALE_PROP_X].size() != PROPERTIES || scaleProperties[SCALE_PROP_Y].size() != PROPERTIES) {
        ROSEN_LOGD("[%{public}s] scaleProperties have wrong number \n", __func__);
        return false;
    }

    scaleValueBegin = { scaleProperties[SCALE_PROP_X][PROP_START], scaleProperties[SCALE_PROP_Y][PROP_START] };
    scaleValue = { scaleProperties[SCALE_PROP_X][PROP_END], scaleProperties[SCALE_PROP_Y][PROP_END] };
    return true;
}

bool RSSymbolAnimation::SetScaleUnitAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (rsNode_ == nullptr || symbolAnimationConfig == nullptr) {
        ROSEN_LOGD("HmSymbol SetScaleUnitAnimation::getNode or get symbolAnimationConfig:failed");
        return false;
    }
    auto nodeNum = symbolAnimationConfig->numNodes;
    if (nodeNum == 0) {
        return false;
    }
    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    auto canvasNode = RSCanvasNode::Create();
    if (rsNode_->canvasNodesListMap.count(symbolSpanId) > 0) {
        rsNode_->canvasNodesListMap[symbolSpanId].emplace_back(canvasNode);
    } else {
        rsNode_->canvasNodesListMap[symbolSpanId] = { canvasNode };
    }

    auto& symbolNode = symbolAnimationConfig->SymbolNodes[UNIT_NODE];
    Vector4f offsets = CalculateOffset(symbolNode.symbolData.path_, symbolNode.nodeBoundary[0],
        symbolNode.nodeBoundary[1]); // index 0 offsetX of layout, 1 offsetY of layout
    if (!SetSymbolGeometry(canvasNode, Vector4f(offsets[0], offsets[1], // 0: offsetX of newNode; 1: offsetY
        symbolNode.nodeBoundary[WIDTH], symbolNode.nodeBoundary[HEIGHT]))) {
        return false;
    }
    Drawing::DrawingPiecewiseParameter scaleUnitParas;
    Vector2f scaleValueBegin;
    Vector2f scaleValue;
    if (!GetScaleUnitAnimationParas(scaleUnitParas, scaleValueBegin, scaleValue)) {
        return false;
    }
    Vector2f scaleValueEnd = scaleValueBegin;
    auto animation = ScaleSymbolAnimation(canvasNode, scaleUnitParas, scaleValueBegin, scaleValue, scaleValueEnd);
    if (animation == nullptr) {
        return false;
    }
    animation->Start(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(symbolNode.nodeBoundary[WIDTH], symbolNode.nodeBoundary[HEIGHT]);
    DrawSymbolOnCanvas(recordingCanvas, symbolNode, offsets);
    canvasNode->FinishRecording();
    rsNode_->AddChild(canvasNode, -1);
    return true;
}

bool RSSymbolAnimation::SetSymbolGeometry(const std::shared_ptr<RSNode>& rsNode, const Vector4f& bounds)
{
    if (rsNode == nullptr) {
        return false;
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
    rsNode_->SetClipToBounds(false);
    rsNode_->SetClipToFrame(false);
    return true;
}

std::shared_ptr<RSAnimation> RSSymbolAnimation::ScaleSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
    const Drawing::DrawingPiecewiseParameter& scaleUnitParas, const Vector2f& scaleValueBegin,
    const Vector2f& scaleValue, const Vector2f& scaleValueEnd)
{
    if (rsNode == nullptr) {
        return nullptr;
    }
    bool isCreate = CreateOrSetModifierValue(scaleStartProperty_, scaleValueBegin);
    if (isCreate) {
        auto scaleModifier = std::make_shared<RSScaleModifier>(scaleStartProperty_);
        rsNode->AddModifier(scaleModifier);
    }
    CreateOrSetModifierValue(scaleProperty_, scaleValue);
    CreateOrSetModifierValue(scaleEndProperty_, scaleValueEnd);
    Vector2f curNodePivot = rsNode->GetStagingProperties().GetPivot();
    if (!IsEqual(curNodePivot, CENTER_NODE_COORDINATE)) {
        isCreate = CreateOrSetModifierValue(pivotProperty_, CENTER_NODE_COORDINATE);
        if (isCreate) {
            auto pivotModifier = std::make_shared<RSPivotModifier>(pivotProperty_);
            rsNode->AddModifier(pivotModifier);
        }
    }

    RSAnimationTimingCurve scaleCurve = SetScaleSpringTimingCurve(scaleUnitParas.curveArgs);

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(scaleStartProperty_);
    keyframeAnimation->SetStartDelay(scaleUnitParas.delay);
    if (scaleUnitParas.duration != 0) {
        keyframeAnimation->SetDuration(scaleUnitParas.duration);
    } else {
        keyframeAnimation->SetDuration(2000); // default duration is 2000ms
    }
    keyframeAnimation->AddKeyFrame(0.25f, scaleProperty_, scaleCurve);
    keyframeAnimation->AddKeyFrame(0.75f, scaleProperty_, scaleCurve);
    keyframeAnimation->AddKeyFrame(1.f, scaleEndProperty_, scaleCurve);
    return keyframeAnimation;
}

RSAnimationTimingCurve RSSymbolAnimation::SetScaleSpringTimingCurve(const std::map<std::string, double_t>& curveArgs)
{
    double scaleVelocity = 0;
    double scaleMass = 0;
    double scaleStiffness = 0;
    double scaleDamping = 0;
    ElementInMap("velocity", curveArgs, scaleVelocity);
    ElementInMap("mass", curveArgs, scaleMass);
    ElementInMap("stiffness", curveArgs, scaleStiffness);
    ElementInMap("damping", curveArgs, scaleDamping);
    RSAnimationTimingCurve scaleCurve = RSAnimationTimingCurve::CreateSpringCurve(static_cast<float>(scaleVelocity),
        static_cast<float>(scaleMass), static_cast<float>(scaleStiffness), static_cast<float>(scaleDamping));
    return scaleCurve;
}

bool RSSymbolAnimation::SetVariableColorAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (rsNode_ == nullptr || symbolAnimationConfig == nullptr) {
        ROSEN_LOGD("HmSymbol SetVariableColorAnimation::getNode or get symbolAnimationConfig:failed");
        return false;
    }
    auto nodeNum = symbolAnimationConfig->numNodes;
    if (nodeNum == 0) {
        return false;
    }

    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    auto& symbolFirstNode = symbolAnimationConfig->SymbolNodes[0]; // calculate offset by the first node
    Vector4f offsets = CalculateOffset(symbolFirstNode.symbolData.path_, symbolFirstNode.nodeBoundary[0],
        symbolFirstNode.nodeBoundary[1]); // index 0 offsetX and 1 offsetY of layout
    for (uint32_t n = 0; n < nodeNum; n++) {
        auto& symbolNode = symbolAnimationConfig->SymbolNodes[n];
        auto canvasNode = RSCanvasNode::Create();
        if (rsNode_->canvasNodesListMap.count(symbolSpanId) > 0) {
            rsNode_->canvasNodesListMap[symbolSpanId].emplace_back(canvasNode);
        } else {
            rsNode_->canvasNodesListMap[symbolSpanId] = { canvasNode };
        }
        if (!SetSymbolGeometry(canvasNode, Vector4f(offsets[0], offsets[1], // 0: offsetX of newNode 1: offsetY
            symbolNode.nodeBoundary[WIDTH], symbolNode.nodeBoundary[HEIGHT]))) {
            return false;
        }
        auto recordingCanvas =
            canvasNode->BeginRecording(symbolNode.nodeBoundary[WIDTH], symbolNode.nodeBoundary[HEIGHT]);
        DrawPathOnCanvas(recordingCanvas, symbolNode, offsets);
        canvasNode->FinishRecording();
        rsNode_->AddChild(canvasNode, -1);

        if (symbolNode.animationIndex == -1) {
            continue;
        }
        std::shared_ptr<RSAnimation> animation = nullptr;
        uint32_t duration = 0;
        int delay = 0;
        std::vector<float> timePercents;
        if (!GetVariableColorAnimationParas(symbolNode.animationIndex, duration, delay, timePercents)) {
            return false;
        }
        auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaPropertyPhases_[0]); // initial the alpha status
        canvasNode->AddModifier(alphaModifier);
        animation = VariableColorSymbolAnimation(canvasNode, duration, delay, timePercents);
        if (animation == nullptr) {
            return false;
        }
        animation->Start(canvasNode);
    }
    return true;
}

bool RSSymbolAnimation::GetVariableColorAnimationParas(
    const uint32_t index, uint32_t& totalDuration, int& delay, std::vector<float>& timePercents)
{
    // AnimationType, Animation groups, animation_mode; the variable color groups is 3 , animation_mode is 1
    auto multiGroupParas = Drawing::HmSymbolConfigOhos::GetGroupParameters(Drawing::VARIABLE_COLOR_TYPE, 3, 1);
    if (multiGroupParas.size() <= index || multiGroupParas[index].empty()) {
        ROSEN_LOGD("[%{public}s] can not get multiGroupParas \n", __func__);
        return false;
    }
    auto oneGroupParas = multiGroupParas[index]; // index means the sequence number of node or animation layer
    if (oneGroupParas.empty()) {
        return false;
    }
    delay = oneGroupParas[0].delay; // 0 means the first phase of a node
    totalDuration = 0;
    // traverse all time phases
    for (unsigned long i = 0; i < oneGroupParas.size(); i++) {
        totalDuration = oneGroupParas[i].duration + totalDuration;
        if (!ElementInMap(ALPHA_PROP, oneGroupParas[i].properties) ||
            oneGroupParas[i].properties[ALPHA_PROP].size() != PROPERTIES) {
            return false;
        }
        // each node needs same alphaPropertyPhases
        // desired result : alphaPropertyPhases_.size() = oneGroupParas.size() + 1
        if (alphaPropertyPhases_.size() <= oneGroupParas.size()) {
            if (i == 0) {
                float alphaValue = oneGroupParas[i].properties.at(ALPHA_PROP)[PROP_START]; // the first value
                std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
                CreateOrSetModifierValue(alphaPropertyPhase, alphaValue);
                alphaPropertyPhases_.push_back(alphaPropertyPhase);
            }
            float alphaValue = oneGroupParas[i].properties.at(ALPHA_PROP)[PROP_END]; // the value of the key frame needs
            std::shared_ptr<RSAnimatableProperty<float>> alphaPropertyPhase = nullptr;
            CreateOrSetModifierValue(alphaPropertyPhase, alphaValue);
            alphaPropertyPhases_.push_back(alphaPropertyPhase);
        }
    }
    return CalcTimePercents(timePercents, static_cast<float>(totalDuration), oneGroupParas);
}

bool RSSymbolAnimation::CalcTimePercents(std::vector<float>& timePercents, const float totalDuration,
    const std::vector<Drawing::DrawingPiecewiseParameter>& oneGroupParas)
{
    if (totalDuration <= 0) {
        return false;
    }
    uint32_t duration = 0;
    float timePercent = 0;
    timePercents.push_back(timePercent); // the first property of timePercent
    for (unsigned long i = 0; i < oneGroupParas.size(); i++) {
        timePercent = static_cast<float>(oneGroupParas[i].duration + duration) / static_cast<float>(totalDuration);
        timePercents.push_back(timePercent);
        duration = duration + oneGroupParas[i].duration;
    }
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

std::shared_ptr<RSAnimation> RSSymbolAnimation::VariableColorSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
    const uint32_t& duration, const int& delay, const std::vector<float>& timePercents)
{
    if (alphaPropertyPhases_.size() == 0 || timePercents.size() != alphaPropertyPhases_.size()) {
        return nullptr;
    }
    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(alphaPropertyPhases_[0]); // initial the alpha status
    if (keyframeAnimation == nullptr || rsNode == nullptr) {
        return nullptr;
    }
    keyframeAnimation->SetStartDelay(delay);
    keyframeAnimation->SetDuration(duration);
    std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> keyframes;
    for (unsigned long i = 1; i < alphaPropertyPhases_.size(); i++) {
        keyframes.push_back(std::make_tuple(timePercents[i], alphaPropertyPhases_[i], RSAnimationTimingCurve::LINEAR));
    }
    keyframeAnimation->AddKeyFrames(keyframes);
    return keyframeAnimation;
}

// base atomizated animation
void RSSymbolAnimation::ScaleAnimationBase(const std::shared_ptr<RSNode>& rsNode,
    Drawing::DrawingPiecewiseParameter& scaleParameter, std::vector<std::shared_ptr<RSAnimation>>& animations)
{
    // validation input
    if (rsNode == nullptr) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return;
    }
    int validSize = 2;
    if (scaleParameter.properties.count("sx") <= 0 || scaleParameter.properties.count("sy") <= 0 ||
        scaleParameter.properties["sx"].size() < validSize || scaleParameter.properties["sx"].size() < validSize) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return;
    }

    SetNodePivot(rsNode);

    const Vector2f scaleValueBegin = { scaleParameter.properties["sx"][0], scaleParameter.properties["sy"][0] };
    const Vector2f scaleValueEnd = { scaleParameter.properties["sx"][1], scaleParameter.properties["sy"][1] };

    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty;
    bool isCreate = CreateOrSetModifierValue(scaleProperty, scaleValueBegin);
    if (!isCreate) {
        ROSEN_LOGD("[%{public}s] : invalid parameter \n", __func__);
        return;
    }

    std::shared_ptr<RSAnimatableProperty<Vector2f>> pivotProperty;
    auto scaleModifier = std::make_shared<Rosen::RSScaleModifier>(scaleProperty);
    rsNode->AddModifier(scaleModifier);

    // set animation curve and protocol
    RSAnimationTimingCurve scaleCurve;
    CreateAnimationTimingCurve(scaleParameter.curveType, scaleParameter.curveArgs, scaleCurve);

    RSAnimationTimingProtocol scaleprotocol;
    int startDelay = int(scaleParameter.delay);
    scaleprotocol.SetStartDelay(startDelay);
    int duration = int(scaleParameter.duration);
    scaleprotocol.SetDuration(duration);

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
    int validSize = 2;
    if (alphaParameter.properties.count("alpha") <= 0 || alphaParameter.properties["alpha"].size() < validSize) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return;
    }

    float alphaBegin = float(alphaParameter.properties["alpha"][0]);
    float alphaValueEnd = float(alphaParameter.properties["alpha"][1]);

    std::shared_ptr<RSAnimatableProperty<float>> alphaProperty;

    if (!CreateOrSetModifierValue(alphaProperty, alphaBegin)) {
        return;
    }
    auto alphaModifier = std::make_shared<Rosen::RSAlphaModifier>(alphaProperty);

    rsNode->AddModifier(alphaModifier);

    RSAnimationTimingProtocol alphaProtocol;
    alphaProtocol.SetStartDelay(alphaParameter.delay);
    alphaProtocol.SetDuration(alphaParameter.duration);
    RSAnimationTimingCurve alphaCurve;
    CreateAnimationTimingCurve(alphaParameter.curveType, alphaParameter.curveArgs, alphaCurve);

    std::vector<std::shared_ptr<RSAnimation>> animations1 = RSNode::Animate(
        alphaProtocol, alphaCurve, [&alphaProperty, &alphaValueEnd]() { alphaProperty->Set(alphaValueEnd); });

    if (animations1.size() > 0 && animations1[0] != nullptr) {
        animations.emplace_back(animations1[0]);
    }
}
} // namespace Rosen
} // namespace OHOS