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
#ifndef USE_ROSEN_DRAWING
    fontMgr_ = SkFontMgr::RefDefault();
#else
    fontMgr_ = RSFontMgr::CreateDefaultFontMgr();
#endif
}

std::shared_ptr<TexgineFontManager> TexgineFontManager::RefDefault()
{
    auto manager = std::make_shared<TexgineFontManager>();
#ifndef USE_ROSEN_DRAWING
    manager->SetFontMgr(SkFontMgr::RefDefault());
#else
    manager->SetFontMgr(RSFontMgr::CreateDefaultFontMgr());
#endif
    return manager;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkFontMgr> TexgineFontManager::GetFontMgr() const
#else
std::shared_ptr<RSFontMgr> TexgineFontManager::GetFontMgr() const
#endif
{
    return fontMgr_;
}

#ifndef USE_ROSEN_DRAWING
void TexgineFontManager::SetFontMgr(const sk_sp<SkFontMgr> mgr)
#else
void TexgineFontManager::SetFontMgr(const std::shared_ptr<RSFontMgr> mgr)
#endif
{
    fontMgr_ = mgr;
}

std::shared_ptr<TexgineTypeface> TexgineFontManager::MatchFamilyStyleCharacter(const std::string &familyName,
    const TexgineFontStyle &style, const char* bcp47[], int bcp47Count, int32_t character)
{
    if (fontMgr_ == nullptr || style.GetFontStyle() == nullptr) {
        return nullptr;
    }

#ifndef USE_ROSEN_DRAWING
    auto tf = fontMgr_->matchFamilyStyleCharacter(familyName.c_str(),
        *style.GetFontStyle(), bcp47, bcp47Count, character);
    return std::make_shared<TexgineTypeface>(tf);
#else
    RSTypeface* tf = fontMgr_->MatchFamilyStyleCharacter(familyName.c_str(),
        *style.GetFontStyle(), bcp47, bcp47Count, character);
    return std::make_shared<TexgineTypeface>(std::shared_ptr<RSTypeface>(tf));
#endif
}

std::shared_ptr<TexgineFontStyleSet> TexgineFontManager::MatchFamily(const std::string &familyName)
{
    if (fontMgr_ == nullptr) {
        return nullptr;
    }

#ifndef USE_ROSEN_DRAWING
    auto set = fontMgr_->matchFamily(familyName.c_str());
#else
    RSFontStyleSet* set = fontMgr_->MatchFamily(familyName.c_str());
#endif
    return std::make_shared<TexgineFontStyleSet>(set);
}

std::shared_ptr<TexgineTypeface> TexgineFontManager::MatchFamilyStyle(const std::string &familyName,
    const TexgineFontStyle &style)
{
#ifndef USE_ROSEN_DRAWING
    if (fontMgr_ == nullptr) {
        return nullptr;
    }
    SkTypeface* skTyepface = nullptr;
    if (familyName != "") {
        skTyepface = fontMgr_->matchFamilyStyle(familyName.c_str(), *style.GetFontStyle());
    } else {
        // nullptr means default generalfamilyName
        skTyepface = fontMgr_->matchFamilyStyle(nullptr, *style.GetFontStyle());
    }

    if (!skTyepface) {
        return nullptr;
    }
    auto typeface = sk_sp<SkTypeface>(skTyepface);
#else
    if (fontMgr_ == nullptr) {
        return nullptr;
    }
    RSTypeface* rsTypeface = nullptr;
    if (familyName != "") {
        rsTypeface = fontMgr_->MatchFamilyStyle(familyName.c_str(), *style.GetFontStyle());
    } else {
        rsTypeface = fontMgr_->MatchFamilyStyle(nullptr, *style.GetFontStyle());
    }

    if (!rsTypeface) {
        return nullptr;
    }
    std::shared_ptr<RSTypeface> typeface(rsTypeface);
#endif
    return std::make_shared<TexgineTypeface>(typeface);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
