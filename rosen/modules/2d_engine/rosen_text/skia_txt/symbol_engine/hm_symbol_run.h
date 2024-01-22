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

#ifndef LIB_TXT_SRC_SYMBOL_ENGINE_HM_SYMBOL_RUN_H
#define LIB_TXT_SRC_SYMBOL_ENGINE_HM_SYMBOL_RUN_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#ifndef USE_ROSEN_DRAWING
#include "include/core/HMSymbol.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkCanvas.h"
#else
#include "drawing.h"
#endif

#include "hm_symbol_txt.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class HMSymbolRun {
public:
    HMSymbolRun() {}
    ~HMSymbolRun() {}

#ifndef USE_ROSEN_DRAWING
    static SymbolLayers GetSymbolLayers(const SkGlyphID& glyphId, const HMSymbolTxt& symbolText);

    static void SetSymbolRenderColor(const SymbolRenderingStrategy& renderMode,
        const std::vector<SColor>& colors, SymbolLayers& symbolInfo);

    static bool GetAnimationGroups(const uint32_t glyohId, const EffectStrategy effectStrategy,
        AnimationSetting& animationOut);

    static bool SetGroupsByEffect(const uint32_t glyphId, const EffectStrategy effectStrategy,
        std::vector<RenderGroup>& renderGroups);

    static void DrawSymbol(SkCanvas* canvas, SkTextBlob* blob, const SkPoint& offset,
        const SkPaint &paint, const HMSymbolTxt &symbolTxt);
#else
    static RSSymbolLayers GetSymbolLayers(const uint16_t& glyphId, const HMSymbolTxt& symbolText);

    static void SetSymbolRenderColor(const RSSymbolRenderingStrategy& renderMode,
        const std::vector<RSSColor>& colors, RSSymbolLayers& symbolInfo);
    
    static bool GetAnimationGroups(const uint32_t glyohId, const RSEffectStrategy effectStrategy,
        RSAnimationSetting& animationOut);

    static bool SetGroupsByEffect(const uint32_t glyphId, const RSEffectStrategy effectStrategy,
        std::vector<RSRenderGroup>& renderGroups);
    
    static void DrawSymbol(RSCanvas* canvas, RSTextBlob* blob, const RSPoint& offset, const HMSymbolTxt &symbolTxt);
#endif
};
}
}
}
#endif