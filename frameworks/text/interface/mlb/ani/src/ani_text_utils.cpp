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

#include "ani_text_utils.h"

#include <cstdarg>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

#include "typography_style.h"

namespace OHOS::Text::ANI {
namespace {
constexpr size_t FILE_HEAD_LENGTH = 7; // 7 is the size of "file://"
} // namespace

ani_status AniTextUtils::ThrowBusinessError(ani_env* env, TextErrorCode errorCode, const char* message)
{
    ani_object aniError;
    ani_status status = AniTextUtils::CreateBusinessError(env, static_cast<int32_t>(errorCode), message, aniError);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to create business, status %{public}d", static_cast<int32_t>(status));
        return status;
    }
    status = env->ThrowError(static_cast<ani_error>(aniError));
    if (status != ANI_OK) {
        TEXT_LOGE("Fail to throw err, status %{public}d", static_cast<int32_t>(status));
        return status;
    }
    return ANI_OK;
}

ani_status AniTextUtils::CreateBusinessError(ani_env* env, int32_t error, const char* message, ani_object& err)
{
    ani_string aniMsg = AniTextUtils::CreateAniStringObj(env, message);
    ani_status status = env->Object_New(AniGlobalClass::GetInstance().businessError,
        AniGlobalMethod::GetInstance().businessErrorCtor, &err, aniMsg, AniTextUtils::CreateAniUndefined(env));
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to new err, status %{public}d", static_cast<int32_t>(status));
        return status;
    }
    status = env->Object_SetPropertyByName_Int(err, "code", static_cast<ani_int>(error));
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to set code, status %{public}d", static_cast<int32_t>(status));
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
    ani_status status = env->Reference_IsUndefined(ref, &isUndefined);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to check if undefined, status %{public}d", static_cast<int32_t>(status));
        return false;
    }
    return isUndefined != 0;
}

ani_object AniTextUtils::CreateAniArray(ani_env* env, size_t size)
{
    ani_object arrayObj = nullptr;
    if (env->Object_New(
        AniGlobalClass::GetInstance().aniArray, AniGlobalMethod::GetInstance().arrayCtor, &arrayObj, size)
        != ANI_OK) {
        TEXT_LOGE("Failed to create object Array");
        return CreateAniUndefined(env);
    }
    return arrayObj;
}

ani_object AniTextUtils::CreateAniMap(ani_env* env)
{
    return AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().aniMap, AniGlobalMethod::GetInstance().map);
}

ani_object AniTextUtils::CreateAniOptionalEnum(ani_env* env, const ani_enum enumType, std::optional<ani_size> index)
{
    if (index.has_value()) {
        return AniTextUtils::CreateAniEnum(env, enumType, index.value());
    } else {
        return AniTextUtils::CreateAniUndefined(env);
    }
}

ani_enum_item AniTextUtils::CreateAniEnum(ani_env* env, const ani_enum enumType, ani_size index)
{
    ani_enum_item enumItem;
    ani_status status = env->Enum_GetEnumItemByIndex(enumType, index, &enumItem);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to create ani enum");
    }
    return enumItem;
}

ani_object AniTextUtils::CreateAniDoubleObj(ani_env* env, double val)
{
    return AniTextUtils::CreateAniObject(
        env, AniGlobalClass::GetInstance().aniDouble, AniGlobalMethod::GetInstance().doubleCtor, val);
}

ani_object AniTextUtils::CreateAniIntObj(ani_env* env, int val)
{
    return AniTextUtils::CreateAniObject(
        env, AniGlobalClass::GetInstance().aniInt, AniGlobalMethod::GetInstance().intCtor, val);
}

ani_object AniTextUtils::CreateAniBooleanObj(ani_env* env, bool val)
{
    return AniTextUtils::CreateAniObject(
        env, AniGlobalClass::GetInstance().aniBoolean, AniGlobalMethod::GetInstance().booleanCtor, val);
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
    utf16Str = std::u16string(reinterpret_cast<const char16_t*>(utf16Buffer), strSize - 1);
    return ANI_OK;
}

