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
#include "line_metrics_converter.h"
#include "text_style_converter.h"

namespace OHOS::Text::NAI {
ani_object LineMetricsConverter::ParseFontMetricsToAni(ani_env* env, const Drawing::FontMetrics& fontMetrics)
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

ani_object LineMetricsConverter::ParseRunMetricsToAni(ani_env* env, const std::map<size_t, RunMetrics>& map)
{
    ani_object mapAniObj = AniTextUtils::CreateAniMap(env);
    ani_ref mapRef = nullptr;
    for (const auto& [key, runMetrics] : map) {
        ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RUNMETRICS_I, ":V");
        if (runMetrics.textStyle != nullptr) {
            env->Object_SetPropertyByName_Ref(aniObj, "textStyle",
                                              TextStyleConverter::ParseTextStyleToAni(env, *runMetrics.textStyle));
        }
        if (ANI_OK
            != env->Object_CallMethodByName_Ref(
                mapAniObj, "set", "Lstd/core/Object;Lstd/core/Object;:Lescompat/Map;", &mapRef,
                AniTextUtils::CreateAniDoubleObj(env, static_cast<ani_double>(key)), aniObj)) {
            TEXT_LOGE("Object_CallMethodByName_Ref set failed");
            break;
        };
    }
    return mapAniObj;
}

ani_object LineMetricsConverter::ParseLineMetricsToAni(ani_env* env, const LineMetrics& lineMetrics)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_LINEMETRICS_I, ":V");
    env->Object_SetPropertyByName_Double(aniObj, "startIndex", ani_int(lineMetrics.startIndex));
    env->Object_SetPropertyByName_Double(aniObj, "endIndex", ani_int(lineMetrics.endIndex));
    env->Object_SetPropertyByName_Double(aniObj, "ascent", ani_double(lineMetrics.ascender));
    env->Object_SetPropertyByName_Double(aniObj, "descent", ani_double(lineMetrics.descender));
    env->Object_SetPropertyByName_Double(aniObj, "height", ani_double(lineMetrics.height));
    env->Object_SetPropertyByName_Double(aniObj, "width", ani_double(lineMetrics.width));
    env->Object_SetPropertyByName_Double(aniObj, "left", ani_double(lineMetrics.x));
    env->Object_SetPropertyByName_Double(aniObj, "baseline", ani_double(lineMetrics.baseline));
    env->Object_SetPropertyByName_Double(aniObj, "lineNumber", ani_int(lineMetrics.lineNumber));
    env->Object_SetPropertyByName_Double(aniObj, "topHeight", ani_double(lineMetrics.y));
    env->Object_SetPropertyByName_Ref(aniObj, "runMetrics", ParseRunMetricsToAni(env, lineMetrics.runMetrics));
    return aniObj;
}
} // namespace OHOS::Text::NAI