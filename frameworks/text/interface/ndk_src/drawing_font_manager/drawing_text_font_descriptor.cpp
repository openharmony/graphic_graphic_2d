/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawing_text_font_descriptor.h"

#include <string_ex.h>

#include "array_mgr.h"
#include "font_descriptor_mgr.h"
#include "font_utils.h"
#include "text/common_utils.h"
#include "utils/text_log.h"

using namespace OHOS::Rosen;
template<typename T1, typename T2>
inline T1* ConvertToOriginalText(T2* ptr)
{
    return reinterpret_cast<T1*>(ptr);
}

[[maybe_unused]] static bool ConvertToString(const uint8_t* data, size_t len, std::string& fullNameString)
{
    if (data == nullptr || len == 0) {
        return false;
    }

    size_t utf16Len = len / sizeof(char16_t);
    std::unique_ptr<char16_t[]> utf16Str = std::make_unique<char16_t[]>(utf16Len);

    errno_t ret = memcpy_s(utf16Str.get(), utf16Len * sizeof(char16_t), data, len);
    if (ret != EOK) {
        return false;
    }

    std::u16string utf16String(utf16Str.get(), utf16Len);
    fullNameString = OHOS::Str16ToStr8(utf16String);
    return !fullNameString.empty();
}

OH_Drawing_FontDescriptor* OH_Drawing_MatchFontDescriptors(OH_Drawing_FontDescriptor* desc, size_t* num)
{
    if (desc == nullptr || num == nullptr) {
        return nullptr;
    }
    using namespace OHOS::Rosen::TextEngine;
    std::shared_ptr<FontParser::FontDescriptor> condition = std::make_shared<FontParser::FontDescriptor>();
    condition->postScriptName = (desc->postScriptName == nullptr) ? "" : desc->postScriptName;
    condition->fullName = (desc->fullName == nullptr) ? "" : desc->fullName;
    condition->fontFamily = (desc->fontFamily == nullptr) ? "" : desc->fontFamily;
    condition->fontSubfamily = (desc->fontSubfamily == nullptr) ? "" : desc->fontSubfamily;
    condition->weight = desc->weight;
    condition->width = desc->width;
    condition->italic = desc->italic;
    condition->monoSpace = desc->monoSpace;
    condition->symbolic = desc->symbolic;

    std::set<std::shared_ptr<FontParser::FontDescriptor>> result;
    FontDescriptorMgrInstance.MatchFontDescriptors(condition, result);
    if (result.empty()) {
        *num = 0;
        return nullptr;
    }

    *num = result.size();
    OH_Drawing_FontDescriptor* descriptors = new (std::nothrow) OH_Drawing_FontDescriptor[*num];
    if (descriptors == nullptr) {
        *num = 0;
        return nullptr;
    }
    
    size_t i = 0;
    for (const auto& item : result) {
        if (!OHOS::Rosen::Drawing::CopyFontDescriptor(&descriptors[i], *item)) {
            break;
        }
        ++i;
    }
    if (i != result.size()) {
        OH_Drawing_DestroyFontDescriptors(descriptors, i);
        *num = 0;
        return nullptr;
    }
    return descriptors;
}

void OH_Drawing_DestroyFontDescriptors(OH_Drawing_FontDescriptor* descriptors, size_t num)
{
    if (descriptors == nullptr || num == 0) {
        return;
    }
    for (size_t i = 0; i < num; ++i) {
        free(descriptors[i].path);
        free(descriptors[i].postScriptName);
        free(descriptors[i].fullName);
        free(descriptors[i].fontFamily);
        free(descriptors[i].fontSubfamily);
    }
    delete[] descriptors;
}

OH_Drawing_FontDescriptor* OH_Drawing_GetFontDescriptorByFullName(const OH_Drawing_String* fullName,
    OH_Drawing_SystemFontType fontType)
{
    if (fullName == nullptr) {
        return nullptr;
    }
    auto systemFontType = static_cast<int32_t>(fontType);
    using namespace OHOS::Rosen::Drawing;
    std::string fullNameString;
    if (!ConvertToString(fullName->strData, fullName->strLen, fullNameString)) {
        TEXT_LOGE("Failed to convert string to utf8");
        return nullptr;
    }
    std::shared_ptr<TextEngine::FontParser::FontDescriptor> result = nullptr;
    FontDescriptorMgrInstance.GetFontDescSharedPtrByFullName(fullNameString, systemFontType, result);
    if (result == nullptr) {
        return nullptr;
    }
    OH_Drawing_FontDescriptor* descriptor = OH_Drawing_CreateFontDescriptor();
    if (descriptor == nullptr) {
        return nullptr;
    }
    if (!OHOS::Rosen::Drawing::CopyFontDescriptor(descriptor, *result)) {
        OH_Drawing_DestroyFontDescriptor(descriptor);
        return nullptr;
    }
    return descriptor;
}

