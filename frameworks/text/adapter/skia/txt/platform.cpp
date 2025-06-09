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

#include "txt/platform.h"

#include <mutex>

namespace OHOS {
namespace Rosen {
namespace SPText {
DefaultFamilyNameMgr::DefaultFamilyNameMgr()
{
#ifdef TEXT_SYSTEM_OHOS
    defaultFamilies_ = { "HarmonyOS-Sans" };
#else
    defaultFamilies_ = { "sans-serif" };
#endif
}

DefaultFamilyNameMgr& DefaultFamilyNameMgr::GetInstance()
{
    static DefaultFamilyNameMgr instance;
    return instance;
}

std::vector<std::string> DefaultFamilyNameMgr::GetDefaultFontFamilies() const
{
    std::shared_lock<std::shared_mutex> readLock(lock_);
    std::vector<std::string> res(themeFamilies_);
    res.insert(res.end(), defaultFamilies_.begin(), defaultFamilies_.end());
    return res;
}

std::vector<std::string> DefaultFamilyNameMgr::GetThemeFontFamilies() const
{
    std::shared_lock<std::shared_mutex> readLock(lock_);
    return themeFamilies_;
}

void DefaultFamilyNameMgr::ModifyThemeFontFamilies(size_t index)
{
    std::unique_lock<std::shared_mutex> writeLock(lock_);
    themeFamilies_.clear();
    for (size_t i = 0; i < index; i += 1) {
        themeFamilies_.emplace_back(GenerateThemeFamilyName(i));
    }
}

std::string DefaultFamilyNameMgr::GenerateThemeFamilyName(size_t index)
{
    if (index == 0) {
        return OHOS_THEME_FONT;
    }
    std::string res { OHOS_THEME_FONT };
    res.push_back('0' + index);
    return res;
}

std::shared_ptr<Drawing::FontMgr> GetDefaultFontManager()
{
    return Drawing::FontMgr::CreateDefaultFontMgr();
}

bool DefaultFamilyNameMgr::IsThemeFontFamily(std::string familyName)
{
    std::transform(familyName.begin(), familyName.end(), familyName.begin(), ::tolower);
    return familyName.find(OHOS_THEME_FONT_LOW) == 0;
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
