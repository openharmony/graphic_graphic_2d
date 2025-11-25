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
#include "ani_text_style_converter.h"
#include "ani_text_utils.h"
#include "font_ani/ani_font.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
namespace {
constexpr std::string_view RUNMETRICS_SIGN = "C{" ANI_INTERFACE_TEXT_STYLE "}C{" ANI_INTERFACE_FONT_METRICS "}:";
constexpr CacheKey RUNMETRICS_KEY{ANI_CLASS_RUNMETRICS, "<ctor>", RUNMETRICS_SIGN};
constexpr CacheKey MAP_SET_KEY{ANI_MAP, "set", "YY:C{std.core.Map}"};
} // namespace

ani_object AniRunMetricsConverter::ParseRunMetricsToAni(ani_env* env, const std::map<size_t, RunMetrics>& runMetrics)
{
    ani_object mapAniObj = AniTextUtils::CreateAniMap(env);
    ani_ref mapRef = nullptr;
    for (const auto& [key, runMetrics] : runMetrics) {
        if (runMetrics.textStyle != nullptr) {
            ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_FIND_CLASS(env, ANI_CLASS_RUNMETRICS),
                ANI_CLASS_FIND_METHOD(env, RUNMETRICS_KEY),
                AniTextStyleConverter::ParseTextStyleToAni(env, *runMetrics.textStyle),
                OHOS::Rosen::Drawing::CreateAniFontMetrics(env, runMetrics.fontMetrics));
            ani_status status = env->Object_CallMethod_Ref(mapAniObj, ANI_CLASS_FIND_METHOD(env, MAP_SET_KEY), &mapRef,
                AniTextUtils::CreateAniIntObj(env, static_cast<int>(key)), aniObj);
            if (status != ANI_OK) {
                TEXT_LOGE("Failed to set run metrics map, key %{public}zu, ret %{public}d", key, status);
            }
        }
    }
    return mapAniObj;
}
} // namespace OHOS::Text::ANI