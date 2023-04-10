/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "dynamic_font_style_set.h"

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"

namespace Texgine {
DynamicFontStyleSet::DynamicFontStyleSet(std::unique_ptr<Typeface> typeface)
{
    typeface_ = std::move(typeface);
}

int DynamicFontStyleSet::Count()
{
    return typeface_ != nullptr ? 1 : 0;
}

void DynamicFontStyleSet::GetStyle(int index, std::shared_ptr<TexgineFontStyle> style,
    std::shared_ptr<TexgineString> name)
{
    if (style == nullptr || style->GetFontStyle() == nullptr) {
        LOG2EX(ERROR) << "style is nullptr";
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    if (index == 0 && typeface_ != nullptr && typeface_->Get() != nullptr) {
        *style = *typeface_->Get()->FontStyle();
    }
}

std::shared_ptr<TexgineTypeface> DynamicFontStyleSet::CreateTypeface(int index)
{
    if (index == 0 && typeface_ != nullptr) {
        return typeface_->Get();
    }
    return nullptr;
}

std::shared_ptr<TexgineTypeface> DynamicFontStyleSet::MatchStyle(std::shared_ptr<TexgineFontStyle> pattern)
{
    if (typeface_ != nullptr) {
        return typeface_->Get();
    }
    return nullptr;
}
} // namespace Texgine
