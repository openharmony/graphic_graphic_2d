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

#ifdef BUILD_NON_SDK_VER
#include "parameter.h"
#endif

#include "utils/text_log.h"

namespace OHOS::Rosen {
namespace {
#ifdef BUILD_NON_SDK_VER
const char* LOCALE_KEY = "persist.global.locale";
const char* DEFAULT_LOCALE_KEY = "const.global.locale";
constexpr int CONFIG_LEN = 128;
#endif
}
FontDescriptorMgr& FontDescriptorMgr::GetInstance()
{
    static FontDescriptorMgr instance;
    return instance;
}

FontDescriptorMgr::FontDescriptorMgr()
{
    ParseAllFontSource();
}

FontDescriptorMgr::~FontDescriptorMgr() {}

void FontDescriptorMgr::ParseAllFontSource()
{
    std::unique_lock<std::mutex> guard(parserMtx_);
    locale_ = GetSystemLocale();
    descCache_.SetLocale(locale_);
    descCache_.ParserSystemFonts();
    descCache_.ParserStylishFonts();
}

void FontDescriptorMgr::ClearFontFileCache()
{
    std::unique_lock<std::mutex> guard(parserMtx_);
    descCache_.ClearFontFileCache();
}

void FontDescriptorMgr::MatchFontDescriptors(FontDescSharedPtr desc, std::set<FontDescSharedPtr>& descs)
{
    CheckLocale();
    std::unique_lock<std::mutex> guard(parserMtx_);
    descCache_.MatchFromFontDescriptor(desc, descs);
}

void FontDescriptorMgr::GetFontDescSharedPtrByFullName(const std::string& fullName,
    const int32_t& systemFontType, FontDescSharedPtr& result)
{
    CheckLocale();
    std::unique_lock<std::mutex> guard(parserMtx_);
    descCache_.GetFontDescSharedPtrByFullName(fullName, systemFontType, result);
}

void FontDescriptorMgr::GetSystemFontFullNamesByType(
    const int32_t &systemFontType, std::unordered_set<std::string> &fontList)
{
    CheckLocale();
    std::unique_lock<std::mutex> guard(parserMtx_);
    descCache_.GetSystemFontFullNamesByType(systemFontType, fontList);
}

std::string FontDescriptorMgr::GetSystemLocale()
{
#ifdef BUILD_NON_SDK_VER
    char param[CONFIG_LEN] = { 0 };
    if (GetParameter(LOCALE_KEY, "", param, CONFIG_LEN - 1) > 0) {
        TEXT_LOGD("Current system locale: %{public}s", param);
        return std::string(param);
    }

    if (GetParameter(DEFAULT_LOCALE_KEY, "", param, CONFIG_LEN - 1) > 0) {
        TEXT_LOGD("Current system default locale: %{public}s", param);
        return std::string(param);
    }
#endif
    return TextEngine::SIMPLIFIED_CHINESE;
}

void FontDescriptorMgr::CheckLocale()
{
    if (GetSystemLocale() == locale_) {
        return;
    }
    ClearFontFileCache();
    ParseAllFontSource();
}
} // namespace OHOS::Rosen
