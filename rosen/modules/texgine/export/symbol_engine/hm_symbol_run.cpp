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
#include "include/pathops/SkPathOps.h"
#include "src/ports/skia_ohos/HmSymbolConfig_ohos.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {

SymbolLayers HMSymbolRun::GetSymbolLayers(const SkGlyphID& glyphId, const HMSymbolTxt& symbolText)
{
    SymbolLayers symbolInfo;
    symbolInfo.symbolGlyphId = glyphId;
    uint32_t symbolId = static_cast<uint32_t>(glyphId);
    SymbolLayersGroups* symbolInfoOrign = HmSymbolConfig_OHOS::getInstance()->getSymbolLayersGroups(symbolId);
    if (symbolInfoOrign == nullptr) {
        return symbolInfo;
    }

    symbolInfo.layers = symbolInfoOrign->layers;
    symbolInfo.renderGroups = symbolInfoOrign->renderModeGroups[SymbolRenderingStrategy::SINGLE];
    symbolInfo.symbolGlyphId = symbolInfoOrign->symbolGlyphId;

    SymbolRenderingStrategy renderMode = symbolText.GetRenderMode();
    if (symbolInfoOrign->renderModeGroups.find(renderMode) != symbolInfoOrign->renderModeGroups.end()) {
        symbolInfo.renderGroups = symbolInfoOrign->renderModeGroups[renderMode];
        std::vector<SColor> colorList = symbolText.GetRenderColor();
        if (!colorList.empty()) {
            SetSymbolRenderColor(renderMode, colorList, symbolInfo);
        }
    }
    return symbolInfo;
}

void HMSymbolRun::SetSymbolRenderColor(const SymbolRenderingStrategy& renderMode, const std::vector<SColor>& colors,
    SymbolLayers& symbolInfo)
{
    if (colors.empty()) {
        return;
    }
    switch(renderMode) {
        // SINGLE and HIERARCHICAL: Supports single color setting
        case SymbolRenderingStrategy::SINGLE:
        case SymbolRenderingStrategy::MULTIPLE_OPACITY:
            for (size_t i = 0; i < symbolInfo.renderGroups.size(); ++i) {
                symbolInfo.renderGroups[i].color.r = colors[0].r; // the 0 indicates the the first color is used
                symbolInfo.renderGroups[i].color.g = colors[0].g; // the 0 indicates the the first color is used
                symbolInfo.renderGroups[i].color.b = colors[0].b; // the 0 indicates the the first color is used
            }
            break;
        // MULTIPLE_COLOR: Supports mutiple color setting
        case SymbolRenderingStrategy::MULTIPLE_COLOR:
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

void HMSymbolRun::DrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob, const std::pair<double, double>& offset,
    const TexginePaint &paint, const TextStyle &style)
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
        SkPoint offsetLocal = SkPoint::Make(offset.first, offset.second);
        canvas.DrawSymbol(symbolData, offsetLocal, paint);
    }else {
        canvas.DrawTextBlob(blob, offset.first, offset.second, paint);
    }
}

}
}
}