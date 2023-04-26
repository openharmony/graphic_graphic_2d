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

#include "variant_font_style_set.h"

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
VariantFontStyleSet::VariantFontStyleSet(const std::shared_ptr<TexgineFontStyleSet> &tfss) noexcept(true)
{
    tfss_ = tfss;
}

VariantFontStyleSet::VariantFontStyleSet(const std::shared_ptr<DynamicFontStyleSet> &dfss) noexcept(true)
{
    dfss_ = dfss;
}

VariantFontStyleSet::VariantFontStyleSet(std::shared_ptr<TexgineFontStyleSet> &tfss) noexcept(true)
{
    tfss_ = std::move(tfss);
}

VariantFontStyleSet::VariantFontStyleSet(std::shared_ptr<DynamicFontStyleSet> &dfss) noexcept(true)
{
    dfss_ = std::move(dfss);
}

VariantFontStyleSet::VariantFontStyleSet(std::nullptr_t) noexcept(true)
{
}

std::shared_ptr<TexgineFontStyleSet> VariantFontStyleSet::TryToTexgineFontStyleSet() const noexcept(false)
{
    CheckPointer(true);
    return tfss_;
}

std::shared_ptr<DynamicFontStyleSet> VariantFontStyleSet::TryToDynamicFontStyleSet() const noexcept(false)
{
    CheckPointer(true);
    return dfss_;
}

int VariantFontStyleSet::Count()
{
    CheckPointer();
    if (tfss_) {
        return tfss_->Count();
    }

    if (dfss_) {
        return dfss_->Count();
    }

    return 0;
}

void VariantFontStyleSet::GetStyle(int index, std::shared_ptr<TexgineFontStyle> style, std::shared_ptr<TexgineString> name)
{
    CheckPointer();
    if (tfss_) {
        return tfss_->GetStyle(index, style, name);
    }

    if (dfss_) {
        return dfss_->GetStyle(index, style, name);
    }
}

std::shared_ptr<TexgineTypeface> VariantFontStyleSet::CreateTypeface(int index)
{
    CheckPointer();
    if (tfss_) {
        return tfss_->CreateTypeface(index);
    }

    if (dfss_) {
        return dfss_->CreateTypeface(index);
    }

    return nullptr;
}

std::shared_ptr<TexgineTypeface> VariantFontStyleSet::MatchStyle(std::shared_ptr<TexgineFontStyle> pattern)
{
    CheckPointer();
    if (tfss_) {
        return tfss_->MatchStyle(pattern);
    }

    if (dfss_) {
        return dfss_->MatchStyle(pattern);
    }

    return nullptr;
}

void VariantFontStyleSet::CheckPointer(bool nullable) const noexcept(false)
{
    if (!nullable && tfss_ == nullptr && dfss_ == nullptr) {
        throw TEXGINE_EXCEPTION(Nullptr);
    }

    if (tfss_ != nullptr && dfss_ != nullptr) {
        throw TEXGINE_EXCEPTION(ErrorStatus);
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
