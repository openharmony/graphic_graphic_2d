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
#include "text/font.h"
#include "text/font_mgr.h"
#include "text/font_metrics.h"
#include "text/font_types.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "utils/point.h"

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
enum class DrawingErrorCode : int32_t {
    OK = 0,
    ERROR_NO_PERMISSION = 201, // the value do not change. It is defined on all system
    ERROR_INVALID_PARAM = 401, // the value do not change. It is defined on all system
    ERROR_DEVICE_NOT_SUPPORT = 801, // the value do not change. It is defined on all system
    ERROR_ABNORMAL_PARAM_VALUE = 18600001, // the value do not change. It is defined on color manager system
};

constexpr char NATIVE_OBJ[] = "nativeObj";
constexpr size_t ARGC_ZERO = 0;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;
constexpr size_t ARGC_FOUR = 4;
constexpr size_t ARGC_FIVE = 5;
constexpr size_t ARGC_SIX = 6;
constexpr size_t ARGC_SEVEN = 7;
constexpr size_t ARGC_EIGHT = 8;
constexpr size_t ARGC_NINE = 9;
constexpr int MAX_PAIRS_PATHVERB = 4;
constexpr int MAX_ELEMENTSIZE = 3000 * 3000;
constexpr int RGBA_MAX = 255;
constexpr const char* ANI_DOUBLE_STRING = "std.core.Double";
constexpr const char* ANI_INT_STRING = "std.core.Int";
constexpr const char* ANI_CLASS_MATRIX_NAME = "@ohos.graphics.drawing.drawing.Matrix";
constexpr const char* ANI_CLASS_COLORFILTER_NAME = "@ohos.graphics.drawing.drawing.ColorFilter";
constexpr const char* ANI_CLASS_PATH_NAME = "@ohos.graphics.drawing.drawing.Path";

ani_status AniThrowError(ani_env* env, const std::string& message);

ani_status ThrowBusinessError(ani_env* env, DrawingErrorCode errorCode, const char* message);

ani_status CreateBusinessError(ani_env* env, int32_t error, const char* message, ani_object& err);

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

ani_status CreateStdStringUtf16(ani_env* env, const ani_string& str, std::u16string& utf16Str);

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

ani_object CreateAniNull(ani_env* env);

bool CreateAniEnumByEnumIndex(ani_env* env, const char* enumDescripter, ani_size index, ani_enum_item& enumItem);

bool GetPointFromAniPointObj(ani_env* env, ani_object obj, Drawing::Point& point);

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
        return CreateAniUndefined(env);
    }

    ani_method aniConstructor;
    if (env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniConstructor) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Class_FindMethod constructor failed");
        return CreateAniUndefined(env);
    }

    ani_object aniObject;
    if (env->Object_New(aniClass, aniConstructor, &aniObject) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Object_New failed");
        return CreateAniUndefined(env);
    }

    ani_method innerMethod;
    if (env->Class_FindMethod(aniClass, "bindNativePtr", "l:", &innerMethod) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Class_FindMethod bindNativePtr failed");
        return CreateAniUndefined(env);
    }
    if (env->Object_CallMethod_Void(aniObject, innerMethod, reinterpret_cast<ani_long>(obj)) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Object_CallMethod_Void failed");
        return CreateAniUndefined(env);
    }

    return aniObject;
}

inline ani_object CreateAniArrayWithSize(ani_env* env, size_t size)
{
    ani_class arrayCls;
    if (env->FindClass("escompat.Array", &arrayCls) != ANI_OK) {
        ROSEN_LOGE("Failed to findClass escompat.Array");
        return CreateAniUndefined(env);
    }
    ani_method arrayCtor;
    if (env->Class_FindMethod(arrayCls, "<ctor>", "i:", &arrayCtor) != ANI_OK) {
        ROSEN_LOGE("Failed to find <ctor>");
        return CreateAniUndefined(env);
    }
    ani_object arrayObj = nullptr;
    if (env->Object_New(arrayCls, arrayCtor, &arrayObj, size) != ANI_OK) {
        ROSEN_LOGE("Failed to create object Array");
        return CreateAniUndefined(env);
    }
    return arrayObj;
}

bool GetColorQuadFromParam(ani_env* env, ani_object obj, Drawing::ColorQuad &color);

bool GetColorQuadFromColorObj(ani_env* env, ani_object obj, Drawing::ColorQuad &color);

ani_status CreateColorObj(ani_env* env, const Drawing::Color& color, ani_object& obj);

bool GetRectFromAniRectObj(ani_env* env, ani_object obj, Drawing::Rect& rect);

ani_status CreateRectObj(ani_env* env, const Drawing::Rect& rect, ani_object& obj);

ani_status GetPointFromPointObj(ani_env* env, ani_object obj, Drawing::Point& point);

ani_status CreatePointObj(ani_env* env, const Drawing::Point& point, ani_object& obj);

bool CreatePointObjAndCheck(ani_env* env, const Drawing::Point& point, ani_object& obj);

bool GetPoint3FromPoint3dObj(ani_env* env, ani_object obj, Drawing::Point3& point3d);

bool SetPointToAniPointArrayWithIndex(ani_env* env, Drawing::Point& point, ani_object& pointArray, uint32_t index);

inline bool CheckDoubleOutOfRange(ani_double val, double lowerBound, double upperBound)
{
    return val < lowerBound || val > upperBound;
}

inline bool CheckInt32OutOfRange(ani_int val, int32_t lowerBound, int32_t upperBound)
{
    return val < lowerBound || val > upperBound;
}

std::shared_ptr<Font> GetThemeFont(std::shared_ptr<Font> font);

std::shared_ptr<Font> MatchThemeFont(std::shared_ptr<Font> font, int32_t unicode);

std::shared_ptr<FontMgr> GetFontMgr(std::shared_ptr<Font> font);

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_DRAWING_UTILS_H