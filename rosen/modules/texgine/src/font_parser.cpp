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

#include "font_parser.h"

#include <codecvt>
#include <iomanip>
#include <securec.h>

#include "font_config.h"
#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define SUCCESSED 0
#define FAILED 1

#define FONT_CONFIG_FILE  "/system/fonts/visibility_list.json"

#define HALF(a) ((a) / 2)

// "weight" and "italic" will assigned value 0 and 1, -1 used to exclude unassigned
FontParser::FontDescriptor::FontDescriptor(): path(""), postScriptName(""), fullName(""),
    fontFamily(""), fontSubfamily(""), weight(-1), width(0), italic(-1), monoSpace(0), symbolic(0)
{
}

FontParser::FontParser()
{
    data_ = nullptr;
    length_ = 0;
    FontConfig fontConfig(FONT_CONFIG_FILE);
    fontSet_ = fontConfig.GetFontSet();
}

void FontParser::ProcessCmapTable(const struct CmapTables* cmapTable, FontParser::FontDescriptor& fontDescriptor)
{
    for (auto i = 0; i < cmapTable->numTables.Get(); ++i) {
        const auto& record = cmapTable->encodingRecords[i];
        FontParser::PlatformId platformId = static_cast<FontParser::PlatformId>(record.platformID.Get());
        FontParser::EncodingIdWin encodingId = static_cast<FontParser::EncodingIdWin>(record.encodingID.Get());
        if (platformId == FontParser::PlatformId::WINDOWS && encodingId == FontParser::EncodingIdWin::SYMBOL) {
            fontDescriptor.symbolic = true;
            break;
        }
    }
}

void FontParser::GetStringFromNameId(FontParser::NameId nameId, const std::string& nameString,
    FontParser::FontDescriptor& fontDescriptor)
{
    switch (nameId) {
        case FontParser::NameId::FONT_FAMILY: {
            if (fontDescriptor.fontFamily.size() == 0) {
                fontDescriptor.fontFamily = nameString;
            }
            break;
        }
        case FontParser::NameId::FONT_SUBFAMILY: {
            if (fontDescriptor.fontSubfamily.size() == 0) {
                fontDescriptor.fontSubfamily = nameString;
            }
            break;
        }
        case FontParser::NameId::FULL_NAME: {
            if (fontDescriptor.fullName.size() == 0) {
                fontDescriptor.fullName = nameString;
            }
            break;
        }
        case FontParser::NameId::POSTSCRIPT_NAME: {
            if (fontDescriptor.postScriptName.size() == 0) {
                fontDescriptor.postScriptName = nameString;
            }
            break;
        }
        default: {
            break;
        }
    }
}

int FontParser::ProcessNameTable(const struct NameTable* nameTable, FontParser::FontDescriptor& fontDescriptor) const
{
    auto count = nameTable->count.Get();
    auto storageOffset = nameTable->storageOffset.Get();
    auto stringStorage = data_ + storageOffset;
    for (int i = 0; i < count; ++i) {
        if (nameTable->nameRecord[i].stringOffset.Get() == 0 || nameTable->nameRecord[i].length.Get() == 0) {
            continue;
        }
        FontParser::NameId nameId = static_cast<FontParser::NameId>(nameTable->nameRecord[i].nameId.Get());
        FontParser::PlatformId platformId =
            static_cast<FontParser::PlatformId>(nameTable->nameRecord[i].platformId.Get());
        auto len = nameTable->nameRecord[i].length.Get();
        auto stringOffset = nameTable->nameRecord[i].stringOffset.Get();
        const char* data = stringStorage + stringOffset;
        if (platformId == FontParser::PlatformId::MACINTOSH) {
            std::string nameString(data, len);
            GetStringFromNameId(nameId, nameString, fontDescriptor);
        } else if (platformId == FontParser::PlatformId::WINDOWS) {
            char* buffer = new (std::nothrow) char[len]();
            if (buffer == nullptr) {
                return FAILED;
            }
#ifndef USE_GRAPHIC_TEXT_GINE
            if (memcpy_s(buffer, len, data, len) != EOK) {
                LOGSO_FUNC_LINE(ERROR) << "memcpy failed";
                delete[] buffer;
                return FAILED;
            }
            const char16_t* strPtr = reinterpret_cast<const char16_t*>(buffer);
#else
#ifdef BUILD_NON_SDK_VER
            if (memcpy_s(buffer, len, data, len) != EOK) {
                LOGSO_FUNC_LINE(ERROR) << "memcpy failed";
                delete[] buffer;
                return FAILED;
            }
#else
            memcpy(buffer, data, len);
#endif
            const char16_t *strPtr = reinterpret_cast<const char16_t *>(buffer);
#endif
            const std::u16string u16str(strPtr, strPtr + HALF(len));
            std::wstring_convert<std::codecvt_utf16<char16_t>, char16_t> converter;
            const std::string name = converter.to_bytes(u16str);
            std::vector<char> vec(name.begin(), name.end());
            vec.erase(std::remove(vec.begin(), vec.end(), '\0'), vec.end());
            std::string nameString = std::string(vec.begin(), vec.end());
            GetStringFromNameId(nameId, nameString, fontDescriptor);
            delete[] buffer;
        }
    }

    return SUCCESSED;
}

void FontParser::ProcessPostTable(const struct PostTable* postTable, FontParser::FontDescriptor& fontDescriptor)
{
    if (postTable->italicAngle.Get() != 0) {
        fontDescriptor.italic = 1; // means support italics
    } else {
        fontDescriptor.italic = 0;
    }
    if (postTable->isFixedPitch.Get() == 1) {
        fontDescriptor.monoSpace = true;
    }
}

