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

#ifndef ROSEN_MODULES_TEXGINE_SRC_FONT_COLLECTION_H
#define ROSEN_MODULES_TEXGINE_SRC_FONT_COLLECTION_H

#include <memory>
#include <string>
#include <vector>

#include "font_styles.h"
#include "texgine_font_manager.h"
#include "typeface.h"
#include "variant_font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class FontCollection {
public:
    FontCollection(std::vector<std::shared_ptr<VariantFontStyleSet>> &&fontStyleSets);

    std::shared_ptr<Typeface> GetTypefaceForChar(const uint32_t &ch, FontStyles &style,
        const std::string &script, const std::string &locale) const;

    std::shared_ptr<Typeface> GetTypefaceForFontStyles(const FontStyles &style, const std::string &script,
        const std::string &locale) const;

    std::shared_ptr<Typeface> FindThemeTypeface(const FontStyles &style) const;

    std::shared_ptr<Typeface> FindFallBackTypeface(const uint32_t &ch, const FontStyles &style,
        const std::string &script, const std::string &locale) const;

    void DisableFallback();
    int DetectionScript(const std::string &script) const;
    int DetectChinesePointUnicode(uint32_t ch) const;
private:
    void SortTypeface(FontStyles &style) const;
    void FillDefaultItalicSupportFile();
    void FillDefaultChinesePointUnicode();
private:
    bool enableFallback_ = true;
    std::vector<std::shared_ptr<VariantFontStyleSet>> fontStyleSets_;

    struct TypefaceCacheKey {
        std::shared_ptr<VariantFontStyleSet> fss = {};
        FontStyles fs = {};

        bool operator <(const struct TypefaceCacheKey &rhs) const
        {
            return fss != rhs.fss ? fss < rhs.fss : fs < rhs.fs;
        }
    };
    static inline std::map<struct TypefaceCacheKey, std::shared_ptr<Typeface>> typefaceCache_;

    struct FallbackCacheKey {
        std::string script = "";
        std::string locale = "";
        FontStyles fs = {};

        bool operator <(const struct FallbackCacheKey &rhs) const
        {
            return script < rhs.script || locale < rhs.locale || fs < rhs.fs;
        }
    };
    std::map<std::string, int> supportScript_;
    std::map<uint32_t, int> chinesePointUnicode_;
    static inline std::map<struct FallbackCacheKey, std::shared_ptr<Typeface>> fallbackCache_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_FONT_COLLECTION_H
