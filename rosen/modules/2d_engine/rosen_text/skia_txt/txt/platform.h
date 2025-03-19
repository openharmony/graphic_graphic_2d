/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_MODULES_SPTEXT_PLATFORM_H
#define ROSEN_MODULES_SPTEXT_PLATFORM_H

#include <shared_mutex>
#include <string>
#include <vector>

#include "utils.h"

#include "text/font_mgr.h"

#ifndef TEXTING_API
#ifdef _WIN32
#define TEXTING_EXPORT __attribute__((dllexport))
#define TEXTING_IMPORT __attribute__((dllimport))
#else
#define TEXTING_EXPORT __attribute__((visibility("default")))
#define TEXTING_IMPORT __attribute__((visibility("default")))
#endif
#ifdef MODULE_TEXTING
#define TEXTING_API TEXTING_EXPORT
#else
#define TEXTING_API TEXTING_IMPORT
#endif
#endif

namespace OHOS {
namespace Rosen {
namespace SPText {
constexpr const char* OHOS_THEME_FONT = "OhosThemeFont";
class TEXTING_API DefaultFamilyNameMgr {
public:
    static DefaultFamilyNameMgr& GetInstance();
    std::vector<std::string> GetDefaultFontFamilies() const;
    std::vector<std::string> GetThemeFontFamilies() const;
    void ModifyThemeFontFamilies(size_t index);
    static std::string GenerateThemeFamilyName(size_t index);

private:
    DefaultFamilyNameMgr();
    DefaultFamilyNameMgr(const DefaultFamilyNameMgr&) = delete;
    const DefaultFamilyNameMgr& operator=(const DefaultFamilyNameMgr&) = delete;
    std::vector<std::string> themeFamilies_;
    std::vector<std::string> defaultFamilies_;
    mutable std::shared_mutex lock_;
};

std::shared_ptr<Drawing::FontMgr> GetDefaultFontManager();
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_PLATFORM_H
