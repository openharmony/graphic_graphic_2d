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
#include <map>
#include <list>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <securec.h>
#ifdef BUILD_NON_SDK_VER
#include <iconv.h>
#endif

#include "font_config.h"
#include "utils/text_log.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define FONT_CONFIG_FILE  "/system/fonts/visibility_list.json"
#define FONT_CONFIG_PROD_FILE "/sys_prod/fonts/visibility_list.json"
#define SYSTEM_FONT_PATH "/system/fonts/"
#define SYS_PROD_FONT_PATH "/sys_prod/fonts/"

constexpr uint32_t HB_TAG(uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4)
{
    // 24 means 32-24 bit, 16 means 24-16 bit, 8 means 16-8 bit, 0 means 8-0 bit, 0xFF means only 8 bit
    return ((c1 & 0xFF) << 24) | ((c2 & 0xFF) << 16) | ((c3 & 0xFF) << 8) | (c4 & 0xFF);
}

FontParser::FontParser()
{
    data_ = nullptr;
    length_ = 0;
        fontSet_.clear();
    FontConfig fontConfig(FONT_CONFIG_FILE);
    auto fonts = fontConfig.GetFontSet();
    fontSet_.insert(fontSet_.end(), fonts.begin(), fonts.end());
    FontConfig fontProdConfig(FONT_CONFIG_PROD_FILE);
    auto prodFonts = fontProdConfig.GetFontSet();
    fontSet_.insert(fontSet_.end(), prodFonts.begin(), prodFonts.end());
}

void FontParser::ProcessTable(const CmapTables* cmapTable, FontParser::FontDescriptor& fontDescriptor)
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
            if (!fontDescriptor.requestedFullname.empty() &&
                fontDescriptor.fullName == fontDescriptor.requestedFullname) {
                break;
            }

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
        willSet = languageId == fontDescriptor.requestedLid ||
            (fieldLid != fontDescriptor.requestedLid && languageId == LANGUAGE_DEFAULT);
    }

    if (willSet) {
        fieldLid = languageId;
        field = nameString;
    }
}

void FontParser::ProcessTable(const NameTable* nameTable, FontParser::FontDescriptor& fontDescriptor)
{
    auto count = nameTable->count.Get();
    auto storageOffset = nameTable->storageOffset.Get();
    const char* stringStorage = reinterpret_cast<const char*>(nameTable) + storageOffset;
    for (int i = 0; i < count; ++i) {
        if (nameTable->nameRecord[i].stringOffset.Get() == 0 && nameTable->nameRecord[i].length.Get() == 0) {
            continue;
        }
        FontParser::NameId nameId = static_cast<FontParser::NameId>(nameTable->nameRecord[i].nameId.Get());
        // Parsing fields with NameId greater than 7 is not currently supported.
        if (nameId > FontParser::NameId::TRADEMARK) {
            continue;
        }
        unsigned int languageId = static_cast<unsigned int>(nameTable->nameRecord[i].languageId.Get());
        FontParser::PlatformId platformId =
            static_cast<FontParser::PlatformId>(nameTable->nameRecord[i].platformId.Get());
        auto len = nameTable->nameRecord[i].length.Get();
        auto stringOffset = nameTable->nameRecord[i].stringOffset.Get();
        const char* data = stringStorage + stringOffset;
        if (platformId == FontParser::PlatformId::MACINTOSH) {
#ifdef BUILD_NON_SDK_VER
            std::string nameString = ConvertToString(std::string(data, len), "GB2312", "UTF-8");
#else
            std::string nameString(data, len);
#endif
            GetStringFromNameId(nameId, languageId, nameString, fontDescriptor);
        } else if (platformId == FontParser::PlatformId::WINDOWS) {
#ifdef BUILD_NON_SDK_VER
            std::string nameString = ConvertToString(std::string(data, len), "UTF-16BE", "UTF-8");
#else
            std::string nameString(data, len);
#endif
            GetStringFromNameId(nameId, languageId, nameString, fontDescriptor);
        }
    }
}

void FontParser::ProcessTable(const PostTable* postTable, FontParser::FontDescriptor& fontDescriptor)
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

template<typename T>
bool FontParser::ParseOneTable(std::shared_ptr<Drawing::Typeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    // get tag by a four bytes string
    uint32_t tag = HB_TAG(T::tag[0], T::tag[1], T::tag[2], T::tag[3]);
    auto size = typeface->GetTableSize(tag);
    if (size < sizeof(T)) {
        TEXT_LOGE("No table");
        return false;
    }
    std::unique_ptr<char[]> tableData = std::make_unique<char[]>(size);
    auto readSize = typeface->GetTableData(tag, 0, size, tableData.get());
    if (size != readSize) {
        TEXT_LOGE("Failed to get table, size %{public}zu, ret %{public}zu", size, readSize);
        return false;
    }
    ProcessTable(reinterpret_cast<T*>(tableData.get()), fontDescriptor);
    return true;
}

using TableTypes = std::tuple<CmapTables, NameTable, PostTable>;

template<typename Tuple, size_t... Is>
bool FontParser::ParseAllTables(
    std::shared_ptr<Drawing::Typeface> typeface, FontParser::FontDescriptor& fontDescriptor, std::index_sequence<Is...>)
{
    return (ParseOneTable<std::tuple_element_t<Is, Tuple>>(typeface, fontDescriptor) && ...);
}

bool FontParser::ParseTable(std::shared_ptr<Drawing::Typeface> typeface, FontParser::FontDescriptor& fontDescriptor)
{
    return ParseAllTables<TableTypes>(
        typeface, fontDescriptor, std::make_index_sequence<std::tuple_size_v<TableTypes>>());
}

