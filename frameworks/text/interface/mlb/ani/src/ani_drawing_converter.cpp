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

#include <cstdint>

#include "ani_common.h"
#include "ani_drawing_converter.h"
#include "ani_text_utils.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

namespace {
bool GetColorValue(ani_env* env, ani_object colorObj, const char* name, int32_t& value)
{
    ani_double tempValue = 0;
    ani_status isAlphaOk =
        env->Object_GetPropertyByName_Double(reinterpret_cast<ani_object>(colorObj), name, &tempValue);
    if (isAlphaOk != ANI_OK) {
        return false;
    }
    value = ConvertClampFromJsValue(tempValue, 0, Drawing::Color::RGB_MAX);
    return true;
}
} // namespace

void AniDrawingConverter::ParseDrawingColorToNative(
    ani_env* env, ani_object obj, const std::string& str, Drawing::Color& colorSrc)
{
    ani_ref colorRef = nullptr;
    ani_status result = env->Object_GetPropertyByName_Ref(obj, str.c_str(), &colorRef);
    if (result != ANI_OK || colorRef == nullptr) {
        TEXT_LOGD("Failed to find param color, ret %{public}d", result);
        return;
    }

    int32_t alpha = 0;
    if (!GetColorValue(env, reinterpret_cast<ani_object>(colorRef), "alpha", alpha)) {
        return;
    }
    int32_t red = 0;
    if (!GetColorValue(env, reinterpret_cast<ani_object>(colorRef), "red", red)) {
        return;
    }
    int32_t green = 0;
    if (!GetColorValue(env, reinterpret_cast<ani_object>(colorRef), "green", green)) {
        return;
    }
    int32_t blue = 0;
    if (!GetColorValue(env, reinterpret_cast<ani_object>(colorRef), "blue", blue)) {
        return;
    }
    colorSrc = Drawing::Color(Drawing::Color::ColorQuadSetARGB(alpha, red, green, blue));
}

ani_object AniDrawingConverter::ParseFontMetricsToAni(ani_env* env, const Drawing::FontMetrics& fontMetrics)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_METRICS, ":V");
    env->Object_SetPropertyByName_Double(aniObj, "flags", ani_int(fontMetrics.fFlags));
    env->Object_SetPropertyByName_Double(aniObj, "top", ani_double(fontMetrics.fTop));
    env->Object_SetPropertyByName_Double(aniObj, "ascent", ani_double(fontMetrics.fAscent));
    env->Object_SetPropertyByName_Double(aniObj, "descent", ani_double(fontMetrics.fDescent));
    env->Object_SetPropertyByName_Double(aniObj, "bottom", ani_double(fontMetrics.fBottom));
    env->Object_SetPropertyByName_Double(aniObj, "leading", ani_double(fontMetrics.fLeading));
    env->Object_SetPropertyByName_Double(aniObj, "avgCharWidth", ani_double(fontMetrics.fAvgCharWidth));
    env->Object_SetPropertyByName_Double(aniObj, "maxCharWidth", ani_double(fontMetrics.fMaxCharWidth));
    env->Object_SetPropertyByName_Double(aniObj, "xMin", ani_double(fontMetrics.fXMin));
    env->Object_SetPropertyByName_Double(aniObj, "xMax", ani_double(fontMetrics.fXMax));
    env->Object_SetPropertyByName_Double(aniObj, "xHeight", ani_double(fontMetrics.fXHeight));
    env->Object_SetPropertyByName_Double(aniObj, "capHeight", ani_double(fontMetrics.fCapHeight));
    env->Object_SetPropertyByName_Double(aniObj, "underlineThickness", ani_double(fontMetrics.fUnderlineThickness));
    env->Object_SetPropertyByName_Double(aniObj, "underlinePosition", ani_double(fontMetrics.fUnderlinePosition));
    env->Object_SetPropertyByName_Double(aniObj, "strikethroughThickness", ani_double(fontMetrics.fStrikeoutThickness));
    env->Object_SetPropertyByName_Double(aniObj, "strikethroughPosition", ani_double(fontMetrics.fStrikeoutPosition));
    return aniObj;
}
} // namespace OHOS::Text::ANI