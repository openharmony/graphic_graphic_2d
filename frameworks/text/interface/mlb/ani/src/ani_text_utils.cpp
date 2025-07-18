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

#include <cstdarg>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

#include "ani_text_utils.h"
#include "typography_style.h"

namespace OHOS::Text::ANI {
constexpr size_t FILE_HEAD_LENGTH = 7; // 7 is the size of "file://"
ani_status AniTextUtils::ThrowBusinessError(ani_env* env, TextErrorCode errorCode, const char* message)
{
    ani_object aniError;
    ani_status status = AniTextUtils::CreateBusinessError(env, static_cast<int32_t>(errorCode), message, aniError);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to create business, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    status = env->ThrowError(static_cast<ani_error>(aniError));
    if (status != ANI_OK) {
        TEXT_LOGE("Fail to throw err, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    return ANI_OK;
}

ani_status AniTextUtils::CreateBusinessError(ani_env* env, int32_t error, const char* message, ani_object& err)
{
    ani_class aniClass;
    ani_status status = env->FindClass("L@ohos/base/BusinessError;", &aniClass);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find class, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    ani_method aniCtor;
    status = env->Class_FindMethod(aniClass, "<ctor>", "Lstd/core/String;Lescompat/ErrorOptions;:V", &aniCtor);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to find ctor, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    ani_string aniMsg = AniTextUtils::CreateAniStringObj(env, message);
    status = env->Object_New(aniClass, aniCtor, &err, aniMsg, AniTextUtils::CreateAniUndefined(env));
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to new err, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    status = env->Object_SetPropertyByName_Double(err, "code", static_cast<ani_int>(error));
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to set code, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    return ANI_OK;
}

ani_object AniTextUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return reinterpret_cast<ani_object>(aniRef);
}

bool AniTextUtils::IsUndefined(ani_env* env, ani_ref ref)
{
    ani_boolean isUndefined = 0;
    ani_status status = env->Reference_IsUndefined(env, &isUndefined);
    if (status != ANI_OK) {
        TEXT_LOGE("Fail to check if undefined, status: %{public}d", static_cast<int32_t>(status));
        return false;
    }
    if (isUndefined != 0) {
        return true;
    }
    return false;
}

ani_object AniTextUtils::CreateAniArray(ani_env* env, size_t size)
{
    ani_class arrayCls;
    if (env->FindClass(ANI_ARRAY, &arrayCls) != ANI_OK) {
        TEXT_LOGE("Failed to findClass Lescompat/Array;");
        return CreateAniUndefined(env);
    }
    ani_method arrayCtor;
    if (env->Class_FindMethod(arrayCls, "<ctor>", "I:V", &arrayCtor) != ANI_OK) {
        TEXT_LOGE("Failed to find <ctor>");
        return CreateAniUndefined(env);
    }
    ani_object arrayObj = nullptr;
    if (env->Object_New(arrayCls, arrayCtor, &arrayObj, size) != ANI_OK) {
        TEXT_LOGE("Failed to create object Array");
        return CreateAniUndefined(env);
    }
    return arrayObj;
}

ani_object AniTextUtils::CreateAniMap(ani_env* env)
{
    return AniTextUtils::CreateAniObject(env, ANI_MAP, ":V");
}

ani_enum_item AniTextUtils::CreateAniEnum(ani_env* env, const char* enum_descriptor, ani_size index)
{
    ani_enum enumType;
    ani_status ret = env->FindEnum(enum_descriptor, &enumType);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find enum,%{public}s", enum_descriptor);
        return nullptr;
    }
    ani_enum_item enumItem;
    env->Enum_GetEnumItemByIndex(enumType, index, &enumItem);
    return enumItem;
}

ani_object AniTextUtils::CreateAniDoubleObj(ani_env* env, double val)
{
    return AniTextUtils::CreateAniObject(env, ANI_OBJECT, "D:V", val);
}

ani_object AniTextUtils::CreateAniIntObj(ani_env* env, int val)
{
    return AniTextUtils::CreateAniObject(env, ANI_INT, "I:V", val);
}

ani_object AniTextUtils::CreateAniBooleanObj(ani_env* env, bool val)
{
    return AniTextUtils::CreateAniObject(env, ANI_BOOLEAN, "Z:V", val);
}

ani_string AniTextUtils::CreateAniStringObj(ani_env* env, const std::string& str)
{
    ani_string result_string{};
    env->String_NewUTF8(str.c_str(), str.size(), &result_string);
    return result_string;
}

ani_string AniTextUtils::CreateAniStringObj(ani_env* env, const std::u16string& str)
{
    ani_string result_string{};
    env->String_NewUTF16(reinterpret_cast<const uint16_t*>(str.c_str()), str.size(), &result_string);
    return result_string;
}

ani_status AniTextUtils::AniToStdStringUtf8(ani_env* env, const ani_string& str, std::string& utf8Str)
{
    ani_size strSize;
    ani_status status = env->String_GetUTF8Size(str, &strSize);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to get utf8 str size");
        return status;
    }

    strSize++;
    std::vector<char> buffer(strSize);
    char* utf8Buffer = buffer.data();

