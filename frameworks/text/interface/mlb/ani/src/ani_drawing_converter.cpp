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

#include "ani_drawing_converter.h"

#include <cstdint>

#include "ani_common.h"
#include "ani_text_utils.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

namespace {
bool GetColorValue(ani_env* env, ani_object colorObj, const char* name, int32_t& value)
{
    ani_int tempValue = 0;
    ani_status isAlphaOk =
        env->Object_GetPropertyByName_Int(reinterpret_cast<ani_object>(colorObj), name, &tempValue);
    if (isAlphaOk != ANI_OK) {
        return false;
    }
    value = ConvertClampFromJsValue(tempValue, 0, Drawing::Color::RGB_MAX);
    return true;
}
} // namespace

ani_status AniDrawingConverter::ParseDrawingColorToNative(
    ani_env* env, ani_object obj, const std::string& str, Drawing::Color& colorSrc)
{
    ani_ref colorRef = nullptr;
    ani_status result = env->Object_GetPropertyByName_Ref(obj, str.c_str(), &colorRef);
    if (result != ANI_OK || colorRef == nullptr) {
        TEXT_LOGD("Failed to find param color, ret %{public}d", result);
        return result;
    }

    int32_t alpha = 0;
    if (!GetColorValue(env, reinterpret_cast<ani_object>(colorRef), "alpha", alpha)) {
        return ANI_INVALID_TYPE;
    }
    int32_t red = 0;
    if (!GetColorValue(env, reinterpret_cast<ani_object>(colorRef), "red", red)) {
        return ANI_INVALID_TYPE;
    }
    int32_t green = 0;
    if (!GetColorValue(env, reinterpret_cast<ani_object>(colorRef), "green", green)) {
        return ANI_INVALID_TYPE;
    }
    int32_t blue = 0;
    if (!GetColorValue(env, reinterpret_cast<ani_object>(colorRef), "blue", blue)) {
        return ANI_INVALID_TYPE;
    }
    colorSrc = Drawing::Color(Drawing::Color::ColorQuadSetARGB(alpha, red, green, blue));
    return ANI_OK;
}

ani_status AniDrawingConverter::ParseDrawingPointToNative(
    ani_env* env, ani_object obj, OHOS::Rosen::Drawing::Point& point)
{
    ani_double x = 0;
    ani_status ret = env->Object_GetPropertyByName_Double(obj, "x", &x);
    if (ret != ANI_OK) {
        TEXT_LOGE("Param x is invalid, ret %{public}d", ret);
        return ret;
    }

    ani_double y = 0;
    ret = env->Object_GetPropertyByName_Double(obj, "y", &y);
    if (ret != ANI_OK) {
        TEXT_LOGE("Param y is invalid, ret %{public}d", ret);
        return ret;
    }
    point.SetX(x);
    point.SetY(y);
    return ret;
}

ani_object AniDrawingConverter::ParseFontMetricsToAni(ani_env* env, const Drawing::FontMetrics& fontMetrics)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_METRICS, "DDDDD:V",
        ani_double(fontMetrics.fTop),
        ani_double(fontMetrics.fAscent),
        ani_double(fontMetrics.fDescent),
        ani_double(fontMetrics.fBottom),
        ani_double(fontMetrics.fLeading)
    );
    return aniObj;
}

ani_status AniDrawingConverter::ParseRectToAni(ani_env* env, const OHOS::Rosen::Drawing::RectF& rect, ani_object& obj)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RECT, "DDDD:V",
        ani_double(rect.left_),
        ani_double(rect.top_),
        ani_double(rect.right_),
        ani_double(rect.bottom_)
    );
    obj = aniObj;
    return ANI_OK;
}

ani_status AniDrawingConverter::ParsePointToAni(ani_env* env, const OHOS::Rosen::Drawing::Point& point, ani_object& obj)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_POINT, "DD:V", 
        ani_double(point.GetX()),
        ani_double(point.GetY())
    );
    obj = aniObj;
    return ANI_OK;
}

ani_status AniDrawingConverter::ParseColorToAni(ani_env* env, const OHOS::Rosen::Drawing::Color& color, ani_object& obj)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_COLOR, "IIII:V",
        ani_int(color.GetAlpha()),
        ani_int(color.GetRed()),
        ani_int(color.GetGreen()),
        ani_int(color.GetBlue())
    );
    obj = aniObj;
    return ANI_OK;
}
} // namespace OHOS::Text::ANI