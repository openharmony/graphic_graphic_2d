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

#include "text/font_filetype.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstddef>
#include <fstream>
#include <memory>
#include <string>

#include "font_harfbuzz.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
#ifdef USE_M133_SKIA
using HBFace = std::unique_ptr<hb_face_t, SkFunctionObject<hb_face_destroy>>;
using HBBlob = std::unique_ptr<hb_blob_t, SkFunctionObject<hb_blob_destroy>>;
#else
template <typename T, typename P, P* p> using resource = std::unique_ptr<T, SkFunctionWrapper<P, p>>;
using HBFace = resource<hb_face_t, decltype(hb_face_destroy), hb_face_destroy>;
using HBBlob = resource<hb_blob_t, decltype(hb_blob_destroy), hb_blob_destroy>;
#endif

static constexpr uint8_t FONT_FILE_NOT_COLLECTION = 1;
static constexpr uint8_t INVALID_FONT_FILE_NUM = 0;
// File header related constants
static constexpr size_t HEADER_MIN_SIZE = 12;                // Minimum header size for basic detection
static constexpr size_t TTC_HEADER_SIZE = 12;                // Minimum TTC header size
static constexpr size_t TABLE_DIR_ENTRY_SIZE = 16;           // Table directory entry size (16 bytes)
static constexpr size_t SAFE_TABLE_SCAN_BUFFER_SIZE = 512;   // Safe buffer size for table directory scanning
// File format signature constants (big-endian)
static constexpr uint32_t TAG_TTCF = 0x74746366;             // 'ttcf' - TrueType Collection signature
static constexpr uint32_t TAG_OTTO = 0x4F54544F;             // 'OTTO' - OpenType CFF format signature
static constexpr uint32_t TAG_TRUE = 0x74727565;             // 'true' - Legacy Apple TrueType signature
static constexpr uint32_t TAG_TT_1_0 = 0x00010000;           // Standard TrueType version signature (1.0)
static constexpr uint32_t TAG_GLYF = 0x676C7966;             // 'glyf' - TrueType glyph data table tag
static constexpr uint32_t TAG_CFF = 0x43464620;              // 'CFF ' - CFF outline data table tag
static constexpr uint32_t TAG_CFF2 = 0x43464632;             // 'CFF2' - CFF2 outline data table tag
// Offset position constants (byte offsets from file/table start)
static constexpr size_t OFFSET_SFNT_SIGNATURE = 0;           // SFNT format signature offset (0 bytes)
static constexpr size_t OFFSET_TABLE_COUNT = 4;              // Table count field offset (4 bytes)
static constexpr size_t OFFSET_TTC_FONT_COUNT = 8;           // Font count in TTC header offset (8 bytes)
static constexpr size_t OFFSET_TTC_FIRST_FONT_OFFSET = 12;   // First font offset in TTC header (12 bytes)
// Limit constants for validation
static constexpr int MAX_TABLE_COUNT = 100;                  // Maximum reasonable number of tables in a font
static constexpr size_t MAX_READ_SIZE = 0x1000000;
static constexpr size_t MAX_SAFE_OFFSET = 0x7FFFFFFF;
// Assistant constants
static constexpr size_t BYTE0_SHIFT = 24;  // First byte shift for big-endian conversion
static constexpr size_t BYTE1_SHIFT = 16;  // Second byte shift for big-endian conversion
static constexpr size_t BYTE2_SHIFT = 8;   // Third byte shift for big-endian conversion
static constexpr uint8_t SHIFT_4_U32 = 4;

// Read a big-endian 32-bit unsigned integer from byte array
static uint32_t ReadUInt32BE(const uint8_t* data)
{
    return (static_cast<uint32_t>(data[0]) << BYTE0_SHIFT) | // 0 means first char
           (static_cast<uint32_t>(data[1]) << BYTE1_SHIFT) | // 1 means second char
           (static_cast<uint32_t>(data[2]) << BYTE2_SHIFT) | // 2 means third char
           static_cast<uint32_t>(data[3]);                   // 3 means last char
}

