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

#ifndef OHOS_ANI_TEXT_UTILS_H
#define OHOS_ANI_TEXT_UTILS_H
#include <ani.h>
#include <memory>
#include <string>
#include <vector>

#include "ani_cache_manager.h"
#include "ani_common.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
class AniTextUtils final {
public:
    static ani_status ThrowBusinessError(ani_env* env, TextErrorCode errorCode, const char* message);
    static ani_status CreateBusinessError(ani_env* env, int32_t error, const char* message, ani_object& err);
    template <typename T>
    static T* GetNativeFromObj(ani_env* env, ani_object obj, const ani_method getNativeMethod);

    static ani_object CreateAniUndefined(ani_env* env);
    static bool IsUndefined(ani_env* env, ani_ref ref);
    template <typename... Args>
    static ani_object CreateAniObject(ani_env* env, const ani_class cls, const ani_method ctor, Args... params);
    static ani_object CreateAniArray(ani_env* env, size_t size);
    template <typename T, typename Converter>
    static ani_object CreateAniArrayAndInitData(ani_env* env, const std::vector<T>& t, size_t size, Converter convert);
    static ani_object CreateAniMap(ani_env* env);
    static ani_enum_item CreateAniEnum(ani_env* env, const ani_enum enumType, ani_size index);
    static ani_object CreateAniDoubleObj(ani_env* env, double val);
    static ani_object CreateAniIntObj(ani_env* env, int val);
    static ani_object CreateAniBooleanObj(ani_env* env, bool val);
    static ani_string CreateAniStringObj(ani_env* env, const std::string& str);
    static ani_string CreateAniStringObj(ani_env* env, const std::u16string& str);

    static ani_status AniToStdStringUtf8(ani_env* env, const ani_string& str, std::string& utf8Str);
    static ani_status AniToStdStringUtf16(ani_env* env, const ani_string& str, std::u16string& utf16Str);
    static bool ReadFile(const std::string& filePath, size_t& dataLen, std::unique_ptr<uint8_t[]>& data);
    static bool SplitAbsoluteFontPath(std::string& absolutePath);

    static ani_status ReadOptionalField(ani_env* env, ani_object obj, const ani_method getPropertyMethod, ani_ref& ref);
    static ani_status ReadOptionalDoubleField(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, double& value);
    static ani_status ReadOptionalIntField(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, int& value);
    static ani_status ReadOptionalStringField(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::string& str);
    static ani_status ReadOptionalU16StringField(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::u16string& str);
    static ani_status ReadOptionalBoolField(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, bool& value);
    template <typename EnumType>
    static ani_status ReadOptionalEnumField(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, EnumType& value);
    template <typename EnumType>
    static ani_status ReadEnumField(ani_env* env, ani_object obj, const ani_method getPropertyMethod, EnumType& value);
    template <typename T, typename Converter>
    static ani_status ReadOptionalArrayField(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::vector<T>& array, Converter convert);
    template <typename T, typename Converter>
    static ani_status ReadArrayField(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::vector<T>& array, Converter convert);

    static ani_status GetPropertyByCache_String(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::string& value);
    static ani_status GetPropertyByCache_U16String(
        ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::u16string& value);
};

template <typename... Args>
ani_object AniTextUtils::CreateAniObject(ani_env* env, const ani_class cls, const ani_method ctor, Args... params)
{
    if (cls == nullptr || ctor == nullptr) {
        TEXT_LOGE("Class or constructor is null");
        return CreateAniUndefined(env);
    }

    ani_object obj = {};
    if (env->Object_New(cls, ctor, &obj, params...) != ANI_OK) {
        TEXT_LOGE("Failed to create object");
        return CreateAniUndefined(env);
    }
    return obj;
}

template <typename T, typename Converter>
ani_object AniTextUtils::CreateAniArrayAndInitData(
    ani_env* env, const std::vector<T>& t, size_t size, Converter convert)
{
    ani_object arrayObj = CreateAniArray(env, size);
    ani_size index = 0;
    for (const T& item : t) {
        ani_object aniObj = convert(env, item);
        ani_status ret = env->Object_CallMethodByName_Void(arrayObj, "$_set", "iY:", index, aniObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Array $_set failed, ret %{public}d", ret);
            continue;
        }
        index++;
    }
    return arrayObj;
}

template <typename T>
T* AniTextUtils::GetNativeFromObj(ani_env* env, ani_object obj, const ani_method getNativeMethod)
{
    ani_status ret;
    ani_long nativeObj{};
    if ((ret = env->Object_CallMethod_Long(obj, getNativeMethod, &nativeObj)) != ANI_OK) {
        TEXT_LOGE("Failed to get native obj, ret %{public}d", ret);
        return nullptr;
    }
    T* object = reinterpret_cast<T*>(nativeObj);
    if (object == nullptr) {
        TEXT_LOGE("object is null");
        return nullptr;
    }
    return object;
};

template <typename EnumType>
ani_status AniTextUtils::ReadOptionalEnumField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, EnumType& value)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, getPropertyMethod, ref);
    if (result == ANI_OK && ref != nullptr) {
        ani_int index = 0;
        result = env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(ref), &index);
        if (result == ANI_OK) {
            value = static_cast<EnumType>(index);
        }
    }
    return result;
};

template <typename EnumType>
ani_status AniTextUtils::ReadEnumField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, EnumType& value)
{
    ani_ref ref = nullptr;
    ani_status result = env->Object_CallMethod_Ref(obj, getPropertyMethod, &ref);
    if (result == ANI_OK && ref != nullptr) {
        ani_int index = 0;
        result = env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(ref), &index);
        if (result == ANI_OK) {
            value = static_cast<EnumType>(index);
        }
    }
    return result;
};

template <typename T, typename Converter>
ani_status AniTextUtils::ReadOptionalArrayField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::vector<T>& array, Converter convert)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, getPropertyMethod, ref);
    if (result != ANI_OK || ref == nullptr) {
        TEXT_LOGE("Failed to read optional field, ret: %{public}d", result);
        return result;
    }

    ani_array arrayObj = reinterpret_cast<ani_array>(ref);
    ani_size length;
    result = env->Array_GetLength(arrayObj, &length);
    if (result != ANI_OK) {
        TEXT_LOGE("Failed to get length of array, ret: %{public}d", result);
        return result;
    }

    for (size_t i = 0; i < length; i++) {
        ani_ref entryRef = nullptr;
        result = env->Array_Get(arrayObj, i, &entryRef);
        if (result != ANI_OK || entryRef == nullptr) {
            TEXT_LOGE("Failed to get array object, ret: %{public}d", result);
            continue;
        }
        array.emplace_back(convert(env, entryRef));
    }
    return ANI_OK;
};

template <typename T, typename Converter>
ani_status AniTextUtils::ReadArrayField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::vector<T>& array, Converter convert)
{
    ani_ref ref = nullptr;
    ani_status result = env->Object_CallMethod_Ref(obj, getPropertyMethod, &ref);
    if (result != ANI_OK || ref == nullptr) {
        TEXT_LOGE("Failed to get field, ret: %{public}d", result);
        return result;
    }

    ani_array arrayObj = reinterpret_cast<ani_array>(ref);
    ani_size length;
    result = env->Array_GetLength(arrayObj, &length);
    if (result != ANI_OK) {
        TEXT_LOGE("Failed to get length of array, ret: %{public}d", result);
        return result;
    }

    for (size_t i = 0; i < length; i++) {
        ani_ref entryRef = nullptr;
        result = env->Array_Get(arrayObj, i, &entryRef);
        if (result != ANI_OK || entryRef == nullptr) {
            TEXT_LOGE("Failed to get array object, ret: %{public}d", result);
            continue;
        }
        array.emplace_back(convert(env, entryRef));
    }
    return ANI_OK;
};
} // namespace OHOS::Text::ANI
#endif // OHOS_ANI_TEXT_UTILS_H