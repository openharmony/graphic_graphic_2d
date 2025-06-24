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

#ifndef LIB_TXT_SRC_SYMBOL_ENGINE_HM_SYMBOL_TXT_H
#define LIB_TXT_SRC_SYMBOL_ENGINE_HM_SYMBOL_TXT_H

#include <bitset>
#include <iostream>
#include <vector>

#include "drawing.h"
#include "rosen_text/symbol_constants.h"
#include "rosen_text/hm_symbol_txt.h"
#include "symbol_gradient.h"

namespace OHOS {
namespace Rosen {
namespace SPText {

class HMSymbolTxt {
public:
    HMSymbolTxt() {}
    ~HMSymbolTxt() {}

    void SetRenderColor(const std::vector<RSSColor>& colorList);

    void SetRenderColor(const RSSColor& colorList);

    void SetRenderMode(RSSymbolRenderingStrategy renderMode);

    void SetSymbolEffect(const RSEffectStrategy& effectStrategy);

    // set animation mode: the 1 is whole or iteratuve, 0 is hierarchical or cumulative
    void SetAnimationMode(uint16_t animationMode);

    void SetRepeatCount(int repeatCount);

    void SetAnimationStart(bool animationStart);

    // set common subtype of symbol animation attribute
    void SetCommonSubType(Drawing::DrawingCommonSubType commonSubType);

    void SetSymbolType(SymbolType symbolType);

    void SetSymbolUid(const size_t symbolUid);

    void SetSymbolBitmap(const SymbolBitmapType& symbolStyleBitmap);

    bool operator ==(HMSymbolTxt const &symbol) const;

    void SetGradients(const std::vector<std::shared_ptr<SymbolGradient>>& gradients);

    std::vector<std::shared_ptr<SymbolGradient>> GetGradients() const;

    void SetSymbolColor(const SymbolColor& symbolColor);

    SymbolColor GetSymbolColor() const;

    std::vector<RSSColor> GetRenderColor() const;

    RSSymbolRenderingStrategy GetRenderMode() const;

    RSEffectStrategy GetEffectStrategy() const;

    uint16_t GetAnimationMode() const;

    int GetRepeatCount() const;

    bool GetAnimationStart() const;

    Drawing::DrawingCommonSubType GetCommonSubType() const;

    SymbolType GetSymbolType() const;

    size_t GetSymbolUid() const;

    const SymbolBitmapType& GetSymbolBitmap() const;

    void SetSymbolShadow(const std::optional<SymbolShadow>& symbolShadow);

    const std::optional<SymbolShadow>& GetSymbolShadow() const;

    std::string familyName_;

private:
    SymbolColor symbolColor_;
    RSSymbolRenderingStrategy renderMode_ = RSSymbolRenderingStrategy::SINGLE;
    RSEffectStrategy effectStrategy_ = RSEffectStrategy::NONE;
    size_t symbolUid_ = 0;
    uint16_t animationMode_ = 0;
    int repeatCount_ = 1;
    bool animationStart_ = false;
    Drawing::DrawingCommonSubType commonSubType_ = Drawing::DrawingCommonSubType::DOWN;
    SymbolType symbolType_{SymbolType::SYSTEM};
    SymbolBitmapType relayoutChangeBitmap_;
    std::optional<SymbolShadow> symbolShadow_;
};
}
}
}
#endif