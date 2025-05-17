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

#include <cmath>

#include "animation/rs_symbol_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "draw/paint.h"
#include "platform/common/rs_log.h"
#include "utils/point.h"

namespace OHOS {
namespace Rosen {
using SymnolBaseFunc = std::function<void(const Drawing::DrawingPiecewiseParameter& parameter)>;
using SymbolBaseFuncMap = std::unordered_map<std::string, SymnolBaseFunc>;

static const Vector2f CENTER_NODE_COORDINATE = { 0.5f, 0.5f }; // scale center node
static const std::string SCALE_PROP_X = "sx";
static const std::string SCALE_PROP_Y = "sy";
static const std::string ALPHA_PROP = "alpha";
static const std::string BLUR_PROP = "blur";
static const std::string TRANSLATE_PROP_X = "tx";
static const std::string TRANSLATE_PROP_Y = "ty";
static const std::string TRANSLATE_RATIO = "tr";
static const std::string CLIP_PROP = "clip";
static const size_t PROPERTIES = 2; // symbol animation property contains two values, change from one to the other
static const unsigned int PROP_START = 0; // symbol animation property contains two values, change from START to the END
static const unsigned int PROP_END = 1; // symbol animation property contains two values, change from START to the END
static const unsigned int NODE_WIDTH = 2;
static const unsigned int NODE_HEIGHT = 3;
static const unsigned int INVALID_STATUS = 0; // invalid status label
static const unsigned int APPEAR_STATUS = 1 ; // appear status label

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
    auto element = curMap.find(curElement);
    return (element != curMap.end());
}

float CurveArgsInMap(const std::string& curElement, const std::map<std::string, float>& curMap)
{
    auto element = curMap.find(curElement);
    if (element == curMap.end()) {
        return 0.0f;
    }
    return element->second;
}

void CreateAnimationTimingCurve(const OHOS::Rosen::Drawing::DrawingCurveType type,
    const std::map<std::string, float>& curveArgs, RSAnimationTimingCurve& curve)
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

/**
 * @brief SymbolAnimation interface for arkui
 * @param symbolAnimationConfig indicates all the information about a symbol
 * @return true if set animation success
 */
bool RSSymbolAnimation::SetSymbolAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (rsNode_ == nullptr || symbolAnimationConfig == nullptr) {
        ROSEN_LOGD("HmSymbol RSSymbolAnimation::getNode or get symbolAnimationConfig:failed");
        return false;
    }

    if (symbolAnimationConfig->effectStrategy == Drawing::DrawingEffectStrategy::NONE) {
        std::lock_guard<std::mutex> lock(rsNode_->childrenNodeLock_);
        rsNode_->replaceNodesSwapArr_[INVALID_STATUS].clear();
        rsNode_->replaceNodesSwapArr_[APPEAR_STATUS].clear();
        rsNode_->canvasNodesListMap_.erase(symbolAnimationConfig->symbolSpanId);
        return true; // pre code already clear nodes.
    }

    if (!symbolAnimationConfig->currentAnimationHasPlayed) {
        switch (symbolAnimationConfig->effectStrategy) {
            case Drawing::DrawingEffectStrategy::REPLACE_APPEAR:
                PopNodeFromReplaceList(symbolAnimationConfig->symbolSpanId);
                break;
            case Drawing::DrawingEffectStrategy::TEXT_FLIP:
                PopNodeFromFlipList(symbolAnimationConfig->symbolSpanId);
                break;
            default:
                NodeProcessBeforeAnimation(symbolAnimationConfig);
                break;
        }
    }

    InitSupportAnimationTable();

    switch (symbolAnimationConfig->effectStrategy) {
        case Drawing::DrawingEffectStrategy::REPLACE_APPEAR:
            return SetReplaceAnimation(symbolAnimationConfig);
        case Drawing::DrawingEffectStrategy::TEXT_FLIP:
            return SetTextFlipAnimation(symbolAnimationConfig);
        case Drawing::DrawingEffectStrategy::DISABLE:
            return SetDisableAnimation(symbolAnimationConfig);
        default:
            return SetPublicAnimation(symbolAnimationConfig);
    }
}

void RSSymbolAnimation::NodeProcessBeforeAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    std::lock_guard<std::mutex> lock(rsNode_->childrenNodeLock_);
    rsNode_->canvasNodesListMap_.erase(symbolAnimationConfig->symbolSpanId);
}

void RSSymbolAnimation::PopNodeFromReplaceList(uint64_t symbolSpanId)
{
    std::lock_guard<std::mutex> lock(rsNode_->childrenNodeLock_);
    if (rsNode_->canvasNodesListMap_.count(symbolSpanId) == 0) {
        rsNode_->canvasNodesListMap_[symbolSpanId] = {};
    }

    for (const auto& config : rsNode_->replaceNodesSwapArr_[INVALID_STATUS]) {
        rsNode_->canvasNodesListMap_[symbolSpanId].erase(config.nodeId);
    }
    rsNode_->replaceNodesSwapArr_[INVALID_STATUS].clear();
}

