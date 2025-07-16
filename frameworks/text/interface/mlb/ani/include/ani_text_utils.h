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

#include "ani_common.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
class AniTextUtils final {
public:
    static ani_status ThrowBusinessError(ani_env* env, TextErrorCode errorCode, const char* message);
    static ani_status CreateBusinessError(ani_env* env, int32_t error, const char* message, ani_object& err);
    template <typename T>
    static T* GetNativeFromObj(ani_env* env, ani_object obj, const char* name = NATIVE_OBJ);

    static ani_object CreateAniUndefined(ani_env* env);
    template <typename... Args>
    static ani_object CreateAniObject(ani_env* env, const std::string name, const char* signature, Args... params);
    static ani_object CreateAniArray(ani_env* env, size_t size);
    template <typename T, typename Converter>
    static ani_object CreateAniArrayAndInitData(ani_env* env, const std::vector<T>& t, size_t size, Converter convert);
    static ani_object CreateAniMap(ani_env* env);
    static ani_enum_item CreateAniEnum(ani_env* env, const char* enum_descriptor, ani_size index);
    static ani_object CreateAniDoubleObj(ani_env* env, double val);
    static ani_object CreateAniIntObj(ani_env* env, int val);
    static ani_object CreateAniBooleanObj(ani_env* env, bool val);
    static ani_string CreateAniStringObj(ani_env* env, const std::string& str);
    static ani_string CreateAniStringObj(ani_env* env, const std::u16string& str);

    static ani_status AniToStdStringUtf8(ani_env* env, const ani_string& str, std::string& utf8Str);
    static ani_status AniToStdStringUtf16(ani_env* env, const ani_string& str, std::u16string& utf16Str);
    static bool ReadFile(const std::string& filePath, size_t& dataLen, std::unique_ptr<uint8_t[]>& data);
    static bool SplitAbsoluteFontPath(std::string& absolutePath);

    static ani_status ReadOptionalField(ani_env* env, ani_object obj, const char* fieldName, ani_ref& ref);
    static ani_status ReadOptionalDoubleField(ani_env* env, ani_object obj, const char* fieldName, double& value);
    static ani_status ReadOptionalStringField(ani_env* env, ani_object obj, const char* fieldName, std::string& str);
    static ani_status ReadOptionalU16StringField(
        ani_env* env, ani_object obj, const char* fieldName, std::u16string& str);
    static ani_status ReadOptionalBoolField(ani_env* env, ani_object obj, const char* fieldName, bool& value);
    template <typename EnumType>
    static ani_status ReadOptionalEnumField(ani_env* env, ani_object obj, const char* fieldName, EnumType& value);
    template <typename EnumType>
    static ani_status ReadEnumField(ani_env* env, ani_object obj, const char* fieldName, EnumType& value);
    template <typename T, typename Converter>
    static ani_status ReadOptionalArrayField(
        ani_env* env, ani_object obj, const char* fieldName, std::vector<T>& array, Converter convert);
};

template <typename... Args>
ani_object AniTextUtils::CreateAniObject(ani_env* env, const std::string name, const char* signature, Args... params)
{
    ani_class cls = nullptr;
    if (env->FindClass(name.c_str(), &cls) != ANI_OK) {
        TEXT_LOGE("Failed to found %{public}s", name.c_str());
        return CreateAniUndefined(env);
    }
    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", signature, &ctor) != ANI_OK) {
        TEXT_LOGE("Failed to get ctor %{public}s", name.c_str());
        return CreateAniUndefined(env);
    }
    ani_object obj = {};
    if (env->Object_New(cls, ctor, &obj, params...) != ANI_OK) {
        TEXT_LOGE("Failed to create object %{public}s", name.c_str());
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
        ani_status ret = env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Array $_set failed, ret:%{public}d", ret);
            continue;
        }
        index++;
    }
    return arrayObj;
}

template <typename T>
T* AniTextUtils::GetNativeFromObj(ani_env* env, ani_object obj, const char* name)
{
    ani_status ret;
    ani_long nativeObj{};
    if ((ret = env->Object_GetFieldByName_Long(obj, name, &nativeObj)) != ANI_OK) {
        TEXT_LOGE("Failed to get native obj, ani_status: %{public}d", ret);
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
ani_status AniTextUtils::ReadOptionalEnumField(ani_env* env, ani_object obj, const char* fieldName, EnumType& value)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, fieldName, ref);
    if (result == ANI_OK && ref != nullptr) {
        ani_size index = 0;
        result = env->EnumItem_GetIndex(reinterpret_cast<ani_enum_item>(ref), &index);
        if (result == ANI_OK) {
            value = static_cast<EnumType>(index);
        }
    }
    return result;
};

template <typename EnumType>
ani_status AniTextUtils::ReadEnumField(ani_env* env, ani_object obj, const char* fieldName, EnumType& value)
{
    ani_ref ref = nullptr;
    ani_status result = env->Object_GetPropertyByName_Ref(obj, fieldName, &ref);
    if (result == ANI_OK && ref != nullptr) {
        ani_size index = 0;
        result = env->EnumItem_GetIndex(reinterpret_cast<ani_enum_item>(ref), &index);
        if (result == ANI_OK) {
            value = static_cast<EnumType>(index);
        }
    }
    return result;
};

template <typename T, typename Converter>
ani_status AniTextUtils::ReadOptionalArrayField(
    ani_env* env, ani_object obj, const char* fieldName, std::vector<T>& array, Converter convert)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, fieldName, ref);
    if (result != ANI_OK || ref == nullptr) {
        return result;
    }

    ani_object arrayObj = reinterpret_cast<ani_object>(ref);
    ani_double length;
    result = env->Object_GetPropertyByName_Double(arrayObj, "length", &length);
    if (result != ANI_OK) {
        TEXT_LOGE("Failed to get length,%{public}s", fieldName);
        return result;
    }

    for (size_t i = 0; i < static_cast<size_t>(length); i++) {
        ani_ref entryRef = nullptr;
        result = env->Object_CallMethodByName_Ref(arrayObj, "$_get", "I:Lstd/core/Object;", &entryRef, i);
        if (result != ANI_OK || entryRef == nullptr) {
            TEXT_LOGE("Failed to get array object,%{public}s", fieldName);
            continue;
        }
        array.emplace_back(convert(env, entryRef));
    }
    return ANI_OK;
};
} // namespace OHOS::Text::ANI
#endif // OHOS_ANI_TEXT_UTILS_H