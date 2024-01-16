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

#ifndef SKIA_FONT_STYLE_SET_H
#define SKIA_FONT_STYLE_SET_H

#include <memory>

#include "include/core/SkFontMgr.h"

#include "text/font_style_set.h"
#include "impl_interface/font_style_set_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontStyleSet : public FontStyleSetImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    explicit SkiaFontStyleSet(sk_sp<SkFontStyleSet> skFontStyleSet);
    virtual ~SkiaFontStyleSet() = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    Typeface* CreateTypeface(int index) override;

    void GetStyle(int32_t index, FontStyle* fontStyle, std::string* styleName) override;

    Typeface* MatchStyle(const FontStyle& pattern) override;

    int Count() override;

    static FontStyleSet* CreateEmpty();

private:
    sk_sp<SkFontStyleSet> skFontStyleSet_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif