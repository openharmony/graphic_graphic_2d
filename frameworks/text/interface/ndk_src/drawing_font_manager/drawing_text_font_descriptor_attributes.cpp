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

static std::unordered_map<OH_Drawing_FontFullDescriptorAttributeId, FontFullDescriptorIntGetter>
    g_fontFullDescriptorIntGetters = {
        { FULL_DESCRIPTOR_ATTR_I_WEIGHT, GetFontFullDescriptorWeight },
        { FULL_DESCRIPTOR_ATTR_I_WIDTH, GetFontFullDescriptorWidth },
        { FULL_DESCRIPTOR_ATTR_I_ITALIC, GetFontFullDescriptorItalic },
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
    };
} // namespace OHOS::Rosen::Text

OH_Drawing_ErrorCode OH_Drawing_GetFontFullDescriptorAttributeInt(const OH_Drawing_FontFullDescriptor* descriptor,
    OH_Drawing_FontFullDescriptorAttributeId id, int* value)
{
    if (descriptor == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
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
        return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
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
        return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
    }

    auto it = Text::g_fontFullDescriptorStrGetters.find(id);
    if (it == Text::g_fontFullDescriptorStrGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }

    return it->second(*reinterpret_cast<const Drawing::FontParser::FontDescriptor*>(descriptor), *str);
}