void RSSymbolAnimation::PopNodeFromFlipList(uint64_t symbolSpanId)
{
    std::lock_guard<std::mutex> lock(rsNode_->childrenNodeLock_);
    auto it = rsNode_->canvasNodesListMap_.find(symbolSpanId);
    if (it == rsNode_->canvasNodesListMap_.end()) {
        it = rsNode_->canvasNodesListMap_.insert({symbolSpanId,
            std::unordered_map<NodeId, std::shared_ptr<RSNode>>()}).first;
    }
    it->second.erase(INVALID_STATUS);
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
        ROSEN_LOGD("[%{public}s]: symbolAnimationConfig or disappearConfig is nullptr", __func__);
        return false;
    }

    disappearConfig->repeatCount = symbolAnimationConfig->repeatCount;
    disappearConfig->animationMode = symbolAnimationConfig->animationMode;
    disappearConfig->animationStart = symbolAnimationConfig->animationStart;
    disappearConfig->symbolSpanId = symbolAnimationConfig->symbolSpanId;
    disappearConfig->commonSubType = symbolAnimationConfig->commonSubType;
    disappearConfig->effectStrategy = symbolAnimationConfig->effectStrategy;

    // count node levels and animation levels
    uint32_t numNodes = 0;
    int animationLevelNum = -1; // -1 is initial value, that is no animation levels
    auto& disappearNodes = rsNode_->replaceNodesSwapArr_[APPEAR_STATUS];
    for (const auto& config : disappearNodes) {
        TextEngine::SymbolNode symbolNode;
        symbolNode.animationIndex = config.symbolNode.animationIndex;
        disappearConfig->symbolNodes.push_back(symbolNode);
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
    if (!symbolAnimationConfig->currentAnimationHasPlayed) {
        auto disappearConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
        if (SetDisappearConfig(symbolAnimationConfig, disappearConfig)) {
            SetReplaceDisappear(disappearConfig);
        }
    }
    SetReplaceAppear(symbolAnimationConfig,
        !rsNode_->replaceNodesSwapArr_[INVALID_STATUS].empty());
    return true;
}

bool RSSymbolAnimation::SetReplaceDisappear(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig->numNodes == 0) {
        ROSEN_LOGD("[%{public}s] numNodes in symbolAnimationConfig is 0", __func__);
        return false;
    }

    auto& disappearNodes = rsNode_->replaceNodesSwapArr_[APPEAR_STATUS];
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    Drawing::DrawingEffectStrategy effectStrategy = Drawing::DrawingEffectStrategy::REPLACE_DISAPPEAR;
    bool res = GetAnimationGroupParameters(symbolAnimationConfig, parameters, effectStrategy);
    for (const auto& config : disappearNodes) {
        if (!res || (config.symbolNode.animationIndex < 0)) {
            ROSEN_LOGD("[%{public}s] invalid initial parameter", __func__);
            continue;
        }
        if (parameters.size() <= static_cast<size_t>(config.symbolNode.animationIndex) ||
            parameters[config.symbolNode.animationIndex].empty()) {
            ROSEN_LOGD("[%{public}s] invalid parameter", __func__);
            continue;
        }
        auto canvasNode = rsNode_->canvasNodesListMap_[symbolAnimationConfig->symbolSpanId][config.nodeId];
        SpliceAnimation(canvasNode, parameters[config.symbolNode.animationIndex]);
    }
    {
        std::lock_guard<std::mutex> lock(rsNode_->childrenNodeLock_);
        swap(rsNode_->replaceNodesSwapArr_[INVALID_STATUS], rsNode_->replaceNodesSwapArr_[APPEAR_STATUS]);
    }
    return true;
}

bool RSSymbolAnimation::SetReplaceAppear(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    bool isStartAnimation)
{
    if (symbolAnimationConfig->symbolNodes.empty()) {
        ROSEN_LOGD("[%{public}s] symbol nodes is empty", __func__);
        return false;
    }
    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    const auto& symbolFirstNode = symbolAnimationConfig->symbolNodes[0]; // calculate offset by the first node
    Vector4f offsets = CalculateOffset(symbolFirstNode.symbolData.path_,
        symbolFirstNode.nodeBoundary[0], symbolFirstNode.nodeBoundary[1]); // index 0 offsetX and 1 offsetY of layout
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    Drawing::DrawingEffectStrategy effectStrategy = Drawing::DrawingEffectStrategy::REPLACE_APPEAR;
    bool res = GetAnimationGroupParameters(symbolAnimationConfig, parameters,
        effectStrategy);
    uint32_t nodeNum = symbolAnimationConfig->numNodes;
    rsNode_->replaceNodesSwapArr_[APPEAR_STATUS].resize(nodeNum);
    for (uint32_t n = 0; n < nodeNum; n++) {
        bool createNewNode = CreateSymbolNode(symbolAnimationConfig, offsets, n);
        auto nodeId = rsNode_->replaceNodesSwapArr_[APPEAR_STATUS][n].nodeId;
        auto canvasNode = std::static_pointer_cast<RSCanvasNode>(rsNode_->canvasNodesListMap_[symbolSpanId][nodeId]);
        if (canvasNode == nullptr) {
            return false;
        }
        auto& symbolNode = symbolAnimationConfig->symbolNodes[n];
        GroupDrawing(canvasNode, symbolNode, offsets);
        bool isInValid = !res || !isStartAnimation || symbolNode.animationIndex < 0 ||
                         parameters.size() <= static_cast<size_t>(symbolNode.animationIndex);
        if (isInValid) {
            ROSEN_LOGD("[%{public}s] invalid animationIndex or parameter", __func__);
            continue;
        }

        if (createNewNode) {
            SpliceAnimation(canvasNode, parameters[symbolNode.animationIndex]);
        }
    }
    return true;
}

