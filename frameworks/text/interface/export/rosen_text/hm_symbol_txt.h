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

#ifndef ROSEN_TEXT_EXPORT_ROSEN_TEXT_HM_SYMBOL_TXT_H
#define ROSEN_TEXT_EXPORT_ROSEN_TEXT_HM_SYMBOL_TXT_H

#include <bitset>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "draw/color.h"
#include "symbol_constants.h"
#include "symbol_gradient.h"
#include "text/hm_symbol.h"


namespace OHOS {
namespace Rosen {
enum class RelayoutSymbolStyleAttribute {
    EFFECT_STRATEGY = 0,
    ANIMATION_MODE = 1,
    ANIMATION_START = 2,
    COMMONSUB_TYPE = 3,
    COLOR_LIST = 4,
    RENDER_MODE = 5,
    GRADIENT_COLOR = 6,
    SYMBOL_SHADOW = 7,

    SYMBOL_ATTRIBUTE_BUTT,
};

enum VisualMode {
    VISUAL_MEDIUM = 0,
    VISUAL_SMALL = 1,
    VISUAL_LARGER = 2
};

using SymbolBitmapType = std::bitset<static_cast<size_t>(RelayoutSymbolStyleAttribute::SYMBOL_ATTRIBUTE_BUTT)>;

struct SymbolColor {
    SymbolColorType colorType = SymbolColorType::COLOR_TYPE;
    std::vector<std::shared_ptr<SymbolGradient>> gradients;
};

class RS_EXPORT HMSymbolTxt {
public:
    HMSymbolTxt() {}
    HMSymbolTxt(const HMSymbolTxt& other) = default;
    HMSymbolTxt& operator=(const HMSymbolTxt& other) = default;
    ~HMSymbolTxt() {}

    void SetRenderColor(const std::vector<Drawing::DrawingSColor>& colorList);

    void SetRenderColor(const std::vector<Drawing::Color>& colorList);

    void SetRenderColor(const Drawing::Color& color);

    void SetRenderColor(const Drawing::DrawingSColor& colorList);

    void SetRenderMode(const uint32_t& renderMode);

    void SetSymbolEffect(const uint32_t& effectStrategy);

    // set animation mode: the 1 is whole or iteratuve, 0 is hierarchical or cumulative
    void SetAnimationMode(const uint16_t animationMode);

    void SetRepeatCount(const int repeatCount);

    void SetAnimationStart(const bool animationStart);

    // set common subtype of symbol animation attribute
    void SetCommonSubType(Drawing::DrawingCommonSubType commonSubType);

    void SetVisualMode(const VisualMode visual);

    void SetSymbolType(SymbolType symbolType);

    void SetSymbolUid(const size_t symbolUid);

    void SetSymbolBitmap(const SymbolBitmapType& symbolStyleBitmap);

    std::vector<Drawing::DrawingSColor> GetRenderColor() const;

    Drawing::DrawingSymbolRenderingStrategy GetRenderMode() const;

    Drawing::DrawingEffectStrategy GetEffectStrategy() const;

    uint16_t GetAnimationMode() const;

    int GetRepeatCount() const;

    bool GetAnimationStart() const;

    std::map<std::string, int> GetVisualMap() const;

    Drawing::DrawingCommonSubType GetCommonSubType() const;

    SymbolType GetSymbolType() const;

    size_t GetSymbolUid() const;

    const SymbolBitmapType& GetSymbolBitmap() const;

    SymbolColor GetSymbolColor() const;

    void SetSymbolColor(const SymbolColor& symbolColor);

    void SetSymbolShadow(const std::optional<SymbolShadow>& symbolShadow);

    const std::optional<SymbolShadow>& GetSymbolShadow() const;
private:
    SymbolColor symbolColor_;
    Drawing::DrawingSymbolRenderingStrategy renderMode_ = Drawing::DrawingSymbolRenderingStrategy::SINGLE;
    Drawing::DrawingEffectStrategy effectStrategy_ = Drawing::DrawingEffectStrategy::NONE;

    // animationMode_ is the implementation mode of the animation effect:
    // common_animations: the 0 is the byLayer effect and 1 is the wholeSymbol effect;
    // variable_color : the 0 is the cumulative effect and 1 is the iteratuve effect.
    size_t symbolUid_ { 0 };
    uint16_t animationMode_ { 0 };
    int repeatCount_ { 1 };
    bool animationStart_ { false };
    std::map<std::string, int> visualMap_;
    Drawing::DrawingCommonSubType commonSubType_ = Drawing::DrawingCommonSubType::DOWN;
    SymbolType symbolType_{SymbolType::SYSTEM};
    SymbolBitmapType relayoutChangeBitmap_;
    std::optional<SymbolShadow> symbolShadow_;
};
}
}
#endif