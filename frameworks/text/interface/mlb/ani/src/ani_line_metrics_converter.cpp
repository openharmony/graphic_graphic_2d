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

#include "ani_line_metrics_converter.h"

#include "ani_common.h"
#include "ani_run_metrics_converter.h"
#include "ani_text_utils.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
ani_object AniLineMetricsConverter::ParseLineMetricsToAni(ani_env* env, const LineMetrics& lineMetrics)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_LINEMETRICS, ":V");
    env->Object_SetPropertyByName_Int(aniObj, "startIndex", ani_int(lineMetrics.startIndex));
    env->Object_SetPropertyByName_Int(aniObj, "endIndex", ani_int(lineMetrics.endIndex));
    env->Object_SetPropertyByName_Double(aniObj, "ascent", ani_double(lineMetrics.ascender));
    env->Object_SetPropertyByName_Double(aniObj, "descent", ani_double(lineMetrics.descender));
    env->Object_SetPropertyByName_Double(aniObj, "height", ani_double(lineMetrics.height));
    env->Object_SetPropertyByName_Double(aniObj, "width", ani_double(lineMetrics.width));
    env->Object_SetPropertyByName_Double(aniObj, "left", ani_double(lineMetrics.x));
    env->Object_SetPropertyByName_Double(aniObj, "baseline", ani_double(lineMetrics.baseline));
    env->Object_SetPropertyByName_Int(aniObj, "lineNumber", ani_int(lineMetrics.lineNumber));
    env->Object_SetPropertyByName_Double(aniObj, "topHeight", ani_double(lineMetrics.y));
    env->Object_SetPropertyByName_Ref(
        aniObj, "runMetrics", AniRunMetricsConverter::ParseRunMetricsToAni(env, lineMetrics.runMetrics));
    return aniObj;
}
} // namespace OHOS::Text::ANI