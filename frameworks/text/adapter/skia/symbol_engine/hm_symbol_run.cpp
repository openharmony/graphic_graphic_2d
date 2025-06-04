/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#include "hm_symbol_run.h"
#include "custom_symbol_config.h"
#include "draw/path.h"
#include "hm_symbol_node_build.h"
#include "include/pathops/SkPathOps.h"
#include "utils/text_log.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
static const std::vector<RSEffectStrategy> COMMON_ANIMATION_TYPES = {
    RSEffectStrategy::SCALE, RSEffectStrategy::APPEAR, RSEffectStrategy::DISAPPEAR,
    RSEffectStrategy::BOUNCE, RSEffectStrategy::REPLACE_APPEAR};

HMSymbolRun::HMSymbolRun(uint64_t symbolId,
    const HMSymbolTxt& symbolTxt,
    const std::shared_ptr<RSTextBlob>& textBlob,
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& animationFunc)
    : symbolTxt_(symbolTxt), symbolId_(symbolId)
{
    if (textBlob) {
        textBlob_ = textBlob;
    } else {
        TEXT_LOGD("Null text blob");
    }
    if (animationFunc) {
        animationFunc_ = animationFunc;
    } else {
        TEXT_LOGD("Null animation func");
    }
}

RSSymbolLayers HMSymbolRun::GetSymbolLayers(uint16_t glyphId, const HMSymbolTxt& symbolText)
{
    RSSymbolLayers symbolInfo;
    symbolInfo.symbolGlyphId = glyphId;
    auto& symbolInfoOrign = symbolLayersGroups_;
    if (symbolInfoOrign.renderModeGroups.empty() || symbolInfoOrign.symbolGlyphId == 0) {
        TEXT_LOGD("Invalid symbol layer groups, glyph id %{public}hu.", glyphId);
        return symbolInfo;
    }

    RSSymbolRenderingStrategy renderMode = symbolText.GetRenderMode();
    if (symbolInfoOrign.renderModeGroups.find(renderMode) == symbolInfoOrign.renderModeGroups.end()) {
        renderMode = RSSymbolRenderingStrategy::SINGLE;
    }

    symbolInfo.layers = symbolInfoOrign.layers;
    if (symbolInfoOrign.renderModeGroups.find(renderMode) != symbolInfoOrign.renderModeGroups.end()) {
        symbolInfo.renderGroups = symbolInfoOrign.renderModeGroups[renderMode];
        symbolInfo.symbolGlyphId = symbolInfoOrign.symbolGlyphId;
    }

    std::vector<RSSColor> colorList = symbolText.GetRenderColor();
    if (!colorList.empty()) {
        SetSymbolRenderColor(renderMode, colorList, symbolInfo);
    }
    return symbolInfo;
}

void HMSymbolRun::SetSymbolRenderColor(const RSSymbolRenderingStrategy& renderMode,
    const std::vector<RSSColor>& colors, RSSymbolLayers& symbolInfo)
{
    if (colors.empty()) {
        return;
    }
    switch (renderMode) {
        // SINGLE and HIERARCHICAL: Supports single color setting
        case RSSymbolRenderingStrategy::SINGLE:
            for (size_t i = 0; i < symbolInfo.renderGroups.size(); ++i) {
                symbolInfo.renderGroups[i].color = colors[0]; // the 0 indicates the the first color is used
            }
            break;
        // MULTIPLE_OPACITY: Supports rgb replace and alphia overlay setting by the first color
        case RSSymbolRenderingStrategy::MULTIPLE_OPACITY:
            for (size_t i = 0; i < symbolInfo.renderGroups.size(); ++i) {
                float colorAlphia = symbolInfo.renderGroups[i].color.a * colors[0].a;
                symbolInfo.renderGroups[i].color.a = std::clamp(colorAlphia, 0.0f, 1.0f); // 0.0: min, 1.0: max
                symbolInfo.renderGroups[i].color.r = colors[0].r; // the 0 indicates the the first color is used
                symbolInfo.renderGroups[i].color.g = colors[0].g; // the 0 indicates the the first color is used
                symbolInfo.renderGroups[i].color.b = colors[0].b; // the 0 indicates the the first color is used
            }
            break;
        // MULTIPLE_COLOR: Supports mutiple color setting
        case RSSymbolRenderingStrategy::MULTIPLE_COLOR:
            for (size_t i = 0; i < symbolInfo.renderGroups.size() && i < colors.size(); ++i) {
                symbolInfo.renderGroups[i].color = colors[i];
            }
            break;
        default:
            break;
    }
}

