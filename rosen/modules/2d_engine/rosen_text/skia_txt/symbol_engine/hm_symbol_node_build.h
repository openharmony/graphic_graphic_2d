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
    SymbolNodeBuild(const RSAnimationSetting &animationSetting, const RSHMSymbolData &symbolData,
    const RSEffectStrategy &effectStrategy, const std::pair<float, float> &offset);

    void AddWholeAnimation(const RSHMSymbolData &symbolData, const Vector4f &nodeBounds,
        std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig);

    static void AddHierarchicalAnimation(RSHMSymbolData &symbolData, const Vector4f &nodeBounds,
        const std::vector<RSGroupSetting> &groupSettings,
        std::shared_ptr<TextEngine::SymbolAnimationConfig> symbolAnimationConfig);

    static void MergeDrawingPath(RSPath& multPath, const RSRenderGroup& group, std::vector<RSPath>& pathLayers);

    ~SymbolNodeBuild() {}
    bool DecomposeSymbolAndDraw();
    void ClearAnimation();
    void SetAnimation(
        const std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>&
        animationFunc)
    {
        animationFunc_ = animationFunc;
    }

    void SetSymbolId(uint64_t symbolSpanId)
    {
        symblSpanId_ = symbolSpanId;
    }

    void SetAnimationMode(uint16_t animationMode)
    {
        animationMode_ = animationMode > 0 ? 1 : 0; // 1 is whole or add, 0 is hierarchical or iterate
    }

    void SetRepeatCount(int repeatCount)
    {
        repeatCount_ = repeatCount;
    }

    void SetAnimationStart(bool animationStart)
    {
        animationStart_ = animationStart;
    }

    void SetCurrentAnimationHasPlayed(bool currentAnimationHasPlayed)
    {
        currentAnimationHasPlayed_ = currentAnimationHasPlayed;
    }

    void SetCommonSubType(Drawing::DrawingCommonSubType commonSubType)
    {
        commonSubType_ = commonSubType;
    }

private:
    RSAnimationSetting animationSetting_;
    RSHMSymbolData symbolData_;
    RSEffectStrategy effectStrategy_ = RSEffectStrategy::NONE;
    float offsetX_ = 0.0f;
    float offsetY_ = 0.0f;
    uint16_t animationMode_ = 0;
    int repeatCount_ = 1;
    Drawing::DrawingCommonSubType commonSubType_ = Drawing::DrawingCommonSubType::DOWN;

    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>
        animationFunc_ = nullptr;
    uint64_t symblSpanId_ = 0;
    bool animationStart_ = false;
    bool currentAnimationHasPlayed_ = false;
};
}
}
}

#endif