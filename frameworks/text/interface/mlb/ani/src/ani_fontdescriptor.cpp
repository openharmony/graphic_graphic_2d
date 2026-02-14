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
const std::string GET_FONT_PATHS_BY_TYPE_SIGNATURE =
    "E{" + std::string(ANI_ENUM_SYSTEM_FONT_TYPE) + "}:C{" + std::string(ANI_ARRAY) + "}";
const std::string IS_FONT_SUPPORTED_SIGNATURE = FONT_PATH_IN_SIGN + ":z";
} // namespace

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
        ani_native_function{"getFontPathsByType", GET_FONT_PATHS_BY_TYPE_SIGNATURE.c_str(),
            reinterpret_cast<void*>(GetFontPathsByType)},
        ani_native_function{"isFontSupported", IS_FONT_SUPPORTED_SIGNATURE.c_str(),
            reinterpret_cast<void*>(IsFontSupported)},
    };

    ret = env->Namespace_BindNativeFunctions(AniGlobalNamespace::GetInstance().text, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for AniFontDescriptor, ret %{public}d", ret);
        return ret;
    }
    return ANI_OK;
}

static ani_status ParseFontVariationToNative(ani_env* env, ani_ref ref,
    TextEngine::FontParser::FontVariation& variation)
{
    if (ref == nullptr) {
        return ANI_ERROR;
    }
    ani_object variationObj = reinterpret_cast<ani_object>(ref);

    ani_ref axisRef = nullptr;
    ani_status ret = env->Object_CallMethod_Ref(variationObj,
        AniGlobalMethod::GetInstance().fontVariationAxis, &axisRef);
    if (ret != ANI_OK || axisRef == nullptr) {
        TEXT_LOGE("ParseFontVariationToNative: Failed to get axisRef, ret=%{public}d", ret);
        return ret;
    }
    ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(axisRef), variation.axis);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseFontVariationToNative: Failed to convert axis to string, ret=%{public}d", ret);
        return ret;
    }

    ret = AniTextUtils::ReadOptionalDoubleField(env, variationObj,
        AniGlobalMethod::GetInstance().fontVariationValue, variation.value);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseFontVariationToNative: Failed to read value, ret=%{public}d", ret);
        return ret;
    }

    return ANI_OK;
}

static ani_status ParseVariationAxisNumericFields(ani_env* env, ani_object axisObj,
    TextEngine::FontParser::FontVariationAxis& variationAxis)
{
    ani_status ret = AniTextUtils::ReadOptionalDoubleField(env, axisObj,
        AniGlobalMethod::GetInstance().fontVariationAxisMinValue, variationAxis.minValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseVariationAxisNumericFields: Failed to read minValue, ret=%{public}d", ret);
        return ret;
    }

    ret = AniTextUtils::ReadOptionalDoubleField(env, axisObj,
        AniGlobalMethod::GetInstance().fontVariationAxisMaxValue, variationAxis.maxValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseVariationAxisNumericFields: Failed to read maxValue, ret=%{public}d", ret);
        return ret;
    }

    ret = AniTextUtils::ReadOptionalDoubleField(env, axisObj,
        AniGlobalMethod::GetInstance().fontVariationAxisDefaultValue, variationAxis.defaultValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseVariationAxisNumericFields: Failed to read defaultValue, ret=%{public}d", ret);
        return ret;
    }

    ret = AniTextUtils::ReadOptionalIntField(env, axisObj,
        AniGlobalMethod::GetInstance().fontVariationAxisFlags, variationAxis.flags);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseVariationAxisNumericFields: Failed to read flags, ret=%{public}d", ret);
        return ret;
    }

    return ANI_OK;
}

ani_status ParseFontVariationAxisToNative(ani_env* env, ani_ref ref,
    TextEngine::FontParser::FontVariationAxis& variationAxis)
{
    if (ref == nullptr) {
        TEXT_LOGE("ParseFontVariationAxisToNative: ref is nullptr");
        return ANI_ERROR;
    }
    ani_object axisObj = reinterpret_cast<ani_object>(ref);

    // Parse required string fields
    ani_status ret = AniTextUtils::ReadOptionalStringField(env, axisObj,
        AniGlobalMethod::GetInstance().fontVariationAxisKey, variationAxis.key);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseFontVariationAxisToNative: Failed to read key, ret=%{public}d", ret);
        return ret;
    }

    ret = AniTextUtils::ReadOptionalStringField(env, axisObj,
        AniGlobalMethod::GetInstance().fontVariationAxisName, variationAxis.name);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseFontVariationAxisToNative: Failed to read name, ret=%{public}d", ret);
        return ret;
    }

    ret = AniTextUtils::ReadOptionalStringField(env, axisObj,
        AniGlobalMethod::GetInstance().fontVariationAxisLocalName, variationAxis.localName);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseFontVariationAxisToNative: Failed to read localName, ret=%{public}d", ret);
        return ret;
    }

    return ParseVariationAxisNumericFields(env, axisObj, variationAxis);
}