    ani_size bytesWritten = 0;
    status = env->String_GetUTF8(str, utf8Buffer, strSize, &bytesWritten);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to get utf8 str");
        return status;
    }

    utf8Buffer[bytesWritten] = '\0';
    utf8Str = std::string(utf8Buffer);
    return ANI_OK;
}

ani_status AniTextUtils::AniToStdStringUtf16(ani_env* env, const ani_string& str, std::u16string& utf16Str)
{
    ani_size strSize;
    ani_status status = env->String_GetUTF16Size(str, &strSize);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to get utf16 str size");
        return status;
    }

    strSize++;
    std::vector<uint16_t> buffer(strSize);
    uint16_t* utf16Buffer = buffer.data();

    ani_size bytesWritten = 0;
    status = env->String_GetUTF16(str, utf16Buffer, strSize, &bytesWritten);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to get utf16 str");
        return status;
    }
    utf16Buffer[bytesWritten] = '\0';
    utf16Str = std::u16string(reinterpret_cast<const char16_t*>(utf16Buffer), strSize);
    return ANI_OK;
}

bool AniTextUtils::ReadFile(const std::string& filePath, size_t& dataLen, std::unique_ptr<uint8_t[]>& data)
{
    char realPath[PATH_MAX] = {0};
    if (realpath(filePath.c_str(), realPath) == nullptr) {
        TEXT_LOGE("Invalid filePath %{public}s", filePath.c_str());
        return false;
    }

    std::ifstream file(realPath);
    if (!file.is_open()) {
        TEXT_LOGE("Failed to open file:%{public}s", filePath.c_str());
        return false;
    }
    file.seekg(0, std::ios::end);
    int length = file.tellg();
    if (length == -1) {
        TEXT_LOGE("Failed to get file length:%{public}s", filePath.c_str());
        file.close();
        return false;
    }
    dataLen = static_cast<size_t>(length);
    data = std::make_unique<uint8_t[]>(dataLen);
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(data.get()), dataLen);
    file.close();
    return true;
}

bool AniTextUtils::SplitAbsoluteFontPath(std::string& absolutePath)
{
    auto iter = absolutePath.find_first_of(':');
    if (iter == std::string::npos) {
        TEXT_LOGE("Failed to find separator in path:%{public}s", absolutePath.c_str());
        return false;
    }
    std::string head = absolutePath.substr(0, iter);
    if ((head == "file" && absolutePath.size() > FILE_HEAD_LENGTH)) {
        absolutePath = absolutePath.substr(iter + 3); // 3 means skip "://"
        // the file format is like "file://system/fonts...",
        return true;
    }

    return false;
}

ani_status AniTextUtils::ReadOptionalField(ani_env* env, ani_object obj, const char* fieldName, ani_ref& ref)
{
    ani_status ret = env->Object_GetPropertyByName_Ref(obj, fieldName, &ref);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to get property %{public}s, ret %{public}d", fieldName, ret);
        return ret;
    }
    ani_boolean isUndefined;
    ret = env->Reference_IsUndefined(ref, &isUndefined);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to check ref is undefined, ret %{public}d", ret);
        ref = nullptr;
        return ret;
    }

    if (isUndefined) {
        ref = nullptr;
    }
    return ret;
}

ani_status AniTextUtils::ReadOptionalDoubleField(ani_env* env, ani_object obj, const char* fieldName, double& value)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, fieldName, ref);
    if (result == ANI_OK && ref != nullptr) {
        env->Object_CallMethodByName_Double(reinterpret_cast<ani_object>(ref), "unboxed", ":D", &value);
    }
    return result;
}


ani_status AniTextUtils::ReadOptionalIntField(ani_env* env, ani_object obj, const char* fieldName, int& value)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, fieldName, ref);
    if (result == ANI_OK && ref != nullptr) {
        env->Object_CallMethodByName_Int(reinterpret_cast<ani_object>(ref), "intValue", ":I", &value);
    }
    return result;
}

ani_status AniTextUtils::ReadOptionalStringField(ani_env* env, ani_object obj, const char* fieldName, std::string& str)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, fieldName, ref);
    if (result == ANI_OK && ref != nullptr) {
        std::string familyName;
        ani_status ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), str);
        if (ret != ANI_OK) {
            return result;
        }
    }
    return result;
}

ani_status AniTextUtils::ReadOptionalU16StringField(
    ani_env* env, ani_object obj, const char* fieldName, std::u16string& str)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, fieldName, ref);
    if (result == ANI_OK && ref != nullptr) {
        result = AniTextUtils::AniToStdStringUtf16(env, reinterpret_cast<ani_string>(ref), str);
    }
    return result;
}

ani_status AniTextUtils::ReadOptionalBoolField(ani_env* env, ani_object obj, const char* fieldName, bool& value)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, fieldName, ref);
    if (result == ANI_OK && ref != nullptr) {
        ani_boolean aniBool;
        result = env->Object_CallMethodByName_Boolean(reinterpret_cast<ani_object>(ref), "isTrue", ":Z", &aniBool);
        if (result == ANI_OK) {
            value = static_cast<bool>(aniBool);
        }
    }
    return result;
}
} // namespace OHOS::Text::ANI