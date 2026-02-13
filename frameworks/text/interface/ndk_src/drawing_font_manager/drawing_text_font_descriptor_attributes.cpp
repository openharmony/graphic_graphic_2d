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

#include <string_ex.h>

#include "array_mgr.h"
#include "drawing_text_font_descriptor.h"
#include "font_descriptor_mgr.h"
#include "font_utils.h"
#include "text/common_utils.h"
#include "utils/text_log.h"

using namespace OHOS::Rosen;

namespace OHOS::Rosen::Text {

typedef OH_Drawing_ErrorCode (*FontFullDescriptorIntGetter)(const Drawing::FontParser::FontDescriptor&, int&);
typedef OH_Drawing_ErrorCode (*FontFullDescriptorBoolGetter)(const Drawing::FontParser::FontDescriptor&, bool&);
typedef OH_Drawing_ErrorCode (*FontFullDescriptorStrGetter)(const Drawing::FontParser::FontDescriptor&,
    OH_Drawing_String&);

OH_Drawing_ErrorCode TranslateStringToOHDrawingString(const std::string& fontFullDescriptorString,
    OH_Drawing_String& value)
{
    std::u16string utf16String = OHOS::Str8ToStr16(fontFullDescriptorString);
    if (utf16String.empty()) {
        TEXT_LOGE("Failed to convert string to utf16: %{public}s", fontFullDescriptorString.c_str());
        value.strLen = 0;
        value.strData = nullptr;
        return OH_DRAWING_SUCCESS;
    }
    value.strLen = utf16String.size() * sizeof(char16_t);
    std::unique_ptr strData = std::make_unique<uint8_t[]>(value.strLen);
    if (memcpy_s(strData.get(), value.strLen, utf16String.c_str(), value.strLen) != EOK) {
        TEXT_LOGE("Failed to memcpy_s length: %{public}u", value.strLen);
        return OH_DRAWING_ERROR_ALLOCATION_FAILED;
    }
    value.strData = strData.release();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontFullDescriptorWeight(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    int& value)
{
    value = static_cast<int>(fontFullDescriptor.weight);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontFullDescriptorWidth(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    int& value)
{
    value = static_cast<int>(fontFullDescriptor.width);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontFullDescriptorItalic(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    int& value)
{
    value = static_cast<int>(fontFullDescriptor.italic);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontFullDescriptorIndex(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    int& value)
{
    value = static_cast<int>(fontFullDescriptor.index);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontFullDescriptorMono(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    bool& value)
{
    value = static_cast<bool>(fontFullDescriptor.monoSpace);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontFullDescriptorSymbolic(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    bool& value)
{
    value = static_cast<bool>(fontFullDescriptor.symbolic);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontFullDescriptorPath(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.path, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorPostScriptName(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.postScriptName, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorFullName(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.fullName, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorFamilyName(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.fontFamily, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorSubFamilyName(const Drawing::FontParser::FontDescriptor& fontFullDescriptor,
    OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.fontSubfamily, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorLocalPostscriptName(
    const Drawing::FontParser::FontDescriptor& fontFullDescriptor, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.localPostscriptName, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorLocalFullName(
    const Drawing::FontParser::FontDescriptor& fontFullDescriptor, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.localFullName, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorLocalFamilyName(
    const Drawing::FontParser::FontDescriptor& fontFullDescriptor, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.localFamilyName, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorLocalSubFamilyName(
    const Drawing::FontParser::FontDescriptor& fontFullDescriptor, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.localSubFamilyName, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorVersion(
    const Drawing::FontParser::FontDescriptor& fontFullDescriptor, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.version, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorManufacture(
    const Drawing::FontParser::FontDescriptor& fontFullDescriptor, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.manufacture, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorCopyright(
    const Drawing::FontParser::FontDescriptor& fontFullDescriptor, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.copyright, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorTrademark(
    const Drawing::FontParser::FontDescriptor& fontFullDescriptor, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.trademark, value);
}

OH_Drawing_ErrorCode GetFontFullDescriptorLicense(
    const Drawing::FontParser::FontDescriptor& fontFullDescriptor, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(fontFullDescriptor.license, value);
}

OH_Drawing_ErrorCode GetFontVariationAxisMinValue(
    const Drawing::FontParser::FontVariationAxis& axis, double& value)
{
    value = axis.minValue;
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontVariationAxisMaxValue(
    const Drawing::FontParser::FontVariationAxis& axis, double& value)
{
    value = axis.maxValue;
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontVariationAxisDefaultValue(
    const Drawing::FontParser::FontVariationAxis& axis, double& value)
{
    value = axis.defaultValue;
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontVariationAxisFlags(
    const Drawing::FontParser::FontVariationAxis& axis, int& value)
{
    value = axis.flags;
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetFontVariationAxisKey(
    const Drawing::FontParser::FontVariationAxis& axis, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(axis.key, value);
}

OH_Drawing_ErrorCode GetFontVariationAxisName(
    const Drawing::FontParser::FontVariationAxis& axis, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(axis.name, value);
}

OH_Drawing_ErrorCode GetFontVariationAxisLocalName(
    const Drawing::FontParser::FontVariationAxis& axis, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(axis.localName, value);
}

OH_Drawing_ErrorCode GetFontVariationInstanceName(
    const Drawing::FontParser::FontVariationInstance& instance, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(instance.name, value);
}

OH_Drawing_ErrorCode GetFontVariationInstanceLocalName(
    const Drawing::FontParser::FontVariationInstance& instance, OH_Drawing_String& value)
{
    return TranslateStringToOHDrawingString(instance.localName, value);
}

static std::unordered_map<OH_Drawing_FontVariationAxisAttributeId,
    OH_Drawing_ErrorCode(*)(const Drawing::FontParser::FontVariationAxis&, double&)>
    g_fontVariationAxisDoubleGetters = {
        { FONT_VARIATION_AIXS_ATTR_D_MIN_VALUE, GetFontVariationAxisMinValue },
        { FONT_VARIATION_AIXS_ATTR_D_MAX_VALUE, GetFontVariationAxisMaxValue },
        { FONT_VARIATION_AIXS_ATTR_D_DEFAULT_VALUE, GetFontVariationAxisDefaultValue },
    };

static std::unordered_map<OH_Drawing_FontVariationAxisAttributeId,
    OH_Drawing_ErrorCode(*)(const Drawing::FontParser::FontVariationAxis&, int&)>
    g_fontVariationAxisIntGetters = {
        { FONT_VARIATION_AIXS_ATTR_I_FLAGS, GetFontVariationAxisFlags },
    };

static std::unordered_map<OH_Drawing_FontVariationAxisAttributeId,
    OH_Drawing_ErrorCode(*)(const Drawing::FontParser::FontVariationAxis&, OH_Drawing_String&)>
    g_fontVariationAxisStrGetters = {
        { FONT_VARIATION_AIXS_ATTR_S_KEY, GetFontVariationAxisKey },
        { FONT_VARIATION_AIXS_ATTR_S_NAME, GetFontVariationAxisName },
        { FONT_VARIATION_AIXS_ATTR_S_LOCAL_NAME, GetFontVariationAxisLocalName },
    };

static std::unordered_map<OH_Drawing_FontVariationInstanceAttributeId,
    OH_Drawing_ErrorCode(*)(const Drawing::FontParser::FontVariationInstance&, OH_Drawing_String&)>
    g_fontVariationInstanceStrGetters = {
        { FONT_VARIATION_INSTANCE_ATTR_S_NAME, GetFontVariationInstanceName },
        { FONT_VARIATION_INSTANCE_ATTR_S_LOCAL_NAME, GetFontVariationInstanceLocalName },
    };

static std::unordered_map<OH_Drawing_FontFullDescriptorAttributeId, FontFullDescriptorIntGetter>
    g_fontFullDescriptorIntGetters = {
        { FULL_DESCRIPTOR_ATTR_I_WEIGHT, GetFontFullDescriptorWeight },
        { FULL_DESCRIPTOR_ATTR_I_WIDTH, GetFontFullDescriptorWidth },
        { FULL_DESCRIPTOR_ATTR_I_ITALIC, GetFontFullDescriptorItalic },
        { FULL_DESCRIPTOR_ATTR_I_INDEX,  GetFontFullDescriptorIndex },
    };

static std::unordered_map<OH_Drawing_FontFullDescriptorAttributeId, FontFullDescriptorBoolGetter>
    g_fontFullDescriptorBoolGetters = {
        { FULL_DESCRIPTOR_ATTR_B_MONO, GetFontFullDescriptorMono },
        { FULL_DESCRIPTOR_ATTR_B_SYMBOLIC, GetFontFullDescriptorSymbolic },
    };

static std::unordered_map<OH_Drawing_FontFullDescriptorAttributeId, FontFullDescriptorStrGetter>
    g_fontFullDescriptorStrGetters = {
        { FULL_DESCRIPTOR_ATTR_S_PATH, GetFontFullDescriptorPath },
        { FULL_DESCRIPTOR_ATTR_S_POSTSCRIPT_NAME, GetFontFullDescriptorPostScriptName },
        { FULL_DESCRIPTOR_ATTR_S_FULL_NAME, GetFontFullDescriptorFullName },
        { FULL_DESCRIPTOR_ATTR_S_FAMILY_NAME, GetFontFullDescriptorFamilyName },
        { FULL_DESCRIPTOR_ATTR_S_SUB_FAMILY_NAME, GetFontFullDescriptorSubFamilyName },
        { FULL_DESCRIPTOR_ATTR_S_LOCAL_POSTSCRIPT_NAME, GetFontFullDescriptorLocalPostscriptName },
        { FULL_DESCRIPTOR_ATTR_S_LOCAL_FULL_NAME, GetFontFullDescriptorLocalFullName },
        { FULL_DESCRIPTOR_ATTR_S_LOCAL_FAMILY_NAME, GetFontFullDescriptorLocalFamilyName },
        { FULL_DESCRIPTOR_ATTR_S_LOCAL_SUB_FAMILY_NAME, GetFontFullDescriptorLocalSubFamilyName },
        { FULL_DESCRIPTOR_ATTR_S_VERSION, GetFontFullDescriptorVersion },
        { FULL_DESCRIPTOR_ATTR_S_MANUFACTURE, GetFontFullDescriptorManufacture },
        { FULL_DESCRIPTOR_ATTR_S_COPYRIGHT, GetFontFullDescriptorCopyright },
        { FULL_DESCRIPTOR_ATTR_S_TRADEMARK, GetFontFullDescriptorTrademark },
        { FULL_DESCRIPTOR_ATTR_S_LICENSE, GetFontFullDescriptorLicense },
    };
} // namespace OHOS::Rosen::Text

OH_Drawing_ErrorCode OH_Drawing_GetFontFullDescriptorAttributeInt(const OH_Drawing_FontFullDescriptor* descriptor,
    OH_Drawing_FontFullDescriptorAttributeId id, int* value)
{
    if (descriptor == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_fontFullDescriptorIntGetters.find(id);
    if (it == Text::g_fontFullDescriptorIntGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(*reinterpret_cast<const Drawing::FontParser::FontDescriptor*>(descriptor), *value);
}

OH_Drawing_ErrorCode OH_Drawing_GetFontFullDescriptorAttributeBool(const OH_Drawing_FontFullDescriptor* descriptor,
    OH_Drawing_FontFullDescriptorAttributeId id, bool* value)
{
    if (descriptor == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_fontFullDescriptorBoolGetters.find(id);
    if (it == Text::g_fontFullDescriptorBoolGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(*reinterpret_cast<const Drawing::FontParser::FontDescriptor*>(descriptor), *value);
}

OH_Drawing_ErrorCode OH_Drawing_GetFontFullDescriptorAttributeString(const OH_Drawing_FontFullDescriptor* descriptor,
    OH_Drawing_FontFullDescriptorAttributeId id, OH_Drawing_String* str)
{
    if (descriptor == nullptr || str == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }

    auto it = Text::g_fontFullDescriptorStrGetters.find(id);
    if (it == Text::g_fontFullDescriptorStrGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }

    return it->second(*reinterpret_cast<const Drawing::FontParser::FontDescriptor*>(descriptor), *str);
}

OH_Drawing_ErrorCode OH_Drawing_GetFontVariationAxisAttributeDouble(
    OH_Drawing_FontVariationAxis* variationAxis, OH_Drawing_FontVariationAxisAttributeId id, double* value)
{
    if (variationAxis == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_fontVariationAxisDoubleGetters.find(id);
    if (it == Text::g_fontVariationAxisDoubleGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(*reinterpret_cast<const Drawing::FontParser::FontVariationAxis*>(variationAxis), *value);
}

OH_Drawing_ErrorCode OH_Drawing_GetFontVariationAxisAttributeInt(
    OH_Drawing_FontVariationAxis* variationAxis, OH_Drawing_FontVariationAxisAttributeId id, int* value)
{
    if (variationAxis == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_fontVariationAxisIntGetters.find(id);
    if (it == Text::g_fontVariationAxisIntGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(*reinterpret_cast<const Drawing::FontParser::FontVariationAxis*>(variationAxis), *value);
}

OH_Drawing_ErrorCode OH_Drawing_GetFontVariationAxisAttributeStr(
    OH_Drawing_FontVariationAxis* variationAxis, OH_Drawing_FontVariationAxisAttributeId id, OH_Drawing_String* str)
{
    if (variationAxis == nullptr || str == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_fontVariationAxisStrGetters.find(id);
    if (it == Text::g_fontVariationAxisStrGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(*reinterpret_cast<const Drawing::FontParser::FontVariationAxis*>(variationAxis), *str);
}

OH_Drawing_ErrorCode OH_Drawing_GetFontVariationInstanceAttributeStr(
    OH_Drawing_FontVariationInstance* variationInstance, OH_Drawing_FontVariationInstanceAttributeId id,
    OH_Drawing_String* str)
{
    if (variationInstance == nullptr || str == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_fontVariationInstanceStrGetters.find(id);
    if (it == Text::g_fontVariationInstanceStrGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(*reinterpret_cast<const Drawing::FontParser::FontVariationInstance*>(variationInstance), *str);
}