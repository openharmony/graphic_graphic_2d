/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_SRC_FONT_PARSER_H
#define ROSEN_MODULES_TEXGINE_SRC_FONT_PARSER_H

#include <string>
#include <vector>

#include "opentype_parser/cmap_table_parser.h"
#include "opentype_parser/name_table_parser.h"
#include "opentype_parser/post_table_parser.h"

#include "text/typeface.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
const std::string SIMPLIFIED_CHINESE = "zh-hans";
const std::string TRADITIONAL_CHINESE = "zh-hant";
const std::string ENGLISH = "en-latn";
const unsigned int LANGUAGE_SC = 2052;
const unsigned int LANGUAGE_TC = 1028;
const unsigned int LANGUAGE_EN = 1033;
const unsigned int LANGUAGE_DEFAULT = LANGUAGE_SC;

class FontParser {
public:
    enum class PlatformId {
        UNITE_CODE = 0,
        MACINTOSH = 1,
        ISO = 2,
        WINDOWS = 3,
        CUSTOM = 4,
    };

    enum class EncodingIdWin {
        SYMBOL = 0,
        UNICODE_BMP = 1,
    };

    enum class Version {
        SYMBOL = 0,
        UNICODE_BMP = 1,
    };

    enum class NameId {
        COPYRIGHT = 0,
        FONT_FAMILY = 1,
        FONT_SUBFAMILY = 2,
        UNIQUE_FONT_ID = 3,
        FULL_NAME = 4,
        VERSION_STRING = 5,
        POSTSCRIPT_NAME = 6,
        TRADEMARK = 7,
    };

    enum SystemFontType : int32_t {
        ALL = 1 << 0,
        GENERIC = 1 << 1,
        STYLISH = 1 << 2,
        INSTALLED = 1 << 3,
        CUSTOMIZED = 1 << 4
    };

    struct FontVariation {
        std::string axis;
        double value = 0.0;
    };

    struct FontVariationAxis {
        std::string key;
        double minValue = 0.0;
        double maxValue = 0.0;
        double defaultValue = 0.0;
        int flags = 0;
        std::string name;
        std::string localName;
    };

    struct FontVariationInstance {
        std::string name;
        std::string localName;
        std::vector<FontVariation> coordinates;
    };

    struct FontDescriptor {
        std::string path;
        std::string postScriptName;
        std::string fullName;
        std::string fontFamily;
        std::string fontSubfamily;
        std::string requestedFullname;
        unsigned int postScriptNameLid = 0;
        unsigned int fullNameLid = 0;
        unsigned int fontFamilyLid = 0;
        unsigned int fontSubfamilyLid = 0;
        unsigned int requestedLid = 0;
        int weight = 0;
        int width = 0;
        int italic = 0;
        bool monoSpace = false;
        bool symbolic = false;
        std::string localPostscriptName;
        std::string localFullName;
        std::string localFamilyName;
        std::string localSubFamilyName;
        std::string version;
        std::string manufacture;
        std::string copyright;
        std::string trademark;
        std::string license;
        int32_t index{0};
        std::vector<FontVariationAxis> variationAxisRecords;
        std::vector<FontVariationInstance> variationInstanceRecords;
        FontDescriptor() = default;
        FontDescriptor(const FontDescriptor&) = default;
        FontDescriptor& operator=(const FontDescriptor& other) = default;
    };

    FontParser();
    std::vector<FontDescriptor> GetVisibilityFonts(const std::string &locale = SIMPLIFIED_CHINESE);
    std::unique_ptr<FontDescriptor> GetVisibilityFontByName(const std::string& fontName,
        const std::string locale = SIMPLIFIED_CHINESE);