ani_status ParseFontVariationInstanceToNative(ani_env* env, ani_ref ref,
    TextEngine::FontParser::FontVariationInstance& variationInstance)
{
    if (ref == nullptr) {
        TEXT_LOGE("ParseFontVariationInstanceToNative: ref is nullptr");
        return ANI_ERROR;
    }
    ani_object instanceObj = reinterpret_cast<ani_object>(ref);

    ani_status ret = AniTextUtils::ReadOptionalStringField(env, instanceObj,
        AniGlobalMethod::GetInstance().fontVariationInstanceName, variationInstance.name);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseFontVariationInstanceToNative: Failed to read name, ret=%{public}d", ret);
        return ret;
    }

    ret = AniTextUtils::ReadOptionalStringField(env, instanceObj,
        AniGlobalMethod::GetInstance().fontVariationInstanceLocalName, variationInstance.localName);
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseFontVariationInstanceToNative: Failed to read localName, ret=%{public}d", ret);
        return ret;
    }

    ret = AniTextUtils::ReadArrayField<TextEngine::FontParser::FontVariation>(env, instanceObj,
        AniGlobalMethod::GetInstance().fontVariationInstanceCoordinates, variationInstance.coordinates,
        [](ani_env* env, ani_ref ref) -> TextEngine::FontParser::FontVariation {
            TextEngine::FontParser::FontVariation variation;
            ani_status ret = ParseFontVariationToNative(env, ref, variation);
            if (ret != ANI_OK) {
                TEXT_LOGE("ParseFontVariationInstanceToNative: Failed to parse variation element");
            }
            return variation;
        });
    if (ret != ANI_OK) {
        TEXT_LOGE("ParseFontVariationInstanceToNative: Failed to read coordinates, ret=%{public}d", ret);
        return ret;
    }

    return ANI_OK;
}

ani_object CreateFontVariationAxisArray(ani_env* env,
    const std::vector<TextEngine::FontParser::FontVariationAxis>& variationAxes)
{
    ani_object arrayObj = AniTextUtils::CreateAniArray(env, variationAxes.size());
    ani_boolean isUndefined = false;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create FontVariationAxis array, returning empty array");
        return AniTextUtils::CreateAniArray(env, 0);
    }

    ani_size index = 0;
    for (const auto& axis : variationAxes) {
        ani_object axisObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().fontVariationAxis,
            AniGlobalMethod::GetInstance().fontVariationAxisCtor,
            AniTextUtils::CreateAniStringObj(env, axis.key),
            ani_double(axis.minValue),
            ani_double(axis.maxValue),
            ani_double(axis.defaultValue),
            ani_int(axis.flags),
            AniTextUtils::CreateAniStringObj(env, axis.name),
            AniTextUtils::CreateAniStringObj(env, axis.localName));

        if (axisObj == nullptr) {
            TEXT_LOGE("Failed to create FontVariationAxis object for key=%{public}s", axis.key.c_str());
            continue;
        }

        ani_status status = env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet,
            index, axisObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to set FontVariationAxis item, index %{public}zu status=%{public}d", index, status);
            continue;
        }
        index++;
    }
    TEXT_LOGI("CreateFontVariationAxisArray: created %{public}zu axes successfully", index);
    return arrayObj;
}

ani_object CreateFontVariationInstanceArray(ani_env* env,
    const std::vector<TextEngine::FontParser::FontVariationInstance>& variationInstances)
{
    ani_object arrayObj = AniTextUtils::CreateAniArray(env, variationInstances.size());
    ani_boolean isUndefined = false;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create FontVariationInstance array, returning empty array");
        return AniTextUtils::CreateAniArray(env, 0);
    }

    ani_size index = 0;
    for (const auto& instance : variationInstances) {
        ani_object coordinatesArray = AniTextUtils::CreateAniArray(env, instance.coordinates.size());
        ani_boolean coordUndefined = false;
        env->Reference_IsUndefined(coordinatesArray, &coordUndefined);
        if (!coordUndefined) {
            ani_size coordIndex = 0;
            for (const auto& coord : instance.coordinates) {
                ani_object coordObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().fontVariation,
                    AniGlobalMethod::GetInstance().fontVariationCtor,
                    AniTextUtils::CreateAniStringObj(env, coord.axis),
                    ani_double(coord.value));

                if (coordObj != nullptr) {
                    ani_status status = env->Object_CallMethod_Void(coordinatesArray,
                        AniGlobalMethod::GetInstance().arraySet, coordIndex, coordObj);
                    if (status != ANI_OK) {
                        TEXT_LOGE("Failed to set coordinate, index %{public}zu status=%{public}d", coordIndex, status);
                    } else {
                        coordIndex++;
                    }
                }
            }
        }
        ani_object instanceObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().fontVariationInstance,
            AniGlobalMethod::GetInstance().fontVariationInstanceCtor,
            AniTextUtils::CreateAniStringObj(env, instance.name),
            AniTextUtils::CreateAniStringObj(env, instance.localName),
            coordinatesArray);

        if (instanceObj == nullptr) {
            TEXT_LOGE("Failed to create FontVariationInstance object for name=%{public}s", instance.name.c_str());
            continue;
        }

        ani_status status = env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, 
            index, instanceObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to set FontVariationInstance item, index %{public}zu status=%{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_status FontDescriptorGetFontWeight(ani_env* env, ani_object obj, FontDescSharedPtr& fontDesc)
{
    ani_ref ref = nullptr;
    ani_status result = AniTextUtils::ReadOptionalField(
        env, obj, AniGlobalMethod::GetInstance().fontDescriptorGetWeight, ref);
    if (result == ANI_OK && ref != nullptr) {
        ani_size index = 0;
        result = env->EnumItem_GetIndex(reinterpret_cast<ani_enum_item>(ref), &index);
        if (result != ANI_OK || index >= AniTextEnum::fontWeight.size()) {
            TEXT_LOGE("Index is invalid %{public}zu, ret %{public}d", index, result);
            return ANI_ERROR;
        }
        int weightValue = 0;
        if (OHOS::MLB::FindFontWeightEnum(static_cast<int>(AniTextEnum::fontWeight[index]), weightValue)) {
            fontDesc.get()->weight = weightValue;
            return ANI_OK;
        } else {
            TEXT_LOGE("Failed to parse font weight, ret %{public}d", result);
            return ANI_ERROR;
        }
    }
    return result;
}

static ani_status ParseFontDescriptorBasicFields(ani_env* env, ani_object& aniObj,
    TextEngine::FontParser::FontDescriptor* fontDesc, ani_status& status)
{
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetPath, path,
        String, fontDesc, status);
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetPostScriptName, postScriptName,
        String, fontDesc, status);
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetFullName, fullName, String,
        fontDesc, status);
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetFontFamily, fontFamily, String,
        fontDesc, status);
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetFontSubfamily, fontSubfamily,
        String, fontDesc, status);
    READ_OPTIONAL_FIELD(
        env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetWidth, width, Int, fontDesc, status);
    READ_OPTIONAL_FIELD(
        env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetItalic, italic, Int, fontDesc, status);
    READ_OPTIONAL_FIELD(env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetMonoSpace, monoSpace, Bool,
        fontDesc, status);
    READ_OPTIONAL_FIELD(
        env, aniObj, AniGlobalMethod::GetInstance().fontDescriptorGetSymbolic, symbolic, Bool, fontDesc, status);
    return status;
}

static ani_status ParseVariationAxisRecords(ani_env* env, ani_object& aniObj,
    std::vector<TextEngine::FontParser::FontVariationAxis>& variationAxisRecords)
{
    ani_ref variationAxesRef = nullptr;
    ani_status status = AniTextUtils::ReadOptionalField(env, aniObj,
        AniGlobalMethod::GetInstance().fontDescriptorGetVariationAxisRecords, variationAxesRef);
    if (status != ANI_OK || variationAxesRef == nullptr) {
        return status;
    }

    ani_array variationAxesArray = reinterpret_cast<ani_array>(variationAxesRef);
    ani_size length = 0;
    status = env->Array_GetLength(variationAxesArray, &length);
    if (status != ANI_OK) {
        return status;
    }

    for (ani_size i = 0; i < length; i++) {
        ani_ref elementRef = nullptr;
        status = env->Array_Get(variationAxesArray, i, &elementRef);
        if (status == ANI_OK && elementRef != nullptr) {
            TextEngine::FontParser::FontVariationAxis axis;
            status = ParseFontVariationAxisToNative(env, elementRef, axis);
            if (status == ANI_OK) {
                variationAxisRecords.push_back(axis);
            }
        }
    }
    return ANI_OK;
}

