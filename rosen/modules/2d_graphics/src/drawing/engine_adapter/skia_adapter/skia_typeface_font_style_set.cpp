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

#include "skia_typeface_font_style_set.h"

#include "include/core/SkTypeface.h"
#include "txt/typeface_font_asset_provider.h"

#include "skia_adapter/skia_typeface.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaTypefaceFontStyleSet::SkiaTypefaceFontStyleSet()
    : SkiaFontStyleSet(std::make_shared<txt::TypefaceFontStyleSet>()) {}

void SkiaTypefaceFontStyleSet::RegisterTypeface(std::shared_ptr<Typeface> typeface)
{
    std::shared_ptr<SkiaTypeface> skiaTypeface = typeface->GetImpl<SkiaTypeface>();
    if (!skiaTypeface) {
        LOGE("skiaTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }
    sk_sp<SkTypeface> skTypeface = skiaTypeface->GetTypeface();
    txt::TypefaceFontStyleSet* fontStyleSet = static_cast<txt::TypefaceFontStyleSet*>(skFontStyleSet_.get());
    fontStyleSet->registerTypeface(skTypeface);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS