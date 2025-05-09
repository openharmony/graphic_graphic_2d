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
#include <cstdint>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

#include "font_config.h"
#include "text/common_utils.h"
#include "text/font_style.h"
#include "text/font_mgr.h"
#include "utils/text_log.h"

#define INSTALL_FONT_CONFIG_FILE "/data/service/el1/public/for-all-app/fonts/install_fontconfig.json"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t WEIGHT_400 = 400;
constexpr int SPECIAL_WEIGHT_DIFF = 50;
constexpr int WEIGHT_MODULE = 100;
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
    stylishFullNameMap_.clear();
    dynamicFullNameMap_.clear();
}

void FontDescriptorCache::ParserSystemFonts()
{
    // System fonts have already been parsed
    if (!fullNameMap_.empty()) {
        return;
    }
    for (auto& item : parser_.GetSystemFonts()) {
        FontDescriptorScatter(item);
    }
    Dump();
}

void FontDescriptorCache::ParserStylishFonts()
{
    // Stylish fonts have already been parsed
    if (!stylishFullNameMap_.empty()) {
        return;
    }
    std::vector<TextEngine::FontParser::FontDescriptor> descriptors = parser_.GetVisibilityFonts(TextEngine::ENGLISH);
    for (const auto& descriptor : descriptors) {
        FontDescSharedPtr descriptorPtr = std::make_shared<TextEngine::FontParser::FontDescriptor>(descriptor);
        descriptorPtr->weight = WeightAlignment(descriptorPtr->weight);
        stylishFullNameMap_[descriptorPtr->fullName].emplace(descriptorPtr);
    }
}

void FontDescriptorCache::ParserFontsByFontType(int32_t fontType)
{
    if (static_cast<uint32_t>(fontType) & TextEngine::FontParser::SystemFontType::GENERIC) {
        ParserSystemFonts();
    }
    if (static_cast<uint32_t>(fontType) & TextEngine::FontParser::SystemFontType::STYLISH) {
        ParserStylishFonts();
    }
}

void FontDescriptorCache::FontDescriptorScatter(FontDescSharedPtr desc)
{
    auto ret = allFontDescriptor_.emplace(desc);
    if (!ret.second) {
        return;
    }

    auto handleMapScatter = [desc](auto& map, const auto& key) {
        map[key].emplace(desc);
    };

    handleMapScatter(fontFamilyMap_, desc->fontFamily);
    handleMapScatter(fullNameMap_, desc->fullName);
    handleMapScatter(postScriptNameMap_, desc->postScriptName);
    handleMapScatter(fontSubfamilyNameMap_, desc->fontSubfamily);

    desc->weight = WeightAlignment(desc->weight);
    if (static_cast<uint32_t>(desc->weight) > WEIGHT_400) {
        boldCache_.emplace(desc);
    }

    if (desc->italic != 0) {
        italicCache_.emplace(desc);
    }

    if (desc->monoSpace) {
        monoSpaceCache_.emplace(desc);
    }

    if (desc->symbolic) {
        symbolicCache_.emplace(desc);
    }
}

bool FontDescriptorCache::ParserInstallFontsPathList(std::vector<std::string>& fontPathList)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr = Drawing::FontMgr::CreateDynamicFontMgr();
    if (fontMgr == nullptr) {
        return false;
    }
    int ret = fontMgr->ParseInstallFontConfig(INSTALL_FONT_CONFIG_FILE, fontPathList);
    return ret == Drawing::FontCheckCode::SUCCESSED;
}

bool FontDescriptorCache::ParserInstallFontsPathList(std::unordered_map<std::string, std::string>& fontPathList)
{
    return TextEngine::FontConfigJson::ParseInstallConfig(INSTALL_FONT_CONFIG_FILE, fontPathList) == 0;
}

std::unordered_set<std::string> FontDescriptorCache::GetInstallFontList()
{
    std::unordered_set<std::string> fullNameList;
    TextEngine::FontFileMap fullNameToPath;
    if (!ParserInstallFontsPathList(fullNameToPath)) {
        TEXT_LOGE("Failed to parser install fonts path list");
        return fullNameList;
    }
    for (const auto& item : fullNameToPath) {
        fullNameList.emplace(item.first);
    }
    return fullNameList;
}

