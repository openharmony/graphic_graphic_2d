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

#include "text/font_metadata.h"

#include <vector>
#include "font_harfbuzz.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
#ifdef USE_M133_SKIA
using HBFace = std::unique_ptr<hb_face_t, SkFunctionObject<hb_face_destroy>>;
#else
template <typename T, typename P, P* p> using resource = std::unique_ptr<T, SkFunctionWrapper<P, p>>;
using HBFace = resource<hb_face_t, decltype(hb_face_destroy), hb_face_destroy>;
#endif

static constexpr uint8_t FONT_INFO_LENGTH_PROTECTION = 1;

const std::unordered_map<OtNameId, hb_ot_name_id_t>& OtNameIdMapper::GetMapping()
{
    static const std::unordered_map<OtNameId, hb_ot_name_id_t> mapping = {
        {OtNameId::FONT_FAMILY,                        HB_OT_NAME_ID_FONT_FAMILY},
        {OtNameId::FONT_SUBFAMILY,                     HB_OT_NAME_ID_FONT_SUBFAMILY},
        {OtNameId::FULL_NAME,                          HB_OT_NAME_ID_FULL_NAME},
        {OtNameId::POSTSCRIPT_NAME,                    HB_OT_NAME_ID_POSTSCRIPT_NAME},
        {OtNameId::VERSION_STRING,                     HB_OT_NAME_ID_VERSION_STRING},
        {OtNameId::MANUFACTURER,                       HB_OT_NAME_ID_MANUFACTURER},
        {OtNameId::COPYRIGHT,                          HB_OT_NAME_ID_COPYRIGHT},
        {OtNameId::TRADEMARK,                          HB_OT_NAME_ID_TRADEMARK},
        {OtNameId::LICENSE,                            HB_OT_NAME_ID_LICENSE},
    };
    return mapping;
}

inline hb_language_t Bcp47StringToLanguageTag(const std::string& language)
{
    return hb_language_from_string(language.c_str(), -1);
}

void GetSingleFontNameEntry(HBFace& face, OtNameId nameId, std::string& result, const std::string& language)
{
    const hb_ot_name_id_t hbNameId = OtNameIdMapper::ToHarfBuzzNameId(nameId);
    if (hbNameId == HB_OT_NAME_ID_INVALID) {
        return;
    }
#ifdef CURRENT_OS_MAC
    return;
#endif
    hb_language_t languageTag = Bcp47StringToLanguageTag(language);
    unsigned int predictLen = hb_ot_name_get_utf8(face.get(), hbNameId, languageTag, nullptr, nullptr);
    predictLen += FONT_INFO_LENGTH_PROTECTION;
    std::vector<char> buffer(predictLen);
    unsigned int actualLen = hb_ot_name_get_utf8(face.get(), hbNameId, languageTag, &predictLen, buffer.data());
    result = std::string(buffer.data(), actualLen);
}

std::unordered_map<std::string, FontLegalInfo> FontMetaDataCollector::GenerateFontLegalInfo(
    const std::shared_ptr<Typeface>& typeface,
    const std::vector<std::string>& languages)
{
    if (!typeface) {
        LOGD("Drawing_Text [GenerateFontLegalInfo] typeface is nullptr!");
        return {};
    }
    HBFace hbFace = FontHarfbuzz::CreateHbFace(*typeface);
    if (!hbFace) {
        LOGE("Drawing_Text [GenerateFontLegalInfo] Creating hbFace false!");
        return {};
    }
    std::unordered_map<std::string, FontLegalInfo> map;
    for (const auto& lang : languages) {
        FontLegalInfo info;
        GetSingleFontNameEntry(hbFace, OtNameId::VERSION_STRING, info.version, lang);
        GetSingleFontNameEntry(hbFace, OtNameId::MANUFACTURER, info.manufacturer, lang);
        GetSingleFontNameEntry(hbFace, OtNameId::COPYRIGHT, info.copyright, lang);
        GetSingleFontNameEntry(hbFace, OtNameId::TRADEMARK, info.trademark, lang);
        GetSingleFontNameEntry(hbFace, OtNameId::LICENSE, info.license, lang);
        if (info.version.empty() &&
            info.manufacturer.empty() &&
            info.copyright.empty() &&
            info.trademark.empty() &&
            info.license.empty()) {
            continue;
        }
        map[lang] = info;
    }
    return map;
}

std::unordered_map<std::string, FontIdentification> FontMetaDataCollector::GenerateFontIdentification(
    const std::shared_ptr<Typeface>& typeface,
    const std::vector<std::string>& languages)
{
    if (!typeface) {
        LOGD("Drawing_Text [GenerateFontIdentification] typeface is nullptr!");
        return {};
    }
    HBFace hbFace = FontHarfbuzz::CreateHbFace(*typeface);
    if (!hbFace) {
        LOGE("Drawing_Text [GenerateFontIdentification] Creating hbFace false!");
        return {};
    }
    std::unordered_map<std::string, FontIdentification> map;
    for (const auto& lang : languages) {
        FontIdentification info;
        GetSingleFontNameEntry(hbFace, OtNameId::FONT_FAMILY, info.fontFamily, lang);
        GetSingleFontNameEntry(hbFace, OtNameId::FONT_SUBFAMILY, info.fontSubFamily, lang);
        GetSingleFontNameEntry(hbFace, OtNameId::FULL_NAME, info.fullName, lang);
        GetSingleFontNameEntry(hbFace, OtNameId::POSTSCRIPT_NAME, info.postScriptName, lang);
        if (info.fontFamily.empty() &&
            info.fontSubFamily.empty() &&
            info.fullName.empty() &&
            info.postScriptName.empty()) {
            continue;
        }
        map[lang] = info;
    }
    return map;
}

std::string FontMetaDataCollector::GetFirstAvailableString(const std::shared_ptr<Typeface>& typeface,
    Drawing::OtNameId nameId)
{
    const hb_ot_name_id_t hbNameId = OtNameIdMapper::ToHarfBuzzNameId(nameId);
    if (typeface == nullptr) {
        LOGD("Drawing_Text [GetFirstAvailableString] typeface is nullptr!");
        return "";
    }
#ifdef CURRENT_OS_MAC
    return "";
#endif
    unsigned int count = 0;
    HBFace hbFace = FontHarfbuzz::CreateHbFace(*typeface);
    const hb_ot_name_entry_t* entries = hb_ot_name_list_names(hbFace.get(), &count);
    for (unsigned int i = 0; i < count; i++) {
        if (entries[i].name_id == hbNameId) {
            return ExtractString(hbFace.get(), entries[i]);
        }
    }
    return "";
}

std::string FontMetaDataCollector::ExtractString(hb_face_t* face, const hb_ot_name_entry_t& entry)
{
#ifdef CURRENT_OS_MAC
    return "";
#endif
    char buffer[BUF_SIZE];
    unsigned int size = BUF_SIZE;
    unsigned int len = hb_ot_name_get_utf8(face, entry.name_id, entry.language, &size, buffer);
    return (len > 0) ? std::string(buffer) : "";
}
} // Drawing
} // Rosen
} // OHOS