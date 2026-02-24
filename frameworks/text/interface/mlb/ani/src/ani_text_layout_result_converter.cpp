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

#include "ani_text_layout_result_converter.h"

#include "ani_common.h"
#include "ani_text_utils.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

ani_status AniTextLayoutResultConverter::ParseTextRectSizeToNative(
    ani_env* env, ani_object object, TextRectSize& rectSize)
{
    ani_double width = 0.0;
    ani_status ret = env->Object_CallMethod_Double(object, AniGlobalMethod::GetInstance().textRectSizeWidth, &width);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to get width, ret %{public}d", ret);
        return ret;
    }
    rectSize.width = width;

    ani_double height = 0.0;
    ret = env->Object_CallMethod_Double(object, AniGlobalMethod::GetInstance().textRectSizeHeight, &height);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to get height, ret %{public}d", ret);
        return ret;
    }
    rectSize.height = height;

    return ANI_OK;
}

ani_status AniTextLayoutResultConverter::ParseTextLayoutResultToAni(
    ani_env* env, const TextLayoutResult& result, ani_object& aniObj)
{
    ani_object fitStrRangeArray = AniTextUtils::CreateAniArray(env, result.fitStrRange.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(fitStrRangeArray, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create fitStrRange array");
        return ANI_INVALID_ARGS;
    }

    ani_size index = 0;
    for (const auto& textRange : result.fitStrRange) {
        ani_object rangeObj = AniTextUtils::CreateAniObject(env,
            AniGlobalClass::GetInstance().range,
            AniGlobalMethod::GetInstance().rangeCtor,
            ani_int(textRange.start), ani_int(textRange.end));
        env->Reference_IsUndefined(rangeObj, &isUndefined);
        if (isUndefined) {
            TEXT_LOGE("Failed to create range object");
            continue;
        }
        ani_status ret = env->Object_CallMethod_Void(
            fitStrRangeArray, AniGlobalMethod::GetInstance().arraySet, index, rangeObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set range item %{public}zu, ret %{public}d", index, ret);
            continue;
        }
        index++;
    }

    ani_object correctRectObj = AniTextUtils::CreateAniObject(env,
        AniGlobalClass::GetInstance().textRectSize,
        AniGlobalMethod::GetInstance().textRectSizeCtor,
        ani_double(result.correctRect.width), ani_double(result.correctRect.height));
    env->Reference_IsUndefined(correctRectObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create correctRect object");
        return ANI_INVALID_ARGS;
    }

    aniObj = AniTextUtils::CreateAniObject(env,
        AniGlobalClass::GetInstance().textLayoutResult,
        AniGlobalMethod::GetInstance().textLayoutResultCtor,
        fitStrRangeArray, correctRectObj);
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create TextLayoutResult object");
        return ANI_INVALID_ARGS;
    }

    return ANI_OK;
}
} // namespace OHOS::Text::ANI
