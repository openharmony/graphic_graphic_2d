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
#include <dirent.h>
#include <iostream>
#include <iomanip>
#include <securec.h>
#ifdef BUILD_NON_SDK_VER
#include <iconv.h>
#endif

#include "font_config.h"
#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define SUCCESSED 0
#define FAILED 1

#define FONT_CONFIG_FILE  "/system/fonts/visibility_list.json"
#define SYSTEM_FONT_PATH "/system/fonts/"

#define HALF(a) ((a) / 2)

// "weight" and "italic" will assigned value 0 and 1, -1 used to exclude unassigned
FontParser::FontDescriptor::FontDescriptor(): path(""), postScriptName(""), fullName(""),
    fontFamily(""), fontSubfamily(""), postScriptNameLid(0), fullNameLid(0), fontFamilyLid(0),
    fontSubfamilyLid(0), requestedLid(0), weight(-1), width(0), italic(-1), monoSpace(0), symbolic(0)
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

void FontParser::GetStringFromNameId(FontParser::NameId nameId, unsigned int languageId, const std::string& nameString,
    FontParser::FontDescriptor& fontDescriptor)
{
    switch (nameId) {
        case FontParser::NameId::FONT_FAMILY: {
            SetNameString(fontDescriptor, fontDescriptor.fontFamily, fontDescriptor.fontFamilyLid,
                languageId, nameString);
            break;
        }
        case FontParser::NameId::FONT_SUBFAMILY: {
            SetNameString(fontDescriptor, fontDescriptor.fontSubfamily, fontDescriptor.fontSubfamilyLid,
                languageId, nameString);
            break;
        }
        case FontParser::NameId::FULL_NAME: {
            SetNameString(fontDescriptor, fontDescriptor.fullName, fontDescriptor.fullNameLid,
                languageId, nameString);
            break;
        }
        case FontParser::NameId::POSTSCRIPT_NAME: {
            SetNameString(fontDescriptor, fontDescriptor.postScriptName, fontDescriptor.postScriptNameLid,
                languageId, nameString);
            break;
        }
        default: {
            break;
        }
    }
}

void FontParser::SetNameString(FontParser::FontDescriptor& fontDescriptor, std::string& field, unsigned int& fieldLid,
    unsigned int languageId, const std::string& nameString)
{
    bool willSet = field.empty();
    if (!willSet) {
        if (languageId == fontDescriptor.requestedLid) {
            willSet = true;
        } else if (fieldLid != fontDescriptor.requestedLid && languageId == LANGUAGE_DEFAULT) {
            willSet = true;
        }
    }

    if (willSet) {
        fieldLid = languageId;
        field = nameString;
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
        unsigned int languageId = static_cast<unsigned int>(nameTable->nameRecord[i].languageId.Get());
        FontParser::PlatformId platformId =
            static_cast<FontParser::PlatformId>(nameTable->nameRecord[i].platformId.Get());
        auto len = nameTable->nameRecord[i].length.Get();
        auto stringOffset = nameTable->nameRecord[i].stringOffset.Get();
        const char* data = stringStorage + stringOffset;
        if (platformId == FontParser::PlatformId::MACINTOSH) {
#ifdef BUILD_NON_SDK_VER
            std::string nameString = ToUtf8(std::string(data, len));
#else
            std::string nameString(data, len);
#endif
            GetStringFromNameId(nameId, languageId, nameString, fontDescriptor);
        } else if (platformId == FontParser::PlatformId::WINDOWS) {
            std::wstring_convert<std::codecvt_utf16<char16_t>, char16_t> converter;
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converterUtf8;
            const std::u16string u16str = converter.from_bytes(data, data + len);
            std::string nameString = converterUtf8.to_bytes(u16str);
            GetStringFromNameId(nameId, languageId, nameString, fontDescriptor);
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

int FontParser::ParseCmapTable(std::shared_ptr<TexgineTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    auto tag = HB_TAG('c', 'm', 'a', 'p');
    auto size = typeface->GetTableSize(tag);
    if (size <= 0) {
        LOGSO_FUNC_LINE(ERROR) << "haven't cmap";
        return FAILED;
    }
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->GetTableData(tag, 0, size, tableData.get());
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

int FontParser::ParseNameTable(std::shared_ptr<TexgineTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    auto tag = HB_TAG('n', 'a', 'm', 'e');
    auto size = typeface->GetTableSize(tag);
    if (size <= 0) {
        LOGSO_FUNC_LINE(ERROR) << "haven't name";
        return FAILED;
    }
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->GetTableData(tag, 0, size, tableData.get());
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

int FontParser::ParsePostTable(std::shared_ptr<TexgineTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    auto tag = HB_TAG('p', 'o', 's', 't');
    auto size = typeface->GetTableSize(tag);
    if (size <= 0) {
        LOGSO_FUNC_LINE(ERROR) << "haven't post";
        return FAILED;
    }
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->GetTableData(tag, 0, size, tableData.get());
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

int FontParser::ParseTable(std::shared_ptr<TexgineTypeface> typeface, FontParser::FontDescriptor& fontDescriptor)
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

int FontParser::SetFontDescriptor(const unsigned int languageId)
{
    for (unsigned int i = 0; i < fontSet_.size(); ++i) {
        FontParser::FontDescriptor fontDescriptor;
        fontDescriptor.requestedLid = languageId;
        fontDescriptor.path = fontSet_[i];
        const char* path = fontSet_[i].c_str();
        auto typeface = TexgineTypeface::MakeFromFile(path);
        if (typeface == nullptr) {
            LOGSO_FUNC_LINE(ERROR) << "typeface is nullptr, can not parse: " << fontDescriptor.path;
            continue;
        }
        auto fontStyle = typeface->GetFontStyle();
        if (fontStyle == nullptr) {
            LOGSO_FUNC_LINE(ERROR) << "fontStyle is nullptr, can not parse: " << fontDescriptor.path;
            continue;
        }
        fontDescriptor.weight = fontStyle->GetWeight();
        fontDescriptor.width = fontStyle->GetWidth();
        if (ParseTable(typeface, fontDescriptor) !=  SUCCESSED) {
            LOGSO_FUNC_LINE(ERROR) << "parse table failed";
            return FAILED;
        }
        visibilityFonts_.emplace_back(fontDescriptor);
    }

    return SUCCESSED;
}

#ifdef BUILD_NON_SDK_VER
std::string FontParser::ToUtf8(const std::string& str)
{
    std::string utf8Str;
    // UTF-8 and GB2312 is encoding format of string
    iconv_t conv = iconv_open("UTF-8", "GB2312");
    if (conv == (iconv_t)-1) {
        return utf8Str;
    }
    char* inBuf = const_cast<char*>(str.c_str());
    size_t inBytesLeft = str.length();
    size_t outBytesLeft = inBytesLeft * 2;
    char* outBuf = new char[outBytesLeft];
    char* outBufStart = outBuf;
    size_t res = iconv(conv, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
    if (res != (size_t)-1) {
        utf8Str.assign(outBufStart, outBuf - outBufStart);
    }
    delete[] outBufStart;
    iconv_close(conv);
    return utf8Str;
}
#endif

std::vector<FontParser::FontDescriptor> FontParser::GetVisibilityFonts(const std::string &locale)
{
    if (SetFontDescriptor(GetLanguageId(locale)) != SUCCESSED) {
        LOGSO_FUNC_LINE(ERROR) << "set visibility font descriptor failed";
    }

    return visibilityFonts_;
}

class SystemFont {
public:
    explicit SystemFont(const char* fPath = SYSTEM_FONT_PATH)
    {
        ParseConfig(fPath);
    }

    ~SystemFont() = default;

    std::shared_ptr<std::vector<std::string>> GetSystemFontSet() const
    {
        return systemFontSet_;
    }

private:
    void ParseConfig(const char* fPath)
    {
        if (fPath == nullptr) {
            return;
        }
        systemFontSet_ = std::make_shared<std::vector<std::string>>();
        DIR *dir = opendir(fPath);
        if (dir == nullptr) {
            return;
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            std::string tmp = entry->d_name;
            systemFontSet_->push_back(SYSTEM_FONT_PATH + tmp);
        }
        closedir(dir);
    }

    std::shared_ptr<std::vector<std::string>> systemFontSet_;
};

std::unique_ptr<FontParser::FontDescriptor> FontParser::ParseFontDescriptor(const std::string& fontName,
    const unsigned int languageId)
{
    SystemFont sSystemFont;
    std::shared_ptr<std::vector<std::string>> systemFontList = sSystemFont.GetSystemFontSet();
    if (systemFontList == nullptr || systemFontList->empty()) {
        return nullptr;
    }

    int systemFontSize = systemFontList->size();
    for (auto font : fontSet_) {
        for (int i = 0; i < systemFontSize; i++) {
            if (systemFontSize <= 0) {
                break;
            }
            if ((*systemFontList)[i] == font) {
                systemFontList->erase(systemFontList->begin() + i);
                systemFontSize --;
                break;
            }
        }

        systemFontList->push_back(font);
    }

    for (int i = systemFontList->size() - 1; i >= 0; --i) {
        FontParser::FontDescriptor fontDescriptor;
        fontDescriptor.requestedLid = languageId;
        fontDescriptor.path = (*systemFontList)[i];
        const char* path = (*systemFontList)[i].c_str();
        auto typeface = TexgineTypeface::MakeFromFile(path);
        if (typeface == nullptr) {
            LOGSO_FUNC_LINE(ERROR) << "typeface is nullptr, can not parse: " << fontDescriptor.path;
            continue;
        }
        auto fontStyle = typeface->GetFontStyle();
        if (fontStyle == nullptr) {
            LOGSO_FUNC_LINE(ERROR) << "fontStyle is nullptr, can not parse: " << fontDescriptor.path;
            continue;
        }
        fontDescriptor.weight = fontStyle->GetWeight();
        fontDescriptor.width = fontStyle->GetWidth();
        if (ParseTable(typeface, fontDescriptor) !=  SUCCESSED) {
            LOGSO_FUNC_LINE(ERROR) << "parse table failed";
            return nullptr;
        }
        std::string name = SYSTEM_FONT_PATH + fontName;
        if (fontDescriptor.fullName == fontName || fontDescriptor.path == name) {
            return std::make_unique<FontDescriptor>(fontDescriptor);
        }
    }

    return nullptr;
}

std::unique_ptr<FontParser::FontDescriptor> FontParser::GetVisibilityFontByName(const std::string& fontName,
    const std::string locale)
{
    return ParseFontDescriptor(fontName, GetLanguageId(locale));
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
