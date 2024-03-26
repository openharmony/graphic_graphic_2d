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

#include "text/font_mgr.h"

#include "impl_factory.h"
#include "impl_interface/font_mgr_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
FontMgr::FontMgr(std::shared_ptr<FontMgrImpl> fontMgrImpl) noexcept : fontMgrImpl_(fontMgrImpl) {}

std::shared_ptr<FontMgr> FontMgr::CreateDefaultFontMgr()
{
    return std::make_shared<FontMgr>(ImplFactory::CreateDefaultFontMgrImpl());
}

#ifndef USE_TEXGINE
std::shared_ptr<FontMgr> FontMgr::CreateDynamicFontMgr()
{
    return std::make_shared<FontMgr>(ImplFactory::CreateDynamicFontMgrImpl());
}

Typeface* FontMgr::LoadDynamicFont(const std::string& familyName, const uint8_t* data, size_t dataLength)
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->LoadDynamicFont(familyName, data, dataLength);
    }
    return nullptr;
}

void FontMgr::LoadThemeFont(const std::string& familyName, const std::string& themeName,
    const uint8_t* data, size_t dataLength)
{
    if (fontMgrImpl_) {
        fontMgrImpl_->LoadThemeFont(familyName, themeName, data, dataLength);
    }
}
#endif

Typeface* FontMgr::MatchFamilyStyleCharacter(const char familyName[], const FontStyle& fontStyle,
                                             const char* bcp47[], int bcp47Count,
                                             int32_t character) const
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->MatchFamilyStyleCharacter(familyName, fontStyle, bcp47, bcp47Count, character);
    }
    return nullptr;
}

FontStyleSet* FontMgr::MatchFamily(const char familyName[]) const
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->MatchFamily(familyName);
    }
    return nullptr;
}

Typeface* FontMgr::MatchFamilyStyle(const char familyName[], const FontStyle& fontStyle) const
{
    if (fontMgrImpl_) {
        return fontMgrImpl_->MatchFamilyStyle(familyName, fontStyle);
    }
    return nullptr;
}

int FontMgr::CountFamilies() const
{
    if (fontMgrImpl_ == nullptr) {
        return 0;
    }
    return fontMgrImpl_->CountFamilies();
}

void FontMgr::GetFamilyName(int index, std::string& str) const
{
    if (fontMgrImpl_ == nullptr) {
        return;
    }
    fontMgrImpl_->GetFamilyName(index, str);
}

FontStyleSet* FontMgr::CreateStyleSet(int index) const
{
    if (fontMgrImpl_ == nullptr) {
        return nullptr;
    }
    return fontMgrImpl_->CreateStyleSet(index);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