// Read a big-endian 16-bit unsigned integer from byte array
static uint16_t ReadUInt16BE(const uint8_t* data)
{
    return (static_cast<uint16_t>(data[0]) << BYTE2_SHIFT) |
           static_cast<uint16_t>(data[1]);
}

static bool ReadFileHeader(const std::string& fileName, uint8_t* buffer, size_t bufferSize)
{
    std::unique_ptr<FILE, decltype(&fclose)> file(fopen(fileName.c_str(), "rb"), fclose);
    if (!file) {
        return false;
    }
    return fread(buffer, 1, bufferSize, file.get()) == bufferSize;
}

static bool ReadFileAtOffset(const std::string& fileName, uint8_t* buffer,
                             size_t bufferSize, size_t offset)
{
    std::unique_ptr<FILE, decltype(&fclose)> file(fopen(fileName.c_str(), "rb"), fclose);
    if (!file) {
        return false;
    }
    if (offset > MAX_SAFE_OFFSET || bufferSize > MAX_READ_SIZE) {
        return false;
    }
    if (fseek(file.get(), static_cast<long>(offset), SEEK_SET) != 0) {
        return false;
    }
    return fread(buffer, 1, bufferSize, file.get()) == bufferSize;
}

FontFileType::FontFileFormat DetectFontTypeQuick(const std::string& fileName)
{
    uint8_t header[HEADER_MIN_SIZE];
    if (!ReadFileHeader(fileName, header, HEADER_MIN_SIZE)) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }

    uint32_t signature = ReadUInt32BE(header + OFFSET_SFNT_SIGNATURE);

    return signature == TAG_TTCF ? FontFileType::FontFileFormat::TTC :
           signature == TAG_TT_1_0 ? FontFileType::FontFileFormat::TTF :
           signature == TAG_TRUE ? FontFileType::FontFileFormat::TTF :
           signature == TAG_OTTO ? FontFileType::FontFileFormat::OTF :
                                   FontFileType::FontFileFormat::UNKNOWN;
}

bool IsCollectionFont(const std::string& fileName, int& outFontCount)
{
    uint8_t header[TTC_HEADER_SIZE];
    if (!ReadFileHeader(fileName, header, TTC_HEADER_SIZE)) {
        outFontCount = 0;
        return false;
    }
    uint32_t signature = ReadUInt32BE(header + OFFSET_SFNT_SIGNATURE);
    if (signature != TAG_TTCF) {
        outFontCount = 0;
        return false;
    }
    outFontCount = static_cast<int>(ReadUInt32BE(header + OFFSET_TTC_FONT_COUNT));
    return true;
}

FontFileType::FontFileFormat DetectOutlineType(const std::string& fileName)
{
    uint8_t minimalHeader[HEADER_MIN_SIZE];
    if (!ReadFileHeader(fileName, minimalHeader, HEADER_MIN_SIZE)) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    uint16_t numTables = ReadUInt16BE(minimalHeader + OFFSET_TABLE_COUNT);
    if (numTables == 0 || numTables > MAX_TABLE_COUNT) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    if (static_cast<size_t>(numTables) > (SIZE_MAX / TABLE_DIR_ENTRY_SIZE)) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    size_t tableDirSize = static_cast<size_t>(numTables) * TABLE_DIR_ENTRY_SIZE;
    if (HEADER_MIN_SIZE > (SIZE_MAX - tableDirSize)) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    size_t neededBufferSize = HEADER_MIN_SIZE + tableDirSize;
    size_t bufferSize = (neededBufferSize > SAFE_TABLE_SCAN_BUFFER_SIZE) ?
                        SAFE_TABLE_SCAN_BUFFER_SIZE : neededBufferSize;
    if (bufferSize < (HEADER_MIN_SIZE + TABLE_DIR_ENTRY_SIZE)) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    auto buffer = std::make_unique<uint8_t[]>(bufferSize);
    if (!ReadFileHeader(fileName, buffer.get(), bufferSize)) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    bool hasGlyfTable = false;
    bool hasCffTable = false;
    size_t tableDirOffset = HEADER_MIN_SIZE;
    size_t maxPossibleTables = (bufferSize - HEADER_MIN_SIZE) / TABLE_DIR_ENTRY_SIZE;
    if (numTables > static_cast<int>(maxPossibleTables)) {
        numTables = static_cast<int>(maxPossibleTables);
    }
    for (int i = 0; i < numTables; i++) {
        if (tableDirOffset + TABLE_DIR_ENTRY_SIZE > bufferSize) {
            break;
        }
        uint32_t tableTag = ReadUInt32BE(buffer.get() + tableDirOffset);
        if (tableTag == TAG_GLYF) {
            hasGlyfTable = true;
        } else if (tableTag == TAG_CFF || tableTag == TAG_CFF2) {
            hasCffTable = true;
        }
        if (hasGlyfTable && hasCffTable) {
            break;
        }
        tableDirOffset += TABLE_DIR_ENTRY_SIZE;
    }
    return (hasGlyfTable && !hasCffTable) ? FontFileType::FontFileFormat::TTF :
           (hasCffTable && !hasGlyfTable) ? FontFileType::FontFileFormat::OTF :
                                            FontFileType::FontFileFormat::UNKNOWN;
}

