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

#ifndef ROSEN_MODULES_TEXGINE_SRC_VARIANT_FONT_STYLE_SET_H
#define ROSEN_MODULES_TEXGINE_SRC_VARIANT_FONT_STYLE_SET_H

#include <memory>

#include "dynamic_font_style_set.h"
#include "texgine_font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class VariantFontStyleSet {
public:
    VariantFontStyleSet() noexcept(true) = default;
    explicit VariantFontStyleSet(const std::shared_ptr<TexgineFontStyleSet> &tfss) noexcept(true);
    explicit VariantFontStyleSet(const std::shared_ptr<DynamicFontStyleSet> &dfss) noexcept(true);
    explicit VariantFontStyleSet(std::shared_ptr<TexgineFontStyleSet> &tfss) noexcept(true);
    explicit VariantFontStyleSet(std::shared_ptr<DynamicFontStyleSet> &dfss) noexcept(true);
    explicit VariantFontStyleSet(std::nullptr_t) noexcept(true);

    std::shared_ptr<TexgineFontStyleSet> TryToTexgineFontStyleSet() const noexcept(false);
    std::shared_ptr<DynamicFontStyleSet> TryToDynamicFontStyleSet() const noexcept(false);

    int Count();
    void GetStyle(int index, std::shared_ptr<TexgineFontStyle> style, std::shared_ptr<TexgineString> name);
    std::shared_ptr<TexgineTypeface> CreateTypeface(int index);
    std::shared_ptr<TexgineTypeface> MatchStyle(std::shared_ptr<TexgineFontStyle> pattern);
private:
    void CheckPointer(bool nullable = false) const noexcept(false);

    std::shared_ptr<TexgineFontStyleSet> tfss_ = nullptr;
    std::shared_ptr<DynamicFontStyleSet> dfss_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_VARIANT_FONT_STYLE_SET_H
