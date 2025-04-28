/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ANI_DRAWING_UTILS_H
#define OHOS_ANI_DRAWING_UTILS_H

#include <ani.h>
#include <hilog/log.h>
#include <string>

#ifdef ROSEN_OHOS

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "AniDrawing"

#define ROSEN_LOGI(format, ...)              \
    HILOG_INFO(LOG_CORE, format, ##__VA_ARGS__)
#define ROSEN_LOGD(format, ...)               \
    HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__)
#define ROSEN_LOGE(format, ...)               \
    HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__)
#else
#define ROSEN_LOGI(format, ...)
#define ROSEN_LOGD(format, ...)
#define ROSEN_LOGE(format, ...)
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

constexpr char NATIVE_OBJ[] = "nativeObj";

template<typename T>
T* GetNativeFromObj(ani_env* env, ani_object obj)
{
    ani_long nativeObj {};
    if (env->Object_GetFieldByName_Long(obj, NATIVE_OBJ, &nativeObj) != ANI_OK) {
        ROSEN_LOGE("[ANI] Object_GetField_Long fetch failed");
        return nullptr;
    }
    T *object = reinterpret_cast<T*>(nativeObj);
    if (!object) {
        ROSEN_LOGE("[ANI] object is null");
        return nullptr;
    }
    return object;
}

ani_status AniThrowError(ani_env* env, const std::string& message);

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_DRAWING_UTILS_H