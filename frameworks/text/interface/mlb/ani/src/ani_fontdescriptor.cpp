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

#include "font_descriptor_mgr.h"

#include <unordered_set>

#include "ani_common.h"
#include "ani_fontdescriptor.h"
#include "ani_resource_parser.h"
#include "ani_text_utils.h"
#include "font_parser.h"
#include "typography_types.h"
#include "utils/text_log.h"
#include "utils/include/font_utils.h"
namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

namespace {
const std::string FONT_PATH_IN_SIGN =
    "X{C{" + std::string(ANI_GLOBAL_RESOURCE) + "}C{" + std::string(ANI_STRING) + "}}";

const std::string GET_SYSTEM_FONT_FULL_NAMES_BY_TYPE_SIGNATURE =
    "E{" + std::string(ANI_ENUM_SYSTEM_FONT_TYPE) + "}:C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_FONT_DESCRIPTOR_BY_FULL_NAME_SIGNATURE = "C{" + std::string(ANI_STRING) + "}E{"
    + std::string(ANI_ENUM_SYSTEM_FONT_TYPE) + "}:C{" + std::string(ANI_INTERFACE_FONT_DESCRIPTOR) + "}";
const std::string MATCH_FONT_DESCRIPTORS_SIGNATURE =
    "C{" + std::string(ANI_INTERFACE_FONT_DESCRIPTOR) + "}:C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_FONT_DESCRIPTORS_FROM_PATH_SIGNATURE = std::string(FONT_PATH_IN_SIGN) +
    ":C{" +std::string(ANI_ARRAY) + "}";
const std::string GET_FONT_UNICODE_SET_SIGNATURE = std::string(FONT_PATH_IN_SIGN) + "i" +
    ":C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_FONT_COUNT_SIGNATURE = std::string(FONT_PATH_IN_SIGN) + ":i";
}

#define READ_OPTIONAL_FIELD(env, obj, method, field, type, fontDescPtr, error_var)                                     \
    do {                                                                                                               \
        ani_status ret = AniTextUtils::ReadOptional##type##Field((env), (obj), (method), (fontDescPtr)->field);        \
        if (ret != ANI_OK) {                                                                                           \
            TEXT_LOGE("Failed to convert optional field: ret %{public}d", ret);                                        \
            (error_var) = ret;                                                                                         \
        }                                                                                                              \
    } while (0)

ani_status AniFontDescriptor::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ret;
    }

    std::array methods = {
        ani_native_function{"getSystemFontFullNamesByTypeSync", GET_SYSTEM_FONT_FULL_NAMES_BY_TYPE_SIGNATURE.c_str(),
            reinterpret_cast<void*>(GetSystemFontFullNamesByType)},
        ani_native_function{"getFontDescriptorByFullNameSync", GET_FONT_DESCRIPTOR_BY_FULL_NAME_SIGNATURE.c_str(),
            reinterpret_cast<void*>(GetFontDescriptorByFullName)},
        ani_native_function{"matchFontDescriptorsSync", MATCH_FONT_DESCRIPTORS_SIGNATURE.c_str(),
            reinterpret_cast<void*>(MatchFontDescriptors)},
        ani_native_function{"getFontDescriptorsFromPathSync", GET_FONT_DESCRIPTORS_FROM_PATH_SIGNATURE.c_str(),
            reinterpret_cast<void*>(GetFontDescriptorsFromPath)},
        ani_native_function{"getFontUnicodeSetSync", GET_FONT_UNICODE_SET_SIGNATURE.c_str(),
            reinterpret_cast<void*>(GetFontUnicodeSet)},
        ani_native_function{"getFontCount", GET_FONT_COUNT_SIGNATURE.c_str(),
            reinterpret_cast<void*>(GetFontCount)},
    };

    ret = env->Namespace_BindNativeFunctions(AniGlobalNamespace::GetInstance().text, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for AniFontDescriptor, ret %{public}d", ret);
        return ret;
    }
    return ANI_OK;
}