static ani_status ParseVariationInstanceRecords(ani_env* env, ani_object& aniObj,
    std::vector<TextEngine::FontParser::FontVariationInstance>& variationInstanceRecords)
{
    ani_ref variationInstancesRef = nullptr;
    ani_status status = AniTextUtils::ReadOptionalField(env, aniObj,
        AniGlobalMethod::GetInstance().fontDescriptorGetVariationInstanceRecords, variationInstancesRef);
    if (status != ANI_OK || variationInstancesRef == nullptr) {
        return status;
    }

    ani_array variationInstancesArray = reinterpret_cast<ani_array>(variationInstancesRef);
    ani_size length = 0;
    status = env->Array_GetLength(variationInstancesArray, &length);
    if (status != ANI_OK) {
        return status;
    }

    for (ani_size i = 0; i < length; i++) {
        ani_ref elementRef = nullptr;
        status = env->Array_Get(variationInstancesArray, i, &elementRef);
        if (status == ANI_OK && elementRef != nullptr) {
            TextEngine::FontParser::FontVariationInstance instance;
            status = ParseFontVariationInstanceToNative(env, elementRef, instance);
            if (status == ANI_OK) {
                variationInstanceRecords.push_back(instance);
            }
        }
    }
    return ANI_OK;
}

ani_status ParseFontDescriptorToNative(ani_env* env, ani_object& aniObj, FontDescSharedPtr& fontDesc)
{
    fontDesc = std::make_shared<TextEngine::FontParser::FontDescriptor>();

    ani_status status = ANI_OK;
    ParseFontDescriptorBasicFields(env, aniObj, fontDesc.get(), status);
    if (status != ANI_OK) {
        return status;
    }

    status = FontDescriptorGetFontWeight(env, aniObj, fontDesc);
    if (status != ANI_OK) {
        return status;
    }

    status = ParseVariationAxisRecords(env, aniObj, fontDesc->variationAxisRecords);
    if (status != ANI_OK) {
        return status;
    }

    status = ParseVariationInstanceRecords(env, aniObj, fontDesc->variationInstanceRecords);
    if (status != ANI_OK) {
        return status;
    }

    return ANI_OK;
}

ani_status ParseFontDescriptorToAni(ani_env* env, const FontDescSharedPtr fontDesc, ani_object& aniObj)
{
    if (fontDesc == nullptr) {
        TEXT_LOGE("ParseFontDescriptorToAni: Empty data fontDesc");
        return ANI_ERROR;
    }
    int result = -1;
    if (!OHOS::MLB::FindFontWeight(OHOS::MLB::RegularWeight(fontDesc->weight), result)) {
        return ANI_ERROR;
    }
    ani_ref variationAxesRef;
    ani_ref variationInstancesRef;
    if (!fontDesc->variationAxisRecords.empty()) {
        variationAxesRef = CreateFontVariationAxisArray(env, fontDesc->variationAxisRecords);
    } else {
        variationAxesRef = AniTextUtils::CreateAniUndefined(env);
    }

    if (!fontDesc->variationInstanceRecords.empty()) {
        variationInstancesRef = CreateFontVariationInstanceArray(env, fontDesc->variationInstanceRecords);
    } else {
        variationInstancesRef = AniTextUtils::CreateAniUndefined(env);
    }

    aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().fontDescriptor,
        AniGlobalMethod::GetInstance().fontDescriptorCtor, AniTextUtils::CreateAniStringObj(env, fontDesc->path),
        AniTextUtils::CreateAniStringObj(env, fontDesc->postScriptName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->fullName),
        AniTextUtils::CreateAniStringObj(env, fontDesc->fontFamily),
        AniTextUtils::CreateAniStringObj(env, fontDesc->fontSubfamily),
        AniTextUtils::CreateAniEnum(env, AniGlobalEnum::GetInstance().fontWeight,
            aniGetEnumIndex(AniTextEnum::fontWeight, static_cast<uint32_t>(result))
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
        ani_int(fontDesc->index),
        variationAxesRef,
        variationInstancesRef
    );
    return ANI_OK;
}

