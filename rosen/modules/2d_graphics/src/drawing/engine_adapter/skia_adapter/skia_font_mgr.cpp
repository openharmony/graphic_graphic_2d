/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "skia_font_mgr.h"

#include "include/core/SkString.h"
#include "include/core/SkTypeface.h"
#ifndef USE_TEXGINE
#include "txt/asset_font_manager.h"
#endif

#include "skia_adapter/skia_convert_utils.h"
#include "skia_adapter/skia_font_style_set.h"
#include "skia_adapter/skia_typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaFontMgr::SkiaFontMgr(sk_sp<SkFontMgr> skFontMgr) : skFontMgr_(skFontMgr) {}

std::shared_ptr<FontMgrImpl> SkiaFontMgr::CreateDefaultFontMgr()
{
    return std::make_shared<SkiaFontMgr>(SkFontMgr::RefDefault());
}

#ifndef USE_TEXGINE
std::shared_ptr<FontMgrImpl> SkiaFontMgr::CreateDynamicFontMgr()
{
    sk_sp<txt::DynamicFontManager> dynamicFontManager = sk_make_sp<txt::DynamicFontManager>();
    return std::make_shared<SkiaFontMgr>(dynamicFontManager);
}

Typeface* SkiaFontMgr::LoadDynamicFont(const std::string& familyName, const uint8_t* data, size_t dataLength)
{
    auto stream = std::make_unique<SkMemoryStream>(data, dataLength, true);
    auto typeface = SkTypeface::MakeFromStream(std::move(stream));
    auto dynamicFontMgr = static_cast<txt::DynamicFontManager*>(skFontMgr_.get());
    if (familyName.empty()) {
        dynamicFontMgr->font_provider().RegisterTypeface(typeface);
    } else {
        dynamicFontMgr->font_provider().RegisterTypeface(typeface, familyName);
    }
    if (!typeface) {
        return nullptr;
    }
    typeface->setIsCustomTypeface(true);
    std::shared_ptr<TypefaceImpl> typefaceImpl = std::make_shared<SkiaTypeface>(typeface);
    return new Typeface(typefaceImpl);
}

void SkiaFontMgr::LoadThemeFont(const std::string& familyName, const std::string& themeName,
    const uint8_t* data, size_t dataLength)
{
    auto dynamicFontMgr = static_cast<txt::DynamicFontManager*>(skFontMgr_.get());
    if (familyName.empty() || data == nullptr) {
        dynamicFontMgr->font_provider().RegisterTypeface(nullptr, themeName);
    } else {
        auto stream = std::make_unique<SkMemoryStream>(data, dataLength, true);
        auto typeface = SkTypeface::MakeFromStream(std::move(stream));
        dynamicFontMgr->font_provider().RegisterTypeface(typeface, themeName);
    }
}
#endif

Typeface* SkiaFontMgr::MatchFamilyStyleCharacter(const char familyName[], const FontStyle& fontStyle,
                                                 const char* bcp47[], int bcp47Count,
                                                 int32_t character)
{
    SkFontStyle skFontStyle;
    SkiaConvertUtils::DrawingFontStyleCastToSkFontStyle(fontStyle, skFontStyle);
    SkTypeface* skTypeface =
        skFontMgr_->matchFamilyStyleCharacter(familyName, skFontStyle, bcp47, bcp47Count, character);
    if (!skTypeface) {
        return nullptr;
    }
    std::shared_ptr<TypefaceImpl> typefaceImpl = std::make_shared<SkiaTypeface>(sk_sp(skTypeface));
    return new Typeface(typefaceImpl);
}

FontStyleSet* SkiaFontMgr::MatchFamily(const char familyName[]) const
{
    SkFontStyleSet* skFontStyleSetPtr = skFontMgr_->matchFamily(familyName);
    if (!skFontStyleSetPtr) {
        return nullptr;
    }
    sk_sp<SkFontStyleSet> skFontStyleSet{skFontStyleSetPtr};
    std::shared_ptr<FontStyleSetImpl> fontStyleSetImpl = std::make_shared<SkiaFontStyleSet>(skFontStyleSet);
    return new FontStyleSet(fontStyleSetImpl);
}

Typeface* SkiaFontMgr::MatchFamilyStyle(const char familyName[], const FontStyle& fontStyle) const
{
    SkFontStyle skFontStyle;
    SkiaConvertUtils::DrawingFontStyleCastToSkFontStyle(fontStyle, skFontStyle);
    SkTypeface* skTypeface =
        skFontMgr_->matchFamilyStyle(familyName, skFontStyle);
    if (!skTypeface) {
        return nullptr;
    }
    std::shared_ptr<TypefaceImpl> typefaceImpl = std::make_shared<SkiaTypeface>(sk_sp(skTypeface));
    return new Typeface(typefaceImpl);
}

int SkiaFontMgr::CountFamilies() const
{
    if (skFontMgr_ == nullptr) {
        return 0;
    }
    return skFontMgr_->countFamilies();
}

void SkiaFontMgr::GetFamilyName(int index, std::string& str) const
{
    if (index < 0 || skFontMgr_ == nullptr) {
        return;
    }
    SkString skName;
    skFontMgr_->getFamilyName(index, &skName);
    str.assign(skName.c_str());
}

FontStyleSet* SkiaFontMgr::CreateStyleSet(int index) const
{
    if (index < 0 || skFontMgr_ == nullptr) {
        return nullptr;
    }
    SkFontStyleSet* skFontStyleSetPtr = skFontMgr_->createStyleSet(index);
    if (!skFontStyleSetPtr) {
        return nullptr;
    }
    sk_sp<SkFontStyleSet> skFontStyleSet{skFontStyleSetPtr};
    std::shared_ptr<FontStyleSetImpl> fontStyleSetImpl = std::make_shared<SkiaFontStyleSet>(skFontStyleSet);
    return new FontStyleSet(fontStyleSetImpl);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS