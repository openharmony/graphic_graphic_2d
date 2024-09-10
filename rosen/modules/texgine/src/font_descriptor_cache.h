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
#include <vector>

#include "font_parser.h"

namespace OHOS::Rosen {
using FontDescSharedPtr = std::shared_ptr<TextEngine::FontParser::FontDescriptor>;
class FontDescriptorCache {
public:
    FontDescriptorCache();
    ~FontDescriptorCache();
    void ParserSystemFonts();
    void MatchFromFontDescriptor(FontDescSharedPtr desc, std::set<FontDescSharedPtr>& result);
    void ClearFontFileCache();
    void Dump();

private:
    void FontDescriptorScatter(FontDescSharedPtr desc);
    bool HandleMapIntersection(std::set<FontDescSharedPtr>& finishRet, const std::string& name,
        std::unordered_map<std::string, std::set<FontDescSharedPtr>>& map);
    bool FilterBoldCache(int weight, std::set<FontDescSharedPtr>& finishRet);
    bool FilterWidthCache(int width, std::set<FontDescSharedPtr>& finishRet);
    bool FilterItalicCache(int italic, std::set<FontDescSharedPtr>& finishRet);
    bool FilterMonoSpaceCache(bool monoSpace, std::set<FontDescSharedPtr>& finishRet);
    bool FilterSymbolicCache(bool symbolic, std::set<FontDescSharedPtr>& finishRet);
    bool FilterTypeStyle(int typeStyle, std::set<FontDescSharedPtr>& finishRet);
    bool IsDefault(FontDescSharedPtr desc);

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
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_FONT_DESCRIPTOR_CACHE_H