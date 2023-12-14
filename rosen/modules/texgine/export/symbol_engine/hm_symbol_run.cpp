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
    RSSymbolLayersGroups* symbolInfoOrign = RSHmSymbolConfig_OHOS::GetSymbolLayersGroups(symbolId);
#endif
    if (symbolInfoOrign == nullptr) {
        return symbolInfo;
    }

    symbolInfo.layers = symbolInfoOrign->layers;
#ifndef USE_ROSEN_DRAWING
    symbolInfo.renderGroups = symbolInfoOrign->renderModeGroups[SymbolRenderingStrategy::SINGLE];
#else
    symbolInfo.renderGroups = symbolInfoOrign->renderModeGroups[RSSymbolRenderingStrategy::SINGLE];
#endif
    symbolInfo.symbolGlyphId = symbolInfoOrign->symbolGlyphId;

#ifndef USE_ROSEN_DRAWING
    SymbolRenderingStrategy renderMode = symbolText.GetRenderMode();
#else
    RSSymbolRenderingStrategy renderMode = symbolText.GetRenderMode();
#endif
    if (symbolInfoOrign->renderModeGroups.find(renderMode) != symbolInfoOrign->renderModeGroups.end()) {
        symbolInfo.renderGroups = symbolInfoOrign->renderModeGroups[renderMode];
#ifndef USE_ROSEN_DRAWING
        std::vector<SColor> colorList = symbolText.GetRenderColor();
#else
        std::vector<RSSColor> colorList = symbolText.GetRenderColor();
#endif
        if (!colorList.empty()) {
            SetSymbolRenderColor(renderMode, colorList, symbolInfo);
        }
    }
    return symbolInfo;
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

void HMSymbolRun::DrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob,
    const std::pair<double, double>& offset, const TexginePaint &paint, const TextStyle &style)
{
    if (blob == nullptr) {
        return;
    }

#ifndef USE_ROSEN_DRAWING
    std::vector<SkGlyphID> glyphIds;
#else
    std::vector<uint16_t> glyphIds;
#endif
    blob->GetGlyphIDs(glyphIds);
    if (glyphIds.size() == 1) {
#ifndef USE_ROSEN_DRAWING
        SkGlyphID glyphId = glyphIds[0];
        SkPath path = blob->GetPathbyGlyphID(glyphId);
        HMSymbolData symbolData;
#else
        uint16_t glyphId = glyphIds[0];
        RSPath path = blob->GetPathbyGlyphID(glyphId);
        RSHMSymbolData symbolData;
#endif
        symbolData.symbolInfo_ = GetSymbolLayers(glyphId, style.symbol);
        if (symbolData.symbolInfo_.symbolGlyphId != glyphId) {
            path = blob->GetPathbyGlyphID(symbolData.symbolInfo_.symbolGlyphId);
        }
        symbolData.path_ = path;
#ifndef USE_ROSEN_DRAWING
        SkPoint offsetLocal = SkPoint::Make(offset.first, offset.second);
#else
        RSPoint offsetLocal = RSPoint{ offset.first, offset.second };
#endif
        canvas.DrawSymbol(symbolData, offsetLocal, paint);
    } else {
        canvas.DrawTextBlob(blob, offset.first, offset.second, paint);
    }
}
}
}
}