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

#include "drawing.h"

#include "hm_symbol_txt.h"
#include "rosen_text/symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class HMSymbolRun {
public:
    ~HMSymbolRun() {}

    HMSymbolRun(uint64_t symbolId,
        const HMSymbolTxt& symbolTxt,
        const std::shared_ptr<RSTextBlob>& textBlob,
        std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& animationFunc)
    {
        symbolId_ = symbolId;
        symbolTxt_ = symbolTxt;
        textBlob_  = textBlob;
        animationFunc_ = animationFunc;
    }

    static RSSymbolLayers GetSymbolLayers(uint16_t glyphId, const HMSymbolTxt& symbolText);

    static void SetSymbolRenderColor(const RSSymbolRenderingStrategy& renderMode,
        const std::vector<RSSColor>& colors, RSSymbolLayers& symbolInfo);

    static bool GetAnimationGroups(uint16_t glyphid, const RSEffectStrategy effectStrategy,
        RSAnimationSetting& animationOut);

    bool SymbolAnimation(const RSHMSymbolData& symbol, uint16_t glyphid, const std::pair<float, float>& offset);

    void ClearSymbolAnimation(const RSHMSymbolData& symbol, const std::pair<float, float>& offset);

    void DrawSymbol(RSCanvas* canvas, const RSPoint& offset);

    void SetAnimationRenderColor(const std::vector<RSSColor>& colorList)
    {
        symbolTxt_.SetRenderColor(colorList);
    }

    void SetRenderMode(RSSymbolRenderingStrategy renderMode)
    {
        symbolTxt_.SetRenderMode(renderMode);
    }

    void SetAnimationStart(const bool animationStart)
    {
        symbolTxt_.SetAnimationStart(animationStart);
    }

    void SetTextBlob(const std::shared_ptr<RSTextBlob>& textBlob);

    void SetAnimation(
        const std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
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
    void OnDrawSymbol(RSCanvas* canvas, const RSHMSymbolData& symbolData, RSPoint locate);

    HMSymbolTxt symbolTxt_;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc_ = nullptr;
    std::shared_ptr<RSTextBlob> textBlob_;
    uint64_t symbolId_ = 0;
    bool animationReset_ = true;
};
}
}
}
#endif