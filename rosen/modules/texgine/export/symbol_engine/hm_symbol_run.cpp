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

#ifndef USE_ROSEN_DRAWING
#include "include/pathops/SkPathOps.h"
#include "src/ports/skia_ohos/HmSymbolConfig_ohos.h"
#else
#include "drawing.h"
#endif
#include "symbol_animation/symbol_node_build.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {

#ifndef USE_ROSEN_DRAWING
SymbolLayers HMSymbolRun::GetSymbolLayers(const SkGlyphID& glyphId, const HMSymbolTxt& symbolText)
#else
RSSymbolLayers HMSymbolRun::GetSymbolLayers(const uint16_t& glyphId, const HMSymbolTxt& symbolText)
#endif
{
#ifndef USE_ROSEN_DRAWING
    SymbolLayers symbolInfo;
#else
    RSSymbolLayers symbolInfo;
#endif
    symbolInfo.symbolGlyphId = glyphId;
    uint32_t symbolId = static_cast<uint32_t>(glyphId);
#ifndef USE_ROSEN_DRAWING
    SymbolLayersGroups* symbolInfoOrign = HmSymbolConfig_OHOS::getInstance()->getSymbolLayersGroups(symbolId);
#else
    std::shared_ptr<RSSymbolLayersGroups> symbolInfoOrign = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(symbolId);
#endif
    if (symbolInfoOrign == nullptr || symbolInfoOrign->symbolGlyphId == 0) {
        return symbolInfo;
    }

#ifndef USE_ROSEN_DRAWING
    SymbolRenderingStrategy renderMode = symbolText.GetRenderMode();
    EffectStrategy effectMode = symbolText.GetEffectStrategy();
    if (symbolInfoOrign->renderModeGroups.find(renderMode) == symbolInfoOrign->renderModeGroups.end()) {
        renderMode = SymbolRenderingStrategy::SINGLE;
#else
    RSSymbolRenderingStrategy renderMode = symbolText.GetRenderMode();
    RSEffectStrategy effectMode = symbolText.GetEffectStrategy();
    if (symbolInfoOrign->renderModeGroups.find(renderMode) == symbolInfoOrign->renderModeGroups.end()) {
        renderMode = RSSymbolRenderingStrategy::SINGLE;
#endif
    }

    symbolInfo.layers = symbolInfoOrign->layers;
    if (symbolInfoOrign->renderModeGroups.find(renderMode) != symbolInfoOrign->renderModeGroups.end()) {
        symbolInfo.renderGroups = symbolInfoOrign->renderModeGroups[renderMode];
        symbolInfo.symbolGlyphId = symbolInfoOrign->symbolGlyphId;
    }

#ifndef USE_ROSEN_DRAWING
    std::vector<SColor> colorList = symbolText.GetRenderColor();
#else
    std::vector<RSSColor> colorList = symbolText.GetRenderColor();
#endif
    if (!colorList.empty()) {
        SetSymbolRenderColor(renderMode, colorList, symbolInfo);
    }

#ifndef USE_ROSEN_DRAWING
    if (effectMode == EffectStrategy::HIERARCHICAL &&
        SetGroupsByEffect(glyphId, effectMode, symbolInfo.renderGroups)) {
#else
    if (effectMode == RSEffectStrategy::HIERARCHICAL &&
        SetGroupsByEffect(glyphId, effectMode, symbolInfo.renderGroups)) {
#endif
        symbolInfo.symbolGlyphId = symbolInfoOrign->symbolGlyphId;
        symbolInfo.effect = effectMode;
    }
    return symbolInfo;
}

#ifndef USE_ROSEN_DRAWING
bool HMSymbolRun::SetGroupsByEffect(const uint32_t glyphId, const EffectStrategy effectStrategy,
    std::vector<RenderGroup>& renderGroups)
#else
bool HMSymbolRun::SetGroupsByEffect(const uint32_t glyphId, const RSEffectStrategy effectStrategy,
    std::vector<RSRenderGroup>& renderGroups)
#endif
{
#if !defined(BUILD_SDK_ANDROID) && !defined(BUILD_SDK_IOS)
#ifndef USE_ROSEN_DRAWING
    AnimationSetting animationSetting;
#else
    RSAnimationSetting animationSetting;
#endif
    if (GetAnimationGroups(glyphId, effectStrategy, animationSetting)) {
#ifndef USE_ROSEN_DRAWING
        std::vector<RenderGroup> newRenderGroups;
        RenderGroup group;
#else
        std::vector<RSRenderGroup> newRenderGroups;
        RSRenderGroup group;
#endif
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

#ifndef USE_ROSEN_DRAWING
void HMSymbolRun::SetSymbolRenderColor(const SymbolRenderingStrategy& renderMode, const std::vector<SColor>& colors,
    SymbolLayers& symbolInfo)
#else
void HMSymbolRun::SetSymbolRenderColor(const RSSymbolRenderingStrategy& renderMode, const std::vector<RSSColor>& colors,
    RSSymbolLayers& symbolInfo)
#endif
{
    if (colors.empty()) {
        return;
    }
    switch (renderMode) {
        // SINGLE and HIERARCHICAL: Supports single color setting
#ifndef USE_ROSEN_DRAWING
        case SymbolRenderingStrategy::SINGLE:
#else
        case RSSymbolRenderingStrategy::SINGLE:
#endif
            for (size_t i = 0; i < symbolInfo.renderGroups.size(); ++i) {
                symbolInfo.renderGroups[i].color = colors[0]; // the 0 indicates the the first color is used
            }
            break;
#ifndef USE_ROSEN_DRAWING
        case SymbolRenderingStrategy::MULTIPLE_OPACITY:
#else
        case RSSymbolRenderingStrategy::MULTIPLE_OPACITY:
#endif
            for (size_t i = 0; i < symbolInfo.renderGroups.size(); ++i) {
                symbolInfo.renderGroups[i].color.r = colors[0].r; // the 0 indicates the the first color is used
                symbolInfo.renderGroups[i].color.g = colors[0].g; // the 0 indicates the the first color is used
                symbolInfo.renderGroups[i].color.b = colors[0].b; // the 0 indicates the the first color is used
            }
            break;
        // MULTIPLE_COLOR: Supports mutiple color setting
#ifndef USE_ROSEN_DRAWING
        case SymbolRenderingStrategy::MULTIPLE_COLOR:
#else
        case RSSymbolRenderingStrategy::MULTIPLE_COLOR:
#endif
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

#ifndef USE_ROSEN_DRAWING
void HMSymbolRun::DrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob,
    const std::pair<double, double>& offset, const TexginePaint &paint, const TextStyle &style)
{
    if (blob == nullptr) {
        return;
    }
    std::vector<SkGlyphID> glyphIds;
    blob->GetGlyphIDs(glyphIds);
    if (glyphIds.size() == 1) {
        SkGlyphID glyphId = glyphIds[0];
        SkPath path = blob->GetPathbyGlyphID(glyphId);
        HMSymbolData symbolData;
        symbolData.symbolInfo_ = GetSymbolLayers(glyphId, style.symbol);
        if (symbolData.symbolInfo_.symbolGlyphId != glyphId) {
            path = blob->GetPathbyGlyphID(symbolData.symbolInfo_.symbolGlyphId);
        }
        symbolData.path_ = path;
        symbolData.symbolId = symbolId_; // symbolspan Id in paragraph
        SkPoint offsetLocal = SkPoint::Make(offset.first, offset.second);
        EffectStrategy symbolEffect = style.symbol.GetEffectStrategy();
        uint32_t symbolId = static_cast<uint32_t>(glyphId);
        if (symbolEffect > 1) { // 1 > has animation
            if (!SymbolAnimation(symbolData, symbolId, offset, style.symbol.GetEffectStrategy())) {
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
#else
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
#endif

#ifndef USE_ROSEN_DRAWING
bool HMSymbolRun::SymbolAnimation(const HMSymbolData symbol, const uint32_t glyohId,
    const std::pair<double, double> offset, const EffectStrategy effectMode)
#else
bool HMSymbolRun::SymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyohId,
        const std::pair<double, double> offset, const RSEffectStrategy effectMode)
#endif
{
#ifndef USE_ROSEN_DRAWING
    if (effectMode == EffectStrategy::NONE) {
        return false;
    }
    AnimationSetting animationSetting;
#else
    if (effectMode == RSEffectStrategy::NONE) {
        return false;
    }
    RSAnimationSetting animationSetting;
#endif

    bool check = GetAnimationGroups(glyohId, effectMode, animationSetting);
    if (!check) {
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

#ifndef USE_ROSEN_DRAWING
void HMSymbolRun::ClearSymbolAnimation(const HMSymbolData symbol, const uint32_t glyohId,
    const std::pair<double, double> offset)
{
    auto effectMode = EffectStrategy::NONE;
    AnimationSetting animationSetting;

    SymbolNodeBuild symbolNode = SymbolNodeBuild(animationSetting, symbol, effectMode, offset);
    symbolNode.SetAnimation(animationFunc_);
    symbolNode.SetSymbolId(symbolId_);
    symbolNode.ClearAnimation();
}
#else
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
#endif

#ifndef USE_ROSEN_DRAWING
bool HMSymbolRun::GetAnimationGroups(const uint32_t glyohId, const EffectStrategy effectStrategy,
    AnimationSetting& animationOut)
{
    SymbolLayersGroups* symbolInfoOrigin = HmSymbolConfig_OHOS::getInstance()->getSymbolLayersGroups(glyohId);
    std::vector<AnimationSetting> animationSettings = symbolInfoOrigin->animationSettings;

    AnimationType animationType = AnimationType::INVALID_ANIMATION_TYPE;
    AnimationSubType animationSubType = AnimationSubType::INVALID_ANIMATION_SUB_TYPE;
    uint32_t animationMode = 1;
    if (effectStrategy == EffectStrategy::SCALE) {
        animationType = AnimationType::SCALE_EFFECT;
        animationSubType = AnimationSubType::UNIT;
        return true;
    }

    if (effectStrategy == EffectStrategy::HIERARCHICAL) {
        animationType = AnimationType::VARIABLE_COLOR;
        animationSubType = AnimationSubType::VARIABLE_3_GROUP;
    }
    for (size_t i = 0; i < animationSettings.size(); i++) {
        if (animationType == animationSettings[i].animationType &&
            animationSubType == animationSettings[i].animationSubType &&
            animationMode == animationSettings[i].animationMode) {
                animationOut = animationSettings[i];
                return true;
            }
    }
    return false;
}
#else
bool HMSymbolRun::GetAnimationGroups(const uint32_t glyohId, const RSEffectStrategy effectStrategy,
    RSAnimationSetting& animationOut)
{
    auto symbolInfoOrigin = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(glyohId);
    std::vector<RSAnimationSetting> animationSettings = symbolInfoOrigin->animationSettings;

    RSAnimationType animationType = RSAnimationType::INVALID_ANIMATION_TYPE;
    RSAnimationSubType animationSubType = RSAnimationSubType::INVALID_ANIMATION_SUB_TYPE;
    uint32_t animationMode = 1;
    if (effectStrategy == RSEffectStrategy::SCALE) {
        animationType = RSAnimationType::SCALE_EFFECT;
        animationSubType = RSAnimationSubType::UNIT;
        return true;
    }

    if (effectStrategy == RSEffectStrategy::HIERARCHICAL) {
        animationType = RSAnimationType::VARIABLE_COLOR;
        animationSubType = RSAnimationSubType::VARIABLE_3_GROUP;
    }

    for (size_t i = 0; i < animationSettings.size(); i++) {
        if (animationType == animationSettings[i].animationType &&
            animationSubType == animationSettings[i].animationSubType &&
            animationMode == animationSettings[i].animationMode) {
                animationOut = animationSettings[i];
                return true;
            }
    }
    return false;
}
#endif
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS