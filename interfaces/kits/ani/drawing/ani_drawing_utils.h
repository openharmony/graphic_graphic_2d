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
#include <iostream>
#include <memory>
#include <string>
#include "effect/color_filter.h"
#include "utils/rect.h"

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

ani_status AniThrowError(ani_env* env, const std::string& message);

inline ani_string CreateAniString(ani_env* env, const std::string& stdStr)
{
    ani_string aniString;
    env->String_NewUTF8(stdStr.c_str(), stdStr.size(), &aniString);
    return aniString;
}

inline std::string CreateStdString(ani_env* env, ani_string aniStr)
{
    ani_size aniStrSize = 0;
    env->String_GetUTF8Size(aniStr, &aniStrSize);
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(aniStrSize + 1);
    ani_size byteSize = 0;
    env->String_GetUTF8(aniStr, buffer.get(), aniStrSize + 1, &byteSize);
    buffer[byteSize] = '\0';
    return std::string(buffer.get());
}

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

ani_object CreateAniUndefined(ani_env* env);

ani_object CreateAniObject(ani_env* env, const char* className, const char* methodSig, ...);

template<typename T>
ani_object CreateAniObjectStatic(ani_env* env, const char* className, T* obj)
{
    if (obj == nullptr) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic obj is nullptr");
        return CreateAniUndefined(env);
    }
    ani_class aniClass;
    if (env->FindClass(className, &aniClass) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic FindClass failed");
        delete obj;
        return CreateAniUndefined(env);
    }

    ani_method aniConstructor;
    if (env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniConstructor) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Class_FindMethod constructor failed");
        delete obj;
        return CreateAniUndefined(env);
    }

    ani_object aniObject;
    if (env->Object_New(aniClass, aniConstructor, &aniObject) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Object_New failed");
        delete obj;
        return CreateAniUndefined(env);
    }

    ani_method innerMethod;
    if (env->Class_FindMethod(aniClass, "bindNativePtr", "J:V", &innerMethod) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Class_FindMethod bindNativePtr failed");
        delete obj;
        return CreateAniUndefined(env);
    }
    if (env->Object_CallMethod_Void(aniObject, innerMethod, reinterpret_cast<ani_long>(obj)) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Object_CallMethod_Void failed");
        delete obj;
        return CreateAniUndefined(env);
    }

    return aniObject;
}

bool GetColorQuadFromParam(ani_env* env, ani_object obj, Drawing::ColorQuad &color);

bool GetColorQuadFromColorObj(ani_env* env, ani_object obj, Drawing::ColorQuad &color);

bool GetRectFromAniRectObj(ani_env* env, ani_object obj, Drawing::Rect& rect);

inline bool CheckDoubleOutOfRange(ani_double val, double lowerBound, double upperBound)
{
    return val < lowerBound || val > upperBound;
}

inline bool CheckInt32OutOfRange(ani_int val, int32_t lowerBound, int32_t upperBound)
{
    return val < lowerBound || val > upperBound;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_DRAWING_UTILS_H