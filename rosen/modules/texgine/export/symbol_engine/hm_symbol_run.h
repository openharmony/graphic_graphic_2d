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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ENGINE_HM_SYMBOL_RUN_H
#define ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ENGINE_HM_SYMBOL_RUN_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>


#include "hm_symbol_txt.h"
#include "texgine/text_style.h"
#include "texgine/utils/exlog.h"
#include "texgine_text_blob.h"
#include "texgine_canvas.h"
#include "symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class HMSymbolRun {
public:
    HMSymbolRun() : animationFunc_(nullptr), symbolId_(0) {}
    ~HMSymbolRun() {}

    static RSSymbolLayers GetSymbolLayers(const uint16_t& glyphId, const HMSymbolTxt& symbolText);

    static void SetSymbolRenderColor(const RSSymbolRenderingStrategy& renderMode, const std::vector<RSSColor>& colors,
        RSSymbolLayers& symbolInfo);

    static bool GetAnimationGroups(const uint32_t glyohId, const RSEffectStrategy effectStrategy,
        RSAnimationSetting& animationOut);

    bool SymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyohId,
        const std::pair<double, double> offset, const RSEffectStrategy effectMode);

    void ClearSymbolAnimation(const RSHMSymbolData symbol, const uint32_t glyohId,
        const std::pair<double, double> offset);

    void DrawSymbol(TexgineCanvas &canvas, const std::shared_ptr<TexgineTextBlob> &blob,
        const std::pair<double, double>& offset, const TexginePaint &paint, const TextStyle &style);

    void SetAnimation(
        const std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
        animationFunc)
    {
        if (animationFunc != nullptr) {
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
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif