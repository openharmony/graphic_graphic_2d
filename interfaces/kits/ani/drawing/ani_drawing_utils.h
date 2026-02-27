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
#include "ani_drawing_cache_utils.h"
#include "ani_drawing_log.h"
#include "ani_drawing_common.h"
#include "draw/color.h"
#include "common/rs_common_def.h"
#include "image/bitmap.h"

#ifdef ROSEN_OHOS
#include "pixel_map.h"
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
    ERROR_PARAM_VERIFICATION_FAILED = 25900001, // after api 18, no throw 401
};

struct RectPropertyConfig {
    const char* propertyName;
    const char* methodName;
    ani_method& method;
    ani_double& result;
};

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

ani_status AniThrowError(ani_env* env, const std::string& message);

ani_status ThrowBusinessError(ani_env* env, DrawingErrorCode errorCode, const char* message);

ani_status CreateBusinessError(ani_env* env, int32_t error, const char* message, ani_object& err);

inline ani_string CreateAniString(ani_env* env, const std::string& stdStr)
{
    ani_string aniString;
    env->String_NewUTF8(stdStr.c_str(), stdStr.size(), &aniString);
    return aniString;
}

std::string CreateStdString(ani_env* env, ani_string aniStr);

ani_status CreateStdStringUtf16(ani_env* env, const ani_string& str, std::u16string& utf16Str);

template<typename T>
T* GetNativeFromObj(ani_env* env, ani_object obj, ani_field nativeObjField)
{
    if (nativeObjField == nullptr) {
        ROSEN_LOGE("nativeObjField is null");
        return nullptr;
    }
    ani_long nativeObj {};
    ani_status ret = env->Object_GetField_Long(obj, nativeObjField, &nativeObj);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Failed to get native obj, ret %{public}d", ret);
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

ani_object CreateAniObject(ani_env* env, const ani_class cls, const ani_method ctor, ...);

ani_object CreateAniNull(ani_env* env);

bool CreateAniEnumByEnumIndex(ani_env* env, const ani_enum enumType, ani_size index, ani_enum_item& enumItem);

bool CreateAniEnumByEnumName(
    ani_env* env, const ani_enum enumType, const std::string itemName, ani_enum_item& enumItem);

template<typename T>
ani_object CreateAniObjectStatic(
    ani_env* env, ani_class cls, const ani_method ctor, const ani_method bindNativePtr, T* obj)
{
    if (obj == nullptr) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic obj is nullptr");
        return CreateAniUndefined(env);
    }
    if (cls == nullptr) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic FindClass failed");
        return CreateAniUndefined(env);
    }

    if (ctor == nullptr) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Class_FindMethod constructor failed");
        return CreateAniUndefined(env);
    }

    ani_object aniObject;
    if (env->Object_New(cls, ctor, &aniObject) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Object_New failed");
        return CreateAniUndefined(env);
    }

    if (bindNativePtr == nullptr) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Class_FindMethod bindNativePtr failed");
        return CreateAniUndefined(env);
    }
    if (env->Object_CallMethod_Void(aniObject, bindNativePtr, reinterpret_cast<ani_long>(obj)) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObjectStatic Object_CallMethod_Void failed");
        return CreateAniUndefined(env);
    }

    return aniObject;
}

ani_array CreateAniArrayWithSize(ani_env* env, size_t size);

bool GetColorQuadFromParam(ani_env* env, ani_object obj, Drawing::ColorQuad &color);

bool GetColorQuadFromColorObj(ani_env* env, ani_object obj, Drawing::ColorQuad &color);

ani_status CreateColorObj(ani_env* env, const Drawing::Color& color, ani_object& obj);

bool GetRectFromAniRectObj(ani_env* env, ani_object obj, Drawing::Rect& rect);

ani_status CreateRectObj(ani_env* env, const Drawing::Rect& rect, ani_object& obj);

bool GetValueFromAniRectObj(ani_env* env, ani_object obj, std::vector<double>& ltrb);

bool DrawingValueConvertToAniRect(ani_env* env, ani_object obj, ani_double left, ani_double top,
    ani_double right, ani_double bottom);

bool GetColor4fFromAniColor4fObj(ani_env* env, ani_object obj, Drawing::Color4f &color);

ani_status CreateColor4fObj(ani_env* env, const Drawing::Color4f& color, ani_object& obj);

ani_status GetPointFromPointObj(ani_env* env, ani_object obj, Drawing::Point& point);

bool ConvertFromAniPointsArray(ani_env* env, ani_array aniPointArray, Drawing::Point* points, uint32_t pointSize);

ani_status CreatePointObj(ani_env* env, const Drawing::Point& point, ani_object& obj);

bool CreatePointObjAndCheck(ani_env* env, const Drawing::Point& point, ani_object& obj);

bool GetPoint3FromPoint3dObj(ani_env* env, ani_object obj, Drawing::Point3& point3d);

bool MakeFontFeaturesFromAniObjArray(ani_env* env, std::shared_ptr<Drawing::DrawingFontFeatures> fontfeatures,
    uint32_t size, ani_array featuresobj);

bool SetPointToAniPointArrayWithIndex(ani_env* env, Drawing::Point& point, ani_array& pointArray, uint32_t index);

inline bool CheckDoubleOutOfRange(ani_double val, double lowerBound, double upperBound)
{
    return val < lowerBound || val > upperBound;
}

inline bool CheckInt32OutOfRange(ani_int val, int32_t lowerBound, int32_t upperBound)
{
    return val < lowerBound || val > upperBound;
}

inline bool IsUndefined(ani_env* env, ani_ref ref)
{
    ani_boolean isUndefined = ANI_FALSE;
    env->Reference_IsUndefined(ref, &isUndefined);
    return isUndefined;
}

inline bool IsNull(ani_env* env, ani_ref ref)
{
    ani_boolean isNull = ANI_FALSE;
    env->Reference_IsNull(ref, &isNull);
    return isNull;
}

inline bool IsReferenceValid(ani_env* env, ani_ref ref)
{
    return !IsUndefined(env, ref) && !IsNull(env, ref);
}

bool DrawingPointConvertToAniPoint(ani_env* env, ani_object obj, const Drawing::Point& point);

bool DrawingRectConvertToAniRect(ani_env* env, ani_object obj, const Drawing::Rect& rect);

std::shared_ptr<Font> GetThemeFont(std::shared_ptr<Font> font);

std::shared_ptr<Font> MatchThemeFont(std::shared_ptr<Font> font, int32_t unicode);

std::shared_ptr<FontMgr> GetFontMgr(std::shared_ptr<Font> font);

#ifdef ROSEN_OHOS
extern std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(Media::ColorSpace colorSpace);
extern Drawing::ColorType PixelFormatToDrawingColorType(Media::PixelFormat pixelFormat);
extern Drawing::AlphaType AlphaTypeToDrawingAlphaType(Media::AlphaType alphaType);
extern std::shared_ptr<Drawing::Image> ExtractDrawingImage(std::shared_ptr<Media::PixelMap> pixelMap);
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_DRAWING_UTILS_H