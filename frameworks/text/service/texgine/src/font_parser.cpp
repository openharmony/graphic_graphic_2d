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
#ifdef ENABLE_OHOS_ENHANCE
#include <sys/mman.h>
#include <sys/stat.h>
#endif
#include <unordered_map>
#ifdef BUILD_NON_SDK_VER
#include <iconv.h>
#ifdef CROSS_PLATFORM
#include <unicode/ucnv.h>
#endif
#endif

#include "font_config.h"
#ifdef ENABLE_OHOS_ENHANCE
#include "locale_config.h"
#endif
#include "text/font_filetype.h"
#include "text/font_metadata.h"
#include "text/font_unicode_query.h"
#include "text/font_variation_info.h"
#include "utils/text_log.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define FONT_CONFIG_FILE  "/system/fonts/visibility_list.json"
#define FONT_CONFIG_PROD_FILE "/sys_prod/fonts/visibility_list.json"
#define SYSTEM_FONT_PATH "/system/fonts/"
#define SYS_PROD_FONT_PATH "/sys_prod/fonts/"

namespace {
std::unordered_map<std::string, std::vector<std::string>> g_localeToBcpTable = {
    {"my", {"my", "my-MM"}},
    {"zh-Hant", {"zh-Hant", "zh-TW", "zh-HK", "zh-MO", "zh-SG"}},
    {"pt-PT", {"pt-PT", "pt"}},
    {"zh-Hans", {"zh-Hans", "zh", "zh-CN"}},
    {"it", {"it", "it-IT", "it-CH"}},
    {"fi", {"fi", "fi-FI"}},
    {"ja", {"ja", "ja-JP"}},
    {"mk", {"mk", "mk-MK"}},
    {"el", {"el", "el-GR"}},
    {"bg", {"bg", "bg-BG"}},
    {"uk", {"uk", "uk-UA"}},
    {"sv", {"sv", "sv-SE"}},
    {"th", {"th", "th-TH"}},
    {"nb", {"nb", "nb-NO"}},
    {"da", {"da", "da-DK"}},
    {"cs", {"cs", "cs-CZ"}},
    {"fr", {"fr", "fr-FR"}},
    {"ru", {"ru", "ru-RU", "ru-MD"}},
    {"et", {"et", "et-EE"}},
    {"en-Latn-US", {"en-US", "en"}},
    {"hu", {"hu", "hu-HU"}},
    {"es", {"es", "es-ES", "es-US"}},
    {"sk", {"sk", "sk-SK"}},
    {"pl", {"pl", "pl-PL"}},
    {"sr-Latn", {"sr-Latn"}},
    {"sl", {"sl", "sl-SI"}},
    {"ro", {"ro", "ro-RO"}},
    {"lv", {"lv", "lv-LV"}},
    {"lt", {"lt", "lt-LT"}},
    {"hr", {"hr", "hr-HR", "hr-BA"}},
    {"be", {"be", "be-BY"}},
    {"ar", {"ar"}},
    {"he", {"he", "he-IL"}},
    {"km", {"km", "km-KH"}},
    {"tr", {"tr", "tr-TR"}},
    {"vi", {"vi", "vi-VN"}},
    {"id", {"id", "id-ID"}},
    {"ms", {"ms", "ms-MY", "ms-BN"}},
    {"pt-BR", {"pt-BR", "pt"}},
    {"hi", {"hi", "hi-IN"}},
    {"nl", {"nl", "nl-NL", "nl-BE"}},
    {"de", {"de", "de-DE", "de-CH"}},
    {"ka", {"ka", "ka-GE"}},
    {"es-US", {"es-US", "es"}},
    {"ko", {"ko", "ko-KR"}},
    {"fil", {"fil", "fil-PH"}},
    {"fa", {"fa", "fa-IR"}},
    {"uz-Latn", {"uz-Latn", "uz-Latn-UZ"}},
    {"bn", {"bn", "bn-BD", "bn-IN"}},
    {"kk", {"kk", "kk-KZ"}},
};
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

void FontParser::ProcessTable(const CmapTables* cmapTable, FontParser::FontDescriptor& fontDescriptor, size_t size)
{
    auto count = cmapTable->numTables.Get();
    for (size_t i = 0; i < count && (sizeof(CmapTables) + (i + 1) * sizeof(EncodingRecord)) <= size; ++i) {
        const auto& record = cmapTable->encodingRecords[i];
        FontParser::PlatformId platformId = static_cast<FontParser::PlatformId>(record.platformID.Get());
        FontParser::EncodingIdWin encodingId = static_cast<FontParser::EncodingIdWin>(record.encodingID.Get());
        if (platformId == FontParser::PlatformId::WINDOWS && encodingId == FontParser::EncodingIdWin::SYMBOL) {
            fontDescriptor.symbolic = true;
            break;
        }
    }
}

bool FontParser::CheckFullNameParamInvalid(FontParser::FontDescriptor& fontDescriptor, unsigned int languageId,
    const std::string& nameString)
{
    if (!fontDescriptor.requestedFullname.empty() &&
        fontDescriptor.fullName == fontDescriptor.requestedFullname) {
        return true;
    }
    if (nameString.empty()) {
        return true;
    }
    if (fontDescriptor.requestedLid != LANGUAGE_DEFAULT) {
        if (fontDescriptor.fullName.empty()) {
            return false;
        }

        if (fontDescriptor.requestedLid == fontDescriptor.fullNameLid) {
            return true;
        }

        if (fontDescriptor.requestedLid != languageId) {
            return true;
        }
    }
    return false;
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
            if (CheckFullNameParamInvalid(fontDescriptor, languageId, nameString)) {
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

void FontParser::ProcessTable(const NameTable* nameTable, FontParser::FontDescriptor& fontDescriptor, size_t size)
{
    auto count = nameTable->count.Get();
    auto storageOffset = nameTable->storageOffset.Get();
    const char* stringStorage = reinterpret_cast<const char*>(nameTable) + storageOffset;
    for (size_t i = 0; i < count && (sizeof(NameTable) + (i + 1) * sizeof(NameRecord)) <= size; ++i) {
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
        if (storageOffset + stringOffset + len > size) {
            TEXT_LOGE("Invalid name table string offset or length");
            continue;
        }
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

void FontParser::ProcessTable(const PostTable* postTable, FontParser::FontDescriptor& fontDescriptor, size_t /* size */)
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
    ProcessTable(reinterpret_cast<T*>(tableData.get()), fontDescriptor, size);
    return true;
}

using TableTypes = std::tuple<CmapTables, NameTable, PostTable>;

template<typename Tuple, size_t... Is>
bool FontParser::ParseAllTables(std::shared_ptr<Drawing::Typeface> typeface, FontParser::FontDescriptor& fontDescriptor,
    std::index_sequence<Is...>)
{
    return (ParseOneTable<std::tuple_element_t<Is, Tuple>>(typeface, fontDescriptor) && ...);
}

bool FontParser::ParseTable(
    std::shared_ptr<Drawing::Typeface> typeface, FontParser::FontDescriptor& fontDescriptor)
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
#ifdef CROSS_PLATFORM
bool CalculateUcharLength(const std::string& src, const std::string& srcType,
    const std::string& targetType, int32_t& ucharLength)
{
    ucharLength = 0;
    bool ret = false;
    if (targetType.find("UTF-8") != std::string::npos && srcType.find("UTF-16BE") != std::string::npos) {
        const size_t utf16BytesPerCodeUnit = 2;
        ret = (src.length() % utf16BytesPerCodeUnit == 0);
        if (ret) {
            ucharLength = src.length() / utf16BytesPerCodeUnit;
        }
    } else if (targetType.find("UTF-8") != std::string::npos && srcType.find("GB2312") != std::string::npos) {
        ucharLength = src.length();
        ret = true;
    }
    return ret;
}

std::string ConvertStringUseIcu(const std::string& src, const std::string& srcType,
    const std::string& targetType)
{
    if (src.empty()) {
        return src;
    }
    
    int32_t ucharCapacity = 0;
    if (!CalculateUcharLength(src, srcType, targetType, ucharCapacity)) {
        return src;
    }

    UErrorCode status = U_ZERO_ERROR;
    UConverter* conv = ucnv_open(srcType.c_str(), &status);
    if (U_FAILURE(status) || !conv) {
        return src;
    }

    std::vector<UChar> ucharBuf(ucharCapacity);
    int32_t ucharCount = ucnv_toUChars(conv, ucharBuf.data(), ucharCapacity, src.data(), src.length(), &status);
    ucnv_close(conv);

    if (U_FAILURE(status)) {
        return src;
    }

    status = U_ZERO_ERROR;
    conv = ucnv_open(targetType.c_str(), &status);
    if (U_FAILURE(status) || !conv) {
        return src;
    }

    const int32_t maxUtf8BytesPerUchar = 4;
    int32_t utf8Len = ucharCount * maxUtf8BytesPerUchar;
    std::vector<char> utf8Buf(utf8Len);
    int32_t actualUtf8Len = ucnv_fromUChars(conv, utf8Buf.data(), utf8Len, ucharBuf.data(), ucharCount, &status);
    ucnv_close(conv);

    if (U_FAILURE(status)) {
        return src;
    }
    return std::string(utf8Buf.data(), actualUtf8Len);
}
#endif

std::string FontParser::ConvertToString(const std::string& src, const std::string& srcType,
    const std::string& targetType)
{
#ifdef CROSS_PLATFORM
    return ConvertStringUseIcu(src, srcType, targetType);
#else
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
#endif
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

std::shared_ptr<FontParser::FontDescriptor> FontParser::ParserFontDescriptorFromPath(
    const std::string& path, size_t index, const std::string& locale) const
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile(path.c_str(), index);
    return CreateFontDescriptor(typeface, GetLanguageId(locale));
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

std::vector<std::shared_ptr<FontParser::FontDescriptor>> FontParser::ParserFontDescriptorsFromStream(
    const void* data, size_t byteLength, const std::string& locale)
{
    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    int index = 0;
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;
    while ((typeface = Drawing::Typeface::MakeFromStream(
        std::make_unique<Drawing::MemoryStream>(data, byteLength), index)) != nullptr) {
        typefaces.push_back(typeface);
        index++;
    }
    return CreateFontDescriptors(typefaces, locale);
}

std::vector<uint32_t> FontParser::GetFontTypefaceUnicode(const std::string& path, int32_t index)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile(path.c_str(), index);
    return Drawing::FontUnicodeQuery::GenerateUnicodeItem(typeface);
}

std::vector<uint32_t> FontParser::GetFontTypefaceUnicode(const void* data, size_t length, int32_t index)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromStream(
        std::make_unique<Drawing::MemoryStream>(data, length), index);
    return Drawing::FontUnicodeQuery::GenerateUnicodeItem(typeface);
}

#ifdef ENABLE_OHOS_ENHANCE
std::vector<uint8_t> FontParser::GetFontDataFromFd(int fd)
{
    struct stat st{};
    if (fstat(fd, &st) < 0 || st.st_size <= 0) {
        TEXT_LOGE("Failed to get fd size");
        return {};
    }

    void* startAddr = ::mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (startAddr == MAP_FAILED) {
        TEXT_LOGE("Failed to exec mmap");
        return {};
    }
    uint8_t* byteData = static_cast<uint8_t*>(startAddr);
    std::vector<uint8_t> fontData(byteData, byteData + st.st_size);
    ::munmap(startAddr, st.st_size);
    return fontData;
}
#endif

std::vector<std::string> FontParser::GetFontFullName(int fd)
{
#ifdef ENABLE_OHOS_ENHANCE
    std::vector<uint8_t> fontData = GetFontDataFromFd(fd);
#else
    std::vector<uint8_t> fontData = {};
#endif
    int32_t fileCount = 0;
    Drawing::FontFileType::GetFontFileType(fontData, fileCount);
    if (fileCount == 0) {
        TEXT_LOGE("Failed to get font count");
        return {};
    }
    std::vector<std::string> result;
    for (int32_t index = 0; index < fileCount; index++) {
        auto stream = std::make_unique<Drawing::MemoryStream>(fontData.data(), fontData.size(), false);
        std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromStream(std::move(stream), index);
        if (typeface == nullptr) {
            TEXT_LOGE("Failed to make typeface");
            return {};
        }
        FontDescriptor desc;
        desc.requestedLid = LANGUAGE_EN;
        if (!ParseOneTable<NameTable>(typeface, desc)) {
            TEXT_LOGE("Failed to parse name table");
            return {};
        }
        result.emplace_back(desc.fullName);
    }
    return result;
}

std::vector<std::string> FontParser::GetBcpTagList()
{
#ifdef ENABLE_OHOS_ENHANCE
    std::string systemLanguage = Global::I18n::LocaleConfig::GetSystemLanguage();
#else
    std::string systemLanguage = "";
#endif
    auto it = g_localeToBcpTable.find(systemLanguage);
    if (it == g_localeToBcpTable.end()) {
        return {};
    }
    return it->second;
}

void FontParser::FillFontDescriptorWithFallback(std::shared_ptr<Drawing::Typeface> typeface, FontDescriptor& desc)
{
    struct FieldMapping {
        std::string* targetField;
        Drawing::OtNameId nameId;
    };

    const FieldMapping fields[] = {
        {&desc.localFamilyName, Drawing::OtNameId::FONT_FAMILY},
        {&desc.localSubFamilyName, Drawing::OtNameId::FONT_SUBFAMILY},
        {&desc.localFullName, Drawing::OtNameId::FULL_NAME},
        {&desc.localPostscriptName, Drawing::OtNameId::POSTSCRIPT_NAME},
        {&desc.version, Drawing::OtNameId::VERSION_STRING},
        {&desc.manufacture, Drawing::OtNameId::MANUFACTURER},
        {&desc.copyright, Drawing::OtNameId::COPYRIGHT},
        {&desc.trademark, Drawing::OtNameId::TRADEMARK},
        {&desc.license, Drawing::OtNameId::LICENSE},
    };

    for (const auto& field : fields) {
        if (field.targetField->empty()) {
            *field.targetField = Drawing::FontMetaDataCollector::GetFirstAvailableString(typeface, field.nameId);
        }
    }
}

namespace {
std::string ExtractEnglishName(const std::unordered_map<std::string, std::string>& nameMap)
{
    auto enIt = nameMap.find("en");
    if (enIt != nameMap.end() && !enIt->second.empty()) {
        return enIt->second;
    }
    if (!nameMap.empty()) {
        return nameMap.begin()->second;
    }
    return "";
}

std::string ExtractLocalName(const std::unordered_map<std::string, std::string>& nameMap,
    const std::vector<std::string>& bcpTagList)
{
    for (const auto& lang : bcpTagList) {
        auto it = nameMap.find(lang);
        if (it != nameMap.end() && !it->second.empty()) {
            return it->second;
        }
    }
    return "";
}
}

void FontParser::FillFontDescriptorWithVariationInfo(std::shared_ptr<Drawing::Typeface> typeface,
    FontDescriptor& desc, const std::vector<std::string>& bcpTagList)
{
    auto axisInfoList = Drawing::FontVariationInfo::GenerateFontVariationAxisInfo(typeface, bcpTagList);

    for (const auto& axisInfo : axisInfoList) {
        FontVariationAxis axis;
        axis.key = axisInfo.axisTag;
        axis.minValue = axisInfo.minValue;
        axis.maxValue = axisInfo.maxValue;
        axis.defaultValue = axisInfo.defaultValue;
        axis.flags = axisInfo.isHidden ? 1 : 0;
        axis.name = ExtractEnglishName(axisInfo.axisTagMapForLanguage);
        axis.localName = ExtractLocalName(axisInfo.axisTagMapForLanguage, bcpTagList);
        desc.variationAxisRecords.push_back(axis);
    }

    auto instanceInfoList = Drawing::FontVariationInfo::GenerateFontVariationInstanceInfo(typeface, bcpTagList);

    for (const auto& instanceInfo : instanceInfoList) {
        FontVariationInstance instance;
        instance.name = ExtractEnglishName(instanceInfo.subfamilyNameMapForLanguage);
        instance.localName = ExtractLocalName(instanceInfo.subfamilyNameMapForLanguage, bcpTagList);
        for (const auto& [axis, value] : instanceInfo.coordinates) {
            FontVariation coord;
            coord.axis = axis;
            coord.value = value;
            instance.coordinates.push_back(coord);
        }
        desc.variationInstanceRecords.push_back(instance);
    }
}

void FontParser::FillFontDescriptorWithLocalInfo(std::shared_ptr<Drawing::Typeface> typeface, FontDescriptor& desc)
{
    std::vector<std::string> bcpTagList = GetBcpTagList();
    bcpTagList.emplace_back("en");

    auto fontIdentificationMap = Drawing::FontMetaDataCollector::GenerateFontIdentification(typeface, bcpTagList);
    for (const auto& tag : bcpTagList) {
        auto it = fontIdentificationMap.find(tag);
        if (it == fontIdentificationMap.end()) {
            continue;
        }
        Drawing::FontIdentification& info = it->second;
        desc.localFamilyName = desc.localFamilyName.empty() ? info.fontFamily : desc.localFamilyName;
        desc.localSubFamilyName = desc.localSubFamilyName.empty() ? info.fontSubFamily : desc.localSubFamilyName;
        desc.localFullName = desc.localFullName.empty() ? info.fullName : desc.localFullName;
        desc.localPostscriptName = desc.localPostscriptName.empty() ? info.postScriptName : desc.localPostscriptName;
    }

    auto fontLegalInfoMap = Drawing::FontMetaDataCollector::GenerateFontLegalInfo(typeface, bcpTagList);
    for (const auto& tag : bcpTagList) {
        auto it = fontLegalInfoMap.find(tag);
        if (it == fontLegalInfoMap.end()) {
            continue;
        }
        Drawing::FontLegalInfo& info = it->second;
        desc.version = desc.version.empty() ? info.version : desc.version;
        desc.manufacture = desc.manufacture.empty() ? info.manufacturer : desc.manufacture;
        desc.copyright = desc.copyright.empty() ? info.copyright : desc.copyright;
        desc.trademark = desc.trademark.empty() ? info.trademark : desc.trademark;
        desc.license = desc.license.empty() ? info.license : desc.license;
    }

    FillFontDescriptorWithVariationInfo(typeface, desc, bcpTagList);
    FillFontDescriptorWithFallback(typeface, desc);
}

int32_t FontParser::GetFontCount(const std::string& path)
{
    int32_t fileCount = 0;
    Drawing::FontFileType::GetFontFileType(path, fileCount);
    return fileCount;
}

int32_t FontParser::GetFontCount(const std::vector<uint8_t>& data)
{
    int32_t fileCount = 0;
    Drawing::FontFileType::GetFontFileType(data, fileCount);
    return fileCount;
}

std::shared_ptr<FontParser::FontDescriptor> FontParser::CreateFontDescriptor(
    const std::shared_ptr<Drawing::Typeface>& typeface, unsigned int languageId)
{
    if (typeface == nullptr) {
        TEXT_LOGE("typeface is nullptr");
        return nullptr;
    }
    FontDescriptor desc;
    desc.requestedLid = languageId;
    desc.path = typeface->GetFontPath();
    desc.index = typeface->GetFontIndex();
    auto fontStyle = typeface->GetFontStyle();
    desc.weight = fontStyle.GetWeight();
    desc.width = fontStyle.GetWidth();
    if (!ParseTable(typeface, desc)) {
        TEXT_LOGE("ParseTable failed for path=%{public}s", desc.path.c_str());
        return nullptr;
    }

    FillFontDescriptorWithLocalInfo(typeface, desc);

    return std::make_shared<FontParser::FontDescriptor>(desc);
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
        std::shared_ptr<FontParser::FontDescriptor> desc = CreateFontDescriptor(item, languageId);
        if (desc == nullptr) {
            continue;
        }
        descriptors.push_back(std::move(desc));
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
        TEXT_LOGE_LIMIT3_MIN("Empty font file map");
        return nullptr;
    }
    if ((*fontFileMap).find(fontName) == (*fontFileMap).end()) {
        TEXT_LOGE_LIMIT3_MIN("Failed to find font name %{public}s", fontName.c_str());
        return nullptr;
    }
    std::string path = SYSTEM_FONT_PATH + (*fontFileMap)[fontName];
        auto typeface = Drawing::Typeface::MakeFromFile(path.c_str());
    if (typeface == nullptr) {
        path = SYS_PROD_FONT_PATH + (*fontFileMap)[fontName];
        typeface = Drawing::Typeface::MakeFromFile(path.c_str());
        if (typeface == nullptr) {
            TEXT_LOGE_LIMIT3_MIN("Failed to parse %{public}s", path.c_str());
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
        TEXT_LOGE_LIMIT3_MIN("Failed to parse table");
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

const std::vector<std::string>& FontParser::GetFontSet() const
{
    return fontSet_;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
