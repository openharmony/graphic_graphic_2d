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

#include "text/font_style_set.h"

#include "impl_interface/font_style_set_impl.h"
#include "static_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
FontStyleSet::FontStyleSet(std::shared_ptr<FontStyleSetImpl> fontStyleSetImpl) noexcept
    : fontStyleSetImpl_(fontStyleSetImpl) {}

Typeface* FontStyleSet::CreateTypeface(int index)
{
    if (fontStyleSetImpl_) {
        return fontStyleSetImpl_->CreateTypeface(index);
    }
    return nullptr;
}

void FontStyleSet::GetStyle(int32_t index, FontStyle* fontStyle, std::string* styleName)
{
    if (fontStyleSetImpl_) {
        fontStyleSetImpl_->GetStyle(index, fontStyle, styleName);
    }
}

Typeface* FontStyleSet::MatchStyle(const FontStyle& pattern)
{
    if (fontStyleSetImpl_) {
        return fontStyleSetImpl_->MatchStyle(pattern);
    }
    return nullptr;
}

int FontStyleSet::Count()
{
    if (fontStyleSetImpl_) {
        return fontStyleSetImpl_->Count();
    }
    return 0;
}

FontStyleSet* FontStyleSet::CreateEmpty()
{
    return StaticFactory::CreateEmpty();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