void HMSymbolRun::UpdateSymbolLayersGroups(uint16_t glyphId)
{
    symbolLayersGroups_.symbolGlyphId = glyphId;
    // Obtaining Symbol Preset LayerGroups Parameters
    if (symbolTxt_.GetSymbolType() == SymbolType::SYSTEM) {
        auto groups = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(glyphId);
        if (groups.renderModeGroups.empty()) {
            TEXT_LOGD("Failed to get system symbol layer groups, glyph id %{public}hu", glyphId);
            symbolLayersGroups_.renderModeGroups = {};
            return;
        }
        symbolLayersGroups_ = groups;
    } else {
        auto groups = CustomSymbolConfig::GetInstance()->GetSymbolLayersGroups(symbolTxt_.familyName_, glyphId);
        if (!groups.has_value()) {
            TEXT_LOGD("Failed to get custom symbol layer groups, glyph id %{public}hu", glyphId);
            symbolLayersGroups_.renderModeGroups = {};
            return;
        }
        symbolLayersGroups_ = groups.value();
    }
}

void HMSymbolRun::DrawSymbol(RSCanvas* canvas, const RSPoint& offset)
{
    if (!textBlob_) {
        TEXT_LOGD("Null text blob");
        return;
    }

    if (!canvas) {
        TEXT_LOGD("Null canvas");
        return;
    }

    std::vector<uint16_t> glyphIds;
    RSTextBlob::GetDrawingGlyphIDforTextBlob(textBlob_.get(), glyphIds);
    if (glyphIds.size() != 1) {
        TEXT_LOGD("Glyph isn't unique");
        canvas->DrawTextBlob(textBlob_.get(), offset.GetX(), offset.GetY());
        return;
    }
    TEXT_LOGD("HmSymbol: DrawSymbol");
    uint16_t glyphId = glyphIds[0];
    OHOS::Rosen::Drawing::Path path = RSTextBlob::GetDrawingPathforTextBlob(glyphId, textBlob_.get());
    RSHMSymbolData symbolData;

    UpdateSymbolLayersGroups(glyphId);
    symbolData.symbolInfo_ = GetSymbolLayers(glyphId, symbolTxt_);
    if (symbolData.symbolInfo_.symbolGlyphId != glyphId) {
        path = RSTextBlob::GetDrawingPathforTextBlob(symbolData.symbolInfo_.symbolGlyphId, textBlob_.get());
    }
    symbolData.path_ = path;
    symbolData.symbolId = symbolId_; // symbolspan Id in paragraph
    RSEffectStrategy symbolEffect = symbolTxt_.GetEffectStrategy();
    std::pair<float, float> offsetXY(offset.GetX(), offset.GetY());
    if (symbolEffect > 0 && symbolTxt_.GetAnimationStart()) { // 0 > has animation
        if (SymbolAnimation(symbolData, offsetXY)) {
            currentAnimationHasPlayed_ = true;
            return;
        }
    }
    ClearSymbolAnimation(symbolData, offsetXY);
    OnDrawSymbol(canvas, symbolData, offset);
    currentAnimationHasPlayed_ = false;
}

void HMSymbolRun::SetRenderColor(const std::vector<RSSColor>& colorList)
{
    symbolTxt_.SetRenderColor(colorList);
}

void HMSymbolRun::SetRenderMode(RSSymbolRenderingStrategy renderMode)
{
    symbolTxt_.SetRenderMode(renderMode);
}

void HMSymbolRun::SetSymbolEffect(const RSEffectStrategy& effectStrategy)
{
    symbolTxt_.SetSymbolEffect(effectStrategy);
    currentAnimationHasPlayed_ = false;
}

void HMSymbolRun::SetAnimationMode(uint16_t animationMode)
{
    symbolTxt_.SetAnimationMode(animationMode);
    currentAnimationHasPlayed_ = false;
}

void HMSymbolRun::SetAnimationStart(bool animationStart)
{
    symbolTxt_.SetAnimationStart(animationStart);
}

void HMSymbolRun::SetCommonSubType(Drawing::DrawingCommonSubType commonSubType)
{
    symbolTxt_.SetCommonSubType(commonSubType);
    currentAnimationHasPlayed_ = false;
}

void HMSymbolRun::SetAnimation(
    const std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
    animationFunc)
{
    if (animationFunc) {
        animationFunc_ = animationFunc;
    }
}

void HMSymbolRun::SetTextBlob(const std::shared_ptr<RSTextBlob>& textBlob)
{
    if (textBlob) {
        std::vector<uint16_t> glyphId1;
        std::vector<uint16_t> glyphId2;
        RSTextBlob::GetDrawingGlyphIDforTextBlob(textBlob_.get(), glyphId1);
        RSTextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphId2);
        // 1 mean the textBlob has one symbol
        if (!(glyphId1.size() == 1 && glyphId2.size() == 1 && glyphId1[0] == glyphId2[0])) {
            currentAnimationHasPlayed_ = false;
        }
        textBlob_ = textBlob;
    }
}

