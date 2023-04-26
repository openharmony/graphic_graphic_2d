/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "texgine_font_manager.h"

#include <mutex>

#include "texgine_font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
TexgineFontManager::TexgineFontManager()
{
    fontMgr_ = SkFontMgr::RefDefault();
}

std::shared_ptr<TexgineFontManager> TexgineFontManager::RefDefault()
{
    auto manager = std::make_shared<TexgineFontManager>();
    manager->SetFontMgr(SkFontMgr::RefDefault());
    return manager;
}

sk_sp<SkFontMgr> TexgineFontManager::GetFontMgr() const
{
    return fontMgr_;
}

void TexgineFontManager::SetFontMgr(const sk_sp<SkFontMgr> mgr)
{
    fontMgr_ = mgr;
}

std::shared_ptr<TexgineTypeface> TexgineFontManager::MatchFamilyStyleCharacter(const std::string &familyName,
    TexgineFontStyle &style, const char *bcp47[], int bcp47Count, int32_t character)
{
    if (fontMgr_ == nullptr || style.GetFontStyle() == nullptr) {
        return nullptr;
    }

    auto tf = fontMgr_->matchFamilyStyleCharacter(familyName.c_str(),
        *style.GetFontStyle(), bcp47, bcp47Count, character);
    return std::make_shared<TexgineTypeface>(tf);
}

std::shared_ptr<TexgineFontStyleSet> TexgineFontManager::MatchFamily(const std::string &familyName)
{
    if (fontMgr_ == nullptr) {
        return nullptr;
    }

    auto set = fontMgr_->matchFamily(familyName.c_str());
    return std::make_shared<TexgineFontStyleSet>(set);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