ani_status ParseFontDescriptorToNative(ani_env* env, ani_object& aniObj, FontDescSharedPtr& fontDesc)
{
    fontDesc = std::make_shared<TextEngine::FontParser::FontDescriptor>();

    ani_status status = ANI_OK;
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetPostScriptName, postScriptName,
        String, fontDesc.get(), status);
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetFullName, fullName, String,
        fontDesc.get(), status);
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetFontFamily, fontFamily, String,
        fontDesc.get(), status);
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetFontSubfamily, fontSubfamily,
        String, fontDesc.get(), status);
    READ_OPTIONAL_FIELD(
        env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetWidth, width, Int, fontDesc.get(), status);
    READ_OPTIONAL_FIELD(
        env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetItalic, italic, Int, fontDesc.get(), status);
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetMonoSpace, monoSpace, Bool,
        fontDesc.get(), status);
    READ_OPTIONAL_FIELD(
        env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetSymbolic, symbolic, Bool, fontDesc.get(), status);

    return status;
}

ani_status ParseFontDescriptorToAni(ani_env* env, const FontDescSharedPtr fontDesc, ani_object& aniObj)
{
    if (fontDesc == nullptr) {
        TEXT_LOGE("Empty data fontDesc");
        return ANI_ERROR;
    }

    std::pair<int, int> result;
    if (!OHOS::MLB::FindFontWeight(fontDesc->weight, result)) {
        TEXT_LOGE("Failed to parse weight");
        return ANI_ERROR;
    }

    aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().fontDescriptor,
        AniGlobalMethod::GetInstance().fontDescriptorCtor, AniTextUtils::CreateAniStringObj(env, fontDesc->path),
        AniTextUtils::CreateAniStringObj(env, fontDesc->postScriptName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->fullName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->fontFamily),
        AniTextUtils::CreateAniStringObj(env, fontDesc->fontSubfamily),
        AniTextUtils::CreateAniEnum(env, AniGlobalEnum::GetInstance().fontWeight,
            aniGetEnumIndex(AniTextEnum::fontWeight, static_cast<uint32_t>(result.second))
            .value_or(static_cast<uint32_t>(FontWeight::W400))),
        ani_int(fontDesc->width),
        ani_int(fontDesc->italic),
        ani_boolean(fontDesc->monoSpace),
        ani_boolean(fontDesc->symbolic),
        AniTextUtils::CreateAniStringObj(env, fontDesc->localPostscriptName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->localFullName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->localFamilyName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->localSubFamilyName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->version),
        AniTextUtils::CreateAniStringObj(env, fontDesc->manufacture),
        AniTextUtils::CreateAniStringObj(env, fontDesc->copyright),
        AniTextUtils::CreateAniStringObj(env, fontDesc->trademark),
        AniTextUtils::CreateAniStringObj(env, fontDesc->license),
        ani_int(fontDesc->index)
    );

    return ANI_OK;
}

ani_object CreateFontDescriptorArray(ani_env* env, const std::vector<FontDescSharedPtr>& fontDescripters)
{
    std::set<FontDescSharedPtr> fontDescripterList(fontDescripters.begin(), fontDescripters.end());

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, fontDescripterList.size());
    ani_boolean isUndefined = false;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniArray(env, 0);
    }

    ani_size index = 0;
    for (const auto& item : fontDescripterList) {
        ani_object aniObj = nullptr;
        if (item != nullptr) {
            item->weight = OHOS::MLB::RegularWeight(item->weight);
        }
        ani_status status = ParseFontDescriptorToAni(env, item, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to parse FontDescriptor to ani,index %{public}zu,status %{public}d", index, status);
            continue;
        }
        status = env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, index, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to set FontDescriptor item,index %{public}zu,status %{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object ProcessStringPath(ani_env* env, ani_object path)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;

    std::string pathStr;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(path), pathStr);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniArray(env, 0);
    }
    if (!AniTextUtils::SplitAbsoluteFontPath(pathStr) || !AniTextUtils::ReadFile(pathStr, dataLen, data).success) {
        TEXT_LOGE("Failed to split absolute font path");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    auto fontDescripters = TextEngine::FontParser::ParserFontDescriptorsFromPath(pathStr);
    return CreateFontDescriptorArray(env, fontDescripters);
}

