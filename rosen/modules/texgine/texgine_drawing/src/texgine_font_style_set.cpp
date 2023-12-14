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

#include "texgine_font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#ifndef USE_ROSEN_DRAWING
TexgineFontStyleSet::TexgineFontStyleSet(SkFontStyleSet *set)
#else
TexgineFontStyleSet::TexgineFontStyleSet(RSFontStyleSet *set)
#endif
{
    set_ = set;
}

TexgineFontStyleSet::~TexgineFontStyleSet()
{
    if (set_) {
#ifndef USE_ROSEN_DRAWING
        set_->unref();
#else
        delete set_;
#endif
    }
}

int TexgineFontStyleSet::Count() const
{
    if (set_ == nullptr) {
        return 0;
    }
#ifndef USE_ROSEN_DRAWING
    return set_->count();
#else
    return set_->Count();
#endif
}

void TexgineFontStyleSet::GetStyle(const int index, std::shared_ptr<TexgineFontStyle> style,
    std::shared_ptr<TexgineString> name) const
{
    if (set_ == nullptr || style == nullptr || name == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    set_->getStyle(index, style->GetFontStyle().get(), name->GetString());
#else
    set_->GetStyle(index, style->GetFontStyle().get(), name->GetString());
#endif
}

std::shared_ptr<TexgineTypeface> TexgineFontStyleSet::CreateTypeface(int index)
{
    if (set_ == nullptr) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    auto typeface = set_->createTypeface(index);
#else
    RSTypeface* typeface = set_->CreateTypeface(index);
#endif
    return std::make_shared<TexgineTypeface>(typeface);
}

std::shared_ptr<TexgineTypeface> TexgineFontStyleSet::MatchStyle(std::shared_ptr<TexgineFontStyle> pattern)
{
    if (set_ == nullptr || pattern == nullptr || pattern->GetFontStyle() == nullptr) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    auto style = *pattern->GetFontStyle();
    auto typeface = set_->matchStyle(style);
#else
    auto style = *pattern->GetFontStyle();
    RSTypeface* typeface = set_->MatchStyle(style);
#endif
    return std::make_shared<TexgineTypeface>(typeface);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