bool RSSymbolAnimation::CreateSymbolNode(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    const Vector4f& offsets, uint32_t index, std::shared_ptr<RSNode> rsNode)
{
    if (rsNode == nullptr) {
        rsNode = rsNode_;
    }
    std::lock_guard<std::mutex> lock(rsNode->childrenNodeLock_);
    auto& symbolNode = symbolAnimationConfig->symbolNodes[index];
    auto symbolBounds = Vector4f(offsets[0], offsets[1], // 0: offsetX of newMode 1: offsetY
        symbolNode.nodeBoundary[NODE_WIDTH], symbolNode.nodeBoundary[NODE_HEIGHT]);
    bool createNewNode = false;
    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    auto outerIter = rsNode->canvasNodesListMap_.find(symbolSpanId);
    if (outerIter == rsNode->canvasNodesListMap_.end()) {
        outerIter = rsNode->canvasNodesListMap_.insert({symbolSpanId,
                    std::unordered_map<NodeId, std::shared_ptr<RSNode>>()}).first;
        createNewNode = true;
    }
    bool isReplaceAppear = symbolAnimationConfig->effectStrategy == Drawing::DrawingEffectStrategy::REPLACE_APPEAR;
    NodeId nodeId = isReplaceAppear ? rsNode->replaceNodesSwapArr_[APPEAR_STATUS][index].nodeId :
                    static_cast<NodeId>(index);
    createNewNode = createNewNode || outerIter->second.find(nodeId) == outerIter->second.end() ||
                    (isReplaceAppear && rsNode->replaceNodesSwapArr_[APPEAR_STATUS][index].animationIndex < 0);
    if (!createNewNode) {
        UpdateSymbolGeometry(rsNode->canvasNodesListMap_[symbolSpanId][nodeId], symbolBounds);
        return false;
    }
    auto childNode = RSCanvasNode::Create(false, false, rsNode->GetRSUIContext());
    if (childNode == nullptr) {
        ROSEN_LOGD("[%{public}s] Failed to create CanvasNode", __func__);
        return false;
    }
    nodeId = isReplaceAppear ? childNode->GetId() : nodeId;
    outerIter->second.insert({nodeId, childNode});
    SetSymbolGeometry(childNode, symbolBounds);
    rsNode->AddChild(childNode, -1);

    if (isReplaceAppear) {
        AnimationNodeConfig appearNodeConfig = {.symbolNode = symbolNode,
                                                .nodeId = nodeId,
                                                .animationIndex = symbolNode.animationIndex};
        rsNode->replaceNodesSwapArr_[APPEAR_STATUS][index] = appearNodeConfig;
    }
    return true;
}

void RSSymbolAnimation::InitSupportAnimationTable()
{
    // Init public animation list
    if (publicSupportAnimations_.empty()) {
        publicSupportAnimations_ = {Drawing::DrawingEffectStrategy::BOUNCE,
                                    Drawing::DrawingEffectStrategy::APPEAR,
                                    Drawing::DrawingEffectStrategy::DISAPPEAR,
                                    Drawing::DrawingEffectStrategy::SCALE};
    }
    if (upAndDownSupportAnimations_.empty()) {
        upAndDownSupportAnimations_ = {Drawing::DrawingEffectStrategy::BOUNCE,
                                       Drawing::DrawingEffectStrategy::SCALE,
                                       Drawing::DrawingEffectStrategy::DISABLE};
    }
}

bool RSSymbolAnimation::GetAnimationGroupParameters(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>>& parameters,
    Drawing::DrawingEffectStrategy& effectStrategy)
{
    // count animation levels
    int animationLevelNum = -1;
    auto nodeNum = symbolAnimationConfig->numNodes;
    for (uint32_t n = 0; n < nodeNum; n++) {
        const auto& symbolNode = symbolAnimationConfig->symbolNodes[n];
        animationLevelNum =
            animationLevelNum < symbolNode.animationIndex ? symbolNode.animationIndex : animationLevelNum;
    }

    if (animationLevelNum < 0) {
        ROSEN_LOGD("[%{public}s] HmSymbol: this symbol does not have an animated layer", __func__);
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
        ROSEN_LOGD("[%{public}s] HmSymbol: GetGroupParameters failed", __func__);
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
        SpliceAnimation(rsNode, parameters);
        return true;
    }

    switch (symbolAnimationConfig->effectStrategy) {
        case Drawing::DrawingEffectStrategy::VARIABLE_COLOR:
            return SetKeyframeAlphaAnimation(rsNode, parameters, symbolAnimationConfig);
        case Drawing::DrawingEffectStrategy::PULSE:
            return SetKeyframeAlphaAnimation(rsNode, parameters, symbolAnimationConfig);
        default:
            ROSEN_LOGD("[%{public}s] not support input animation type", __func__);
            return false;
    }
}

bool RSSymbolAnimation::SetPublicAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig->symbolNodes.empty()) {
        ROSEN_LOGD("[%{public}s] symbol nodes is empty", __func__);
        return false;
    }
    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    const auto& symbolFirstNode = symbolAnimationConfig->symbolNodes[0]; // calculate offset by the first node
    Vector4f offsets = CalculateOffset(symbolFirstNode.symbolData.path_, symbolFirstNode.nodeBoundary[0],
        symbolFirstNode.nodeBoundary[1]); // index 0 offsetX and 1 offsetY of layout
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    bool res = GetAnimationGroupParameters(symbolAnimationConfig, parameters, symbolAnimationConfig->effectStrategy);
    uint32_t nodeNum = symbolAnimationConfig->numNodes;
    for (uint32_t n = 0; n < nodeNum; n++) {
        bool createNewNode = CreateSymbolNode(symbolAnimationConfig, offsets, n);
        auto canvasNode = std::static_pointer_cast<RSCanvasNode>(rsNode_->canvasNodesListMap_[symbolSpanId][n]);
        if (canvasNode == nullptr) {
            return false;
        }
        auto& symbolNode = symbolAnimationConfig->symbolNodes[n];
        GroupDrawing(canvasNode, symbolNode, offsets);

        bool isInValid = !res || symbolNode.animationIndex < 0 ||
                         parameters.size() <= static_cast<size_t>(symbolNode.animationIndex);
        if (isInValid) {
            ROSEN_LOGD("[%{public}s] invalid animationIndex or parameter", __func__);
            continue;
        }

        if (createNewNode) {
            ChooseAnimation(canvasNode, parameters[symbolNode.animationIndex], symbolAnimationConfig);
        }
    }
    return true;
}

// set text flip animation
bool RSSymbolAnimation::SetTextFlipAnimation(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig->parameters.size() < PROPERTIES) {
        ROSEN_LOGE("Invalid animation parameters of text flip, parameters.size: %{public}zu ",
            symbolAnimationConfig->parameters.size());
        return false;
    }

    if (!symbolAnimationConfig->currentAnimationHasPlayed) {
        SetFlipDisappear(symbolAnimationConfig, rsNode_);
    }
    return SetFlipAppear(symbolAnimationConfig, rsNode_,
        !symbolAnimationConfig->currentAnimationHasPlayed);
}

