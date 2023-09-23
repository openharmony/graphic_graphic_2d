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

#ifndef SKIA_TYPEFACE_FONT_STYLE_SET_H
#define SKIA_TYPEFACE_FONT_STYLE_SET_H

#include <string>

#include "skia_adapter/skia_font_style_set.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTypefaceFontStyleSet : public SkiaFontStyleSet {
public:
    SkiaTypefaceFontStyleSet();
    ~SkiaTypefaceFontStyleSet() override = default;

    void RegisterTypeface(std::shared_ptr<Typeface> typeface) override;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif