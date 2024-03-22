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
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MAX_WIDTH 10
#define MAX_SLANT 2
#define MAX_WEIGHT 10
#define FIRST_PRIORITY 10000
#define SECOND_PRIORITY 100
#define MULTIPLE 100
#define SUPPORTFILE 1
FontCollection::FontCollection(std::vector<std::shared_ptr<VariantFontStyleSet>> &&fontStyleSets)
    : fontStyleSets_(fontStyleSets)
{
    FillDefaultItalicSupportFile();
    FillDefaultChinesePointUnicode();
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
            return (fs1->GetWeight() != fs2->GetWeight()) ? fs1->GetWeight() < fs2->GetWeight()
                : fs1->GetSlant() < fs2->GetSlant();
        }
    );

    std::vector<int> weights;
    for (auto ty : typefaces) {
        auto weight = ty->GetFontStyle()->GetFontStyle()->GetWeight() / MULTIPLE;
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

void FontCollection::FillDefaultItalicSupportFile()
{
    supportScript_.insert(std::make_pair("Latn", SUPPORTFILE)); // Latn means Latin Language
    supportScript_.insert(std::make_pair("Zyyy", SUPPORTFILE)); // Zyyy means department of mathematics
}

void FontCollection::FillDefaultChinesePointUnicode()
{
    chinesePointUnicode_.insert(std::make_pair(0xFF0C, SUPPORTFILE)); // 0xFF0C is ，
    chinesePointUnicode_.insert(std::make_pair(0x3002, SUPPORTFILE)); // 0x3002 is 。
    chinesePointUnicode_.insert(std::make_pair(0xFF01, SUPPORTFILE)); // 0xFF01 is ！
    chinesePointUnicode_.insert(std::make_pair(0xFF1B, SUPPORTFILE)); // 0xFF1B is ；
    chinesePointUnicode_.insert(std::make_pair(0x3001, SUPPORTFILE)); // 0x3001 is 、
    chinesePointUnicode_.insert(std::make_pair(0xFFE5, SUPPORTFILE)); // 0xFFE5 is ￥
    chinesePointUnicode_.insert(std::make_pair(0xFF08, SUPPORTFILE)); // 0xFF08 is （
    chinesePointUnicode_.insert(std::make_pair(0xFF09, SUPPORTFILE)); // 0xFF09 is ）
    chinesePointUnicode_.insert(std::make_pair(0x300A, SUPPORTFILE)); // 0x300A is 《
    chinesePointUnicode_.insert(std::make_pair(0x300B, SUPPORTFILE)); // 0x300B is 》
    chinesePointUnicode_.insert(std::make_pair(0x3010, SUPPORTFILE)); // 0x3010 is 【
    chinesePointUnicode_.insert(std::make_pair(0x3011, SUPPORTFILE)); // 0x3011 is 】
    chinesePointUnicode_.insert(std::make_pair(0xFF1F, SUPPORTFILE)); // 0xFF1F is ？
    chinesePointUnicode_.insert(std::make_pair(0xFF1A, SUPPORTFILE)); // 0xFF1A is ：
}

int FontCollection::DetectionScript(const std::string &script) const
{
    return supportScript_.find(script)->second;
}

int FontCollection::DetectChinesePointUnicode(uint32_t ch) const
{
    return chinesePointUnicode_.find(ch)->second;
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
            typeface->ComputeFakeryItalic(style.GetFontStyle());
            typeface->ComputeFakery(style.GetWeight());
            return typeface;
        }
    }
    auto typeface = FindThemeTypeface(style);
    if (typeface) {
        typeface->ComputeFakery(style.GetWeight());
        typeface->ComputeFakeryItalic(style.GetFontStyle());
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
            score += (MAX_WIDTH - std::abs(providingStyle.GetFontStyle()->GetWidth() -
                                          matchingStyle->GetFontStyle()->GetWidth())) * SECOND_PRIORITY;
            score += (MAX_SLANT - std::abs(providingStyle.GetFontStyle()->GetSlant() -
                                          matchingStyle->GetFontStyle()->GetSlant())) * FIRST_PRIORITY;
            score += (MAX_WEIGHT - std::abs(providingStyle.GetFontStyle()->GetWeight() / MULTIPLE -
                                           matchingStyle->GetFontStyle()->GetWeight() / MULTIPLE));
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
    TexgineFontStyle tfs = style.ToTexgineFontStyle();
    if (style.GetFontStyle()) {
        std::shared_ptr<TexgineTypeface> fallbackTypeface = nullptr;
        std::shared_ptr<Typeface> typeface = nullptr;
        if (DetectionScript(script) == SUPPORTFILE && DetectChinesePointUnicode(ch) != SUPPORTFILE) {
            fallbackTypeface = fm->MatchFamilyStyle("", tfs);
            if (fallbackTypeface == nullptr || fallbackTypeface->GetTypeface() == nullptr) {
                LOGE("No have fallbackTypeface from matchFamilyStyle");
                return nullptr;
            }
            typeface = std::make_shared<Typeface>(fallbackTypeface);
            typeface->ComputeFakeryItalic(false); // false means value method for obtaining italics
        } else {
            fallbackTypeface = fm->MatchFamilyStyleCharacter("", tfs, bcp47.data(), bcp47.size(), ch);
            if (fallbackTypeface == nullptr || fallbackTypeface->GetTypeface() == nullptr) {
                LOGE("No have fallbackTypeface from matchFamilyStyleCharacter");
                return nullptr;
            }
            typeface = std::make_shared<Typeface>(fallbackTypeface);
            typeface->ComputeFakeryItalic(true); // true means text offset produces italic method
        }

        return typeface;
    }
    std::shared_ptr<TexgineTypeface> fallbackTypeface = fm->MatchFamilyStyleCharacter("", tfs,
        bcp47.data(), bcp47.size(), ch);
    if (fallbackTypeface == nullptr || fallbackTypeface->GetTypeface() == nullptr) {
        return nullptr;
    }
    auto typeface = std::make_shared<Typeface>(fallbackTypeface);
    return typeface;
}

void FontCollection::DisableFallback()
{
    enableFallback_ = false;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
