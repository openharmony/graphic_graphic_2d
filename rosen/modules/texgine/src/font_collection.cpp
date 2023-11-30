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

#include "font_collection.h"

#include "texgine/dynamic_font_provider.h"
#include "texgine/system_font_provider.h"
#include "texgine/theme_font_provider.h"
#include "texgine/typography_types.h"
#include "texgine/utils/exlog.h"
#include "texgine_string.h"
#include "texgine_font.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MAX_WIDTH 10
#define MAX_SLANT 2
#define MAX_WEIGHT 10
#define FIRST_PRIORITY 10000
#define SECOND_PRIORITY 100
#define MULTIPLE 100

FontCollection::FontCollection(std::vector<std::shared_ptr<VariantFontStyleSet>> &&fontStyleSets)
    : fontStyleSets_(fontStyleSets)
{
}

void FontCollection::SortTypeface(FontStyles &style) const
{
    std::vector<std::shared_ptr<TexgineTypeface>> typefaces;
    for (const auto &fontStyleSet : fontStyleSets_) {
        if (fontStyleSet == nullptr) {
            continue;
        }
        for (int i = 0; i < fontStyleSet->Count(); i++) {
            typefaces.push_back(fontStyleSet->CreateTypeface(i));
        }
    }

    std::sort(typefaces.begin(), typefaces.end(),
        [](std::shared_ptr<TexgineTypeface> &ty1, const std::shared_ptr<TexgineTypeface> &ty2) {
            auto fs1 = ty1->GetFontStyle()->GetFontStyle();
            auto fs2 = ty2->GetFontStyle()->GetFontStyle();
#ifndef USE_ROSEN_DRAWING
            return (fs1->weight() != fs2->weight()) ? fs1->weight() < fs2->weight() : fs1->slant() < fs2->slant();
#else
            return (fs1->GetWeight() != fs2->GetWeight()) ? fs1->GetWeight() < fs2->GetWeight()
                : fs1->GetSlant() < fs2->GetSlant();
#endif
        }
    );

    std::vector<int> weights;
    for (auto ty : typefaces) {
#ifndef USE_ROSEN_DRAWING
        auto weight = ty->GetFontStyle()->GetFontStyle()->weight() / MULTIPLE;
#else
        auto weight = ty->GetFontStyle()->GetFontStyle()->GetWeight() / MULTIPLE;
#endif
        weights.push_back(weight);
    }

    for (size_t i = 0; i < weights.size(); i++) {
        if (weights[i] == style.GetWeight()) {
            break;
        }
        if (weights[i] > style.GetWeight()) {
            if (i > 0) {
                style.SetWeight(weights[i - 1]);
                break;
            }
        }
    }
}

std::shared_ptr<Typeface> FontCollection::GetTypefaceForChar(const uint32_t &ch, FontStyles &style,
    const std::string &script, const std::string &locale) const
{
    SortTypeface(style);
    auto fs = std::make_shared<TexgineFontStyle>();
    *fs = style.ToTexgineFontStyle();
    for (const auto &fontStyleSet : fontStyleSets_) {
        std::shared_ptr<Typeface> typeface = nullptr;
        struct TypefaceCacheKey key = {.fss = fontStyleSet, .fs = style};
        if (auto it = typefaceCache_.find(key); it != typefaceCache_.end()) {
            typeface->ComputeFakeryItalic(style.GetFontStyle());
            typeface->ComputeFakery(style.GetWeight());
            typeface = it->second;
        } else {
            if (fontStyleSet == nullptr) {
                continue;
            }
            auto texgineTypeface = fontStyleSet->MatchStyle(fs);
            if (texgineTypeface == nullptr || texgineTypeface->GetTypeface() == nullptr) {
                continue;
            }
            typeface = std::make_shared<Typeface>(texgineTypeface);
            typefaceCache_[key] = typeface;
        }

        if (typeface->Has(ch)) {
            return typeface;
        }
    }
    auto typeface = FindThemeTypeface(style);
    if (typeface) {
        typeface->ComputeFakeryItalic(style.GetFontStyle());
        typeface->ComputeFakery(style.GetWeight());
    }
    if (typeface == nullptr) {
        typeface = FindFallBackTypeface(ch, style, script, locale);
    }
    if (typeface == nullptr) {
        typeface = GetTypefaceForFontStyles(style, script, locale);
    }
    return typeface;
}

std::shared_ptr<Typeface> FontCollection::FindThemeTypeface(const FontStyles &style) const
{
    std::shared_ptr<VariantFontStyleSet> styleSet = ThemeFontProvider::GetInstance()->MatchFamily("");
    if (styleSet != nullptr) {
        auto fs = std::make_shared<TexgineFontStyle>();
        *fs = style.ToTexgineFontStyle();
        auto texgineTypeface = styleSet->MatchStyle(fs);
        if (texgineTypeface != nullptr && texgineTypeface->GetTypeface() != nullptr) {
            return std::make_shared<Typeface>(texgineTypeface);
        }
    }
    return nullptr;
}

std::shared_ptr<Typeface> FontCollection::GetTypefaceForFontStyles(const FontStyles &style,
    const std::string &script, const std::string &locale) const
{
    auto providingStyle = style.ToTexgineFontStyle();

    int bestScore = 0;
    int bestIndex = 0;
    std::shared_ptr<VariantFontStyleSet> bestFontStyleSet = nullptr;
    for (auto &fontStyleSet : fontStyleSets_) {
        auto count = fontStyleSet->Count();
        for (auto i = 0; i < count; i++) {
            auto matchingStyle = std::make_shared<TexgineFontStyle>();
            auto styleName = std::make_shared<TexgineString>();
            fontStyleSet->GetStyle(i, matchingStyle, styleName);

            int score = 0;
#ifndef USE_ROSEN_DRAWING
            score += (MAX_WIDTH - std::abs(providingStyle.GetFontStyle()->width() -
                                          matchingStyle->GetFontStyle()->width())) * SECOND_PRIORITY;
            score += (MAX_SLANT - std::abs(providingStyle.GetFontStyle()->slant() -
                                          matchingStyle->GetFontStyle()->slant())) * FIRST_PRIORITY;
            score += (MAX_WEIGHT - std::abs(providingStyle.GetFontStyle()->weight() / MULTIPLE -
                                           matchingStyle->GetFontStyle()->weight() / MULTIPLE));
#else
            score += (MAX_WIDTH - std::abs(providingStyle.GetFontStyle()->GetWidth() -
                                          matchingStyle->GetFontStyle()->GetWidth())) * SECOND_PRIORITY;
            score += (MAX_SLANT - std::abs(providingStyle.GetFontStyle()->GetSlant() -
                                          matchingStyle->GetFontStyle()->GetSlant())) * FIRST_PRIORITY;
            score += (MAX_WEIGHT - std::abs(providingStyle.GetFontStyle()->GetWeight() / MULTIPLE -
                                           matchingStyle->GetFontStyle()->GetWeight() / MULTIPLE));
#endif
            if (score > bestScore) {
                bestScore = score;
                bestIndex = i;
                bestFontStyleSet = fontStyleSet;
            }
        }
    }

    if (bestFontStyleSet != nullptr &&
        (bestFontStyleSet->TryToTexgineFontStyleSet() != nullptr || bestFontStyleSet->TryToDynamicFontStyleSet())) {
        auto ttf = bestFontStyleSet->CreateTypeface(bestIndex);
        return std::make_shared<Typeface>(ttf);
    }

    return FindFallBackTypeface(' ', style, script, locale);
}

std::shared_ptr<Typeface> FontCollection::FindFallBackTypeface(const uint32_t &ch, const FontStyles &style,
    const std::string &script, const std::string &locale) const
{
    if (!enableFallback_) {
        return nullptr;
    }
    // fallback cache
    struct FallbackCacheKey key = {.script = script, .locale = locale, .fs = style};
    if (auto it = fallbackCache_.find(key); it != fallbackCache_.end() && it->second->Has(ch)) {
        return it->second;
    }

    // fallback
    std::vector<const char *> bcp47;
    if (!script.empty()) {
        bcp47.push_back(script.data());
    } else if (!locale.empty()) {
        bcp47.push_back(locale.data());
    }

    auto fm = TexgineFontManager::RefDefault();
    if (fm == nullptr) {
        return nullptr;
    }

    if (style.GetFontStyle()) {
        typefaceCache_.clear();
        TexgineFontStyle tfs = style.ToTexgineFontStyle();
        std::shared_ptr<TexgineTypeface> fallbackTypeface = fm->MatchFamilyStyleCharacter("", tfs,
            bcp47.data(), bcp47.size(), ch);
        if (fallbackTypeface == nullptr || fallbackTypeface->GetTypeface() == nullptr) {
            return nullptr;
        }
        fallbackTypeface->InputOriginalStyle(true); // true means record italic style
        auto typeface = std::make_shared<Typeface>(fallbackTypeface);
        return typeface;
    }

    TexgineFontStyle tfs = style.ToTexgineFontStyle();
    std::shared_ptr<TexgineTypeface> fallbackTypeface = fm->MatchFamilyStyleCharacter("", tfs,
        bcp47.data(), bcp47.size(), ch);
    if (fallbackTypeface == nullptr || fallbackTypeface->GetTypeface() == nullptr) {
        return nullptr;
    }
    auto typeface = std::make_shared<Typeface>(fallbackTypeface);
    fallbackCache_[key] = typeface;
    return typeface;
}

void FontCollection::DisableFallback()
{
    enableFallback_ = false;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
