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

#include <mutex>

#include "utils/text_log.h"

namespace OHOS::Rosen {
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
    std::unique_lock guard(parserMtx_);
    descCache_.ParserSystemFonts();
    descCache_.ParserStylishFonts();
}

void FontDescriptorMgr::ClearFontFileCache()
{
    std::unique_lock guard(parserMtx_);
    descCache_.ClearFontFileCache();
}

void FontDescriptorMgr::GetFontDescSharedPtrByFullName(const std::string& fullName,
    const int32_t& systemFontType, FontDescSharedPtr& result)
{
    std::shared_lock guard(parserMtx_);
    descCache_.GetFontDescSharedPtrByFullName(fullName, systemFontType, result);
}

void FontDescriptorMgr::GetSystemFontFullNamesByType(
    const int32_t &systemFontType, std::unordered_set<std::string> &fontList)
{
    std::unique_lock guard(parserMtx_);
    descCache_.GetSystemFontFullNamesByType(systemFontType, fontList);
}
} // namespace OHOS::Rosen
