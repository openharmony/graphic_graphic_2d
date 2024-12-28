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


#include "font_descriptor_mgr.h"

#include "utils/text_log.h"

namespace OHOS::Rosen {
FontDescriptorMgr& FontDescriptorMgr::GetInstance()
{
    static FontDescriptorMgr instance;
    return instance;
}

FontDescriptorMgr::FontDescriptorMgr()
{
    
}

FontDescriptorMgr::~FontDescriptorMgr() {}

void FontDescriptorMgr::ParseAllFontSource()
{
    descCache_.ParserSystemFonts();
    descCache_.ParserStylishFonts();
    hasParseAllFont = true;
}

void FontDescriptorMgr::ClearFontFileCache()
{
    std::unique_lock<std::mutex> guard(parserMtx_);
    descCache_.ClearFontFileCache();
    hasParseAllFont = false;
}

bool FontDescriptorMgr::ProcessSystemFontType(const int32_t& systemFontType, int32_t& fontType)
{
    if ((static_cast<uint32_t>(systemFontType) & (TextEngine::FontParser::SystemFontType::ALL |
        TextEngine::FontParser::SystemFontType::GENERIC |
        TextEngine::FontParser::SystemFontType::STYLISH |
        TextEngine::FontParser::SystemFontType::INSTALLED |
        TextEngine::FontParser::SystemFontType::CUSTOMIZED)) != systemFontType) {
        TEXT_LOGE("SystemFontType is invalid, systemFontType: %{public}d", systemFontType);
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

void FontDescriptorMgr::MatchFontDescriptors(FontDescSharedPtr desc, std::set<FontDescSharedPtr>& descs)
{
    std::unique_lock<std::mutex> guard(parserMtx_);
    if (!hasParseAllFont) {   
        ParseAllFontSource();
    }
    descCache_.MatchFromFontDescriptor(desc, descs);
}

void FontDescriptorMgr::GetFontDescSharedPtrByFullName(const std::string& fullName,
    const int32_t& systemFontType, FontDescSharedPtr& result)
{
    std::unique_lock<std::mutex> guard(parserMtx_);
    int32_t fontType = 0;
    if (!ProcessSystemFontType(systemFontType, fontType)) {
        result = nullptr;
        return;
    }
    if (((fontType & TextEngine::FontParser::SystemFontType::GENERIC) ||
        (fontType & TextEngine::FontParser::SystemFontType::STYLISH) ||
        (fontType & TextEngine::FontParser::SystemFontType::INSTALLED) ||
        (fontType & TextEngine::FontParser::SystemFontType::ALL)) && !hasParseAllFont) {
        ParseAllFontSource();
    }
    
    descCache_.GetFontDescSharedPtrByFullName(fullName, systemFontType, result);
}

void FontDescriptorMgr::GetSystemFontFullNamesByType(
    const int32_t &systemFontType, std::unordered_set<std::string> &fontList)
{
    std::unique_lock<std::mutex> guard(parserMtx_);
    int32_t fontType = 0;
    if (!ProcessSystemFontType(systemFontType, fontType)) {
        fontList.clear();
        return;
    }
    if (((fontType & TextEngine::FontParser::SystemFontType::GENERIC) ||
        (fontType & TextEngine::FontParser::SystemFontType::STYLISH) ||
        (fontType & TextEngine::FontParser::SystemFontType::INSTALLED) ||
        (fontType & TextEngine::FontParser::SystemFontType::ALL)) && !hasParseAllFont) {
        ParseAllFontSource();
    }
    descCache_.GetSystemFontFullNamesByType(systemFontType, fontList);
}

void FontDescriptorMgr::CacheDynamicTypeface(std::shared_ptr<Drawing::Typeface> typeface, const std::string &familyName)
{
    if (typeface == nullptr) {
        return;
    }
    std::unique_lock<std::mutex> guard(parserMtx_);
    descCache_.CacheDynamicTypeface(typeface, familyName);
}

void FontDescriptorMgr::DeleteDynamicTypefaceFromCache(const std::string &familyName)
{
    std::unique_lock<std::mutex> guard(parserMtx_);
    descCache_.DeleteDynamicTypefaceFromCache(familyName);
}
} // namespace OHOS::Rosen
