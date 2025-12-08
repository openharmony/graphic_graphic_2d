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

#include <cstring>

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
    HBBlob hbBlob;
    hbBlob.reset(hb_blob_create_from_file(path.c_str()));
    return DetectFormatWithFileCount(hbBlob, fileCount);
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
    hbBlob.reset(hb_blob_create(reinterpret_cast<const char*>(data.data()),
                                dataLength,
                                HB_MEMORY_MODE_READONLY,
                                nullptr,
                                nullptr));
    return DetectFormatWithFileCount(hbBlob, fileCount);
}
} // Drawing
} // Rosen
} // OHOS