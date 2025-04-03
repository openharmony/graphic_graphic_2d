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

#include "hm_symbol_txt.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
static const float MINLOSE = 1E-6;

bool HMSymbolTxt::operator ==(HMSymbolTxt const &symbol) const
{
    if (colorList_.size() != symbol.colorList_.size()) {
        return false;
    }
    if (renderMode_ != symbol.renderMode_ || effectStrategy_ != symbol.effectStrategy_) {
        return false;
    }
    for (size_t i = 0; i < colorList_.size(); i++) {
        if (abs(colorList_[i].a - symbol.colorList_[i].a) > MINLOSE ||
            colorList_[i].r != symbol.colorList_[i].r ||
            colorList_[i].g != symbol.colorList_[i].g ||
            colorList_[i].b != symbol.colorList_[i].b) {
            return false;
        }
    }

    return (animationMode_ == symbol.animationMode_) && (animationStart_ == symbol.animationStart_) &&
        (commonSubType_ == symbol.commonSubType_) && (familyName_ == symbol.familyName_);
}

void HMSymbolTxt::SetRenderColor(const std::vector<RSSColor>& colorList)
{
    colorList_ = colorList;
}

void HMSymbolTxt::SetRenderColor(const RSSColor& colorList)
{
    colorList_ = {colorList};
}

void HMSymbolTxt::SetRenderMode(RSSymbolRenderingStrategy renderMode)
{
    renderMode_ = renderMode;
}

void HMSymbolTxt::SetSymbolEffect(const RSEffectStrategy& effectStrategy)
{
    effectStrategy_ = effectStrategy;
}

std::vector<RSSColor> HMSymbolTxt::GetRenderColor() const
{
    return colorList_;
}

RSSymbolRenderingStrategy HMSymbolTxt::GetRenderMode() const
{
    return renderMode_;
}

RSEffectStrategy HMSymbolTxt::GetEffectStrategy() const
{
    return effectStrategy_;
}

void HMSymbolTxt::SetAnimationMode(uint16_t animationMode)
{
    animationMode_ = animationMode > 0 ? 1 : 0; // 1 is whole or add, 0 is hierarchical or iterate
}

void HMSymbolTxt::SetRepeatCount(int repeatCount)
{
    repeatCount_ = repeatCount;
}

void HMSymbolTxt::SetAnimationStart(bool animationStart)
{
    animationStart_ = animationStart;
}

uint16_t HMSymbolTxt::GetAnimationMode() const
{
    return animationMode_;
}

int HMSymbolTxt::GetRepeatCount() const
{
    return repeatCount_;
}

bool HMSymbolTxt::GetAnimationStart() const
{
    return animationStart_;
}

void HMSymbolTxt::SetCommonSubType(Drawing::DrawingCommonSubType commonSubType)
{
    commonSubType_ = commonSubType;
}

Drawing::DrawingCommonSubType HMSymbolTxt::GetCommonSubType() const
{
    return commonSubType_;
}

void HMSymbolTxt::SetSymbolType(SymbolType symbolType)
{
    symbolType_ = symbolType;
}

SymbolType HMSymbolTxt::GetSymbolType() const
{
    return symbolType_;
}


void HMSymbolTxt::SetSymbolUid(const size_t symbolUid)
{
    symbolUid_ = symbolUid;
}

size_t HMSymbolTxt::GetSymbolUid() const
{
    return symbolUid_;
}

void HMSymbolTxt::SetSymbolBitmap(const SymbolBitmapType& symbolStyleBitmap)
{
    relayoutChangeBitmap_ = symbolStyleBitmap;
}

const SymbolBitmapType& HMSymbolTxt::GetSymbolBitmap() const
{
    return relayoutChangeBitmap_;
}
} // SPText
} // Rosen
} // OHOS