static bool ReadFirstFontOffset(const std::string& fileName, uint32_t& outOffset)
{
    const size_t neededSize = OFFSET_TTC_FIRST_FONT_OFFSET + SHIFT_4_U32;
    auto header = std::make_unique<uint8_t[]>(neededSize);
    if (!ReadFileHeader(fileName, header.get(), neededSize)) {
        return false;
    }
    outOffset = ReadUInt32BE(header.get() + OFFSET_TTC_FIRST_FONT_OFFSET);
    return true;
}

FontFileType::FontFileFormat DetectCollectionType(const std::string& fileName)
{
    int fontCount = 0;
    if (!IsCollectionFont(fileName, fontCount) || fontCount == 0) {
        fontCount = 0;
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    uint32_t firstFontOffset = 0;
    if (!ReadFirstFontOffset(fileName, firstFontOffset)) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    // Read SFNT signature of first font
    uint8_t signatureBuffer[SHIFT_4_U32];
    if (!ReadFileAtOffset(fileName, signatureBuffer, SHIFT_4_U32, firstFontOffset)) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    uint32_t signature = ReadUInt32BE(signatureBuffer);
    if (signature == TAG_TT_1_0 || signature == TAG_TRUE) {
        return FontFileType::FontFileFormat::TTC;
    } else if (signature == TAG_OTTO) {
        return FontFileType::FontFileFormat::OTC;
    }
    return FontFileType::FontFileFormat::TTC;
}

FontFileType::FontFileFormat DetectFontTypeComplete(const std::string& fileName)
{
    FontFileType::FontFileFormat quickType = DetectFontTypeQuick(fileName);
    if (quickType == FontFileType::FontFileFormat::UNKNOWN) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    if (quickType == FontFileType::FontFileFormat::TTC) {
        return DetectCollectionType(fileName);
    }
    if (quickType == FontFileType::FontFileFormat::TTF ||
        quickType == FontFileType::FontFileFormat::OTF) {
        FontFileType::FontFileFormat outlineType = DetectOutlineType(fileName);
        if (outlineType != FontFileType::FontFileFormat::UNKNOWN) {
            return outlineType;
        }
        return quickType;
    }
    
    return FontFileType::FontFileFormat::UNKNOWN;
}

inline bool HasTable(HBFace& face, hb_tag_t tableTag)
{
    HBBlob blob;
    blob.reset(hb_face_reference_table(face.get(), tableTag));
    if (!blob) { // only for protection
        return false;
    }
    return hb_blob_get_length(blob.get()) > 0;
}

FontFileType::FontFileFormat DetectFormat(HBFace& face)
{
    if (!face) {
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    if (HasTable(face, HB_TAG('C', 'F', 'F', ' ')) ||
        HasTable(face, HB_TAG('C', 'F', 'F', '2'))) {
        return FontFileType::FontFileFormat::OTF;
    }
    if (HasTable(face, HB_TAG('g', 'l', 'y', 'f')) ||
        HasTable(face, HB_TAG('l', 'o', 'c', 'a'))) {
        return FontFileType::FontFileFormat::TTF;
    }
    if (HasTable(face, HB_TAG('c', 'm', 'a', 'p')) &&
        HasTable(face, HB_TAG('h', 'e', 'a', 'd')) &&
        HasTable(face, HB_TAG('h', 'h', 'e', 'a')) &&
        HasTable(face, HB_TAG('h', 'm', 't', 'x'))) {
        return FontFileType::FontFileFormat::TTF;
    }
    return FontFileType::FontFileFormat::UNKNOWN;
}

FontFileType::FontFileFormat DetectFormatWithFileCount(HBBlob& blob, int& fileCount)
{
    if (!blob) { // only for protection
        fileCount = INVALID_FONT_FILE_NUM;
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    fileCount = static_cast<int>(hb_face_count(blob.get()));
    if (fileCount == INVALID_FONT_FILE_NUM) { // never become negative
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    HBFace face;
    face.reset(hb_face_create(blob.get(), 0));
    FontFileType::FontFileFormat firstFaceType = DetectFormat(face);
    if (fileCount == FONT_FILE_NOT_COLLECTION) {
        return firstFaceType;
    }
    if (firstFaceType == FontFileType::FontFileFormat::TTF) {
        return FontFileType::FontFileFormat::TTC;
    }
    if (firstFaceType == FontFileType::FontFileFormat::OTF) {
        return FontFileType::FontFileFormat::OTC;
    }
    return FontFileType::FontFileFormat::UNKNOWN;
}

FontFileType::FontFileFormat FontFileType::GetFontFileType(const std::shared_ptr<Typeface>& typeface)
{
    if (!typeface) {
        LOGD("Drawing_Text [GetFontFileType] typeface is invalid!");
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    HBFace hbFace = FontHarfbuzz::CreateHbFace(*typeface);
    return DetectFormat(hbFace); // input is typeface indicating only 1 face.
}

FontFileType::FontFileFormat FontFileType::GetFontFileType(const std::string& path, int& fileCount)
{
    if (path.empty()) {
        fileCount = INVALID_FONT_FILE_NUM;
        LOGD("Drawing_Text [GetFontFileType] path is empty!");
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    auto outType = FontFileType::FontFileFormat::UNKNOWN;
    // Detect font type
    outType = DetectFontTypeComplete(path);
    // If collection font, get detailed font count
    if (outType == FontFileType::FontFileFormat::TTC ||
        outType == FontFileType::FontFileFormat::OTC) {
        IsCollectionFont(path, fileCount);
    } else if (outType != FontFileType::FontFileFormat::UNKNOWN) {
        fileCount = 1;
    } else {
        fileCount = 0;
    }
    return outType;
}

FontFileType::FontFileFormat FontFileType::GetFontFileType(const std::vector<uint8_t>& data, int& fileCount)
{
    size_t dataLength = data.size();
    if (dataLength == 0) {
        fileCount = INVALID_FONT_FILE_NUM;
        LOGE("Drawing_Text [GetFontFileType] dataLength is invalid!");
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    HBBlob hbBlob;
    hbBlob.reset(hb_blob_create_or_fail(reinterpret_cast<const char*>(data.data()),
                                        dataLength,
                                        HB_MEMORY_MODE_READONLY,
                                        nullptr,
                                        nullptr));
    if (!hbBlob.get()) {
        LOGE("Drawing_Text [GetFontFileType] font file type can't be desolved, Harfbuzz in error!");
        fileCount = INVALID_FONT_FILE_NUM;
        return FontFileType::FontFileFormat::UNKNOWN;
    }
    return DetectFormatWithFileCount(hbBlob, fileCount);
}
} // Drawing
} // Rosen
} // OHOS