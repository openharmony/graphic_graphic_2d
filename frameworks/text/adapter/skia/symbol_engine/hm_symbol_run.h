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
    HMSymbolRun() {}

    ~HMSymbolRun() {}

    HMSymbolRun(uint64_t symbolId,
        const HMSymbolTxt& symbolTxt,
        const std::shared_ptr<RSTextBlob>& textBlob,
        std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& animationFunc);

    static void SetSymbolRenderColor(const RSSymbolRenderingStrategy& renderMode,
        const std::vector<RSSColor>& colors, RSSymbolLayers& symbolInfo);

    void DrawSymbol(RSCanvas* canvas, const RSPoint& offset);

    void SetRenderColor(const std::vector<RSSColor>& colorList);

    void SetRenderMode(RSSymbolRenderingStrategy renderMode);

    void SetSymbolEffect(const RSEffectStrategy& effectStrategy);

    void SetAnimationMode(uint16_t animationMode);

    void SetAnimationStart(bool animationStart);

    void SetCommonSubType(Drawing::DrawingCommonSubType commonSubType);

    void SetTextBlob(const std::shared_ptr<RSTextBlob>& textBlob);

    void SetAnimation(
        const std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
        animationFunc);

    uint64_t GetSymbolUid() const
    {
        return symbolTxt_.GetSymbolUid();
    }

    void SetSymbolUid(uint64_t symbolUid)
    {
        symbolTxt_.SetSymbolUid(symbolUid);
        symbolId_ = symbolUid;
    }

    void SetSymbolTxt(const HMSymbolTxt& hmsymbolTxt)
    {
        symbolTxt_ = hmsymbolTxt;
        symbolId_ = symbolTxt_.GetSymbolUid();
    }

    const HMSymbolTxt& GetSymbolTxt()
    {
        return symbolTxt_;
    }

    void SetGradients(const std::vector<std::shared_ptr<SymbolGradient>>& gradients);

private:
    void OnDrawSymbol(RSCanvas* canvas, const RSHMSymbolData& symbolData, RSPoint locate);

    void DrawPaths(RSCanvas* canvas, const std::vector<RSPath>& multPaths, const RSPath& path);

    bool GetAnimationGroups(const RSEffectStrategy effectStrategy, RSAnimationSetting& animationOut);

    RSSymbolLayers GetSymbolLayers(uint16_t glyphId, const HMSymbolTxt& symbolText);

    void SetRenderColor(const RSSymbolRenderingStrategy& renderMode, RSSymbolLayers& symbolInfo);

    void SetGradientColor(const RSSymbolRenderingStrategy& renderMode, const RSSymbolLayers& symbolInfo);

    void UpdateSymbolLayersGroups(uint16_t glyphId);

    bool SymbolAnimation(const RSHMSymbolData& symbol, const std::pair<float, float>& offset);

    void ClearSymbolAnimation(const RSHMSymbolData& symbol, const std::pair<float, float>& offset);

    RSSymbolLayersGroups symbolLayersGroups_;
    HMSymbolTxt symbolTxt_;
    std::vector<std::shared_ptr<SymbolGradient>> gradients_;
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc_ = nullptr;
    std::shared_ptr<RSTextBlob> textBlob_ = nullptr;
    uint64_t symbolId_ = 0;
    bool currentAnimationHasPlayed_ = false;
};
}
}
}
#endif