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
#include "ani_text_utils.h"
#include "font_parser.h"
#include "typography_types.h"
#include "utils/text_log.h"
namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

namespace {
std::unordered_map<int, int> g_weightMap = {
    {100, static_cast<int>(FontWeight::W100)},
    {200, static_cast<int>(FontWeight::W200)},
    {300, static_cast<int>(FontWeight::W300)},
    {400, static_cast<int>(FontWeight::W400)},
    {500, static_cast<int>(FontWeight::W500)},
    {600, static_cast<int>(FontWeight::W600)},
    {700, static_cast<int>(FontWeight::W700)},
    {800, static_cast<int>(FontWeight::W800)},
    {900, static_cast<int>(FontWeight::W900)}
};
}

#define READ_OPTIONAL_FIELD(env, obj, param, field, type, fontDescPtr, error_var) \
    do { \
        ani_status ret = AniTextUtils::ReadOptional##type##Field( \
            (env), (obj), (param), (fontDescPtr)->field); \
        if (ret != ANI_OK) { \
            TEXT_LOGE("Failed to convert %{public}s: ret %{public}d", param.GetCacheKey(), ret); \
            (error_var) = ret; \
        } \
    } while (0)

ani_status AniFontDescriptor::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ret;
    }

    ani_namespace ns = nullptr;
    ret = AniTextUtils::FindNamespaceWithCache(env, ANI_NAMESPACE_TEXT, ns);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find namespace %{public}s, ret %{public}d", ANI_NAMESPACE_TEXT, ret);
        return ret;
    }

    std::string getSystemFontFullNamesByTypeSignature =
        "E{" + std::string(ANI_ENUM_SYSTEM_FONT_TYPE) + "}:C{" + std::string(ANI_ARRAY) + "}";
    std::string getFontDescriptorByFullNameSignature = "C{" + std::string(ANI_STRING) + "}E{"
        + std::string(ANI_ENUM_SYSTEM_FONT_TYPE) + "}:C{" + std::string(ANI_INTERFACE_FONT_DESCRIPTOR) + "}";
    std::string matchFontDescriptorsSignature =
        "C{" + std::string(ANI_INTERFACE_FONT_DESCRIPTOR) + "}:C{" + std::string(ANI_ARRAY) + "}";

    std::array methods = {
        ani_native_function{"getSystemFontFullNamesByTypeSync", getSystemFontFullNamesByTypeSignature.c_str(),
            reinterpret_cast<void*>(GetSystemFontFullNamesByType)},
        ani_native_function{"getFontDescriptorByFullNameSync", getFontDescriptorByFullNameSignature.c_str(),
            reinterpret_cast<void*>(GetFontDescriptorByFullName)},
        ani_native_function{"matchFontDescriptorsSync", matchFontDescriptorsSignature.c_str(),
            reinterpret_cast<void*>(MatchFontDescriptors)},
    };

    ret = env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size());
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
    static AniCacheParam postScriptNameParam =
        { ANI_INTERFACE_FONT_DESCRIPTOR, "<get>postScriptName", ANI_WRAP_RETURN_C(ANI_STRING) };
    READ_OPTIONAL_FIELD(env, aniObj, postScriptNameParam, postScriptName, String, fontDesc.get(), status);
    static AniCacheParam fullNameParam =
        { ANI_INTERFACE_FONT_DESCRIPTOR, "<get>fullName", ANI_WRAP_RETURN_C(ANI_STRING) };
    READ_OPTIONAL_FIELD(env, aniObj, fullNameParam, fullName, String, fontDesc.get(), status);
    static AniCacheParam fontFamilyParam =
        { ANI_INTERFACE_FONT_DESCRIPTOR, "<get>fontFamily", ANI_WRAP_RETURN_C(ANI_STRING) };
    READ_OPTIONAL_FIELD(env, aniObj, fontFamilyParam, fontFamily, String, fontDesc.get(), status);
    static AniCacheParam fontSubfamilyParam =
        { ANI_INTERFACE_FONT_DESCRIPTOR, "<get>fontSubfamily", ANI_WRAP_RETURN_C(ANI_STRING) };
    READ_OPTIONAL_FIELD(env, aniObj, fontSubfamilyParam, fontSubfamily, String, fontDesc.get(), status);
    static AniCacheParam widthParam = { ANI_INTERFACE_FONT_DESCRIPTOR, "<get>width", ANI_WRAP_RETURN_C(ANI_INT) };
    READ_OPTIONAL_FIELD(env, aniObj, widthParam, width, Int, fontDesc.get(), status);
    static AniCacheParam italicParam = { ANI_INTERFACE_FONT_DESCRIPTOR, "<get>italic", ANI_WRAP_RETURN_C(ANI_INT) };
    READ_OPTIONAL_FIELD(env, aniObj, italicParam, italic, Int, fontDesc.get(), status);
    static AniCacheParam monoSpaceParam = { ANI_INTERFACE_FONT_DESCRIPTOR, "<get>monoSpace", ANI_WRAP_RETURN_C(ANI_BOOLEAN) };
    READ_OPTIONAL_FIELD(env, aniObj, monoSpaceParam, monoSpace, Bool, fontDesc.get(), status);
    static AniCacheParam symbolicParam = { ANI_INTERFACE_FONT_DESCRIPTOR, "<get>symbolic", ANI_WRAP_RETURN_C(ANI_BOOLEAN) };
    READ_OPTIONAL_FIELD(env, aniObj, symbolicParam, symbolic, Bool, fontDesc.get(), status);

    return status;
}

ani_status ParseFontDescriptorToAni(ani_env* env, const FontDescSharedPtr fontDesc, ani_object& aniObj)
{
    if (fontDesc == nullptr) {
        TEXT_LOGE("Empty data fontDesc");
        return ANI_ERROR;
    }

    auto iter = g_weightMap.find(fontDesc->weight);
    if (iter == g_weightMap.end()) {
        TEXT_LOGE("Failed to parse weight");
        return ANI_ERROR;
    }
    static const std::string aniStringDescriptor = "C{" + std::string(ANI_STRING) + "}";
    static const std::string sign = aniStringDescriptor + aniStringDescriptor + aniStringDescriptor +
        aniStringDescriptor + aniStringDescriptor + "E{" + std::string(ANI_ENUM_FONT_WEIGHT) +
        "}iizz:";

    aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_DESCRIPTOR, sign.c_str(),
        AniTextUtils::CreateAniStringObj(env, fontDesc->path),
        AniTextUtils::CreateAniStringObj(env, fontDesc->postScriptName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->fullName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->fontFamily),
        AniTextUtils::CreateAniStringObj(env, fontDesc->fontSubfamily),
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_FONT_WEIGHT, static_cast<int>(iter->second)),
        ani_int(fontDesc->width),
        ani_int(fontDesc->italic),
        ani_boolean(fontDesc->monoSpace),
        ani_boolean(fontDesc->symbolic)
    );

    return ANI_OK;
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
        if (ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "iY:", index, aniStr)) {
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
        status = env->Object_CallMethodByName_Void(arrayObj, "$_set", "iY:", index, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to set FontDescriptor item,index %{public}zu,status %{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}
} // namespace OHOS::Text::ANI