void RSSymbolAnimation::SetFlipDisappear(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    std::shared_ptr<RSNode>& rsNode)
{
    const auto& symbolId = symbolAnimationConfig->symbolSpanId;
    if (rsNode->canvasNodesListMap_.find(symbolId) == rsNode->canvasNodesListMap_.end()) {
        return ;
    }
    if (rsNode->canvasNodesListMap_[symbolId].find(APPEAR_STATUS) == rsNode->canvasNodesListMap_[symbolId].end()) {
        return;
    }
    auto& canvasNode = rsNode->canvasNodesListMap_[symbolId][APPEAR_STATUS];
    auto parameter = UpdateParametersDelay(symbolAnimationConfig->parameters[0],
        symbolAnimationConfig->effectElement.delay);
    SpliceAnimation(canvasNode, parameter);
    {
        std::lock_guard<std::mutex> lock(rsNode->childrenNodeLock_);
        rsNode->canvasNodesListMap_[symbolId].insert({INVALID_STATUS, canvasNode});
        rsNode->canvasNodesListMap_[symbolId].erase(APPEAR_STATUS);
    }
}

bool RSSymbolAnimation::SetFlipAppear(
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    std::shared_ptr<RSNode>& rsNode, bool isStartAnimation)
{
    auto symbolSpanId = symbolAnimationConfig->symbolSpanId;
    TextEngine::SymbolNode symbolNode;
    const auto& effectNode = symbolAnimationConfig->effectElement;
    if (!effectNode.path.IsValid()) {
        return true;
    }
    // creat childNode
    Vector4f offsets = CalculateOffset(effectNode.path, effectNode.offset.GetX(),
        effectNode.offset.GetY());
    // 0: offsetX, 1: offsetY of childNode
    auto bounds = Vector4f(offsets[0], offsets[1], effectNode.width, effectNode.height);
    {
        std::lock_guard<std::mutex> lock(rsNode->childrenNodeLock_);
        if (rsNode->canvasNodesListMap_.find(symbolSpanId) == rsNode->canvasNodesListMap_.end()) {
            rsNode->canvasNodesListMap_.insert({symbolSpanId,
                std::unordered_map<NodeId, std::shared_ptr<RSNode>>()});
        }
        if (rsNode->canvasNodesListMap_[symbolSpanId].find(APPEAR_STATUS) ==
            rsNode->canvasNodesListMap_[symbolSpanId].end()) {
            auto childNode = RSCanvasNode::Create();
            SetSymbolGeometry(childNode, bounds);
            rsNode->canvasNodesListMap_[symbolSpanId].insert((std::make_pair(APPEAR_STATUS, childNode)));
            rsNode->AddChild(childNode, -1);
            isStartAnimation = true;
        }
        if (!isStartAnimation) {
            UpdateSymbolGeometry(rsNode_->canvasNodesListMap_[symbolSpanId][APPEAR_STATUS], bounds);
        }
    }
    auto canvasNode = std::static_pointer_cast<RSCanvasNode>(
        rsNode_->canvasNodesListMap_[symbolSpanId][APPEAR_STATUS]);
    if (canvasNode == nullptr) {
        ROSEN_LOGE("Failed to get canvasNode of TextFilp");
        return false;
    }
    // draw path
    auto recordingCanvas = canvasNode->BeginRecording(effectNode.width, effectNode.height);
    DrawPathOnCanvas(recordingCanvas, effectNode, symbolAnimationConfig->color, {offsets[0], offsets[1]});
    canvasNode->FinishRecording();
    if (isStartAnimation) {
        auto parameter = UpdateParametersDelay(symbolAnimationConfig->parameters[1], effectNode.delay);
        SpliceAnimation(canvasNode, parameter);
    }
    return true;
}

std::vector<Drawing::DrawingPiecewiseParameter> RSSymbolAnimation::UpdateParametersDelay(
    const std::vector<Drawing::DrawingPiecewiseParameter>& parameters, int delay)
{
    std::vector<Drawing::DrawingPiecewiseParameter> outParameters = parameters;
    for (auto& parameter : outParameters) {
        parameter.delay += delay;
    }
    return outParameters;
}

void RSSymbolAnimation::DrawPathOnCanvas(ExtendRecordingCanvas* recordingCanvas,
    const TextEngine::TextEffectElement& effectNode,
    const Drawing::Color& color, const Drawing::Point& offset)
{
    if (recordingCanvas == nullptr) {
        return;
    }

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(color.CastToColorQuad());
    auto path = effectNode.path;
    path.Offset(effectNode.offset.GetX() - offset.GetX(), effectNode.offset.GetY() - offset.GetY());
    recordingCanvas->AttachBrush(brush);
    recordingCanvas->DrawPath(path);
    recordingCanvas->DetachBrush();
}

