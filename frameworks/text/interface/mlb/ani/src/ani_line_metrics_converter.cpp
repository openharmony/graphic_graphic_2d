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
namespace {
constexpr CacheKey LINEMETRICS_KEY{ANI_CLASS_LINEMETRICS, "<ctor>", "iiddddddidC{std.core.Map}:"};
}

ani_object AniLineMetricsConverter::ParseLineMetricsToAni(ani_env* env, const LineMetrics& lineMetrics)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_FIND_CLASS(env, ANI_CLASS_LINEMETRICS),
        ANI_CLASS_FIND_METHOD(env, LINEMETRICS_KEY), ani_int(lineMetrics.startIndex), ani_int(lineMetrics.endIndex),
        ani_double(lineMetrics.ascender), ani_double(lineMetrics.descender), ani_double(lineMetrics.height),
        ani_double(lineMetrics.width), ani_double(lineMetrics.x), ani_double(lineMetrics.baseline),
        ani_int(lineMetrics.lineNumber), ani_double(lineMetrics.y),
        AniRunMetricsConverter::ParseRunMetricsToAni(env, lineMetrics.runMetrics));
    return aniObj;
}
} // namespace OHOS::Text::ANI