OH_Drawing_Array* OH_Drawing_GetSystemFontFullNamesByType(OH_Drawing_SystemFontType fontType)
{
    auto systemFontType = static_cast<int32_t>(fontType);
    std::unordered_set<std::string> fullNameList;
    FontDescriptorMgrInstance.GetSystemFontFullNamesByType(systemFontType, fullNameList);
    std::unique_ptr array = std::make_unique<ObjectArray>();
    std::unique_ptr addr = std::make_unique<OH_Drawing_String[]>(fullNameList.size());
    array->type = ObjectType::STRING;
    size_t num = 0;
    for (const auto& fullName : fullNameList) {
        std::u16string utf16String = OHOS::Str8ToStr16(fullName);
        if (utf16String.empty()) {
            TEXT_LOGE("Failed to convert string to utf16: %{public}s", fullName.c_str());
            continue;
        }
        addr[num].strLen = utf16String.size() * sizeof(char16_t);
        std::unique_ptr strData = std::make_unique<uint8_t[]>(addr[num].strLen);
        if (memcpy_s(strData.get(), addr[num].strLen, utf16String.c_str(), addr[num].strLen) != EOK) {
            TEXT_LOGE("Failed to memcpy_s length: %{public}u", addr[num].strLen);
            continue;
        }
        addr[num].strData = strData.release();
        num += 1;
    }
    if (num == 0) {
        TEXT_LOGI_LIMIT3_MIN("Failed to get font full name, font type: %{public}d", static_cast<int32_t>(fontType));
        return nullptr;
    }
    array->num = num;
    array->addr = addr.release();
    return reinterpret_cast<OH_Drawing_Array*>(array.release());
}

const OH_Drawing_String* OH_Drawing_GetSystemFontFullNameByIndex(OH_Drawing_Array* fullNameArray, size_t index)
{
    ObjectArray* fullNameList = ConvertToOriginalText<ObjectArray>(fullNameArray);
    if (fullNameList != nullptr && fullNameList->type == ObjectType::STRING &&
        index < fullNameList->num && fullNameList->addr != nullptr) {
        OH_Drawing_String* drawingString = ConvertToOriginalText<OH_Drawing_String>(fullNameList->addr);
        if (drawingString != nullptr) {
            return &drawingString[index];
        }
    }
    return nullptr;
}

void OH_Drawing_DestroySystemFontFullNames(OH_Drawing_Array* fullNameArray)
{
    ObjectArray* fullNameList = ConvertToOriginalText<ObjectArray>(fullNameArray);
    if (fullNameList == nullptr || fullNameList->type != ObjectType::STRING) {
        return;
    }
    OH_Drawing_String* drawingStringArray = ConvertToOriginalText<OH_Drawing_String>(fullNameList->addr);
    if (drawingStringArray == nullptr) {
        return;
    }
    for (size_t i = 0; i < fullNameList->num; ++i) {
        if (drawingStringArray[i].strData == nullptr) {
            continue;
        }
        delete[] drawingStringArray[i].strData;
        drawingStringArray[i].strData = nullptr;
    }
    delete[] drawingStringArray;
    fullNameList->addr = nullptr;
    fullNameList->num = 0;
    fullNameList->type = ObjectType::INVALID;
    delete fullNameList;
}