    std::vector<std::shared_ptr<FontDescriptor>> GetSystemFonts(const std::string locale = ENGLISH);
    const std::vector<std::string>& GetFontSet() const;
    static std::vector<std::shared_ptr<FontDescriptor>> ParserFontDescriptorsFromPath(
        const std::string& path, const std::string& locale = ENGLISH);
    static std::vector<std::shared_ptr<FontDescriptor>> ParserFontDescriptorsFromStream(
        const void* data, size_t byteLength, const std::string& locale = ENGLISH);
    std::shared_ptr<FontDescriptor> ParserFontDescriptorFromPath(
        const std::string& path, size_t index, const std::string& locale = ENGLISH) const;
    static std::shared_ptr<FontDescriptor> CreateFontDescriptor(
        const std::shared_ptr<Drawing::Typeface>& typefaces, unsigned int languageId);
    static std::vector<std::shared_ptr<FontDescriptor>> CreateFontDescriptors(
        const std::vector<std::shared_ptr<Drawing::Typeface>>& typefaces, const std::string& locale = ENGLISH);
    static std::vector<uint32_t> GetFontTypefaceUnicode(const std::string& path, int32_t index);
    static std::vector<uint32_t> GetFontTypefaceUnicode(const void* data, size_t length, int32_t index);
    static std::vector<std::string> GetFontFullName(int fd);
    static int32_t GetFontCount(const std::string& path);
    static int32_t GetFontCount(const std::vector<uint8_t>& data);

private:
    static void GetStringFromNameId(NameId nameId, unsigned int languageId, const std::string& nameString,
        FontDescriptor& fontDescriptor);
    static void ProcessTable(const CmapTables* cmapTable, FontDescriptor& fontDescriptor, size_t size);
    static void ProcessTable(const NameTable* nameTable, FontDescriptor& fontDescriptor, size_t size);
    static void ProcessTable(const PostTable* postTable, FontDescriptor& fontDescriptor, size_t /* size */);
    template<typename T>
    static bool ParseOneTable(std::shared_ptr<Drawing::Typeface> typeface, FontParser::FontDescriptor& fontDescriptor);
    template<typename Tuple, size_t... Is>
    static bool ParseAllTables(
        std::shared_ptr<Drawing::Typeface> typeface, FontDescriptor& fontDescriptor, std::index_sequence<Is...>);
    static bool ParseTable(std::shared_ptr<Drawing::Typeface> typeface, FontDescriptor& fontDescriptor);
    static void FillFontDescriptorWithLocalInfo(std::shared_ptr<Drawing::Typeface> typeface, FontDescriptor& desc);
    static void FillFontDescriptorWithVariationInfo(std::shared_ptr<Drawing::Typeface> typeface,
                                                     FontDescriptor& desc, const std::vector<std::string>& bcpTagList);
    static void FillFontDescriptorWithFallback(std::shared_ptr<Drawing::Typeface> typeface, FontDescriptor& desc);
    static std::vector<std::string> GetBcpTagList();
    bool SetFontDescriptor(const unsigned int languageId);
    std::unique_ptr<FontParser::FontDescriptor> ParseFontDescriptor(
        const std::string& fontName, const unsigned int languageId);
    static bool CheckFullNameParamInvalid(FontParser::FontDescriptor& fontDescriptor, unsigned int languageId,
        const std::string& nameString);
    static void SetNameString(FontParser::FontDescriptor& fontDescriptor, std::string& field, unsigned int& fieldLid,
        unsigned int languageId, const std::string& nameString);
    static int GetLanguageId(const std::string& locale)
    {
        std::string localeLower = locale;
        transform(localeLower.begin(), localeLower.end(), localeLower.begin(), tolower);
        if (localeLower.empty()) {
            return LANGUAGE_SC;
        } else if (localeLower.find(TRADITIONAL_CHINESE) == 0) {
            return LANGUAGE_TC;
        } else if (localeLower.find(ENGLISH) == 0) {
            return LANGUAGE_EN;
        } else {
            return LANGUAGE_SC;
        }
    }
#ifdef BUILD_NON_SDK_VER
    static std::string ConvertToString(const std::string& src, const std::string& srcType,
        const std::string& targetType);
#endif

#ifdef ENABLE_OHOS_ENHANCE
    static std::vector<uint8_t> GetFontDataFromFd(int fd);
#endif

    const char* data_;
    unsigned int length_;
    std::vector<std::string> fontSet_;
    std::vector<FontDescriptor> visibilityFonts_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_FONT_PARSER_H
