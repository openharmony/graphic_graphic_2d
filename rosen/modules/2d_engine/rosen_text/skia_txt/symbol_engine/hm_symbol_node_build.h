/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ANIMATION_SYMBOL_NODE_BUILD_H
#define ROSEN_MODULES_TEXGINE_EXPORT_SYMBOL_ANIMATION_SYMBOL_NODE_BUILD_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "drawing.h"
#include "symbol_engine/hm_symbol_txt.h"
#include "rosen_text/symbol_animation_config.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
namespace SPText {

class SymbolNodeBuild {
public:
    SymbolNodeBuild(const RSAnimationSetting animationSetting, const RSHMSymbolData symbolData,
    const RSEffectStrategy effectStrategy, const std::pair<double, double> offset);

    void AddWholeAnimation(const RSHMSymbolData &symbolData, const Vector4f &nodeBounds,
        std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig);

    static void AddHierarchicalAnimation(RSHMSymbolData &symbolData, const Vector4f &nodeBounds,
        std::vector<RSGroupSetting> &groupSettings,
        std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig);
    ~SymbolNodeBuild() {}
    bool DecomposeSymbolAndDraw();
    void ClearAnimation();
    void SetAnimation(
        const std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
        animationFunc)
    {
        animationFunc_ = animationFunc;
    }

    void SetSymbolId(const uint64_t& symbolSpanId)
    {
        symblSpanId_ = symbolSpanId;
    }

    void SetAnimationMode(const uint16_t animationMode) {
        animationMode_ = animationMode;
    }

    void SetRepeatCount(const int repeatCount) {
        repeatCount_ = repeatCount;
    }

    void SetAminationStart(const bool aminationStart) {
        aminationStart_ = aminationStart;
    }

private:
    RSAnimationSetting animationSetting_;
    RSHMSymbolData symbolData_;
    RSEffectStrategy effectStrategy_ = RSEffectStrategy::NONE;
    double offsetX_;
    double offsetY_;
    uint16_t animationMode_ = 0;
    int repeatCount_ = 1;
    bool aminationStart_ = false;

    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc_ = nullptr;
    uint64_t symblSpanId_ = 0;
};
}
}
}

#endif