void RSSymbolAnimation::GroupAnimationStart(
    const std::shared_ptr<RSNode>& rsNode, std::vector<std::shared_ptr<RSAnimation>>& animations)
{
    if (rsNode == nullptr || animations.empty()) {
        ROSEN_LOGD("[%{public}s] : invalid input", __func__);
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
    if (rsNode == nullptr) {
        ROSEN_LOGD("Interpolator is null, return FRACTION_MIN.");
        return;
    }
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

void RSSymbolAnimation::CreateSameNode(uint64_t symbolId, std::shared_ptr<RSNode>& rsNode)
{
    Vector4f bounds = rsNode_->GetStagingProperties().GetBounds();
    auto symbolBounds = Vector4f(0, 0, bounds.z_, bounds.w_);

    std::lock_guard<std::mutex> lock(rsNode_->childrenNodeLock_);
    auto outerIter = rsNode_->canvasNodesListMap_.find(symbolId);
    if (outerIter == rsNode_->canvasNodesListMap_.end()) {
        outerIter = rsNode_->canvasNodesListMap_.insert({symbolId,
            std::unordered_map<NodeId, std::shared_ptr<RSNode>>()}).first;
    }
    if (outerIter->second.find(symbolId) == outerIter->second.end()) {
        auto childNode = RSCanvasNode::Create();
        SetSymbolGeometry(childNode, symbolBounds);
        outerIter->second.insert({symbolId, childNode});
        rsNode_->AddChild(childNode, -1);
    }
    rsNode = outerIter->second[symbolId];
}

// set Disable Animation
bool RSSymbolAnimation::SetDisableAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (symbolAnimationConfig->symbolNodes.size() < 1) { // 1: symbol nodes must be greater than 1
        ROSEN_LOGE("HmSymbol Failed to set disable animation, symbol size: %{public}zu",
            symbolAnimationConfig->symbolNodes.size());
        return false;
    }

    // get the disable animation paramters
    std::vector<std::vector<Drawing::DrawingPiecewiseParameter>> parameters;
    if (!GetAnimationGroupParameters(symbolAnimationConfig, parameters, symbolAnimationConfig->effectStrategy)) {
        ROSEN_LOGE("HmSymbol Failed to get disable parameters");
        return false;
    }
    SetDisableParameter(parameters[parameters.size() - 1], symbolAnimationConfig);

    // Create a child node the same as the rsNode_
    std::shared_ptr<RSNode> rsNode = nullptr;
    CreateSameNode(symbolAnimationConfig->symbolSpanId, rsNode);
    if (rsNode == nullptr) {
        ROSEN_LOGE("HmSymbol Failed to create rsNode");
        return false;
    }
    rsNode->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
    rsNode->SetColorBlendApplyType(RSColorBlendApplyType::SAVE_LAYER_ALPHA);

    // calculate offset by the first symbol node config
    const auto& symbolFirstNode = symbolAnimationConfig->symbolNodes[0];
    Vector4f offsets = CalculateOffset(symbolFirstNode.symbolData.path_, symbolFirstNode.nodeBoundary[0],
        symbolFirstNode.nodeBoundary[1]); //index 0 offsetX, 1 offsety of layout
    
    // set animation base layers
    if (!SetDisableBaseLayer(rsNode, symbolAnimationConfig, parameters, offsets)) {
        return false;
    }

    // set animation clip layer
    uint32_t n = symbolAnimationConfig->symbolNodes.size() -1;
    const auto& symbolNode = symbolAnimationConfig->symbolNodes[n];
    bool isInValid = symbolNode.animationIndex < 0 || symbolNode.isMask ||
        parameters.size() <= static_cast<size_t>(symbolNode.animationIndex);
    if (isInValid) {
        ROSEN_LOGE("Invalid parameter of clip layer in HmSymbol");
        return false;
    }
    return SetClipAnimation(rsNode_, symbolAnimationConfig, parameters[symbolNode.animationIndex], n, offsets);
}

// set disable parameter with slope of symbol
void RSSymbolAnimation::SetDisableParameter(std::vector<Drawing::DrawingPiecewiseParameter>& parameter,
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (parameter.empty()) {
        return;
    }
    uint32_t n = symbolAnimationConfig->symbolNodes.size();
    const auto& symbolNode = symbolAnimationConfig->symbolNodes[n - 1];
    if (symbolNode.pathsInfo.empty()) {
        return;
    }

    auto slope = symbolAnimationConfig->slope;
    auto rect = symbolNode.pathsInfo[0].path.GetBounds();
    float w = rect.GetWidth();
    float h = rect.GetHeight();
    float distance = std::sqrt(w * w + h * h);
    float angle = std::atan(slope);
    float x = std::cos(angle) * distance;
    float y = std::sin(angle) * distance;

    for (auto& param: parameter) {
        std::vector<float> ratio;
        if (param.properties.count(TRANSLATE_RATIO)) {
            ratio = param.properties[TRANSLATE_RATIO];
        }
        if (param.properties.count(CLIP_PROP)) {
            ratio = param.properties[CLIP_PROP];
        }
        if (ratio.size() < PROPERTIES) {
            continue;
        }
        param.properties.insert({TRANSLATE_PROP_X, {x * ratio[PROP_START], x * ratio[PROP_END]}});
        param.properties.insert({TRANSLATE_PROP_Y, {y * ratio[PROP_START], y * ratio[PROP_END]}});
    }

}

bool RSSymbolAnimation::SetDisableBaseLayer(const std::shared_ptr<RSNode>& rsNode,
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    const std::vector<std::vector<Drawing::DrawingPiecewiseParameter>>& parameters,
    const Vector4f& offsets)
{
    auto symbolId = symbolAnimationConfig->symbolSpanId;
    for (uint32_t n = 0; n < symbolAnimationConfig->symbolNodes.size() -1; n++) {
        bool createNewNode = CreateSymbolNode(symbolAnimationConfig, offsets, n, rsNode);
        auto canvasNode = std::static_pointer_cast<RSCanvasNode>(rsNode->canvasNodesListMap_[symbolId][n]);
        if (canvasNode == nullptr) {
            ROSEN_LOGE("HmSymbol canvasNode is nullptr");
            return false;
        }
        auto& symbolNode = symbolAnimationConfig->symbolNodes[n];
        auto recordingCanvas = canvasNode->BeginRecording(symbolNode.nodeBoundary[NODE_WIDTH],
            symbolNode.nodeBoundary[NODE_HEIGHT]);
        DrawPathOnCanvas(recordingCanvas, symbolNode, offsets);
        canvasNode->FinishRecording();

        bool isInValid = symbolNode.animationIndex < 0 ||
            parameters.size() <= static_cast<size_t>(symbolNode.animationIndex);
        if (isInValid) {
            continue;
        }
        if (createNewNode) {
            SpliceAnimation(canvasNode, parameters[symbolNode.animationIndex]);
        }
    }
    return true;
}

bool RSSymbolAnimation::SetClipAnimation(const std::shared_ptr<RSNode>& rsNode,
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig,
    const std::vector<Drawing::DrawingPiecewiseParameter>& parameters,
    uint32_t index, const Vector4f& offsets)
{
    if (parameters.size() < 3) {
        ROSEN_LOGD("Invalid clip parameter in HmSymbol");
        return false;
    }
    // create clip layer node
    auto symbolId = symbolAnimationConfig->symbolSpanId;
    bool createNewNode = CreateSymbolNode(symbolAnimationConfig, offsets, index, rsNode_);
    auto canvasNode = std::static_pointer_cast<RSCanvasNode>(rsNode->canvasNodesListMap_[symbolId][index]);
    if (canvasNode == nullptr) {
            ROSEN_LOGE("HmSymbol canvasNode is nullptr");
            return false;
    }
    auto symbolNode = symbolAnimationConfig->symbolNodes[index];
    GroupDrawing(canvasNode, symbolNode, offsets, true);

    // create a node that displays slashes
    auto symbolBounds = Vector4f(0.0f, 0.0f, symbolNode.nodeBoundary[NODE_WIDTH],
        symbolNode.nodeBoundary[NODE_HEIGHT]);
    createNewNode = CreateSymbolNode(symbolAnimationConfig, symbolBounds, index,
        rsNode->canvasNodesListMap_[symbolId][index]);
    auto canvasNodeLine = std::static_pointer_cast<RSCanvasNode>(canvasNode->canvasNodesListMap_[symbolId][index]);
    if (canvasNodeLine == nullptr) {
        ROSEN_LOGE("HmSymbol canvasNode is nullptr");
        return false;
    }
    GroupDrawing(canvasNodeLine, symbolNode, offsets, false);
    // set animation to clip layer
    if (createNewNode) {
        std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
        std::shared_ptr<RSAnimatableProperty<Vector2f>> translateRatioProperty = nullptr;
        TranslateAnimationBase(canvasNode, translateRatioProperty, parameters[0], groupAnimation);

        std::vector<std::shared_ptr<RSAnimation>> groupAnimation1 = {};
        std::shared_ptr<RSAnimatableProperty<float>> alphaProperty = nullptr;
        AlphaAnimationBase(canvasNodeLine, alphaProperty, parameters[1], groupAnimation1);
        std::shared_ptr<RSAnimatableProperty<Vector2f>> clipProperty = nullptr;
        TranslateAnimationBase(canvasNodeLine, clipProperty, parameters[2], groupAnimation1);
    }
    return true;
}

void RSSymbolAnimation::SpliceAnimation(const std::shared_ptr<RSNode>& rsNode,
    const std::vector<Drawing::DrawingPiecewiseParameter>& parameters)
{
    if (rsNode == nullptr) {
        ROSEN_LOGD("RsNode is null, failed to SpliceAnimation.");
        return;
    }
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty = nullptr;
    std::shared_ptr<RSAnimatableProperty<float>> alphaProperty = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> translateProperty = nullptr;
    std::shared_ptr<RSAnimatableProperty<float>> blurProperty = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> clipProperty = nullptr;
    std::vector<std::shared_ptr<RSAnimation>> groupAnimation = {};
    SymbolBaseFuncMap funcMap = {
        {SCALE_PROP_X, [&, this](const Drawing::DrawingPiecewiseParameter& parameter)
            {
                ScaleAnimationBase(rsNode, scaleProperty, parameter, groupAnimation);
            }
        },
        {ALPHA_PROP, [&, this](const Drawing::DrawingPiecewiseParameter& parameter)
            {
                AlphaAnimationBase(rsNode, alphaProperty, parameter, groupAnimation);
            }
        },
        {TRANSLATE_PROP_X, [&, this](const Drawing::DrawingPiecewiseParameter& parameter)
            {
                TranslateAnimationBase(rsNode, translateProperty, parameter, groupAnimation);
            }
        },
        {BLUR_PROP, [&, this](const Drawing::DrawingPiecewiseParameter& parameter)
            {
                BlurAnimationBase(rsNode, blurProperty, parameter, groupAnimation);
            }
        },
        {CLIP_PROP, [&, this](const Drawing::DrawingPiecewiseParameter& parameter)
            {
                TranslateAnimationBase(rsNode, clipProperty, parameter, groupAnimation);
            }
        }
    };

    for (const auto& parameter : parameters) {
        for (const auto& pair : parameter.properties) {
            if (funcMap.count(pair.first) > 0) {
                funcMap[pair.first](parameter);
                break;
            }
        }
    }
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

void RSSymbolAnimation::UpdateSymbolGeometry(const std::shared_ptr<RSNode>& rsNode, const Vector4f& bounds)
{
    if (rsNode == nullptr) {
        return;
    }
    rsNode->SetBounds(bounds);
    rsNode->SetFrame(bounds);
}

void RSSymbolAnimation::GroupDrawing(const std::shared_ptr<RSCanvasNode>& canvasNode,
    TextEngine::SymbolNode& symbolNode, const Vector4f& offsets, bool isClip)
{
    // if there is mask layer, set the blendmode on the original node rsNode_
    if (symbolNode.isMask) {
        rsNode_->SetColorBlendMode(RSColorBlendMode::SRC_OVER);
        rsNode_->SetColorBlendApplyType(RSColorBlendApplyType::SAVE_LAYER_ALPHA);
    }

    // drawing a symbol or a path group
    auto recordingCanvas = canvasNode->BeginRecording(symbolNode.nodeBoundary[NODE_WIDTH],
                                                      symbolNode.nodeBoundary[NODE_HEIGHT]);
    if (isClip) {
        DrawClipOnCanvas(recordingCanvas, symbolNode, offsets);
    } else {
        DrawPathOnCanvas(recordingCanvas, symbolNode, offsets);
    }
    canvasNode->FinishRecording();
}

void RSSymbolAnimation::DrawPathOnCanvas(
    ExtendRecordingCanvas* recordingCanvas, TextEngine::SymbolNode& symbolNode, const Vector4f& offsets)
{
    if (recordingCanvas == nullptr) {
        return;
    }
    Drawing::Brush brush;
    Drawing::Pen pen;
    brush.SetAntiAlias(true);
    pen.SetAntiAlias(true);
    if (symbolNode.isMask) {
        brush.SetBlendMode(Drawing::BlendMode::CLEAR);
        pen.SetBlendMode(Drawing::BlendMode::CLEAR);
    }
    for (auto& pathInfo: symbolNode.pathsInfo) {
        SetIconProperty(brush, pen, pathInfo.color);
        pathInfo.path.Offset(offsets[2], offsets[3]); // index 2 offsetX 3 offsetY
        recordingCanvas->AttachBrush(brush);
        recordingCanvas->AttachPen(pen);
        recordingCanvas->DrawPath(pathInfo.path);
        recordingCanvas->DetachBrush();
        recordingCanvas->DetachPen();
    }
}

void RSSymbolAnimation::DrawClipOnCanvas(
    ExtendRecordingCanvas* recordingCanvas, TextEngine::SymbolNode& symbolNode, const Vector4f& offsets)
{
    if (recordingCanvas == nullptr) {
        return;
    }
    Drawing::Brush brush;
    Drawing::Pen pen;
    brush.SetAntiAlias(true);
    pen.SetAntiAlias(true);

    for (auto pathInfo: symbolNode.pathsInfo) {
        pathInfo.path.Offset(offsets[2], offsets[3]); // index 2 offsetX 3 offsetY
        recordingCanvas->AttachBrush(brush);
        recordingCanvas->AttachPen(pen);
        recordingCanvas->ClipPath(pathInfo.path, Drawing::ClipOp::INTERSECT, true);
        recordingCanvas->DetachBrush();
        recordingCanvas->DetachPen();
    }
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

bool RSSymbolAnimation::SetKeyframeAlphaAnimation(const std::shared_ptr<RSNode>& rsNode,
    std::vector<Drawing::DrawingPiecewiseParameter>& parameters,
    const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    if (rsNode == nullptr || symbolAnimationConfig == nullptr) {
        ROSEN_LOGD("HmSymbol SetVariableColorAnimation::getNode or get symbolAnimationConfig:failed");
        return false;
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

    if (symbolAnimationConfig->effectStrategy == Drawing::DrawingEffectStrategy::VARIABLE_COLOR &&
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
    // traverse all time stages
    for (size_t i = 0; i < oneGroupParas.size(); i++) {
        int interval = 0;
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
    timePercents.push_back(0); // the first property of timePercent
    for (int i = 0; i < static_cast<int>(oneGroupParas.size()) - 1; i++) {
        int interval = 0; // Interval between two animations
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

void RSSymbolAnimation::SetIconProperty(Drawing::Brush& brush, Drawing::Pen& pen, Drawing::DrawingSColor& color)
{
    brush.SetColor(Drawing::Color::ColorQuadSetARGB(0xFF, color.r, color.g, color.b));
    brush.SetAlphaF(color.a);
    brush.SetAntiAlias(true);

    pen.SetColor(Drawing::Color::ColorQuadSetARGB(0xFF, color.r, color.g, color.b));
    pen.SetAlphaF(color.a);
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

// base atomizated animation
void RSSymbolAnimation::ScaleAnimationBase(const std::shared_ptr<RSNode>& rsNode,
    std::shared_ptr<RSAnimatableProperty<Vector2f>>& scaleProperty,
    const Drawing::DrawingPiecewiseParameter& scaleParameter, std::vector<std::shared_ptr<RSAnimation>>& animations)
{
    const auto& properties = scaleParameter.properties;
    if (properties.count(SCALE_PROP_X) <= 0 || properties.count(SCALE_PROP_Y) <= 0 ||
        properties.at(SCALE_PROP_X).size() < PROPERTIES ||
        properties.at(SCALE_PROP_Y).size() < PROPERTIES) {
        ROSEN_LOGD("Invalid parameter input of scale.");
        return;
    }
    if (rsNode == nullptr) {
        ROSEN_LOGD("[%{public}s] : invalid input \n", __func__);
        return;
    }

    if (scaleProperty == nullptr) {
        SetNodePivot(rsNode);
        const Vector2f scaleValueBegin = {properties.at(SCALE_PROP_X).at(0), properties.at(SCALE_PROP_Y).at(0)};
        SymbolAnimation::CreateOrSetModifierValue(scaleProperty, scaleValueBegin);
        auto scaleModifier = std::make_shared<Rosen::RSScaleModifier>(scaleProperty);
        rsNode->AddModifier(scaleModifier);
    }

    const Vector2f scaleValueEnd = {properties.at(SCALE_PROP_X).at(PROP_END), properties.at(SCALE_PROP_Y).at(PROP_END)};

    // set animation curve and protocol
    RSAnimationTimingCurve scaleCurve;
    SymbolAnimation::CreateAnimationTimingCurve(scaleParameter.curveType, scaleParameter.curveArgs, scaleCurve);

    RSAnimationTimingProtocol scaleprotocol;
    scaleprotocol.SetStartDelay(scaleParameter.delay);
    if (scaleParameter.duration > 0) {
        scaleprotocol.SetDuration(scaleParameter.duration);
    }

    // set animation
    std::vector<std::shared_ptr<RSAnimation>> animations1 = RSNode::Animate(rsNode->GetRSUIContext(),
        scaleprotocol, scaleCurve, [&scaleProperty, &scaleValueEnd]() { scaleProperty->Set(scaleValueEnd); });

    if (animations1.size() > 0 && animations1[0] != nullptr) {
        animations.emplace_back(animations1[0]);
    }
}

void RSSymbolAnimation::AlphaAnimationBase(const std::shared_ptr<RSNode>& rsNode,
    std::shared_ptr<RSAnimatableProperty<float>>& alphaProperty,
    const Drawing::DrawingPiecewiseParameter& alphaParameter, std::vector<std::shared_ptr<RSAnimation>>& animations)
{
    // validation input
    if (rsNode == nullptr) {
        ROSEN_LOGD("[%{public}s] : invalid input", __func__);
        return;
    }
    const auto& properties = alphaParameter.properties;
    if (properties.count(ALPHA_PROP) <= 0 || properties.at(ALPHA_PROP).size() < PROPERTIES) {
        ROSEN_LOGD("Invalid parameter input of alpha.");
        return;
    }

    if (alphaProperty == nullptr) {
        float alphaValueBegin = static_cast<float>(properties.at(ALPHA_PROP).at(PROP_START));
        SymbolAnimation::CreateOrSetModifierValue(alphaProperty, alphaValueBegin);
        auto alphaModifier = std::make_shared<Rosen::RSAlphaModifier>(alphaProperty);
        rsNode->AddModifier(alphaModifier);
    }
    float alphaValueEnd = static_cast<float>(properties.at(ALPHA_PROP).at(PROP_END));

    // set animation protocol and curve
    RSAnimationTimingProtocol alphaProtocol;
    alphaProtocol.SetStartDelay(alphaParameter.delay);
    if (alphaParameter.duration > 0) {
        alphaProtocol.SetDuration(alphaParameter.duration);
    }
    RSAnimationTimingCurve alphaCurve;
    SymbolAnimation::CreateAnimationTimingCurve(alphaParameter.curveType, alphaParameter.curveArgs, alphaCurve);

    std::vector<std::shared_ptr<RSAnimation>> animations1 = RSNode::Animate(rsNode->GetRSUIContext(),
        alphaProtocol, alphaCurve, [&alphaProperty, &alphaValueEnd]() { alphaProperty->Set(alphaValueEnd); });

    if (animations1.size() > 0 && animations1[0] != nullptr) {
        animations.emplace_back(animations1[0]);
    }
}

void RSSymbolAnimation::TranslateAnimationBase(const std::shared_ptr<RSNode>& rsNode,
    std::shared_ptr<RSAnimatableProperty<Vector2f>>& property,
    const Drawing::DrawingPiecewiseParameter& parameter,
    std::vector<std::shared_ptr<RSAnimation>>& animations)
{
    const auto& properties = parameter.properties;
    bool isInvalid = !properties.count(TRANSLATE_PROP_X) || !properties.count(TRANSLATE_PROP_Y) ||
        properties.at(TRANSLATE_PROP_X).size() < PROPERTIES ||
        properties.at(TRANSLATE_PROP_Y).size() < PROPERTIES;
    if (isInvalid) {
        ROSEN_LOGD("Invalid parameter input of translate.");
        return;
    }

    if (property == nullptr) {
        const Vector2f valueBegin = {properties.at(TRANSLATE_PROP_X).at(0), properties.at(TRANSLATE_PROP_Y).at(0)};
        SymbolAnimation::CreateOrSetModifierValue(property, valueBegin);
        auto modifier = std::make_shared<Rosen::RSTranslateModifier>(property);
        rsNode->AddModifier(modifier);
    }

    const Vector2f valueEnd = {properties.at(TRANSLATE_PROP_X).at(PROP_END),
        properties.at(TRANSLATE_PROP_Y).at(PROP_END)};

    // set animation curve and protocol
    RSAnimationTimingCurve timeCurve;
    SymbolAnimation::CreateAnimationTimingCurve(parameter.curveType, parameter.curveArgs, timeCurve);

    RSAnimationTimingProtocol protocol;
    protocol.SetStartDelay(parameter.delay);
    if (parameter.duration > 0) {
        protocol.SetDuration(parameter.duration);
    }

    // set animation
    std::vector<std::shared_ptr<RSAnimation>> animations1 = RSNode::Animate(rsNode->GetRSUIContext(),
        protocol, timeCurve, [&property, &valueEnd]() { property->Set(valueEnd); });

    if (animations1.size() > 0 && animations1[0] != nullptr) {
        animations.emplace_back(animations1[0]);
    }
}

void RSSymbolAnimation::BlurAnimationBase(const std::shared_ptr<RSNode>& rsNode,
    std::shared_ptr<RSAnimatableProperty<float>>& property,
    const Drawing::DrawingPiecewiseParameter& parameter,
    std::vector<std::shared_ptr<RSAnimation>>& animations)
{
    const auto& properties = parameter.properties;
    bool isInValid = !properties.count(BLUR_PROP) || properties.at(BLUR_PROP).size() < PROPERTIES;
    if (isInValid) {
        ROSEN_LOGD("Invalid parameter input of blur.");
        return;
    }

    if (property == nullptr) {
        float valueBegin = properties.at(BLUR_PROP).at(PROP_START);
        SymbolAnimation::CreateOrSetModifierValue(property, valueBegin);
        auto modifier = std::make_shared<Rosen::RSForegroundEffectRadiusModifier>(property);
        rsNode->AddModifier(modifier);
    }

    float valueEnd = properties.at(BLUR_PROP).at(PROP_END);

    // set animation curve and protocol
    RSAnimationTimingCurve timeCurve;
    SymbolAnimation::CreateAnimationTimingCurve(parameter.curveType, parameter.curveArgs, timeCurve);

    RSAnimationTimingProtocol protocol;
    protocol.SetStartDelay(parameter.delay);
    if (parameter.duration > 0) {
        protocol.SetDuration(parameter.duration);
    }

    // set animation
    std::vector<std::shared_ptr<RSAnimation>> animations1 = RSNode::Animate(rsNode->GetRSUIContext(),
        protocol, timeCurve, [&property, &valueEnd]() { property->Set(valueEnd); });

    if (animations1.size() > 0 && animations1[0] != nullptr) {
        animations.emplace_back(animations1[0]);
    }
}
} // namespace Rosen
} // namespace OHOS