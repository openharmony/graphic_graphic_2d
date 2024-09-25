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

#include "font_descriptor_cache.h"

#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "font_config.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t WEIGHT_400 = 400;
}

FontDescriptorCache::FontDescriptorCache() {}

FontDescriptorCache::~FontDescriptorCache() {}

void FontDescriptorCache::ClearFontFileCache()
{
    allFontDescriptor_.clear();
    fontFamilyMap_.clear();
    fullNameMap_.clear();
    postScriptNameMap_.clear();
    fontSubfamilyNameMap_.clear();
    boldCache_.clear();
    italicCache_.clear();
    monoSpaceCache_.clear();
    symbolicCache_.clear();
}

void FontDescriptorCache::ParserSystemFonts()
{
    for (auto& item : parser_.GetSystemFonts()) {
        FontDescriptorScatter(item);
    }
    Dump();
}

void FontDescriptorCache::FontDescriptorScatter(FontDescSharedPtr desc)
{
    auto ret = allFontDescriptor_.emplace(desc);
    if (!ret.second) {
        return;
    }

    auto handleMapScatter = [&](auto& map, const auto& key) {
        map[key].emplace(desc);
    };

    handleMapScatter(fontFamilyMap_, desc->fontFamily);
    handleMapScatter(fullNameMap_, desc->fullName);
    handleMapScatter(postScriptNameMap_, desc->postScriptName);
    handleMapScatter(fontSubfamilyNameMap_, desc->fontSubfamily);

    if (desc->weight > WEIGHT_400) {
        desc->typeStyle |= TextEngine::FontParser::FontTypeStyle::BOLD;
        boldCache_.emplace(desc);
    }

    if (desc->italic != 0) {
        desc->typeStyle |= TextEngine::FontParser::FontTypeStyle::ITALIC;
        italicCache_.emplace(desc);
    }

    if (desc->monoSpace) {
        monoSpaceCache_.emplace(desc);
    }

    if (desc->symbolic) {
        symbolicCache_.emplace(desc);
    }
}

bool FontDescriptorCache::HandleMapIntersection(std::set<FontDescSharedPtr>& finishRet, const std::string& name,
    std::unordered_map<std::string, std::set<FontDescSharedPtr>>& map)
{
    if (name.empty()) {
        return true;
    }
    auto iter = map.find(name);
    if (iter == map.end()) {
        return false;
    }
    if (finishRet.empty()) {
        finishRet = iter->second;
    } else {
        std::set<FontDescSharedPtr> temp;
        std::set_intersection(iter->second.begin(), iter->second.end(), finishRet.begin(), finishRet.end(),
            std::insert_iterator<std::set<FontDescSharedPtr>>(temp, temp.begin()));
        if (temp.empty()) {
            return false;
        }
        finishRet = std::move(temp);
    }
    return true;
}

bool FontDescriptorCache::FilterBoldCache(int weight, std::set<FontDescSharedPtr>& finishRet)
{
    if (weight < 0) {
        return false;
    }

    if (weight == 0) {
        return true;
    }

    std::set<FontDescSharedPtr> temp;
    std::set<FontDescSharedPtr>::iterator begin;
    std::set<FontDescSharedPtr>::iterator end;
    if (!finishRet.empty()) {
        begin = finishRet.begin();
        end = finishRet.end();
    } else if (weight > WEIGHT_400) {
        begin = boldCache_.begin();
        end = boldCache_.end();
    } else {
        begin = allFontDescriptor_.begin();
        end = allFontDescriptor_.end();
    }
    std::for_each(begin, end, [&](FontDescSharedPtr item) {
        if (item->weight == weight) {
            temp.emplace(item);
        }
    });

    if (temp.empty()) {
        TEXT_LOGD("Failed to match weight");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool FontDescriptorCache::FilterWidthCache(int width, std::set<FontDescSharedPtr>& finishRet)
{
    if (width < 0) {
        return false;
    }

    if (width == 0) {
        return true;
    }

    std::set<FontDescSharedPtr> temp;
    std::set<FontDescSharedPtr>::iterator begin;
    std::set<FontDescSharedPtr>::iterator end;
    if (!finishRet.empty()) {
        begin = finishRet.begin();
        end = finishRet.end();
    } else {
        begin = allFontDescriptor_.begin();
        end = allFontDescriptor_.end();
    }
    std::for_each(begin, end, [&](FontDescSharedPtr item) {
        if (item->width == width) {
            temp.emplace(item);
        }
    });

    if (temp.empty()) {
        TEXT_LOGD("Failed to match width");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool FontDescriptorCache::FilterItalicCache(int italic, std::set<FontDescSharedPtr>& finishRet)
{
    if (italic == 0) {
        return true;
    }
    std::set<FontDescSharedPtr> temp;
    if (!finishRet.empty()) {
        std::for_each(finishRet.begin(), finishRet.end(), [&](FontDescSharedPtr item) {
            if (item->italic != 0) {
                temp.emplace(item);
            }
        });
    } else {
        temp = italicCache_;
    }
    if (temp.empty()) {
        TEXT_LOGD("Failed to match italic");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool FontDescriptorCache::FilterMonoSpaceCache(bool monoSpace, std::set<FontDescSharedPtr>& finishRet)
{
    if (!monoSpace) {
        return true;
    }

    std::set<FontDescSharedPtr> temp;
    if (!finishRet.empty()) {
        std::for_each(finishRet.begin(), finishRet.end(), [&](FontDescSharedPtr item) {
            if (item->monoSpace) {
                temp.emplace(item);
            }
        });
    } else {
        temp = monoSpaceCache_;
    }
    if (temp.empty()) {
        TEXT_LOGD("Failed to match monoSpace");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool FontDescriptorCache::FilterSymbolicCache(bool symbolic, std::set<FontDescSharedPtr>& finishRet)
{
    if (!symbolic) {
        return true;
    }
    std::set<FontDescSharedPtr> temp;
    if (!finishRet.empty()) {
        std::for_each(finishRet.begin(), finishRet.end(), [&](FontDescSharedPtr item) {
            if (item->symbolic) {
                temp.emplace(item);
            }
        });
    } else {
        temp = symbolicCache_;
    }
    if (temp.empty()) {
        TEXT_LOGD("Failed to match symbolic");
        return false;
    }
    finishRet = std::move(temp);
    return true;
}

bool FontDescriptorCache::FilterTypeStyle(int typeStyle, std::set<FontDescSharedPtr>& finishRet)
{
    if (typeStyle < 0) {
        return false;
    }
    if (typeStyle == 0) {
        return true;
    }

    bool italicFlag = typeStyle & TextEngine::FontParser::FontTypeStyle::ITALIC;
    bool boldFlag = typeStyle & TextEngine::FontParser::FontTypeStyle::BOLD;
    auto handleCache = [&](const std::set<FontDescSharedPtr>& cache, const char* cacheName) {
        if (cache.empty()) {
            TEXT_LOGD("%{public}s is empty", cacheName);
            return false;
        }
        if (finishRet.empty()) {
            finishRet = cache;
        } else {
            std::set<FontDescSharedPtr> temp;
            std::set_intersection(finishRet.begin(), finishRet.end(), cache.begin(), cache.end(),
                                  std::inserter(temp, temp.begin()));
            if (temp.empty()) {
                TEXT_LOGD("Failed to match typeStyle %{public}s", cacheName);
                return false;
            }
            finishRet = std::move(temp);
        }
        return true;
    };
    if (italicFlag && !handleCache(italicCache_, "italic")) {
        return false;
    }
    if (boldFlag && !handleCache(boldCache_, "bold")) {
        return false;
    }
    return true;
}

bool FontDescriptorCache::IsDefault(FontDescSharedPtr desc)
{
    if (desc->fontFamily.empty() && desc->fullName.empty() && desc->postScriptName.empty()
        && desc->fontSubfamily.empty() && desc->weight == 0 && desc->width == 0 && desc->italic == 0
        && !desc->monoSpace && !desc->symbolic && desc->typeStyle == 0) {
        return true;
    }
    return false;
}

void FontDescriptorCache::MatchFromFontDescriptor(FontDescSharedPtr desc, std::set<FontDescSharedPtr>& result)
{
    if (desc == nullptr) {
        TEXT_LOGE("desc is nullptr");
        return;
    }

    if (IsDefault(desc)) {
        result = std::set<FontDescSharedPtr>(allFontDescriptor_.begin(), allFontDescriptor_.end());
        return;
    }

    std::set<FontDescSharedPtr> finishRet;
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->fontFamily, fontFamilyMap_), return,
        "Failed to match fontFamily");
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->fullName, fullNameMap_), return, "Failed to match fullName");
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->postScriptName, postScriptNameMap_), return,
        "Failed to match postScriptName");
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->fontSubfamily, fontSubfamilyNameMap_), return,
        "Failed to match fontSubfamily");

    TEXT_CHECK(FilterBoldCache(desc->weight, finishRet), return);
    TEXT_CHECK(FilterWidthCache(desc->width, finishRet), return);
    TEXT_CHECK(FilterItalicCache(desc->italic, finishRet), return);
    TEXT_CHECK(FilterMonoSpaceCache(desc->monoSpace, finishRet), return);
    TEXT_CHECK(FilterSymbolicCache(desc->symbolic, finishRet), return);
    TEXT_CHECK(FilterTypeStyle(desc->typeStyle, finishRet), return);
    result = std::move(finishRet);
}

void FontDescriptorCache::Dump()
{
    TEXT_LOGD("allFontDescriptor size: %{public}zu, fontFamilyMap size: %{public}zu, fullNameMap size: %{public}zu \
        postScriptNameMap size: %{public}zu, fontSubfamilyNameMap size: %{public}zu, boldCache size: %{public}zu \
        italicCache size: %{public}zu, monoSpaceCache size: %{public}zu, symbolicCache size: %{public}zu",
        allFontDescriptor_.size(), fontFamilyMap_.size(), fullNameMap_.size(), postScriptNameMap_.size(),
        fontSubfamilyNameMap_.size(), boldCache_.size(), italicCache_.size(), monoSpaceCache_.size(),
        symbolicCache_.size());
}
}