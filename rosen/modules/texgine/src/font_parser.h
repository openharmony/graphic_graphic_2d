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
#include <include/core/SkFontMgr.h>
#include <include/core/SkString.h>

#include "include/core/SkTypeface.h"
#include "opentype_parser/cmap_table_parser.h"
#include "opentype_parser/name_table_parser.h"
#include "opentype_parser/post_table_parser.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
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
        int weight;
        int width;
        bool italic;
        bool monoSpace;
        bool symbolic;
    };

    FontParser();
    std::vector<FontDescriptor> GetVisibilityFonts();

private:
    static void GetStringFromNameId(NameId nameId, const std::string& nameString, FontDescriptor& fontDescriptor);
    static void ProcessCmapTable(const struct CmapTables* cmapTable, FontDescriptor& fontDescriptor);
    int ProcessNameTable(const struct NameTable* nameTable, FontDescriptor& fontDescriptor) const;
    static void ProcessPostTable(const struct PostTable* postTable, FontDescriptor& fontDescriptor);
    int ParseCmapTable(sk_sp<SkTypeface> typeface, FontDescriptor& fontDescriptor);
    int ParseNameTable(sk_sp<SkTypeface> typeface, FontDescriptor& fontDescriptor);
    int ParsePostTable(sk_sp<SkTypeface> typeface, FontDescriptor& fontDescriptor);
    int ParseTable(sk_sp<SkTypeface> typeface, FontDescriptor& fontDescriptor);
    int SetFontDescriptor();

    const char* data_;
    unsigned int length_;
    std::vector<std::string> fontSet_;
    std::vector<FontDescriptor> visibilityFonts_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_FONT_PARSER_H
