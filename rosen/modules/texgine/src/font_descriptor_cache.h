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

#ifndef OHOS_ROSEN_FONT_DESCRIPTOR_CACHE_H
#define OHOS_ROSEN_FONT_DESCRIPTOR_CACHE_H

#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "font_config.h"
#include "font_parser.h"

namespace OHOS::Rosen {
using FontDescSharedPtr = std::shared_ptr<TextEngine::FontParser::FontDescriptor>;
class FontDescriptorCache {
public:
    FontDescriptorCache();
    ~FontDescriptorCache();
    void ParserSystemFonts();
    void ParserStylishFonts();
    void ClearFontFileCache();
    void Dump();
    void GetFontDescSharedPtrByFullName(const std::string& fullName,
        const int32_t& systemFontType, FontDescSharedPtr& result) const;
    void GetSystemFontFullNamesByType(const int32_t& systemFontType, std::unordered_set<std::string>& fontList);

private:
    void FontDescriptorScatter(FontDescSharedPtr desc);
    bool ParserInstallFontsPathList(std::vector<std::string>& fontPathList);
    static bool ParserInstallFontsPathList(TextEngine::FullNameToPath& fontPathList);
    static bool ProcessSystemFontType(const int32_t& systemFontType, int32_t& fontType);
    bool ParseInstallFontDescSharedPtrByName(const std::string& fullName, FontDescSharedPtr& result) const;
    std::unordered_set<std::string> GetInstallFontList();
    std::unordered_set<std::string> GetStylishFontList();
    std::unordered_set<std::string> GetGenericFontList();
    static int32_t WeightAlignment(int32_t weight);

private:
    TextEngine::FontParser parser_;

    struct FontDescriptorEqual {
        bool operator()(const FontDescSharedPtr& lhs, const FontDescSharedPtr& rhs) const
        {
            if (lhs->fontFamily == rhs->fontFamily) {
                return lhs->fullName < rhs->fullName;
            }
            return lhs->fontFamily < rhs->fontFamily;
        }
    };
    std::set<FontDescSharedPtr, FontDescriptorEqual> allFontDescriptor_;
    std::unordered_map<std::string, std::set<FontDescSharedPtr>> fontFamilyMap_;
    std::unordered_map<std::string, std::set<FontDescSharedPtr>> fullNameMap_;
    std::unordered_map<std::string, std::set<FontDescSharedPtr>> postScriptNameMap_;
    std::unordered_map<std::string, std::set<FontDescSharedPtr>> fontSubfamilyNameMap_;
    std::set<FontDescSharedPtr> boldCache_;
    std::set<FontDescSharedPtr> italicCache_;
    std::set<FontDescSharedPtr> monoSpaceCache_;
    std::set<FontDescSharedPtr> symbolicCache_;
    std::unordered_map<std::string, std::set<FontDescSharedPtr>> stylishFullNameMap_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_FONT_DESCRIPTOR_CACHE_H