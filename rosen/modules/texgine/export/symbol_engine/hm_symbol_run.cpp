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

#include "hm_symbol_run.h"

#include "drawing.h"
#include "symbol_animation/symbol_node_build.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {

RSSymbolLayers HMSymbolRun::GetSymbolLayers(const uint16_t& glyphId, const HMSymbolTxt& symbolText)
{
    RSSymbolLayers symbolInfo;
    symbolInfo.symbolGlyphId = glyphId;
    uint32_t symbolId = static_cast<uint32_t>(glyphId);
    std::shared_ptr<RSSymbolLayersGroups> symbolInfoOrign = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(symbolId);
    if (symbolInfoOrign == nullptr || symbolInfoOrign->symbolGlyphId == 0) {
        return symbolInfo;
    }

    RSSymbolRenderingStrategy renderMode = symbolText.GetRenderMode();
    if (symbolInfoOrign->renderModeGroups.find(renderMode) == symbolInfoOrign->renderModeGroups.end()) {
        renderMode = RSSymbolRenderingStrategy::SINGLE;
    }

    symbolInfo.layers = symbolInfoOrign->layers;
    if (symbolInfoOrign->renderModeGroups.find(renderMode) != symbolInfoOrign->renderModeGroups.end()) {
        symbolInfo.renderGroups = symbolInfoOrign->renderModeGroups[renderMode];
        symbolInfo.symbolGlyphId = symbolInfoOrign->symbolGlyphId;
    }

    std::vector<RSSColor> colorList = symbolText.GetRenderColor();
    if (!colorList.empty()) {
        SetSymbolRenderColor(renderMode, colorList, symbolInfo);
    }
    return symbolInfo;
}

bool HMSymbolRun::SetGroupsByEffect(const uint32_t glyphId, const RSEffectStrategy effectStrategy,
    std::vector<RSRenderGroup>& renderGroups)
{
#if !defined(BUILD_SDK_ANDROID) && !defined(BUILD_SDK_IOS)
    RSAnimationSetting animationSetting;
    if (GetAnimationGroups(glyphId, effectStrategy, animationSetting)) {
        std::vector<RSRenderGroup> newRenderGroups;
        RSRenderGroup group;
        for (size_t i = 0, j = 0; i < animationSetting.groupSettings.size(); i++) {
            if (j < renderGroups.size()) {
                group = renderGroups[j];
                j++;
            }
            group.groupInfos = animationSetting.groupSettings[i].groupInfos;
            newRenderGroups.push_back(group);
        }
        if (!newRenderGroups.empty()) {
            renderGroups = newRenderGroups;
            return true;
        }
    }
#endif
    return false;
}

void HMSymbolRun::SetSymbolRenderColor(const RSSymbolRenderingStrategy& renderMode, const std::vector<RSSColor>& colors,
    RSSymbolLayers& symbolInfo)
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

void HMSymbolRun::DrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob,
    const std::pair<double, double>& offset, const TexginePaint &paint, const TextStyle &style)
{
    if (blob == nullptr) {
        return;
    }
    std::vector<uint16_t> glyphIds;
    blob->GetGlyphIDs(glyphIds);
    if (glyphIds.size() == 1) {
        uint16_t glyphId = glyphIds[0];
        RSPath path = blob->GetPathbyGlyphID(glyphId);
        RSHMSymbolData symbolData;
        symbolData.symbolInfo_ = GetSymbolLayers(glyphId, style.symbol);
        if (symbolData.symbolInfo_.symbolGlyphId != glyphId) {
            path = blob->GetPathbyGlyphID(symbolData.symbolInfo_.symbolGlyphId);
        }
        symbolData.path_ = path;
        symbolData.symbolId = symbolId_; // symbolspan Id in paragraph
        RSPoint offsetLocal = RSPoint{ offset.first, offset.second };
        RSEffectStrategy symbolEffect = style.symbol.GetEffectStrategy();
        uint32_t symbolId = static_cast<uint32_t>(glyphId);
        if (symbolEffect > 1) { // 1 > has animation
            if (!SymbolAnimation(symbolData, symbolId, offset, style.symbol.GetEffectStrategy())) {
                ClearSymbolAnimation(symbolData, symbolId, offset);
                canvas.DrawSymbol(symbolData, offsetLocal, paint);
            }
        } else {
            ClearSymbolAnimation(symbolData, symbolId, offset);
            canvas.DrawSymbol(symbolData, offsetLocal, paint);
        }
    } else {
        canvas.DrawTextBlob(blob, offset.first, offset.second, paint);
    }
}

bool HMSymbolRun::SymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyohId,
        const std::pair<double, double> offset, const RSEffectStrategy effectMode)
{
    if (effectMode == RSEffectStrategy::NONE) {
        return false;
    }
    RSAnimationSetting animationSetting;
    int scaleType = static_cast<int>(RSEffectStrategy::SCALE);

    bool check = GetAnimationGroups(glyohId, effectMode, animationSetting);
    if ((!check) && static_cast<int>(effectMode) != scaleType) {
        return check;
    }
    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSetting, symbol, effectMode, offset);
    symbolNode.SetAnimation(animationFunc_);
    symbolNode.SetSymbolId(symbolId_);
    if (!symbolNode.DecomposeSymbolAndDraw()) {
        return false;
    }
    return true;
}

void HMSymbolRun::ClearSymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyohId,
        const std::pair<double, double> offset)
{
    auto effectMode = RSEffectStrategy::NONE;
    RSAnimationSetting animationSetting;

    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSetting, symbol, effectMode, offset);
    symbolNode.SetAnimation(animationFunc_);
    symbolNode.SetSymbolId(symbolId_);
    symbolNode.ClearAnimation();
}

bool HMSymbolRun::GetAnimationGroups(const uint32_t glyohId, const RSEffectStrategy effectStrategy,
    RSAnimationSetting& animationOut)
{
    auto symbolInfoOrigin = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(glyohId);
    if (symbolInfoOrigin == nullptr) {
        return false;
    }
    std::vector<RSAnimationSetting> animationSettings = symbolInfoOrigin->animationSettings;
    RSAnimationType animationType =  static_cast<RSAnimationType>(effectStrategy);

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
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS