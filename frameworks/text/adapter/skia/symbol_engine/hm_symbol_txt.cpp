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

bool HMSymbolTxt::operator ==(HMSymbolTxt const &symbol) const
{
    auto symbolColor = symbol.GetSymbolColor();
    bool isUnequal = symbolColor_.colorType != symbolColor.colorType ||
        symbolColor_.gradients.size() != symbolColor.gradients.size();
    if (isUnequal) {
        return false;
    }
    if (renderMode_ != symbol.renderMode_ || effectStrategy_ != symbol.effectStrategy_) {
        return false;
    }
    for (size_t i = 0; i < symbolColor_.gradients.size(); i++) {
        bool isUnequal = symbolColor_.gradients[i] == nullptr || symbolColor.gradients[i] == nullptr;
        if (isUnequal) {
            return false;
        }
        if (!symbolColor_.gradients[i]->IsNearlyEqual(symbolColor.gradients[i])) {
            return false;
        }
    }

    return (animationMode_ == symbol.animationMode_) && (animationStart_ == symbol.animationStart_) &&
        (commonSubType_ == symbol.commonSubType_) && (familyName_ == symbol.familyName_);
}

void HMSymbolTxt::SetRenderColor(const std::vector<RSSColor>& colorList)
{
    symbolColor_.colorType = SymbolColorType::COLOR_TYPE;
    std::vector<std::shared_ptr<SymbolGradient>> gradients;
    for (auto color : colorList) {
        auto gradient =  std::make_shared<SymbolGradient>();
        std::vector<Drawing::ColorQuad> colors;
        Drawing::Color color1;
        color1.SetRgb(color.r, color.g, color.b);
        color1.SetAlphaF(color.a);
        colors.push_back(color1.CastToColorQuad());
        gradient->SetColors(colors);
        gradients.push_back(gradient);
    }
    symbolColor_.gradients = gradients;
}

void HMSymbolTxt::SetRenderColor(const RSSColor& colorList)
{
    std::vector<RSSColor> colors = {colorList};
    SetRenderColor(colors);
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
    std::vector<RSSColor> colorList;
    for (const auto& gradient : symbolColor_.gradients) {
        bool isInvalid = gradient == nullptr || gradient->GetColors().empty();
        if (isInvalid) {
            continue;
        }
        auto gradientColor = gradient->GetColors()[0];
        Drawing::Color color(gradientColor);
        RSSColor scolor = {color.GetAlphaF(), color.GetRed(), color.GetGreen(), color.GetBlue()};
        colorList.push_back(scolor);
    }
    return colorList;
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

void HMSymbolTxt::SetGradients(const std::vector<std::shared_ptr<SymbolGradient>>& gradients)
{
    symbolColor_.colorType = SymbolColorType::GRADIENT_TYPE;
    symbolColor_.gradients = gradients;
}

std::vector<std::shared_ptr<SymbolGradient>> HMSymbolTxt::GetGradients() const
{
    return symbolColor_.gradients;
}

void HMSymbolTxt::SetSymbolColor(const SymbolColor& symbolColor)
{
    symbolColor_ = symbolColor;
}

SymbolColor HMSymbolTxt::GetSymbolColor() const
{
    return symbolColor_;
}
} // SPText
} // Rosen
} // OHOS
