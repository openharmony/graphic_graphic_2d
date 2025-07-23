/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_TEXT_ANI_TEXT_RECT_CONVERTER_H
#define OHOS_TEXT_ANI_TEXT_RECT_CONVERTER_H

#include <ani.h>
#include <cstddef>

#include "text/font_metrics.h"
#include "typography.h"

namespace OHOS::Text::ANI {
struct RectRange {
    size_t start;
    size_t end;
};

class AniTextRectConverter {
public:
    static ani_status ParseRangeToNative(ani_env* env, ani_object obj, RectRange& rectRange);
    static ani_status ParseWidthStyleToNative(
        ani_env* env, ani_object obj, OHOS::Rosen::TextRectWidthStyle& widthStyle);
    static ani_status ParseHeightStyleToNative(
        ani_env* env, ani_object obj, OHOS::Rosen::TextRectHeightStyle& heightStyle);

    static ani_status ParseTextBoxToAni(ani_env* env, const OHOS::Rosen::TextRect textRect, ani_object& obj);
    static ani_status ParseBoundaryToAni(ani_env* env, OHOS::Rosen::Boundary& Boundary, ani_object& obj);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_TEXT_RECT_CONVERTER_H