std::unordered_set<std::string> FontDescriptorCache::GetStylishFontList()
{
    std::unordered_set<std::string> fullNameList;
    for (const auto& temp : stylishFullNameMap_) {
        fullNameList.emplace(temp.first);
    }
    return fullNameList;
}

std::unordered_set<std::string> FontDescriptorCache::GetGenericFontList()
{
    std::unordered_set<std::string> fullNameList;
    for (const auto& temp : allFontDescriptor_) {
        fullNameList.emplace(temp->fullName);
    }
    return fullNameList;
}

std::unordered_set<std::string> FontDescriptorCache::GetDynamicFontList()
{
    std::unordered_set<std::string> fullNameList;
    for (const auto& temp : dynamicFullNameMap_) {
        fullNameList.emplace(temp.first);
    }
    return fullNameList;
}

bool FontDescriptorCache::ProcessSystemFontType(int32_t systemFontType, int32_t& fontType)
{
    if ((static_cast<uint32_t>(systemFontType) & (TextEngine::FontParser::SystemFontType::ALL |
        TextEngine::FontParser::SystemFontType::GENERIC |
        TextEngine::FontParser::SystemFontType::STYLISH |
        TextEngine::FontParser::SystemFontType::INSTALLED |
        TextEngine::FontParser::SystemFontType::CUSTOMIZED)) != systemFontType) {
        TEXT_LOGE("Invalid system font type %{public}d", systemFontType);
        return false;
    }
    fontType = systemFontType;
    if (static_cast<uint32_t>(systemFontType) & TextEngine::FontParser::SystemFontType::ALL) {
        fontType = TextEngine::FontParser::SystemFontType::GENERIC |
            TextEngine::FontParser::SystemFontType::STYLISH |
            TextEngine::FontParser::SystemFontType::INSTALLED |
            TextEngine::FontParser::SystemFontType::CUSTOMIZED;
    }
    return true;
}

void FontDescriptorCache::GetSystemFontFullNamesByType(
    int32_t systemFontType, std::unordered_set<std::string> &fontList)
{
    if (systemFontType < 0) {
        TEXT_LOGE("Invalid system font type %{public}d", systemFontType);
        return;
    }
    int32_t fontType = 0;
    if (!ProcessSystemFontType(systemFontType, fontType)) {
        fontList.clear();
        return;
    }

    ParserFontsByFontType(fontType);

    uint32_t fontCategory = static_cast<uint32_t>(fontType);
    if (fontCategory & TextEngine::FontParser::SystemFontType::GENERIC) {
        auto fullNameList = GetGenericFontList();
        fontList.insert(fullNameList.begin(), fullNameList.end());
    }

    if (fontCategory & TextEngine::FontParser::SystemFontType::STYLISH) {
        auto fullNameList = GetStylishFontList();
        fontList.insert(fullNameList.begin(), fullNameList.end());
    }

    if (fontCategory & TextEngine::FontParser::SystemFontType::INSTALLED) {
        auto fullNameList = GetInstallFontList();
        fontList.insert(fullNameList.begin(), fullNameList.end());
    }

    if (fontCategory & TextEngine::FontParser::SystemFontType::CUSTOMIZED) {
        auto fullNameList = GetDynamicFontList();
        fontList.insert(fullNameList.begin(), fullNameList.end());
    }
}

bool FontDescriptorCache::ParseInstallFontDescSharedPtrByName(const std::string& fullName, FontDescSharedPtr& result)
{
    TextEngine::FontFileMap fullNameToPath;
    if (!ParserInstallFontsPathList(fullNameToPath)) {
        TEXT_LOGE("Failed to parser install fonts path list");
        return false;
    }
    std::vector<FontDescSharedPtr> descriptors = parser_.ParserFontDescriptorsFromPath(fullNameToPath[fullName]);
    for (const auto& item : descriptors) {
        if (item->fullName == fullName) {
            item->weight = WeightAlignment(item->weight);
            result = item;
            return true;
        }
    }
    TEXT_LOGE_LIMIT3_MIN("Failed to parser installed font descriptor by full name: %{public}s", fullName.c_str());
    return false;
}

bool FontDescriptorCache::GetFontTypeFromParams(const std::string& fullName,
    int32_t systemFontType, int32_t& fontType)
{
    if (fullName.empty()) {
        TEXT_LOGE("Empty full name");
        return false;
    }
    if (!ProcessSystemFontType(systemFontType, fontType)) {
        return false;
    }
    if (systemFontType < 0) {
        TEXT_LOGE("Invalid system font type %{public}d", systemFontType);
        return false;
    }
    return true;
}

void FontDescriptorCache::GetFontDescSharedPtrByFullName(const std::string& fullName,
    int32_t systemFontType, FontDescSharedPtr& result)
{
    int32_t fontType = 0;
    if (!GetFontTypeFromParams(fullName, systemFontType, fontType)) {
        return;
    }

    ParserFontsByFontType(fontType);

    auto tryFindFontDescriptor = [&fullName, &result](const std::unordered_map<std::string,
        std::set<FontDescSharedPtr>>& map) -> bool {
        auto it = map.find(fullName);
        if (it != map.end()) {
            result = *(it->second.begin());
            return true;
        }
        return false;
    };

    uint32_t  fontCategory = static_cast<uint32_t>(fontType);
    if ((fontCategory & TextEngine::FontParser::SystemFontType::GENERIC) &&
        tryFindFontDescriptor(fullNameMap_)) {
        return;
    }
    if ((fontCategory & TextEngine::FontParser::SystemFontType::STYLISH) &&
        tryFindFontDescriptor(stylishFullNameMap_)) {
        return;
    }
    if ((fontCategory & TextEngine::FontParser::SystemFontType::INSTALLED) &&
        ParseInstallFontDescSharedPtrByName(fullName, result)) {
        return;
    }
    if ((fontCategory & TextEngine::FontParser::SystemFontType::CUSTOMIZED)) {
        auto it = dynamicFullNameMap_.find(fullName);
        if (it != dynamicFullNameMap_.end()) {
            result = dynamicFullNameMap_[fullName];
            return;
        }
    }
    TEXT_LOGD("Failed to get fontDescriptor by full name: %{public}s", fullName.c_str());
    result = nullptr;
}

void FontDescriptorCache::CacheDynamicTypeface(std::shared_ptr<Drawing::Typeface> typeface,
    const std::string &familyName)
{
    std::vector<std::shared_ptr<TextEngine::FontParser::FontDescriptor>> fontDescArr =
        parser_.CreateFontDescriptors({typeface});
    for (auto fontDesc : fontDescArr) {
        fontDesc->fontFamily = familyName;
        dynamicFullNameMap_[fontDesc->fontFamily] = fontDesc;
    }
}

void FontDescriptorCache::DeleteDynamicTypefaceFromCache(const std::string &familyName)
{
    dynamicFullNameMap_.erase(familyName);
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
    } else if (static_cast<uint32_t>(weight) > WEIGHT_400) {
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

bool FontDescriptorCache::IsDefault(FontDescSharedPtr desc)
{
    if (desc->fontFamily.empty() && desc->fullName.empty() && desc->postScriptName.empty()
        && desc->fontSubfamily.empty() && desc->weight == 0 && desc->width == 0 && desc->italic == 0
        && !desc->monoSpace && !desc->symbolic) {
        return true;
    }
    return false;
}

void FontDescriptorCache::MatchFromFontDescriptor(FontDescSharedPtr desc, std::set<FontDescSharedPtr>& result)
{
    if (desc == nullptr) {
        TEXT_LOGE("Null desc");
        return;
    }
    ParserSystemFonts();
    if (IsDefault(desc)) {
        result = std::set<FontDescSharedPtr>(allFontDescriptor_.begin(), allFontDescriptor_.end());
        return;
    }
    desc->weight = (desc->weight > 0) ? WeightAlignment(desc->weight) : desc->weight;
    std::set<FontDescSharedPtr> finishRet;
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->fontFamily, fontFamilyMap_), return,
        "Failed to match font family");
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->fullName, fullNameMap_), return, "Failed to match fullName");
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->postScriptName, postScriptNameMap_), return,
        "Failed to match post script name");
    TEXT_INFO_CHECK(HandleMapIntersection(finishRet, desc->fontSubfamily, fontSubfamilyNameMap_), return,
        "Failed to match font subfamily");

    TEXT_CHECK(FilterBoldCache(desc->weight, finishRet), return);
    TEXT_CHECK(FilterWidthCache(desc->width, finishRet), return);
    TEXT_CHECK(FilterItalicCache(desc->italic, finishRet), return);
    TEXT_CHECK(FilterMonoSpaceCache(desc->monoSpace, finishRet), return);
    TEXT_CHECK(FilterSymbolicCache(desc->symbolic, finishRet), return);
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

int32_t FontDescriptorCache::WeightAlignment(int32_t weight)
{
    if (weight < Drawing::FontStyle::THIN_WEIGHT) {
        return Drawing::FontStyle::THIN_WEIGHT;
    }

    if (weight > Drawing::FontStyle::EXTRA_BLACK_WEIGHT) {
        return Drawing::FontStyle::EXTRA_BLACK_WEIGHT;
    }

    if ((weight % WEIGHT_MODULE) == 0) {
        return weight;
    }

    static const std::vector<int> weightType = {
        Drawing::FontStyle::THIN_WEIGHT,
        Drawing::FontStyle::EXTRA_LIGHT_WEIGHT,
        Drawing::FontStyle::LIGHT_WEIGHT,
        Drawing::FontStyle::NORMAL_WEIGHT,
        Drawing::FontStyle::MEDIUM_WEIGHT,
        Drawing::FontStyle::SEMI_BOLD_WEIGHT,
        Drawing::FontStyle::BOLD_WEIGHT,
        Drawing::FontStyle::EXTRA_BOLD_WEIGHT,
        Drawing::FontStyle::BLACK_WEIGHT,
        Drawing::FontStyle::EXTRA_BLACK_WEIGHT
    };
    // Obtain weight ranges for non-whole hundred values
    auto it = std::lower_bound(weightType.begin(), weightType.end(), weight);
    std::vector<int> targetRange = { *(it - 1), *it };

    /**
     * When the font weight is less than NORMAL_WEIGHT, round down as much as possible;
     * when the font weight exceeds NORMAL_WEIGHT, round up where possible. For example, when weight is 360,
     * the final font weight is set to 300; when weight is 620, the final font weight is set to 700.
     */
    uint32_t minDiff = 0xFFFFFFFF;
    int resultWeight = 0;
    for (const auto& item : targetRange) {
        /**
         * The maximum weight is EXTRA_BLACK_WEIGHT (1000), when weight and item are at the different
         * side of NORMAL_WEIGHT, the weight difference between them should be more than 500 (1000/2).
         */
        uint32_t weightDiff = 0;
        constexpr int kWeightDiffThreshold = Drawing::FontStyle::EXTRA_BLACK_WEIGHT / 2;
        if ((weight == Drawing::FontStyle::NORMAL_WEIGHT && item == Drawing::FontStyle::MEDIUM_WEIGHT) ||
            (weight == Drawing::FontStyle::MEDIUM_WEIGHT && item == Drawing::FontStyle::NORMAL_WEIGHT)) {
            weightDiff = static_cast<uint32_t>(SPECIAL_WEIGHT_DIFF);
        } else if (weight <= Drawing::FontStyle::NORMAL_WEIGHT) {
            weightDiff = (item <= weight) ? static_cast<uint32_t>(weight - item) :
                static_cast<uint32_t>(item - weight + kWeightDiffThreshold);
        } else if (weight > Drawing::FontStyle::NORMAL_WEIGHT) {
            weightDiff = (item >= weight) ? static_cast<uint32_t>(item - weight) :
                static_cast<uint32_t>(weight - item + kWeightDiffThreshold);
        }

        // Retain the font weight with the smallest difference
        if (weightDiff < minDiff) {
            minDiff = weightDiff;
            resultWeight = item;
        }
    }
    return resultWeight;
}
}