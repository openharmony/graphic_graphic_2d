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

#include "ani_run_metrics_converter.h"

#include "ani_common.h"
#include "ani_drawing_converter.h"
#include "ani_text_style_converter.h"
#include "ani_text_utils.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
ani_object AniRunMetricsConverter::ParseRunMetricsToAni(ani_env* env, const std::map<size_t, RunMetrics>& runMetrics)
{
    ani_object mapAniObj = AniTextUtils::CreateAniMap(env);
    ani_ref mapRef = nullptr;
    for (const auto& [key, runMetrics] : runMetrics) {
        ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RUNMETRICS, ":V");
        if (runMetrics.textStyle != nullptr) {
            env->Object_SetPropertyByName_Ref(
                aniObj, "textStyle", AniTextStyleConverter::ParseTextStyleToAni(env, *runMetrics.textStyle));
            env->Object_SetPropertyByName_Ref(
                aniObj, "fontMetrics", AniDrawingConverter::ParseFontMetricsToAni(env, runMetrics.fontMetrics));
        }
        ani_status status =
            env->Object_CallMethodByName_Ref(mapAniObj, "set", "Lstd/core/Object;Lstd/core/Object;:Lescompat/Map;",
                &mapRef, AniTextUtils::CreateAniIntObj(env, static_cast<int>(key)), aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to set run metrics map, ret %{public}d", status);
            continue;
        };
    }
    return mapAniObj;
}
} // namespace OHOS::Text::ANI