bool FontParser::SetFontDescriptor(const unsigned int languageId)
{
        visibilityFonts_.clear();
    std::list<std::string> fontSetCache;
    for (unsigned int i = 0; i < fontSet_.size(); ++i) {
        FontParser::FontDescriptor fontDescriptor;
        fontDescriptor.requestedLid = languageId;
        fontDescriptor.path = fontSet_[i];
        const char* path = fontSet_[i].c_str();
        auto typeface = Drawing::Typeface::MakeFromFile(path);
        if (typeface == nullptr) {
            TEXT_LOGE("Failed to parse file %{public}s", fontDescriptor.path.c_str());
            continue;
        }
        auto fontStyle = typeface->GetFontStyle();
        fontDescriptor.weight = fontStyle.GetWeight();
        fontDescriptor.width = fontStyle.GetWidth();
        if (!ParseTable(typeface, fontDescriptor)) {
            TEXT_LOGE("Failed to parse table");
            return false;
        }
        size_t idx = fontSet_[i].rfind('/');
        std::string fontName = fontSet_[i].substr(idx + 1, fontSet_[i].size() - idx - 1);
        if (std::find(fontSetCache.begin(), fontSetCache.end(), fontName) == fontSetCache.end()) {
            fontSetCache.push_back(fontName);
            visibilityFonts_.emplace_back(fontDescriptor);
        }
    }

    return true;
}

#ifdef BUILD_NON_SDK_VER
std::string FontParser::ConvertToString(const std::string& src, const std::string& srcType,
    const std::string& targetType)
{
    std::string utf8Str;
    iconv_t conv = iconv_open(targetType.c_str(), srcType.c_str());
    if (conv == (iconv_t)-1) {
        return utf8Str;
    }
    char* inBuf = const_cast<char*>(src.c_str());
    size_t inBytesLeft = src.length();
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
    if (!SetFontDescriptor(GetLanguageId(locale))) {
        TEXT_LOGE("Failed to set visibility font descriptor");
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

std::vector<std::shared_ptr<FontParser::FontDescriptor>> FontParser::GetSystemFonts(const std::string locale)
{
    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces = Drawing::Typeface::GetSystemFonts();
    return CreateFontDescriptors(typefaces, locale);
}

std::vector<std::shared_ptr<FontParser::FontDescriptor>> FontParser::ParserFontDescriptorsFromPath(
    const std::string& path, const std::string& locale)
{
    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    int index = 0;
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;
    while ((typeface = Drawing::Typeface::MakeFromFile(path.c_str(), index)) != nullptr) {
        typefaces.push_back(typeface);
        index++;
    }
    return CreateFontDescriptors(typefaces, locale);
}

std::vector<std::shared_ptr<FontParser::FontDescriptor>> FontParser::CreateFontDescriptors(
    const std::vector<std::shared_ptr<Drawing::Typeface>>& typefaces, const std::string& locale)
{
    if (typefaces.empty()) {
        return {};
    }

    std::vector<std::shared_ptr<FontDescriptor>> descriptors;
    descriptors.reserve(typefaces.size());
    unsigned int languageId = static_cast<unsigned int>(GetLanguageId(locale));
    for (auto& item : typefaces) {
        FontDescriptor desc;
        desc.requestedLid = languageId;
        desc.path = item->GetFontPath();
        auto fontStyle = item->GetFontStyle();
        desc.weight = fontStyle.GetWeight();
        desc.width = fontStyle.GetWidth();
        if (!ParseTable(item, desc)) {
            continue;
        }
        descriptors.emplace_back(std::make_shared<FontDescriptor>(desc));
    }
    return descriptors;
}

std::unique_ptr<FontParser::FontDescriptor> FontParser::ParseFontDescriptor(const std::string& fontName,
    const unsigned int languageId)
{
    FontConfigJson fontConfigJson;
    fontConfigJson.ParseFontFileMap();
    std::shared_ptr<FontFileMap> fontFileMap = fontConfigJson.GetFontFileMap();
    if (fontFileMap == nullptr || (*fontFileMap).empty()) {
        TEXT_LOGE("Empty font file map");
        return nullptr;
    }
    if ((*fontFileMap).find(fontName) == (*fontFileMap).end()) {
        TEXT_LOGE("Failed to find font name %{pubic}s", fontName.c_str());
        return nullptr;
    }
    std::string path = SYSTEM_FONT_PATH + (*fontFileMap)[fontName];
        auto typeface = Drawing::Typeface::MakeFromFile(path.c_str());
    if (typeface == nullptr) {
        path = SYS_PROD_FONT_PATH + (*fontFileMap)[fontName];
        typeface = Drawing::Typeface::MakeFromFile(path.c_str());
        if (typeface == nullptr) {
            TEXT_LOGE("Failed to parse %{public}s", path.c_str());
            return nullptr;
        }
    }

    FontParser::FontDescriptor fontDescriptor;
    fontDescriptor.requestedLid = languageId;
    fontDescriptor.path = path;

    fontDescriptor.requestedFullname = fontName;
    auto fontStyle = typeface->GetFontStyle();
    fontDescriptor.weight = fontStyle.GetWeight();
    fontDescriptor.width = fontStyle.GetWidth();

    if (!ParseTable(typeface, fontDescriptor)) {
        TEXT_LOGE("Failed to parse table");
        return nullptr;
    }
    if (fontDescriptor.fullName == fontName) {
        return std::make_unique<FontDescriptor>(fontDescriptor);
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
