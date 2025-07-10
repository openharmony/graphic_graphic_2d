/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "hm_symbol_node_build.h"

#include "common/rs_common_def.h"
#include "include/pathops/SkPathOps.h"
#include "utils/text_log.h"

namespace OHOS {
namespace Rosen {
namespace SPText {

/**
 * @brief  Obtain the group id to layer
 * @param groupIds (output paramer) the index of groupIds if layer index, the groupIds[index] is the group index
 * @param renderGroup the renderGroup info of symbol
 * @param index the renderGroup index
 */
static void GetLayersGroupId(std::vector<size_t>& groupIds, const RSRenderGroup& renderGroup, size_t index)
{
    for (const auto& groupInfo : renderGroup.groupInfos) {
        for (size_t j : groupInfo.layerIndexes) {
            if (j < groupIds.size()) {
                groupIds[j] = index;
            }
        }
        for (size_t j : groupInfo.maskIndexes) {
            if (j < groupIds.size()) {
                groupIds[j] = index;
            }
        }
    }
}

/**
 * @brief Merge the mask paths.
 * @param maskIndexes the indexes of maskLayer of group informations
 * @param pathLayers the path data of layers on symbol, the value of maskIndexes < pathLayers.size
 * @param pathsColorIndex the start pathsColor position for merging mask pathh
 * @param pathsColor (output paramer) the result of the merge
 */
static void MergeMaskPath(const std::vector<size_t>& maskIndexes, std::vector<RSPath>& pathLayers,
    size_t pathsColorIndex, std::vector<TextEngine::NodeLayerInfo>& pathsColor)
{
    RSPath maskPath;
    for (size_t maskIndex : maskIndexes) {
        if (maskIndex < pathLayers.size()) {
            maskPath.AddPath(pathLayers[maskIndex]);
        }
    }
    if (!maskPath.IsValid()) {
        return;
    }

    for (size_t j = pathsColorIndex; j < pathsColor.size(); j++) {
        Drawing::Path outPath;
        bool isOk = outPath.Op(pathsColor[j].path, maskPath, Drawing::PathOp::DIFFERENCE);
        pathsColor[j].path = isOk ? outPath : pathsColor[j].path;
    }
}

/**
 * @brief Merge the paths of Group by layer color.
 * @param groupInfos a group informations of dynamic drawing
 * @param pathLayers the path data of layers on symbol
 * @param groupIndexes the indexes list of groups on colorGroups, the groupIndexes.size == pathLayers.size, \n
 * groupIndexes.size == pathLayers.size, groupIndexes[groupIndexes.size - 1] < colorGroups.size
 * @param pathsColor (output paramer) the result of the merge, where each merged path corresponds to a specific color
 * @param colorGroups the groups informations of static drawing, include color data
 */
static void MergePathByLayerColor(const std::vector<RSGroupInfo>& groupInfos,
    std::vector<RSPath>& pathLayers, const std::vector<size_t>& groupIndexes,
    std::vector<TextEngine::NodeLayerInfo>& pathsColor,
    std::vector<std::shared_ptr<SymbolGradient>>& colorGroups)
{
    size_t pathsColorIndex = 0;
    std::shared_ptr<SymbolGradient> color = nullptr;
    for (const auto& groupInfo : groupInfos) {
        TextEngine::NodeLayerInfo tempLayer;
        size_t currentIndex = 0; // the current layerindex, that effective index of tempLayer
        bool isFirst = true;
        for (size_t layerIndex: groupInfo.layerIndexes) {
            if (layerIndex >= pathLayers.size() || layerIndex >= groupIndexes.size()) {
                continue;
            }
            if (isFirst) { // initialize tempLayer
                auto groupIndex = groupIndexes[layerIndex];
                tempLayer.color = groupIndex < colorGroups.size() ? colorGroups[groupIndex] : color;
                tempLayer.path.AddPath(pathLayers[layerIndex]);
                currentIndex = layerIndex;
                isFirst = false;
                continue;
            }
            // If the groupIndex of two paths is different, update pathsColor and tempLayer
            if (groupIndexes[currentIndex] != groupIndexes[layerIndex]) {
                pathsColor.push_back(tempLayer);
                tempLayer.path.Reset();
                auto groupIndex = groupIndexes[layerIndex];
                tempLayer.color = groupIndex < colorGroups.size() ? colorGroups[groupIndex] : color;
                currentIndex = layerIndex;
            }
            tempLayer.path.AddPath(pathLayers[layerIndex]);
        }
        pathsColor.push_back(tempLayer);
        if (!groupInfo.maskIndexes.empty()) {
            MergeMaskPath(groupInfo.maskIndexes, pathLayers, pathsColorIndex, pathsColor);
        }
        pathsColorIndex = pathsColor.size();
    }
}

/**
 * @brief check if the symbol group is a mask layer, that it has only mask indexes
 * @param multPath (output paramer) combine all paths of mask indexes in groupInfos
 * @param groupInfos the groupsInfos of symbol
 * @param pathLayers the all paths of symbol
 * @return true if the group is a mask layer
 */
static bool IsMaskLayer(RSPath& multPath, const std::vector<RSGroupInfo>& groupInfos, std::vector<RSPath>& pathLayers)
{
    for (const auto& groupInfo : groupInfos) {
        RSPath pathTemp;
        if (groupInfo.layerIndexes.size() > 0) {
            return false;
        }
        for (auto k : groupInfo.maskIndexes) {
            if (k >= pathLayers.size()) {
                continue;
            }
            pathTemp.AddPath(pathLayers[k]);
        }
        multPath.AddPath(pathTemp);
    }
    return true;
}

void SymbolNodeBuild::MergeDrawingPath(RSPath& multPath, const RSRenderGroup& group, std::vector<RSPath>& pathLayers)
{
    for (const auto& groupInfo : group.groupInfos) {
        RSPath pathTemp;
        for (auto i : groupInfo.layerIndexes) {
            if (i < pathLayers.size()) {
                pathTemp.AddPath(pathLayers[i]);
            }
        }
        RSPath maskPath;
        for (auto j : groupInfo.maskIndexes) {
            if (j < pathLayers.size()) {
                maskPath.AddPath(pathLayers[j]);
            }
        }
        if (maskPath.IsValid()) {
            Drawing::Path outPath;
            auto isOk = outPath.Op(pathTemp, maskPath, Drawing::PathOp::DIFFERENCE);
            pathTemp = isOk ? outPath : pathTemp;
            multPath.SetFillStyle(pathTemp.GetFillStyle());
        }
        multPath.AddPath(pathTemp);
    }
}

SymbolNodeBuild::SymbolNodeBuild(const RSAnimationSetting &animationSetting, const RSHMSymbolData &symbolData,
    const RSEffectStrategy &effectStrategy,
    const std::pair<float, float> &offset) : animationSetting_(animationSetting),
    symbolData_(symbolData), effectStrategy_(effectStrategy),
    offsetX_(offset.first), offsetY_(offset.second) {}

void SymbolNodeBuild::AddWholeAnimation(const RSHMSymbolData &symbolData, const Vector4f &nodeBounds,
    std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig)
{
    TextEngine::SymbolNode symbolNode;
    symbolNode.symbolData = symbolData;
    symbolNode.nodeBoundary = nodeBounds;

    std::vector<RSPath> paths;
    RSHMSymbol::PathOutlineDecompose(symbolData.path_, paths);
    std::vector<RSPath> pathLayers;
    RSHMSymbol::MultilayerPath(symbolData.symbolInfo_.layers, paths, pathLayers);
    std::vector<RSRenderGroup> groups = symbolData.symbolInfo_.renderGroups;
    UpdateGradient(groups, pathLayers, symbolData.path_);
    TEXT_LOGD("Render group size %{public}zu", groups.size());
    for (size_t i = 0; i < groups.size(); i++) {
        RSPath multPath;
        MergeDrawingPath(multPath, groups[i], pathLayers);
        TextEngine::NodeLayerInfo pathInfo;
        pathInfo.path = multPath;
        bool isvalid = i < gradients_.size() && gradients_[i] != nullptr;
        if (isvalid) {
            pathInfo.color = gradients_[i];
        }
        symbolNode.pathsInfo.push_back(pathInfo);
    }

    symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
}

void SymbolNodeBuild::SetSymbolNodeColors(const TextEngine::SymbolNode& symbolNode,
    TextEngine::SymbolNode& outSymbolNode)
{
    bool isNoNeed = symbolNode.pathsInfo.empty() || outSymbolNode.pathsInfo.empty() ||
        outSymbolNode.pathsInfo[0].color; // If color != nullptr is no need to set
    if (isNoNeed) {
        return;
    }

    std::shared_ptr<SymbolGradient> color;
    if (disableSlashColor_ != nullptr) {
        disableSlashColor_->Make(outSymbolNode.pathsInfo[0].path.GetBounds());
        color = disableSlashColor_;
    } else {
        color = symbolNode.pathsInfo[0].color;
    }
    for (auto& pathInfo: outSymbolNode.pathsInfo) {
        pathInfo.color = color;
    }
}

void SymbolNodeBuild::AddHierarchicalAnimation(const RSHMSymbolData &symbolData, const Vector4f &nodeBounds,
    const std::vector<RSGroupSetting> &groupSettings,
    std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig)
{
    if (symbolAnimationConfig == nullptr) {
        symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    }
    std::vector<RSPath> paths;
    RSHMSymbol::PathOutlineDecompose(symbolData.path_, paths);
    std::vector<RSPath> pathLayers;
    RSHMSymbol::MultilayerPath(symbolData.symbolInfo_.layers, paths, pathLayers);
    UpdateGradient(symbolData.symbolInfo_.renderGroups, pathLayers, symbolData.path_);

    // Obtain the group id of layer
    std::vector<size_t> groupIds(pathLayers.size(), pathLayers.size());
    for (size_t idx = 0; idx < symbolData.symbolInfo_.renderGroups.size(); idx++) {
        GetLayersGroupId(groupIds, symbolData.symbolInfo_.renderGroups[idx], idx);
    }

    // Splitting animation nodes information
    for (const auto& groupSetting: groupSettings) {
        TextEngine::SymbolNode symbolNode;
        TextEngine::NodeLayerInfo maskPath;
        bool isMask = IsMaskLayer(maskPath.path, groupSetting.groupInfos, pathLayers);
        if (!isMask) {
            MergePathByLayerColor(groupSetting.groupInfos, pathLayers, groupIds, symbolNode.pathsInfo,
                gradients_);
        } else {
            symbolNode.pathsInfo.push_back(maskPath);
        }
        symbolNode.nodeBoundary = nodeBounds;
        symbolNode.symbolData = symbolData;
        symbolNode.animationIndex = groupSetting.animationIndex;
        symbolNode.isMask = isMask;
        symbolAnimationConfig->symbolNodes.push_back(symbolNode);
    }
    bool isDisableType = (effectStrategy_ == RSEffectStrategy::DISABLE) && (
        symbolAnimationConfig->symbolNodes.size() > 2); // 2: disableType symbol nodes size must be greater than 2
    if (isDisableType) {
        uint32_t last = symbolAnimationConfig->symbolNodes.size() - 1; // the last
        uint32_t last3 = symbolAnimationConfig->symbolNodes.size() - 3; // 3: the third from the bottom
        SetSymbolNodeColors(symbolAnimationConfig->symbolNodes[last3], symbolAnimationConfig->symbolNodes[last]);
    }
    symbolAnimationConfig->numNodes = symbolAnimationConfig->symbolNodes.size();
}

std::shared_ptr<SymbolGradient> SymbolNodeBuild::CreateGradient(
    const std::shared_ptr<SymbolGradient>& gradient)
{
    if (gradient == nullptr) {
        return nullptr;
    }

    std::shared_ptr<SymbolGradient> outGradient = nullptr;
    if (gradient->GetGradientType() == GradientType::LINE_GRADIENT) {
        auto lineGradient = std::static_pointer_cast<SymbolLineGradient>(gradient);
        outGradient = std::make_shared<SymbolLineGradient>(lineGradient->GetAngle());
    } else if (gradient->GetGradientType() == GradientType::RADIAL_GRADIENT) {
        auto radiaGradient = std::static_pointer_cast<SymbolRadialGradient>(gradient);
        auto newRadiaGradient = std::make_shared<SymbolRadialGradient>(radiaGradient->GetCenterPoint(),
            radiaGradient->GetRadiusRatio());
        if (!radiaGradient->IsRadiusRatio()) {
            newRadiaGradient->SetRadius(radiaGradient->GetRadius());
        }
        outGradient = newRadiaGradient;
    } else {
        outGradient = std::make_shared<SymbolGradient>();
    }

    outGradient->SetTileMode(gradient->GetTileMode());
    outGradient->SetColors(gradient->GetColors());
    outGradient->SetPositions(gradient->GetPositions());

    return outGradient;
}

void SymbolNodeBuild::UpdateGradient(const std::vector<RSRenderGroup>& groups,
    std::vector<RSPath>& pathLayers, const RSPath& path)
{
    if (gradients_.empty()) {
        return;
    }

    std::vector<std::shared_ptr<SymbolGradient>> gradients;
    if (renderMode_ == RSSymbolRenderingStrategy::SINGLE) {
        if (gradients_[0] != nullptr) {
            gradients_[0]->Make(path.GetBounds());
        }
        for (size_t i = 0; i < groups.size(); i++) {
            gradients.push_back(gradients_[0]);
        }
        gradients_ = gradients;
        return;
    }

    for (size_t i = 0; i < groups.size() && i < gradients_.size(); i++) {
        RSPath multPath;
        MergeDrawingPath(multPath, groups[i], pathLayers);
        auto gradient = gradients_[i];
        if (gradient != nullptr) {
            gradient->Make(multPath.GetBounds());
            gradients.push_back(gradient);
        }
    }
    gradients_ = gradients;
}

void SymbolNodeBuild::ClearAnimation()
{
    if (animationFunc_ == nullptr) {
        return;
    }
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    if (symbolAnimationConfig == nullptr) {
        return;
    }
    symbolAnimationConfig->effectStrategy = Drawing::DrawingEffectStrategy::NONE;
    symbolAnimationConfig->symbolSpanId = symblSpanId_;
    animationFunc_(symbolAnimationConfig);
}

bool SymbolNodeBuild::DecomposeSymbolAndDraw()
{
    if (symbolData_.symbolInfo_.renderGroups.empty()) {
        TEXT_LOGD("Render group is empty");
        return false;
    }
    if (animationFunc_ == nullptr) {
        TEXT_LOGD("Null animation func");
        return false;
    }
    auto symbolAnimationConfig = std::make_shared<TextEngine::SymbolAnimationConfig>();
    if (symbolAnimationConfig == nullptr) {
        return false;
    }
    auto rect = symbolData_.path_.GetBounds();
    float nodeWidth = rect.GetWidth();
    float nodeHeight = rect.GetHeight();
    Vector4f nodeBounds = Vector4f(offsetX_, offsetY_, nodeWidth, nodeHeight);

    if (effectStrategy_ == RSEffectStrategy::VARIABLE_COLOR || animationMode_ == 0) {
        AddHierarchicalAnimation(symbolData_, nodeBounds, animationSetting_.groupSettings, symbolAnimationConfig);
    } else {
        AddWholeAnimation(symbolData_, nodeBounds, symbolAnimationConfig);
    }

    symbolAnimationConfig->effectStrategy = effectStrategy_;
    symbolAnimationConfig->repeatCount = repeatCount_;
    symbolAnimationConfig->animationMode = animationMode_;
    symbolAnimationConfig->animationStart = animationStart_;
    symbolAnimationConfig->symbolSpanId = symblSpanId_;
    symbolAnimationConfig->commonSubType = commonSubType_;
    symbolAnimationConfig->currentAnimationHasPlayed = currentAnimationHasPlayed_;
    symbolAnimationConfig->slope = slope_;
    symbolAnimationConfig->symbolShadow = symbolShadow_;
    return animationFunc_(symbolAnimationConfig);
}
}
}
}