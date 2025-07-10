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

#include "rosen_text/hm_symbol_txt.h"

#define RENDER_SINGLE 0
#define RENDER_MULTIPLE_COLOR 1
#define RENDER_MULTIPLE_OPACITY 2

namespace OHOS {
namespace Rosen {

// the symbol animation type mapping table
static const std::map<uint32_t, Drawing::DrawingEffectStrategy> EFFECT_TYPES = {
    {0, Drawing::DrawingEffectStrategy::NONE},
    {1, Drawing::DrawingEffectStrategy::SCALE},
    {2, Drawing::DrawingEffectStrategy::VARIABLE_COLOR},
    {3, Drawing::DrawingEffectStrategy::APPEAR},
    {4, Drawing::DrawingEffectStrategy::DISAPPEAR},
    {5, Drawing::DrawingEffectStrategy::BOUNCE},
    {6, Drawing::DrawingEffectStrategy::PULSE},
    {7, Drawing::DrawingEffectStrategy::REPLACE_APPEAR},
    {8, Drawing::DrawingEffectStrategy::DISABLE},
    {9, Drawing::DrawingEffectStrategy::QUICK_REPLACE_APPEAR}
};

void HMSymbolTxt::SetRenderColor(const std::vector<Drawing::DrawingSColor>& colorList)
{
    symbolColor_.colorType = SymbolColorType::COLOR_TYPE;
    symbolColor_.gradients.clear();
    std::vector<std::shared_ptr<SymbolGradient>> gradients;
    for (const auto& color : colorList) {
        auto gradient = std::make_shared<SymbolGradient>();
        std::vector<Drawing::ColorQuad> colors;
        Drawing::Color color1;
        color1.SetRgb(color.r, color.g, color.b, color.a);
        colors.push_back(color1.CastToColorQuad());
        gradient->SetColors(colors);
        gradients.push_back(gradient);
    }
    symbolColor_.gradients = gradients;
}

void HMSymbolTxt::SetRenderColor(const std::vector<Drawing::Color>& colorList)
{
    symbolColor_.colorType = SymbolColorType::COLOR_TYPE;
    symbolColor_.gradients.clear();
    std::vector<std::shared_ptr<SymbolGradient>> gradients;
    for (auto color : colorList) {
        auto gradient = std::make_shared<SymbolGradient>();
        std::vector<Drawing::ColorQuad> colors;
        Drawing::Color color1;
        color1.SetRgb(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
        colors.push_back(color1.CastToColorQuad());
        gradient->SetColors(colors);
        gradients.push_back(gradient);
    }
    symbolColor_.gradients = gradients;
}

void HMSymbolTxt::SetRenderColor(const Drawing::Color& color)
{
    symbolColor_.colorType = SymbolColorType::COLOR_TYPE;
    symbolColor_.gradients.clear();
    std::vector<std::shared_ptr<SymbolGradient>> gradients;
    auto gradient = std::make_shared<SymbolGradient>();
    std::vector<Drawing::ColorQuad> colors;
    Drawing::Color color1;
    color1.SetRgb(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
    colors.push_back(color1.CastToColorQuad());
    gradient->SetColors(colors);
    gradients.push_back(gradient);
    symbolColor_.gradients = gradients;
}

void HMSymbolTxt::SetRenderColor(const Drawing::DrawingSColor& colorList)
{
    symbolColor_.colorType = SymbolColorType::COLOR_TYPE;
    symbolColor_.gradients.clear();
    std::vector<std::shared_ptr<SymbolGradient>> gradients;
    auto gradient = std::make_shared<SymbolGradient>();
    std::vector<Drawing::ColorQuad> colors;
    Drawing::Color color1;
    color1.SetRgb(colorList.r, colorList.g, colorList.b, colorList.a);
    colors.push_back(color1.CastToColorQuad());
    gradient->SetColors(colors);
    gradients.push_back(gradient);
    symbolColor_.gradients = gradients;
}

void HMSymbolTxt::SetRenderMode(const uint32_t& renderMode)
{
    switch (renderMode) {
        case RENDER_SINGLE:
            renderMode_ = Drawing::DrawingSymbolRenderingStrategy::SINGLE;
            break;
        case RENDER_MULTIPLE_OPACITY:
            renderMode_ = Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY;
            break;
        case RENDER_MULTIPLE_COLOR:
            renderMode_ = Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_COLOR;
            break;
        default:
            break;
    }
}

void HMSymbolTxt::SetSymbolEffect(const uint32_t& effectStrategy)
{
    auto iter = EFFECT_TYPES.find(effectStrategy);
    if (iter != EFFECT_TYPES.end()) {
        effectStrategy_ = iter->second;
    }
}

std::vector<Drawing::DrawingSColor> HMSymbolTxt::GetRenderColor() const
{
    std::vector<Drawing::DrawingSColor> colorList;
    for (const auto& gradient : symbolColor_.gradients) {
        bool isInvalid = gradient == nullptr || gradient->GetColors().empty();
        if (isInvalid) {
            continue;
        }
        auto gradientColor = gradient->GetColors()[0];
        Drawing::Color color(gradientColor);
        Drawing::DrawingSColor scolor = {color.GetAlphaF(), color.GetRed(), color.GetGreen(), color.GetBlue()};
        colorList.push_back(scolor);
    }
    return colorList;
}

Drawing::DrawingSymbolRenderingStrategy HMSymbolTxt::GetRenderMode() const
{
    return renderMode_;
}

Drawing::DrawingEffectStrategy HMSymbolTxt::GetEffectStrategy() const
{
    return effectStrategy_;
}

void HMSymbolTxt::SetAnimationMode(const uint16_t animationMode)
{
    animationMode_ = animationMode > 0 ? 1 : 0; // 1 is whole or iteratuve, 0 is hierarchical or cumulative
}

void HMSymbolTxt::SetRepeatCount(const int repeatCount)
{
    repeatCount_ = repeatCount;
}

void HMSymbolTxt::SetAnimationStart(const bool animationStart)
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

void HMSymbolTxt::SetVisualMode(const VisualMode visual)
{
    visualMap_.clear();
    if (visual == VisualMode::VISUAL_SMALL) {
        visualMap_["ss01"] = 1;
    }

    if (visual == VisualMode::VISUAL_LARGER) {
        visualMap_["ss02"] = 1;
    }
}

std::map<std::string, int> HMSymbolTxt::GetVisualMap() const
{
    return visualMap_;
}

// set common subtype of symbol animation attribute
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

size_t HMSymbolTxt::GetSymbolUid() const
{
    return symbolUid_;
}

void HMSymbolTxt::SetSymbolUid(const size_t symbolUid)
{
    symbolUid_ = symbolUid;
}

const SymbolBitmapType& HMSymbolTxt::GetSymbolBitmap() const
{
    return relayoutChangeBitmap_;
}

void HMSymbolTxt::SetSymbolBitmap(const SymbolBitmapType& symbolStyleBitmap)
{
    relayoutChangeBitmap_ = symbolStyleBitmap;
}

SymbolColor HMSymbolTxt::GetSymbolColor() const
{
    return symbolColor_;
}

void HMSymbolTxt::SetSymbolColor(const SymbolColor& symbolColor)
{
    symbolColor_ = symbolColor;
}

void HMSymbolTxt::SetSymbolShadow(const std::optional<SymbolShadow>& symbolShadow)
{
    symbolShadow_ = symbolShadow;
}

const std::optional<SymbolShadow>& HMSymbolTxt::GetSymbolShadow() const
{
    return symbolShadow_;
}
} // namespace Rosen
} // namespace OHOS