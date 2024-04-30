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
#include "draw/path.h"
#include "hm_symbol_node_build.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
static const std::vector<RSEffectStrategy> COMMON_ANIMATION_TYPES = {
    RSEffectStrategy::SCALE, RSEffectStrategy::APPEAR, RSEffectStrategy::DISAPPEAR,
    RSEffectStrategy::BOUNCE, RSEffectStrategy::REPLACE_APPEAR};

RSSymbolLayers HMSymbolRun::GetSymbolLayers(const uint16_t& glyphId, const HMSymbolTxt& symbolText)
{
    RSSymbolLayers symbolInfo;
    symbolInfo.symbolGlyphId = glyphId;
    uint32_t symbolId = static_cast<uint32_t>(glyphId);
    RSSymbolLayersGroups symbolInfoOrign = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(symbolId);
    if (symbolInfoOrign.renderModeGroups.empty() || symbolInfoOrign.symbolGlyphId == 0) {
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
        case RSSymbolRenderingStrategy::MULTIPLE_OPACITY:
            for (size_t i = 0; i < symbolInfo.renderGroups.size(); ++i) {
                symbolInfo.renderGroups[i].color.r = colors[0].r; // the 0 indicates the the first color is used
                symbolInfo.renderGroups[i].color.g = colors[0].g; // the 0 indicates the the first color is used
                symbolInfo.renderGroups[i].color.b = colors[0].b; // the 0 indicates the the first color is used
            }
            break;
        // MULTIPLE_COLOR: Supports mutiple color setting
        case RSSymbolRenderingStrategy::MULTIPLE_COLOR:
            for (size_t i = 0, j = 0; i < symbolInfo.renderGroups.size() && j < colors.size(); ++i, ++j) {
                symbolInfo.renderGroups[i].color.r = colors[j].r;
                symbolInfo.renderGroups[i].color.g = colors[j].g;
                symbolInfo.renderGroups[i].color.b = colors[j].b;
            }
            break;
        default:
            break;
    }
}

void HMSymbolRun::DrawSymbol(RSCanvas* canvas, RSTextBlob* blob, const RSPoint& offset, const HMSymbolTxt &symbolTxt)
{
    if (canvas == nullptr || blob == nullptr) {
        return;
    }

    std::vector<uint16_t> glyphIds;
    RSTextBlob::GetDrawingGlyphIDforTextBlob(blob, glyphIds);
    if (glyphIds.size() == 1) {
        uint16_t glyphId = glyphIds[0];
        OHOS::Rosen::Drawing::Path path = RSTextBlob::GetDrawingPathforTextBlob(glyphId, blob);
        RSHMSymbolData symbolData;

        symbolData.symbolInfo_ = GetSymbolLayers(glyphId, symbolTxt);
        if (symbolData.symbolInfo_.symbolGlyphId != glyphId) {
            path = RSTextBlob::GetDrawingPathforTextBlob(symbolData.symbolInfo_.symbolGlyphId, blob);
        }
        symbolData.path_ = path;
        symbolData.symbolId = symbolId_; // symbolspan Id in paragraph
        RSEffectStrategy symbolEffect = symbolTxt.GetEffectStrategy();
        uint32_t symbolId = static_cast<uint32_t>(glyphId);
        std::pair<double, double> offsetXY(offset.GetX(), offset.GetY());
        if (symbolEffect > 0 && symbolTxt.GetAnimationStart()) { // 0 > has animation
            if (!SymbolAnimation(symbolData, symbolId, offsetXY, symbolTxt)) {
                ClearSymbolAnimation(symbolData, symbolId, offsetXY);
                canvas->DrawSymbol(symbolData, offset);
            }
        } else {
            ClearSymbolAnimation(symbolData, symbolId, offsetXY);
            canvas->DrawSymbol(symbolData, offset);
        }
    } else {
        canvas->DrawTextBlob(blob, offset.GetX(), offset.GetY());
    }
}

bool HMSymbolRun::SymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyphid,
    const std::pair<double, double> offset, const HMSymbolTxt& symbolTxt)
{
    RSEffectStrategy effectMode = symbolTxt.GetEffectStrategy();
    uint16_t animationMode = symbolTxt.GetAnimationMode();
    if (effectMode == RSEffectStrategy::NONE) {
        return false;
    }
    RSAnimationSetting animationSetting;
    if (animationMode == 0 || effectMode == RSEffectStrategy::VARIABLE_COLOR) {
        if (!GetAnimationGroups(glyphid, effectMode, animationSetting)) {
            return false;
        }

        if (std::count(COMMON_ANIMATION_TYPES.begin(), COMMON_ANIMATION_TYPES.end(), effectMode) != 0 &&
            animationSetting.groupSettings.size() == 1) {
            animationMode = 1; // the 1 is wholeSymbol effect
        }
    }
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSetting, symbol, effectMode, offset);
    symbolNode.SetAnimation(animationFunc_);
    symbolNode.SetSymbolId(symbolId_);
    symbolNode.SetAnimationMode(animationMode);
    symbolNode.SetRepeatCount(symbolTxt.GetRepeatCount());
    symbolNode.SetAnimationStart(symbolTxt.GetAnimationStart());
    symbolNode.SetCommonSubType(symbolTxt.GetCommonSubType());
    if (!symbolNode.DecomposeSymbolAndDraw()) {
        return false;
    }
    return true;
}

void HMSymbolRun::ClearSymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyphid,
        const std::pair<double, double> offset)
{
    auto effectMode = RSEffectStrategy::NONE;
    RSAnimationSetting animationSetting;

    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSetting, symbol, effectMode, offset);
    symbolNode.SetAnimation(animationFunc_);
    symbolNode.SetSymbolId(symbolId_);
    symbolNode.ClearAnimation();
}

bool HMSymbolRun::GetAnimationGroups(const uint32_t glyphid, const RSEffectStrategy effectStrategy,
    RSAnimationSetting& animationOut)
{
    auto symbolInfoOrigin = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(glyphid);
    std::vector<RSAnimationSetting> animationSettings = symbolInfoOrigin.animationSettings;
    RSAnimationType animationType = static_cast<RSAnimationType>(effectStrategy);

    for (size_t i = 0; i < animationSettings.size(); i++) {
        if (std::find(animationSettings[i].animationTypes.begin(), animationSettings[i].animationTypes.end(),
            animationType) == animationSettings[i].animationTypes.end()) {
            continue;
        }
        if (animationSettings[i].groupSettings.size() > 0) {
            animationOut = animationSettings[i];
            return true;
        }
    }
    return false;
}
}
}
}