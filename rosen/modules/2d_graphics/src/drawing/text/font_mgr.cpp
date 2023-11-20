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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
