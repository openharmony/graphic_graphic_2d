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

#include <hb.h>
#include <string>
#include <vector>

#include "opentype_parser/cmap_table_parser.h"
#include "opentype_parser/name_table_parser.h"
#include "opentype_parser/post_table_parser.h"

#include "texgine_typeface.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define SIMPLIFIED_CHINESE "zh-hans"
#define TRADITIONAL_CHINESE "zh-hant"
#define ENGLISH "en-latn"
#define LANGUAGE_SC 2052
#define LANGUAGE_TC 1028
#define LANGUAGE_EN 1033
#define LANGUAGE_DEFAULT LANGUAGE_SC

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

    struct FontDescriptor {
        FontDescriptor();
        std::string path;
        std::string postScriptName;
        std::string fullName;
        std::string fontFamily;
        std::string fontSubfamily;
        unsigned int postScriptNameLid;
        unsigned int fullNameLid;
        unsigned int fontFamilyLid;
        unsigned int fontSubfamilyLid;
        unsigned int requestedLid;
        int weight;
        int width;
        int italic;
        bool monoSpace;
        bool symbolic;
    };

    FontParser();
    std::vector<FontDescriptor> GetVisibilityFonts(const std::string locale = SIMPLIFIED_CHINESE);
    std::unique_ptr<FontDescriptor> GetVisibilityFontByName(const std::string& fontName,
        const std::string locale = SIMPLIFIED_CHINESE);

private:
    static void GetStringFromNameId(NameId nameId, unsigned int languageId, const std::string& nameString,
        FontDescriptor& fontDescriptor);
    static void ProcessCmapTable(const struct CmapTables* cmapTable, FontDescriptor& fontDescriptor);
    int ProcessNameTable(const struct NameTable* nameTable, FontDescriptor& fontDescriptor) const;
    static void ProcessPostTable(const struct PostTable* postTable, FontDescriptor& fontDescriptor);
    int ParseCmapTable(std::shared_ptr<TexgineTypeface> typeface, FontDescriptor& fontDescriptor);
    int ParseNameTable(std::shared_ptr<TexgineTypeface> typeface, FontDescriptor& fontDescriptor);
    int ParsePostTable(std::shared_ptr<TexgineTypeface> typeface, FontDescriptor& fontDescriptor);
    int ParseTable(std::shared_ptr<TexgineTypeface> typeface, FontDescriptor& fontDescriptor);
    int SetFontDescriptor(const unsigned int languageId);
    std::unique_ptr<FontParser::FontDescriptor> ParseFontDescriptor(const std::string& fontName,
        const unsigned int languageId);
    static void SetNameString(FontParser::FontDescriptor& fontDescriptor, std::string& field, unsigned int& fieldLid,
        unsigned int languageId, const std::string& nameString);
    int GetLanguageId(const std::string& locale)
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
    static std::string ToUtf8(const std::string& gbkStr);
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
