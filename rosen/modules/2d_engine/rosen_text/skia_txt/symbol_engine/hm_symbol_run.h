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
#include "rosen_text/symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class HMSymbolRun {
public:
    HMSymbolRun() {}
    ~HMSymbolRun() {}

#ifndef USE_ROSEN_DRAWING
    SymbolLayers GetSymbolLayers(const SkGlyphID& glyphId, const HMSymbolTxt& symbolText);

    void SetSymbolRenderColor(const SymbolRenderingStrategy& renderMode,
        const std::vector<SColor>& colors, SymbolLayers& symbolInfo);

    bool GetAnimationGroups(const uint32_t glyphid, const EffectStrategy effectStrategy,
        AnimationSetting& animationOut);

    bool SetGroupsByEffect(const uint32_t glyphId, const EffectStrategy effectStrategy,
        std::vector<RenderGroup>& renderGroups);

    bool SymbolAnimation(const HMSymbolData symbol, const uint32_t glyphid,
        const std::pair<double, double> offset, const EffectStrategy effectMode);

    void ClearSymbolAnimation(const HMSymbolData symbol, const uint32_t glyphid,
        const std::pair<double, double> offset);

    void DrawSymbol(SkCanvas* canvas, SkTextBlob* blob, const SkPoint& offset,
        const SkPaint &paint, const HMSymbolTxt &symbolTxt);
#else
    RSSymbolLayers GetSymbolLayers(const uint16_t& glyphId, const HMSymbolTxt& symbolText);

    void SetSymbolRenderColor(const RSSymbolRenderingStrategy& renderMode,
        const std::vector<RSSColor>& colors, RSSymbolLayers& symbolInfo);
    
    bool GetAnimationGroups(const uint32_t glyphid, const RSEffectStrategy effectStrategy,
        RSAnimationSetting& animationOut);

    bool SetGroupsByEffect(const uint32_t glyphId, const RSEffectStrategy effectStrategy,
        std::vector<RSRenderGroup>& renderGroups);

    bool SymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyphid,
        const std::pair<double, double> offset, const RSEffectStrategy effectMode);

    void ClearSymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyphid,
        const std::pair<double, double> offset);
    
    void DrawSymbol(RSCanvas* canvas, RSTextBlob* blob, const RSPoint& offset, const HMSymbolTxt &symbolTxt);
#endif

    void SetAnimation(std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
        animationFunc)
    {
        if (animationFunc) {
            animationFunc_ = animationFunc;
        }
    }

    void SetSymbolId(const uint64_t& symbolId)
    {
        symbolId_ = symbolId;
    }

private:
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc_ = nullptr;

    uint64_t symbolId_;
};
}
}
}
#endif