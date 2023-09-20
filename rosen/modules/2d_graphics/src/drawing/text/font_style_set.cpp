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

#include "src/ports/skia_ohos/FontConfig_ohos.h"

#include "impl_factory.h"
#include "impl_interface/font_style_set_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
FontStyleSet::FontStyleSet() : fontStyleSetImpl_(ImplFactory::CreateTypefaceFontStyleSetImpl()) {}

FontStyleSet::FontStyleSet(const std::shared_ptr<FontConfig_OHOS>& fontConfig, int index, bool isFallback)
    : fontStyleSetImpl_(ImplFactory::CreateFontStyleSetOhosImpl(fontConfig, index, isFallback)) {}

FontStyleSet::FontStyleSet(std::shared_ptr<FontStyleSetImpl> fontStyleSetImpl) noexcept
    : fontStyleSetImpl_(fontStyleSetImpl) {}

Typeface* FontStyleSet::CreateTypeface(int index)
{
    if (fontStyleSetImpl_) {
        return fontStyleSetImpl_->CreateTypeface(index);
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

void FontStyleSet::RegisterTypeface(std::shared_ptr<Typeface> typeface)
{
    if (fontStyleSetImpl_) {
        fontStyleSetImpl_->RegisterTypeface(typeface);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