ani_object ProcessResourcePath(ani_env* env, ani_object path)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;
    AniResource resource = AniResourceParser::ParseResource(env, path);
    if (!AniResourceParser::ResolveResource(resource, dataLen, data).success) {
        TEXT_LOGE("Failed to resolve resource");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    auto fontDescripters = TextEngine::FontParser::ParserFontDescriptorsFromStream(data.get(), dataLen);
    return CreateFontDescriptorArray(env, fontDescripters);
}

ani_object AniFontDescriptor::GetSystemFontFullNamesByType(ani_env* env, ani_enum_item fontType)
{
    ani_int typeIndex;
    ani_status ret = env->EnumItem_GetValue_Int(fontType, &typeIndex);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse fontType,ret %{public}d", ret);
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Parameter fontType is invalid");
        return AniTextUtils::CreateAniUndefined(env);
    }

    TextEngine::FontParser::SystemFontType systemFontType =
        static_cast<TextEngine::FontParser::SystemFontType>(typeIndex);

    std::unordered_set<std::string> fontList;
    FontDescriptorMgrInstance.GetSystemFontFullNamesByType(systemFontType, fontList);

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, fontList.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& item : fontList) {
        ani_string aniStr = AniTextUtils::CreateAniStringObj(env, item);
        if (ANI_OK != env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, index, aniStr)) {
            TEXT_LOGE("Failed to set fontList item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniFontDescriptor::GetFontDescriptorByFullName(ani_env* env, ani_string fullName, ani_enum_item fontType)
{
    std::string fullNameStr;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, fullName, fullNameStr);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse fullName,ret %{public}d", ret);
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Parameter fullName is invalid");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_int typeIndex;
    ret = env->EnumItem_GetValue_Int(fontType, &typeIndex);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse fontType,ret %{public}d", ret);
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Parameter fontType is invalid");
        return AniTextUtils::CreateAniUndefined(env);
    }

    TextEngine::FontParser::SystemFontType systemFontType =
        static_cast<TextEngine::FontParser::SystemFontType>(typeIndex);

    FontDescSharedPtr resultDesc = nullptr;
    FontDescriptorMgrInstance.GetFontDescSharedPtrByFullName(fullNameStr, systemFontType, resultDesc);
    ani_object descAniObj = nullptr;
    if (resultDesc != nullptr) {
        resultDesc->weight = OHOS::MLB::RegularWeight(resultDesc->weight);
    }
    ret = ParseFontDescriptorToAni(env, resultDesc, descAniObj);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse FontDescSharedPtr,ret %{public}d", ret);
        return AniTextUtils::CreateAniUndefined(env);
    }
    return descAniObj;
}

ani_object AniFontDescriptor::MatchFontDescriptors(ani_env* env, ani_object desc)
{
    FontDescSharedPtr matchDesc = nullptr;
    ani_status ret = ParseFontDescriptorToNative(env, desc, matchDesc);
    if (ret != ANI_OK || matchDesc == nullptr) {
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed to parse desc");
        return AniTextUtils::CreateAniUndefined(env);
    }
    std::set<FontDescSharedPtr> matchResult;
    FontDescriptorMgrInstance.MatchFontDescriptors(matchDesc, matchResult);

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, matchResult.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& item : matchResult) {
        ani_object aniObj = nullptr;
        ani_status status = ParseFontDescriptorToAni(env, item, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to parse FontDescriptor to ani,index %{public}zu,status %{public}d", index, status);
            continue;
        }
        status = env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, index, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to set FontDescriptor item,index %{public}zu,status %{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniFontDescriptor::GetFontDescriptorsFromPath(ani_env* env, ani_object path)
{
    ani_boolean isString = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().aniString, &isString);
    if (isString) {
        return ProcessStringPath(env, path);
    }

    ani_boolean isResource = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().globalResource, &isResource);
    if (isResource) {
        return ProcessResourcePath(env, path);
    }

    return AniTextUtils::CreateAniArray(env, 0);
}

ani_object GenerateUnicodeSetArray(ani_env* env, const std::vector<uint32_t>& unicodeSet)
{
    ani_object arrayObj = AniTextUtils::CreateAniArray(env, unicodeSet.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    ani_size index = 0;
    for (const auto& unicode : unicodeSet) {
        ani_status ret = env->Object_CallMethod_Void(
            arrayObj, AniGlobalMethod::GetInstance().arraySet, index, AniTextUtils::CreateAniIntObj(env, unicode));
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set unicode item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object GetUnicodeSetByPath(ani_env* env, ani_object path, ani_int index)
{
    std::string pathStr;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(path), pathStr);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to get path");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    if (!AniTextUtils::SplitAbsoluteFontPath(pathStr)) {
        TEXT_LOGE("Failed to split absolute font path: %{public}s", pathStr.c_str());
        return AniTextUtils::CreateAniArray(env, 0);
    }

    std::vector<uint32_t> unicodeSet = TextEngine::FontParser::GetFontTypefaceUnicode(pathStr, index);
    return GenerateUnicodeSetArray(env, unicodeSet);
}

ani_object GetUnicodeSetByResource(ani_env* env, ani_object path, ani_int index)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;
    AniResource resource = AniResourceParser::ParseResource(env, path);
    if (!AniResourceParser::ResolveResource(resource, dataLen, data).success) {
        TEXT_LOGE("Failed to resolve resource");
        return AniTextUtils::CreateAniArray(env, 0);
    }

    std::vector<uint32_t> unicodeSet = TextEngine::FontParser::GetFontTypefaceUnicode(data.get(), dataLen, index);
    return GenerateUnicodeSetArray(env, unicodeSet);
}

ani_object AniFontDescriptor::GetFontUnicodeSet(ani_env* env, ani_object path, ani_int index)
{
    ani_boolean isString = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().aniString, &isString);
    if (isString) {
        return GetUnicodeSetByPath(env, path, index);
    }
    ani_boolean isResource = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().globalResource, &isResource);
    if (isResource) {
        return GetUnicodeSetByResource(env, path, index);
    }

    return AniTextUtils::CreateAniArray(env, 0);
}

ani_int GetFontCountByPath(ani_env* env, ani_object path)
{
    std::string pathStr;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(path), pathStr);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to get path");
        return 0;
    }
    if (!AniTextUtils::SplitAbsoluteFontPath(pathStr)) {
        TEXT_LOGE("Failed to split absolute font path: %{public}s", pathStr.c_str());
        return 0;
    }
    return TextEngine::FontParser::GetFontCount(pathStr);
}

ani_int GetFontCountByResource(ani_env* env, ani_object path)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;
    AniResource resource = AniResourceParser::ParseResource(env, path);
    if (!AniResourceParser::ResolveResource(resource, dataLen, data).success) {
        TEXT_LOGE("Failed to resolve resource");
        return 0;
    }
    std::vector<uint8_t> fontData(data.get(), data.get() + dataLen);
    return TextEngine::FontParser::GetFontCount(fontData);
}

ani_int AniFontDescriptor::GetFontCount(ani_env* env, ani_object path)
{
    ani_boolean isString = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().aniString, &isString);
    if (isString) {
        return GetFontCountByPath(env, path);
    }
    ani_boolean isResource = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().globalResource, &isResource);
    if (isResource) {
        return GetFontCountByResource(env, path);
    }
    return 0;
}
} // namespace OHOS::Text::ANI