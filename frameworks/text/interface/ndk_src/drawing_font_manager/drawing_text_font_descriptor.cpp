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
    ObjectArray* descriptorList = ConvertToOriginalText<ObjectArray>(descriptorArray);
    if (descriptorList == nullptr || descriptorList->type != ObjectType::FONT_FULL_DESCRIPTOR) {
        return;
    }
    Drawing::FontParser::FontDescriptor** descriptors =
        reinterpret_cast<Drawing::FontParser::FontDescriptor**>(descriptorList->addr);
    if (descriptors == nullptr) {
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