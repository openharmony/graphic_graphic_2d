/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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
#include "text_font_utils.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace skt = skia::textlayout;
static const int WEIGHT_BASE = 100;

FontWeight TextFontUtils::GetTxtFontWeight(int fontWeight)
{
    constexpr int minWeight = static_cast<int>(FontWeight::W100);
    constexpr int maxWeight = static_cast<int>(FontWeight::W900);

    int weight = std::clamp((fontWeight - WEIGHT_BASE) / WEIGHT_BASE, minWeight, maxWeight);
    return static_cast<FontWeight>(weight);
}

FontStyle TextFontUtils::GetTxtFontStyle(RSFontStyle::Slant slant)
{
    return slant == RSFontStyle::Slant::UPRIGHT_SLANT ? FontStyle::NORMAL : FontStyle::ITALIC;
}

RSFontStyle::Weight TextFontUtils::GetSkiaFontWeight(FontWeight spFontWeight)
{
    constexpr int minWeight = static_cast<int>(FontWeight::W100);
    constexpr int maxWeight = static_cast<int>(FontWeight::W900);

    int fontWeight = static_cast<int>(spFontWeight);
    fontWeight = std::clamp(fontWeight, minWeight, maxWeight) + 1;

    return static_cast<RSFontStyle::Weight>(fontWeight * WEIGHT_BASE);
}

RSFontStyle::Slant TextFontUtils::GetSkiaFontSlant(FontStyle spSlant)
{
    return spSlant == FontStyle::NORMAL ? RSFontStyle::UPRIGHT_SLANT : RSFontStyle::ITALIC_SLANT;
}

RSFontStyle::Width TextFontUtils::GetSkiaFontWidth(FontWidth spFontWidth)
{
    return RSFontStyle::Width(static_cast<int>(spFontWidth));
}

std::vector<SkString> TextFontUtils::GetSkiaFontfamilies(const std::vector<std::string>& spFontFamiles)
{
    std::vector<SkString> skiaFontFamiles;
    std::transform(spFontFamiles.begin(), spFontFamiles.end(), std::back_inserter(skiaFontFamiles),
        [](const std::string& f) { return SkString(f.c_str()); });
    return skiaFontFamiles;
}

void TextFontUtils::MakeFontArguments(skt::TextStyle& skStyle, const FontVariations& fontVariations)
{
    constexpr size_t axisLen = 4;

    std::vector<SkFontArguments::VariationPosition::Coordinate> coordinates;
    for (const auto& [axis, value] : fontVariations.GetAxisValues()) {
        if (axis.length() == axisLen) {
            coordinates.push_back({
                SkSetFourByteTag(axis[0], axis[1], axis[2], axis[3]),
                value,
            });
        }
    }
    SkFontArguments::VariationPosition position = { coordinates.data(), static_cast<int>(coordinates.size()) };

    SkFontArguments arguments;
    arguments.setVariationDesignPosition(position);
    skStyle.setFontArguments(arguments);
}

skt::TextShadow TextFontUtils::MakeTextShadow(const TextShadow& txtShadow)
{
    skt::TextShadow shadow;
    shadow.fOffset = txtShadow.offset;
    shadow.fBlurSigma = txtShadow.blurSigma;
    shadow.fColor = txtShadow.color;
    return shadow;
}

RSFontStyle::Slant TextFontUtils::ConvertToRSFontSlant(FontStyle fontStyle)
{
    RSFontStyle::Slant slant;
    switch (fontStyle) {
        case FontStyle::NORMAL:
            slant = RSFontStyle::Slant::UPRIGHT_SLANT;
            break;
        case FontStyle::ITALIC:
            slant = RSFontStyle::Slant::ITALIC_SLANT;
            break;
        case FontStyle::OBLIQUE:
            slant = RSFontStyle::Slant::OBLIQUE_SLANT;
            break;
        default:
            slant = RSFontStyle::Slant::UPRIGHT_SLANT;
    }
    return slant;
}

int TextFontUtils::ConvertToSkFontWeight(FontWeight fontWeight)
{
    return static_cast<int>(fontWeight) * WEIGHT_BASE + WEIGHT_BASE;
}

RSFontStyle TextFontUtils::MakeFontStyle(FontWeight fontWeight, FontWidth fontWidth, FontStyle fontStyle)
{
    auto weight = TextFontUtils::ConvertToSkFontWeight(fontWeight);
    auto width = static_cast<RSFontStyle::Width>(fontWidth);
    auto slant = TextFontUtils::ConvertToRSFontSlant(fontStyle);
    return RSFontStyle(weight, width, slant);
}

}  // namespace SPText
}  // namespace Rosen
}  // namespace OHOS