AniTextResult AniTextUtils::ReadFile(const std::string& filePath, size_t& dataLen, std::unique_ptr<uint8_t[]>& data)
{
    char realPath[PATH_MAX] = {0};
    if (realpath(filePath.c_str(), realPath) == nullptr) {
        TEXT_LOGE("Invalid filePath %{public}s", filePath.c_str());
        return AniTextResult::Error(MLB::ERROR_FILE_NOT_EXIST);
    }

    std::ifstream file(realPath);
    if (!file.is_open()) {
        TEXT_LOGE("Failed to open file:%{public}s", filePath.c_str());
        return AniTextResult::Error(MLB::ERROR_FILE_OPEN_FAILED);
    }
    file.seekg(0, std::ios::end);
    if (file.fail()) {
        return AniTextResult::Error(MLB::ERROR_FILE_SEEK_FAILED);
    }
    int length = file.tellg();
    if (length == -1) {
        TEXT_LOGE("Failed to get file length:%{public}s", filePath.c_str());
        file.close();
        return AniTextResult::Error(MLB::ERROR_FILE_SIZE_FAILED);
    } else if (length == 0) {
        TEXT_LOGE("File is empty: %{public}s", filePath.c_str());
        file.close();
        return AniTextResult::Error(MLB::ERROR_FILE_EMPTY);
    }
    dataLen = static_cast<size_t>(length);
    data = std::make_unique<uint8_t[]>(dataLen);
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(data.get()), dataLen);
    if (file.fail()) {
        return AniTextResult::Error(MLB::ERROR_FILE_READ_FAILED);
    }
    file.close();
    return AniTextResult::Success();
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
        // File path must start with 'file://', e.g., file:///path/to/font.ttf
        return true;
    }

    return false;
}

ani_status AniTextUtils::ReadOptionalField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, ani_ref& ref)
{
    ani_status ret = env->Object_CallMethod_Ref(obj, getPropertyMethod, &ref);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to get property, ret %{public}d", ret);
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

ani_status AniTextUtils::ReadOptionalDoubleField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, double& value)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, getPropertyMethod, ref);
    if (result == ANI_OK && ref != nullptr) {
        result = env->Object_CallMethod_Double(
            reinterpret_cast<ani_object>(ref), AniGlobalMethod::GetInstance().doubleGet, &value);
    }
    return result;
}

ani_status AniTextUtils::ReadOptionalIntField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, int& value)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, getPropertyMethod, ref);
    if (result == ANI_OK && ref != nullptr) {
        result = env->Object_CallMethod_Int(
            reinterpret_cast<ani_object>(ref), AniGlobalMethod::GetInstance().intGet, &value);
    }
    return result;
}

ani_status AniTextUtils::ReadOptionalStringField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::string& str)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, getPropertyMethod, ref);
    if (result == ANI_OK && ref != nullptr) {
        result = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), str);
    }
    return result;
}

ani_status AniTextUtils::ReadOptionalU16StringField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::u16string& str)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, getPropertyMethod, ref);
    if (result == ANI_OK && ref != nullptr) {
        result = AniTextUtils::AniToStdStringUtf16(env, reinterpret_cast<ani_string>(ref), str);
    }
    return result;
}

ani_status AniTextUtils::ReadOptionalBoolField(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, bool& value)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(env, obj, getPropertyMethod, ref);
    if (result == ANI_OK && ref != nullptr) {
        ani_boolean aniBool;
        result = env->Object_CallMethod_Boolean(
            reinterpret_cast<ani_object>(ref), AniGlobalMethod::GetInstance().booleanGet, &aniBool);
        if (result == ANI_OK) {
            value = static_cast<bool>(aniBool);
        }
    }
    return result;
}

ani_status AniTextUtils::GetPropertyByCache_String(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::string& value)
{
    ani_ref ref = nullptr;
    ani_status result = env->Object_CallMethod_Ref(obj, getPropertyMethod, &ref);
    if (result == ANI_OK && ref != nullptr) {
        result = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), value);
    }
    return result;
}

ani_status AniTextUtils::GetPropertyByCache_U16String(
    ani_env* env, ani_object obj, const ani_method getPropertyMethod, std::u16string& value)
{
    ani_ref ref = nullptr;
    ani_status result = env->Object_CallMethod_Ref(obj, getPropertyMethod, &ref);
    if (result == ANI_OK && ref != nullptr) {
        result = AniTextUtils::AniToStdStringUtf16(env, reinterpret_cast<ani_string>(ref), value);
    }
    return result;
}
} // namespace OHOS::Text::ANI