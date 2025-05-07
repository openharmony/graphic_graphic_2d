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

#include "ani.h"
#include "ani_common.h"
#include "ani_text_utils.h"
#include "drawing_converter.h"

namespace OHOS::Text::NAI {
inline void ConvertClampFromJsValue(ani_env* env, ani_double jsValue, int32_t& value, int32_t lo, int32_t hi)
{
    value = static_cast<int32_t>(jsValue);
    value = std::clamp(value, lo, hi);
}

void DrawingConverter::ParseDrawingColorToNative(ani_env* env, ani_object obj, const std::string& str,
                                                 Drawing::Color& colorSrc)
{
    ani_ref tempValue = nullptr;
    ani_double tempValueChild{0};
    ani_status result = env->Object_GetPropertyByName_Ref(obj, str.c_str(), &tempValue);
    if (result != ANI_OK || tempValue == nullptr) {
        TEXT_LOGD("[ANI] set color faild");
        return;
    }
    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(tempValue), "alpha", &tempValueChild);
    ConvertClampFromJsValue(env, tempValueChild, alpha, 0, Drawing::Color::RGB_MAX);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(tempValue), "red", &tempValueChild);
    ConvertClampFromJsValue(env, tempValueChild, red, 0, Drawing::Color::RGB_MAX);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(tempValue), "green", &tempValueChild);
    ConvertClampFromJsValue(env, tempValueChild, green, 0, Drawing::Color::RGB_MAX);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(tempValue), "blue", &tempValueChild);
    ConvertClampFromJsValue(env, tempValueChild, blue, 0, Drawing::Color::RGB_MAX);

    Drawing::Color color(Drawing::Color::ColorQuadSetARGB(alpha, red, green, blue));
    colorSrc = color;
}
} // namespace OHOS::Text::NAI