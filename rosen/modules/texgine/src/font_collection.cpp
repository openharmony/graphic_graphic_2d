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
#include "texgine/typography_types.h"
#include "texgine/utils/exlog.h"
#include "texgine_string.h"

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

std::shared_ptr<Typeface> FontCollection::GetTypefaceForChar(const uint32_t &ch,
    const FontStyles &style, const std::string &script, const std::string &locale) const
{
    for (const auto &fontStyleSet : fontStyleSets_) {
        std::shared_ptr<Typeface> typeface = nullptr;
        struct TypefaceCacheKey key = {.fss = fontStyleSet, .fs = style};
        if (auto it = typefaceCache_.find(key); it != typefaceCache_.end()) {
            typeface = it->second;
        } else {
            if (fontStyleSet == nullptr) {
                continue;
            }
            auto fs = std::make_shared<TexgineFontStyle>();
            *fs = style.ToTexgineFontStyle();
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

    return FindFallBackTypeface(ch, style, script, locale);
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
            score += (MAX_WIDTH - std::abs(providingStyle.GetFontStyle()->width() -
                                          matchingStyle->GetFontStyle()->width())) * SECOND_PRIORITY;
            score += (MAX_SLANT - std::abs(providingStyle.GetFontStyle()->slant() -
                                          matchingStyle->GetFontStyle()->slant())) * FIRST_PRIORITY;
            score += (MAX_WEIGHT - std::abs(providingStyle.GetFontStyle()->weight() / 100 -
                                           matchingStyle->GetFontStyle()->weight() / MULTIPLE));
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
    if (auto it = fallbackCache_.find(key); it != fallbackCache_.end()) {
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

    auto tfs = style.ToTexgineFontStyle();
    auto fallbackTypeface = fm->MatchFamilyStyleCharacter(nullptr, tfs,
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