ani_object CreateFontDescriptorArray(ani_env* env, const std::vector<FontDescSharedPtr>& fontDescripters)
{
    TEXT_LOGI("CreateFontDescriptorArray: called with %{public}zu descriptors", fontDescripters.size());

    std::set<FontDescSharedPtr> fontDescripterList(fontDescripters.begin(), fontDescripters.end());

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, fontDescripterList.size());
    ani_boolean isUndefined = false;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("CreateFontDescriptorArray: Failed to create arrayObject");
        return AniTextUtils::CreateAniArray(env, 0);
    }

    ani_size index = 0;
    for (const auto& item : fontDescripterList) {
        TEXT_LOGI("CreateFontDescriptorArray: processing descriptor[%{public}zu] path=%{public}s", index, item->path.c_str());

        ani_object aniObj = nullptr;
        ani_status status = ParseFontDescriptorToAni(env, item, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("CreateFontDescriptorArray: Failed to parse FontDescriptor to ani, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        status = env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, index, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("CreateFontDescriptorArray: Failed to set FontDescriptor item, index %{public}zu, status %{public}d", index, status);
            continue;
        }
        index++;
    }

    TEXT_LOGI("CreateFontDescriptorArray: completed, created %{public}zu descriptor objects", index);
    return arrayObj;
}

ani_object ProcessStringPath(ani_env* env, ani_object path)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;

    std::string pathStr;
    ani_status ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(path), pathStr);
    if (ret != ANI_OK) {
        TEXT_LOGE("ProcessStringPath: Failed to convert path to string, ret=%{public}d", ret);
        return AniTextUtils::CreateAniArray(env, 0);
    }

    TEXT_LOGI("ProcessStringPath: parsing font from path=%{public}s", pathStr.c_str());

    if (!AniTextUtils::SplitAbsoluteFontPath(pathStr) || !AniTextUtils::ReadFile(pathStr, dataLen, data).success) {
        TEXT_LOGE("ProcessStringPath: Failed to split absolute font path or read file");
        return AniTextUtils::CreateAniArray(env, 0);
    }

    auto fontDescripters = TextEngine::FontParser::ParserFontDescriptorsFromPath(pathStr);
    TEXT_LOGI("ProcessStringPath: parsed %{public}zu font descriptors from path=%{public}s", fontDescripters.size(), pathStr.c_str());

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

ani_object AniFontDescriptor::GetFontPathsByType(ani_env* env, ani_enum_item fontType)
{
    ani_int typeIndex;
    ani_status ret = env->EnumItem_GetValue_Int(fontType, &typeIndex);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse fontType, ret %{public}d", ret);
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Parameter fontType is invalid");
        return AniTextUtils::CreateAniUndefined(env);
    }

    TextEngine::FontParser::SystemFontType systemFontType =
        static_cast<TextEngine::FontParser::SystemFontType>(typeIndex);

    std::unordered_set<std::string> fontPaths;
    FontDescriptorMgrInstance.GetFontPathsByType(systemFontType, fontPaths);

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, fontPaths.size());
    ani_boolean isUndefined = false;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    ani_size index = 0;
    for (const auto& item : fontPaths) {
        ani_string aniStr = AniTextUtils::CreateAniStringObj(env, item);
        if (ANI_OK != env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, index, aniStr)) {
            TEXT_LOGE("Failed to set fontList item %{public}zu", index);
            continue;
        }
        index += 1;
    }
    return arrayObj;
}

ani_boolean AniFontDescriptor::IsFontSupported(ani_env* env, ani_object path)
{
    ani_boolean isString = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().aniString, &isString);

    if (isString) {
        std::string pathStr;
        ani_status ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(path), pathStr);
        if (ret != ANI_OK) {
            return false;
        }
        if (!AniTextUtils::SplitAbsoluteFontPath(pathStr)) {
            TEXT_LOGE("Failed to split absolute font path");
            return false;
        }
        return Drawing::Typeface::MakeFromFile(pathStr.c_str()) != nullptr;
    }

    ani_boolean isResource = false;
    env->Object_InstanceOf(path, AniGlobalClass::GetInstance().globalResource, &isResource);
    if (isResource) {
        std::unique_ptr<uint8_t[]> data;
        size_t size = 0;
        AniResourceParser::ResolveResource(AniResourceParser::ParseResource(env, path), size, data);
        auto stream = std::make_unique<Drawing::MemoryStream>(data.get(), size);
        return Drawing::Typeface::MakeFromStream(std::move(stream)) != nullptr;
    }
    return false;
}
} // namespace OHOS::Text::ANI