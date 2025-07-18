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

#ifndef OHOS_TEXT_ANI_DRAWING_CONVERTER_H
#define OHOS_TEXT_ANI_DRAWING_CONVERTER_H

#include <ani.h>

#include "draw/color.h"
#include "text/font_metrics.h"

namespace OHOS::Text::ANI {

inline int32_t ConvertClampFromJsValue(ani_double jsValue, int32_t low, int32_t high)
{
    return std::clamp(static_cast<int32_t>(jsValue), low, high);
}

class AniDrawingConverter final {
public:
    static ani_status ParseDrawingColorToNative(
        ani_env* env, ani_object obj, const std::string& str, OHOS::Rosen::Drawing::Color& colorSrc);

    static ani_object ParseFontMetricsToAni(ani_env* env, const OHOS::Rosen::Drawing::FontMetrics& fontMetrics);
    static ani_status ParseColorToAni(ani_env* env, const OHOS::Rosen::Drawing::Color& color, ani_object& obj);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_DRAWING_CONVERTER_H