int FontParser::ParseCmapTable(sk_sp<SkTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    auto tag = HB_TAG('c', 'm', 'a', 'p');
    auto size = typeface->getTableSize(tag);
    if (size <= 0) {
        LOGSO_FUNC_LINE(ERROR) << "haven't cmap";
        return FAILED;
    }
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->getTableData(tag, 0, size, tableData.get());
    if (size != retTableData) {
        LOGSO_FUNC_LINE(ERROR) <<"get table data failed size: " << size << ", ret: " << retTableData;
        return FAILED;
    }
    hb_blob_t* hblob = nullptr;
    hblob = hb_blob_create(
        reinterpret_cast<const char*>(tableData.get()), size, HB_MEMORY_MODE_WRITABLE, tableData.get(), nullptr);
    if (hblob == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "hblob is nullptr";
        return FAILED;
    }
    data_ = hb_blob_get_data(hblob, nullptr);
    length_ = hb_blob_get_length(hblob);
    auto parseCmap = std::make_shared<CmapTableParser>(data_, length_);
    auto cmapTable = parseCmap->Parse(data_, length_);
    ProcessCmapTable(cmapTable, fontDescriptor);

    return SUCCESSED;
}

int FontParser::ParseNameTable(sk_sp<SkTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    auto tag = HB_TAG('n', 'a', 'm', 'e');
    auto size = typeface->getTableSize(tag);
    if (size <= 0) {
        LOGSO_FUNC_LINE(ERROR) << "haven't name";
        return FAILED;
    }
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->getTableData(tag, 0, size, tableData.get());
    if (size != retTableData) {
        LOGSO_FUNC_LINE(ERROR) <<"get table data failed size: " << size << ", ret: " << retTableData;
        return FAILED;
    }
    hb_blob_t* hblob = nullptr;
    hblob = hb_blob_create(
        reinterpret_cast<const char*>(tableData.get()), size, HB_MEMORY_MODE_WRITABLE, tableData.get(), nullptr);
    if (hblob == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "hblob is nullptr";
        return FAILED;
    }
    data_ = hb_blob_get_data(hblob, nullptr);
    length_ = hb_blob_get_length(hblob);
    auto parseName = std::make_shared<NameTableParser>(data_, length_);
    auto nameTable = parseName->Parse(data_, length_);
    int ret = ProcessNameTable(nameTable, fontDescriptor);
    if (ret != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "process name table failed";
        return FAILED;
    }

    return SUCCESSED;
}

int FontParser::ParsePostTable(sk_sp<SkTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    auto tag = HB_TAG('p', 'o', 's', 't');
    auto size = typeface->getTableSize(tag);
    if (size <= 0) {
        LOGSO_FUNC_LINE(ERROR) << "haven't post";
        return FAILED;
    }
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->getTableData(tag, 0, size, tableData.get());
    if (size != retTableData) {
        LOGSO_FUNC_LINE(ERROR) <<"get table data failed size: " << size << ", ret: " << retTableData;
        return FAILED;
    }
    hb_blob_t* hblob = nullptr;
    hblob = hb_blob_create(
        reinterpret_cast<const char*>(tableData.get()), size, HB_MEMORY_MODE_WRITABLE, tableData.get(), nullptr);
    if (hblob == nullptr) {
        LOGSO_FUNC_LINE(ERROR) << "hblob is nullptr";
        return FAILED;
    }
    data_ = hb_blob_get_data(hblob, nullptr);
    length_ = hb_blob_get_length(hblob);
    auto parsePost = std::make_shared<PostTableParser>(data_, length_);
    auto postTable = parsePost->Parse(data_, length_);
    ProcessPostTable(postTable, fontDescriptor);

    return SUCCESSED;
}

int FontParser::ParseTable(sk_sp<SkTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    if (ParseCmapTable(typeface, fontDescriptor) != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "parse cmap failed";
        return FAILED;
    }
    if (ParseNameTable(typeface, fontDescriptor) != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "parse name failed";
        return FAILED;
    }
    if (ParsePostTable(typeface, fontDescriptor) != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "parse post failed";
        return FAILED;
    }

    return SUCCESSED;
}

int FontParser::SetFontDescriptor()
{
    for (unsigned int i = 0; i < fontSet_.size(); ++i) {
        FontParser::FontDescriptor fontDescriptor;
        fontDescriptor.path = fontSet_[i];
        const char* path = fontSet_[i].c_str();
        auto typeface = SkTypeface::MakeFromFile(path);
        if (typeface == nullptr) {
            LOGSO_FUNC_LINE(ERROR) << "typeface is nullptr, can not parse: " << fontDescriptor.path;
            continue;
        }
        auto fontStyle = typeface->fontStyle();
        fontDescriptor.weight = fontStyle.weight();
        fontDescriptor.width = fontStyle.width();
        if (ParseTable(typeface, fontDescriptor) !=  SUCCESSED) {
            LOGSO_FUNC_LINE(ERROR) << "parse table failed";
            return FAILED;
        }
        visibilityFonts_.emplace_back(fontDescriptor);
    }

    return SUCCESSED;
}

std::vector<FontParser::FontDescriptor> FontParser::GetVisibilityFonts()
{
    if (SetFontDescriptor() != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "set visibility font descriptor failed";
    }

    return visibilityFonts_;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
