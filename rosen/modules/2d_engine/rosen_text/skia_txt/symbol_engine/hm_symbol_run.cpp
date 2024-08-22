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
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
static const std::vector<RSEffectStrategy> COMMON_ANIMATION_TYPES = {
    RSEffectStrategy::SCALE, RSEffectStrategy::APPEAR, RSEffectStrategy::DISAPPEAR,
    RSEffectStrategy::BOUNCE, RSEffectStrategy::REPLACE_APPEAR};

RSSymbolLayers HMSymbolRun::GetSymbolLayers(uint16_t glyphId, const HMSymbolTxt& symbolText)
{
    RSSymbolLayers symbolInfo;
    symbolInfo.symbolGlyphId = glyphId;
    RSSymbolLayersGroups symbolInfoOrign = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(glyphId);
    if (symbolInfoOrign.renderModeGroups.empty() || symbolInfoOrign.symbolGlyphId == 0) {
        LOGD("[%{public}s] HmSymbol: GetSymbolLayersGroups of graphId %{public}d failed\n",
            __func__, std::static_cast<int>(glyphId));
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

void HMSymbolRun::DrawSymbol(RSCanvas* canvas, RSTextBlob* blob, const RSPoint& offset, const HMSymbolTxt& symbolTxt)
{
    if (canvas == nullptr || blob == nullptr) {
        LOGD("[%{public}s] HmSymbol: the canvas or textBlob is nullptr\n", __func__);
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
        std::pair<float, float> offsetXY(offset.GetX(), offset.GetY());
        if (symbolEffect > 0 && symbolTxt.GetAnimationStart()) { // 0 > has animation
            if (!SymbolAnimation(symbolData, glyphId, offsetXY, symbolTxt)) {
                ClearSymbolAnimation(symbolData, offsetXY);
                canvas->DrawSymbol(symbolData, offset);
            }
        } else {
            ClearSymbolAnimation(symbolData, offsetXY);
            canvas->DrawSymbol(symbolData, offset);
        }
    } else {
        LOGD("[%{public}s] HmSymbol: the glyphIds is > 1\n", __func__);
        canvas->DrawTextBlob(blob, offset.GetX(), offset.GetY());
    }
}

bool HMSymbolRun::SymbolAnimation(const RSHMSymbolData& symbol, uint16_t glyphid,
    const std::pair<float, float>& offset, const HMSymbolTxt& symbolTxt)
{
    RSEffectStrategy effectMode = symbolTxt.GetEffectStrategy();
    uint16_t animationMode = symbolTxt.GetAnimationMode();
    if (effectMode == RSEffectStrategy::NONE) {
        LOGD("[%{public}s] HmSymbol: the RSEffectStrategy is NONE\n", __func__);
        return false;
    }
    RSAnimationSetting animationSetting;
    if (animationMode == 0 || effectMode == RSEffectStrategy::VARIABLE_COLOR) {
        if (!GetAnimationGroups(glyphid, effectMode, animationSetting)) {
            LOGD("[%{public}s] HmSymbol: GetAnimationGroups of glyphId %{public}d is failed\n",
                __func__, std::static_cast<int>(glyphid));
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
    symbolNode.SetRepeatCount(symbolTxt.GetRepeatCount());
    symbolNode.SetAnimationStart(symbolTxt.GetAnimationStart());
    symbolNode.SetCommonSubType(symbolTxt.GetCommonSubType());
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

bool HMSymbolRun::GetAnimationGroups(uint16_t glyphid, const RSEffectStrategy effectStrategy,
    RSAnimationSetting& animationOut)
{
    auto symbolInfoOrigin = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(glyphid);
    RSAnimationType animationType = static_cast<RSAnimationType>(effectStrategy);

    for (auto& animationSetting: symbolInfoOrigin.animationSettings) {
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