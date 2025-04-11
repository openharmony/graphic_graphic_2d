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

#ifndef OHOS_ROSEN_FONT_DESCRIPTOR_MGR_H
#define OHOS_ROSEN_FONT_DESCRIPTOR_MGR_H

#ifdef BUILD_NON_SDK_VER
#include <nocopyable.h>
#endif

#include <mutex>

#include "font_descriptor_cache.h"
#include "font_parser.h"

namespace OHOS::Rosen {
using FontDescSharedPtr = std::shared_ptr<TextEngine::FontParser::FontDescriptor>;

class FontDescriptorMgr {
public:
    static FontDescriptorMgr& GetInstance();
    virtual ~FontDescriptorMgr();

#ifdef BUILD_NON_SDK_VER
    DISALLOW_COPY_AND_MOVE(FontDescriptorMgr);
#endif

    void ParseAllFontSource();
    void MatchFontDescriptors(FontDescSharedPtr desc, std::set<FontDescSharedPtr>& descs);
    void GetFontDescSharedPtrByFullName(const std::string& fullName,
        const int32_t& systemFontType, FontDescSharedPtr& result);
    void GetSystemFontFullNamesByType(const int32_t& systemFontType, std::unordered_set<std::string>& fontList);
    void ClearFontFileCache();

    void CacheDynamicTypeface(std::shared_ptr<Drawing::Typeface> typeface, const std::string &familyName);
    void DeleteDynamicTypefaceFromCache(const std::string &familyName);

private:
    FontDescriptorMgr() = default;
    FontDescriptorCache descCache_;
    std::mutex parserMtx_;
};
} // namespace OHOS::Rosen
#define FontDescriptorMgrInstance OHOS::Rosen::FontDescriptorMgr::GetInstance()
#endif // OHOS_ROSEN_FONT_DESCRIPTOR_MGR_H