const OH_Drawing_FontFullDescriptor* OH_Drawing_GetFontFullDescriptorByIndex(OH_Drawing_Array* descriptorArray,
    size_t index)
{
    if (descriptorArray == nullptr) {
        return nullptr;
    }

    auto arrayFontFullDescriptors = reinterpret_cast<ObjectArray*>(descriptorArray);
    if (arrayFontFullDescriptors != nullptr && arrayFontFullDescriptors->addr != nullptr &&
        arrayFontFullDescriptors->type == FONT_FULL_DESCRIPTOR && index < arrayFontFullDescriptors->num) {
        Drawing::FontParser::FontDescriptor** fontFullDescriptor =
            reinterpret_cast<Drawing::FontParser::FontDescriptor**>(arrayFontFullDescriptors->addr);
        return reinterpret_cast<OH_Drawing_FontFullDescriptor*>(fontFullDescriptor[index]);
    }

    return nullptr;
}

OH_Drawing_Array* OH_Drawing_GetFontFullDescriptorsFromPath(const char* path)
{
    if (path == nullptr) {
        return nullptr;
    }
    std::vector<std::shared_ptr<Drawing::FontParser::FontDescriptor>> fontFullDescriptors =
        TextEngine::FontParser::ParserFontDescriptorsFromPath(path);
    if (fontFullDescriptors.empty()) {
        return nullptr;
    }
    std::unique_ptr array = std::make_unique<ObjectArray>();
    std::unique_ptr addr = std::make_unique<Drawing::FontParser::FontDescriptor* []>(fontFullDescriptors.size());
    array->type = ObjectType::FONT_FULL_DESCRIPTOR;

    size_t num = 0;
    for (const auto& fontFullDescriptor : fontFullDescriptors) {
        addr[num] = new Drawing::FontParser::FontDescriptor(*fontFullDescriptor);
        num += 1;
    }
    array->addr = addr.release();
    array->num = num;
    return reinterpret_cast<OH_Drawing_Array*>(array.release());
}

OH_Drawing_Array* OH_Drawing_GetFontFullDescriptorsFromStream(const void* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return nullptr;
    }
    std::vector<std::shared_ptr<Drawing::FontParser::FontDescriptor>> fontFullDescriptors =
        TextEngine::FontParser::ParserFontDescriptorsFromStream(data, size);
    if (fontFullDescriptors.empty()) {
        return nullptr;
    }
    std::unique_ptr array = std::make_unique<ObjectArray>();
    std::unique_ptr addr = std::make_unique<Drawing::FontParser::FontDescriptor* []>(fontFullDescriptors.size());
    array->type = ObjectType::FONT_FULL_DESCRIPTOR;

    size_t num = 0;
    for (const auto& fontFullDescriptor : fontFullDescriptors) {
        addr[num] = new Drawing::FontParser::FontDescriptor(*fontFullDescriptor);
        num += 1;
    }
    array->addr = addr.release();
    array->num = num;
    return reinterpret_cast<OH_Drawing_Array*>(array.release());
}

void OH_Drawing_DestroyFontFullDescriptors(OH_Drawing_Array* descriptorArray)
{
    if (descriptorArray == nullptr) {
        return;
    }

    ObjectArray* descriptorList = ConvertToOriginalText<ObjectArray>(descriptorArray);
    if (descriptorList == nullptr) {
        return;
    }

    if (descriptorList->type != ObjectType::FONT_FULL_DESCRIPTOR) {
        delete descriptorList;
        return;
    }

    Drawing::FontParser::FontDescriptor** descriptors =
        reinterpret_cast<Drawing::FontParser::FontDescriptor**>(descriptorList->addr);
    if (descriptors == nullptr) {
        descriptorList->addr = nullptr;
        descriptorList->num = 0;
        descriptorList->type = ObjectType::INVALID;
        delete descriptorList;
        return;
    }

    for (size_t i = 0; i < descriptorList->num; ++i) {
        if (descriptors[i] == nullptr) {
            continue;
        }
        delete descriptors[i];
        descriptors[i] = nullptr;
    }
    delete[] descriptors;
    descriptorList->addr = nullptr;
    descriptorList->num = 0;
    descriptorList->type = ObjectType::INVALID;
    delete descriptorList;
}

OH_Drawing_ErrorCode GetUnicodeArray(const std::vector<uint32_t>& fontUnicodeArray,
    int32_t** unicodeArray, int32_t* arrayLength)
{
    if (fontUnicodeArray.empty() || unicodeArray == nullptr || arrayLength == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }

    int32_t* result = static_cast<int32_t*>(malloc(fontUnicodeArray.size() * sizeof(int32_t)));
    if (result == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    for (size_t i = 0; i < fontUnicodeArray.size(); ++i) {
        result[i] = static_cast<int32_t>(fontUnicodeArray[i]);
    }

    *unicodeArray = result;
    *arrayLength = static_cast<int32_t>(fontUnicodeArray.size());

    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_GetFontUnicodeArrayFromFile(const char* fontSrc, uint32_t index,
    int32_t** unicodeArray, int32_t* arrayLength)
{
    if (fontSrc == nullptr || unicodeArray == nullptr || arrayLength == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto result = TextEngine::FontParser::GetFontTypefaceUnicode(fontSrc, index);
    return GetUnicodeArray(result, unicodeArray, arrayLength);
}

OH_Drawing_ErrorCode OH_Drawing_GetFontUnicodeArrayFromBuffer(uint8_t* fontBuffer, size_t length, uint32_t index,
    int32_t** unicodeArray, int32_t* arrayLength)
{
    if (fontBuffer == nullptr || unicodeArray == nullptr || arrayLength == nullptr || length == 0) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto result = TextEngine::FontParser::GetFontTypefaceUnicode(fontBuffer, length, index);
    return GetUnicodeArray(result, unicodeArray, arrayLength);
}

uint32_t OH_Drawing_GetFontCountFromFile(const char* fontSrc)
{
    uint32_t fileCount = 0;
    if (fontSrc != nullptr) {
        fileCount = static_cast<uint32_t>(TextEngine::FontParser::GetFontCount(fontSrc));
    }
    return fileCount;
}

uint32_t OH_Drawing_GetFontCountFromBuffer(uint8_t* fontBuffer, size_t length)
{
    uint32_t fileCount = 0;
    if (fontBuffer != nullptr && length != 0) {
        std::vector<uint8_t> fontData(fontBuffer, fontBuffer + length);
        fileCount = static_cast<uint32_t>(TextEngine::FontParser::GetFontCount(fontData));
    }
    return fileCount;
}

OH_Drawing_String* OH_Drawing_GetFontPathsByType(OH_Drawing_SystemFontType fontType, size_t* num)
{
    auto systemFontType = static_cast<int32_t>(fontType);
    std::unordered_set<std::string> fontPaths;
    size_t index = 0;
    if (num == nullptr) {
        num = &index;
    }
    FontDescriptorMgrInstance.GetFontPathsByType(systemFontType, fontPaths);
    if (fontPaths.empty()) {
        TEXT_LOGI_LIMIT3_MIN("Failed to get font path by type: %{public}d", systemFontType);
        *num = 0;
        return nullptr;
    }
    // +1 for nullptr termination
    std::unique_ptr<OH_Drawing_String[]> stringArr = std::make_unique<OH_Drawing_String[]>(fontPaths.size() + 1);
    size_t byteLength = sizeof(OH_Drawing_String) * (fontPaths.size() + 1);
    if (memset_s(stringArr.get(), byteLength, 0, byteLength) != EOK) {
        TEXT_LOGE("Failed to memset_s length: %{public}zu", byteLength);
        *num = 0;
        return nullptr;
    }
    for (const auto& path : fontPaths) {
        std::u16string utf16String = OHOS::Str8ToStr16(path);
        if (utf16String.empty()) {
            TEXT_LOGE("Failed to convert string to utf16: %{public}s", path.c_str());
            continue;
        }
        stringArr[index].strLen = utf16String.size() * sizeof(char16_t);
        std::unique_ptr strData = std::make_unique<uint8_t[]>(stringArr[index].strLen);
        if (memcpy_s(strData.get(), stringArr[index].strLen, utf16String.c_str(), stringArr[index].strLen) != EOK) {
            TEXT_LOGE("Failed to memcpy_s length: %{public}u", stringArr[index].strLen);
            continue;
        }
        stringArr[index].strData = strData.release();
        index += 1;
    }
    *num = index;
    if (index == 0) {
        TEXT_LOGI_LIMIT3_MIN("Failed to get font path, font type: %{public}d", static_cast<int32_t>(fontType));
        return nullptr;
    }
    return stringArr.release();
}

static OH_Drawing_Array* CreateVariationAxisArray(
    const std::vector<TextEngine::FontParser::FontVariationAxis>& variationAxes)
{
    if (variationAxes.empty()) {
        return nullptr;
    }

    std::unique_ptr array = std::make_unique<ObjectArray>();
    std::unique_ptr addr = std::make_unique<Drawing::FontParser::FontVariationAxis*[]>(variationAxes.size());
    array->type = ObjectType::FONT_VARIATION_AXIS;

    size_t num = 0;
    for (const auto& axis : variationAxes) {
        addr[num] = new Drawing::FontParser::FontVariationAxis(axis);
        num++;
    }
    array->addr = addr.release();
    array->num = num;
    return reinterpret_cast<OH_Drawing_Array*>(array.release());
}

static OH_Drawing_Array* CreateVariationInstanceArray(
    const std::vector<TextEngine::FontParser::FontVariationInstance>& variationInstances)
{
    if (variationInstances.empty()) {
        return nullptr;
    }

    std::unique_ptr array = std::make_unique<ObjectArray>();
    std::unique_ptr addr = std::make_unique<Drawing::FontParser::FontVariationInstance* []>(
        variationInstances.size());
    array->type = ObjectType::FONT_VARIATION_INSTANCE;

    size_t num = 0;
    for (const auto& instance : variationInstances) {
        addr[num] = new Drawing::FontParser::FontVariationInstance(instance);
        num++;
    }
    array->addr = addr.release();
    array->num = num;
    return reinterpret_cast<OH_Drawing_Array*>(array.release());
}

OH_Drawing_Array* OH_Drawing_GetFontFullDescriptorAttributeArray(const OH_Drawing_FontFullDescriptor* descriptor,
    OH_Drawing_FontFullDescriptorAttributeId id)
{
    if (descriptor == nullptr) {
        return nullptr;
    }

    const auto& fontDescriptor = *reinterpret_cast<const TextEngine::FontParser::FontDescriptor*>(descriptor);

    if (id == FULL_DESCRIPTOR_ATTR_O_VARIATION_AXIS) {
        return CreateVariationAxisArray(fontDescriptor.variationAxisRecords);
    }
    if (id == FULL_DESCRIPTOR_ATTR_O_VARIATION_INSTANCE) {
        return CreateVariationInstanceArray(fontDescriptor.variationInstanceRecords);
    }
    return nullptr;
}

void OH_Drawing_DestroyFontVariationAxis(OH_Drawing_Array* array)
{
    if (array == nullptr) {
        return;
    }

    ObjectArray* axisArray = reinterpret_cast<ObjectArray*>(array);
    if (axisArray == nullptr) {
        return;
    }

    if (axisArray->type != ObjectType::FONT_VARIATION_AXIS) {
        delete axisArray;
        return;
    }

    Drawing::FontParser::FontVariationAxis** axes =
        reinterpret_cast<Drawing::FontParser::FontVariationAxis**>(axisArray->addr);
    if (axes == nullptr) {
        axisArray->addr = nullptr;
        axisArray->num = 0;
        axisArray->type = ObjectType::INVALID;
        delete axisArray;
        return;
    }

    for (size_t i = 0; i < axisArray->num; ++i) {
        if (axes[i] == nullptr) {
            continue;
        }
        delete axes[i];
        axes[i] = nullptr;
    }
    delete[] axes;
    axisArray->addr = nullptr;
    axisArray->num = 0;
    axisArray->type = ObjectType::INVALID;
    delete axisArray;
}

void OH_Drawing_DestroyFontVariationInstance(OH_Drawing_Array* array)
{
    if (array == nullptr) {
        return;
    }

    ObjectArray* instanceArray = reinterpret_cast<ObjectArray*>(array);
    if (instanceArray == nullptr) {
        return;
    }

    if (instanceArray->type != ObjectType::FONT_VARIATION_INSTANCE) {
        delete instanceArray;
        return;
    }

    Drawing::FontParser::FontVariationInstance** instances =
        reinterpret_cast<Drawing::FontParser::FontVariationInstance**>(instanceArray->addr);
    if (instances == nullptr) {
        instanceArray->addr = nullptr;
        instanceArray->num = 0;
        instanceArray->type = ObjectType::INVALID;
        delete instanceArray;
        return;
    }

    for (size_t i = 0; i < instanceArray->num; ++i) {
        if (instances[i] == nullptr) {
            continue;
        }
        delete instances[i];
        instances[i] = nullptr;
    }
    delete[] instances;
    instanceArray->addr = nullptr;
    instanceArray->num = 0;
    instanceArray->type = ObjectType::INVALID;
    delete instanceArray;
}

OH_Drawing_FontVariationAxis* OH_Drawing_GetFontVariationAxisByIndex(
    OH_Drawing_Array* array, size_t index)
{
    if (array == nullptr) {
        return nullptr;
    }

    ObjectArray* axisArray = reinterpret_cast<ObjectArray*>(array);
    if (axisArray != nullptr && axisArray->addr != nullptr &&
        axisArray->type == ObjectType::FONT_VARIATION_AXIS && index < axisArray->num) {
        Drawing::FontParser::FontVariationAxis** variationAxis =
            reinterpret_cast<Drawing::FontParser::FontVariationAxis**>(axisArray->addr);
        return reinterpret_cast<OH_Drawing_FontVariationAxis*>(variationAxis[index]);
    }

    return nullptr;
}

OH_Drawing_FontVariationInstance* OH_Drawing_GetFontVariationInstanceByIndex(
    OH_Drawing_Array* array, size_t index)
{
    if (array == nullptr) {
        return nullptr;
    }

    ObjectArray* instanceArray = reinterpret_cast<ObjectArray*>(array);
    if (instanceArray != nullptr && instanceArray->addr != nullptr &&
        instanceArray->type == ObjectType::FONT_VARIATION_INSTANCE && index < instanceArray->num) {
        Drawing::FontParser::FontVariationInstance** variationInstance =
            reinterpret_cast<Drawing::FontParser::FontVariationInstance**>(instanceArray->addr);
        return reinterpret_cast<OH_Drawing_FontVariationInstance*>(variationInstance[index]);
    }

    return nullptr;
}
OH_Drawing_FontFullDescriptor* OH_Drawing_GetFontFullDescriptorByFullName(const OH_Drawing_String* fullName,
    OH_Drawing_SystemFontType fontType)
{
    if (fullName == nullptr) {
        return nullptr;
    }

    auto systemFontType = static_cast<int32_t>(fontType);
    using namespace OHOS::Rosen::Drawing;
    std::string fullNameString;
    if (!ConvertToString(fullName->strData, fullName->strLen, fullNameString)) {
        TEXT_LOGE("Failed to convert string to utf8");
        return nullptr;
    }

    std::shared_ptr<TextEngine::FontParser::FontDescriptor> result = nullptr;
    FontDescriptorMgrInstance.GetFontDescSharedPtrByFullName(fullNameString, systemFontType, result);
    if (result == nullptr) {
        return nullptr;
    }

    Drawing::FontParser::FontDescriptor* descriptor = new (std::nothrow) Drawing::FontParser::FontDescriptor(*result);
    if (descriptor == nullptr) {
        return nullptr;
    }

    return reinterpret_cast<OH_Drawing_FontFullDescriptor*>(descriptor);
}

OH_Drawing_FontVariationInstanceCoordinate* OH_Drawing_GetFontVariationInstanceCoordinate(
    OH_Drawing_FontVariationInstance* variationInstance, size_t* arrayLength)
{
    if (variationInstance == nullptr || arrayLength == nullptr) {
        return nullptr;
    }

    const auto& instance = *reinterpret_cast<const Drawing::FontParser::FontVariationInstance*>(variationInstance);
    if (instance.coordinates.empty()) {
        *arrayLength = 0;
        return nullptr;
    }

    *arrayLength = instance.coordinates.size();
    OH_Drawing_FontVariationInstanceCoordinate* coordinates =
        new (std::nothrow) OH_Drawing_FontVariationInstanceCoordinate[*arrayLength];
    if (coordinates == nullptr) {
        *arrayLength = 0;
        return nullptr;
    }

    for (size_t i = 0; i < *arrayLength; ++i) {
        coordinates[i].axisKey = strdup(instance.coordinates[i].axis.c_str());
        if (coordinates[i].axisKey == nullptr) {
            for (size_t j = 0; j < i; ++j) {
                free(coordinates[j].axisKey);
            }
            delete[] coordinates;
            *arrayLength = 0;
            return nullptr;
        }
        coordinates[i].value = instance.coordinates[i].value;
    }

    return coordinates;
}