void HMSymbolRun::OnDrawSymbol(RSCanvas* canvas, const RSHMSymbolData& symbolData, RSPoint locate)
{
    RSPath path(symbolData.path_);

    // 1.0 move path
    path.Offset(locate.GetX(), locate.GetY());
    if (symbolData.symbolInfo_.renderGroups.empty()) {
        TEXT_LOGD("Empty render groups");
        canvas->DrawPath(path);
        return;
    }

    // 2.0 split path
    std::vector<RSPath> paths;
    RSHMSymbol::PathOutlineDecompose(path, paths);
    std::vector<RSPath> pathLayers;
    RSHMSymbol::MultilayerPath(symbolData.symbolInfo_.layers, paths, pathLayers);

    // 3.0 set paint
    Drawing::Brush brush;
    Drawing::Pen pen;
    brush.SetAntiAlias(true);
    pen.SetAntiAlias(true);

    // draw path
    std::vector<RSRenderGroup> groups = symbolData.symbolInfo_.renderGroups;
    TEXT_LOGD("RenderGroup size %{public}zu", groups.size());
    RSColor color;
    for (auto group : groups) {
        RSPath multPath;
        SymbolNodeBuild::MergeDrawingPath(multPath, group, pathLayers);
        // set color
        color.SetRgb(group.color.r, group.color.g, group.color.b);
        color.SetAlphaF(group.color.a);
        brush.SetColor(color);
        pen.SetColor(color);
        canvas->AttachPen(pen);
        canvas->AttachBrush(brush);
        canvas->DrawPath(multPath);
        canvas->DetachPen();
        canvas->DetachBrush();
    }
}

bool HMSymbolRun::SymbolAnimation(const RSHMSymbolData& symbol, const std::pair<float, float>& offset)
{
    RSEffectStrategy effectMode = symbolTxt_.GetEffectStrategy();
    uint16_t animationMode = symbolTxt_.GetAnimationMode();
    if (effectMode == RSEffectStrategy::NONE) {
        TEXT_LOGD("Invalid effect mode");
        return false;
    }
    RSAnimationSetting animationSetting;
    if (animationMode == 0 || effectMode == RSEffectStrategy::VARIABLE_COLOR) {
        if (!GetAnimationGroups(effectMode, animationSetting)) {
            TEXT_LOGD("Invalid animation setting");
            return false;
        }

        if (std::find(COMMON_ANIMATION_TYPES.begin(), COMMON_ANIMATION_TYPES.end(), effectMode) !=
            COMMON_ANIMATION_TYPES.end() && animationSetting.groupSettings.size() == 1) {
            animationMode = 1; // the 1 is wholeSymbol effect
        }
    }
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSetting, symbol, effectMode, offset);
    symbolNode.SetAnimation(animationFunc_);
    symbolNode.SetSymbolId(symbolId_);
    symbolNode.SetAnimationMode(animationMode);
    symbolNode.SetRepeatCount(symbolTxt_.GetRepeatCount());
    symbolNode.SetAnimationStart(symbolTxt_.GetAnimationStart());
    symbolNode.SetCommonSubType(symbolTxt_.GetCommonSubType());
    symbolNode.SetCurrentAnimationHasPlayed(currentAnimationHasPlayed_);
    symbolNode.SetSlope(animationSetting.slope);
    if (effectMode == RSEffectStrategy::DISABLE) {
        symbolNode.SetCommonSubType(animationSetting.commonSubType);
    }
    return symbolNode.DecomposeSymbolAndDraw();
}

void HMSymbolRun::ClearSymbolAnimation(const RSHMSymbolData& symbol, const std::pair<float, float>& offset)
{
    auto effectMode = RSEffectStrategy::NONE;
    RSAnimationSetting animationSetting;

    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSetting, symbol, effectMode, offset);
    symbolNode.SetAnimation(animationFunc_);
    symbolNode.SetSymbolId(symbolId_);
    symbolNode.ClearAnimation();
}

bool HMSymbolRun::GetAnimationGroups(const RSEffectStrategy effectStrategy,
    RSAnimationSetting& animationOut)
{
    RSAnimationType animationType = static_cast<RSAnimationType>(effectStrategy);

    for (const auto& animationSetting : symbolLayersGroups_.animationSettings) {
        if (std::find(animationSetting.animationTypes.begin(), animationSetting.animationTypes.end(),
            animationType) == animationSetting.animationTypes.end()) {
            continue;
        }
        if (!animationSetting.groupSettings.empty()) {
            animationOut = animationSetting;
            return true;
        }
    }
    return false;
}
}
}
}