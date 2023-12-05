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

#include "skia_font_mgr.h"

#include "include/core/SkTypeface.h"

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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS