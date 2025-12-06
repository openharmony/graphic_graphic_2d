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


#include "text/font_variation_info.h"

#include "font_harfbuzz.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr uint8_t TAG_LEN = 5;
static constexpr uint8_t TAG_CHAR_1ST = 0;
static constexpr uint8_t TAG_CHAR_2ND = 1;
static constexpr uint8_t TAG_CHAR_3RD = 2;
static constexpr uint8_t TAG_CHAR_4TH = 3;
static constexpr uint8_t TAG_CHAR_END = 4;
static constexpr uint8_t PACK_SHIFT_1ST = 8;
static constexpr uint8_t PACK_SHIFT_2ND = 16;
static constexpr uint8_t PACK_SHIFT_3RD = 24;
static constexpr uint8_t MASK_BYTE = 0xFF;

std::vector<FontAxisInfo> FontVariationInfo::GenerateFontVariationAxisInfo(const std::shared_ptr<Typeface>& typeface,
                                                                           const std::vector<std::string>& languages)
{
    if (!typeface) {
        LOGE("Drawing_Text [GenerateFontVariationAxisInfo] typeface is nullptr!");
        return {};
    }
    FontVariationInfo fontVarHandler;
    fontVarHandler.LoadFromTypeface(typeface, languages, false);
    return fontVarHandler.mAxisInfo;
}

std::vector<FontInstanceInfo> FontVariationInfo::GenerateFontVariationInstanceInfo(
    const std::shared_ptr<Typeface>& typeface,
    const std::vector<std::string>& languages)
{
    if (!typeface) {
        LOGE("Drawing_Text [GenerateFontVariationInstanceInfo] typeface is nullptr!");
        return {};
    }
    FontVariationInfo fontVarHandler;
    fontVarHandler.LoadFromTypeface(typeface, languages, true);
    return fontVarHandler.mInstanceInfo;
}

bool FontVariationInfo::LoadFromTypeface(const std::shared_ptr<Typeface>& typeface,
                                         const std::vector<std::string>& languages,
                                         bool needInstance)
{
    if (!InitializedFace(typeface)) {
        LOGE("Drawing_Text [LoadFromTypeface] typeface is invalid!");
        return false;
    }
    if (!IsVariableFont()) {
        LOGE("Drawing_Text [LoadFromTypeface] not variable font!");
        return false;
    }
    ExtractAxisInfo(languages);
    if (needInstance) { // only when need instance info
        ExtractInstanceInfo(languages);
    }
    return true;
}

bool FontVariationInfo::InitializedFace(const std::shared_ptr<Typeface>& typeface)
{
    if (!typeface) {
        LOGE("Drawing_Text [InitializedFace] typeface is nullptr!");
        return false;
    }
    mFace = FontHarfbuzz::CreateHbFace(*typeface);
    return true;
}

void FontVariationInfo::Cleanup()
{
    mFace = nullptr;
    mAxisInfo.clear();
    mInstanceInfo.clear();
}

void FontVariationInfo::ExtractAxisInfo(const std::vector<std::string>& languages)
{
    unsigned int axisCount = hb_ot_var_get_axis_count(mFace.get());
    if (axisCount == 0) {
        LOGE("Drawing_Text [ExtractAxisInfo] axisCount is 0!");
        return;
    }

    std::vector<hb_ot_var_axis_info_t> axes(axisCount);
    hb_ot_var_get_axis_infos(mFace.get(), 0, &axisCount, axes.data());
    for (unsigned int i = 0; i < axisCount; ++i) {
        FontAxisInfo axis;
        char tagStr[TAG_LEN];
        tagStr[TAG_CHAR_1ST] = static_cast<unsigned char>((axes[i].tag >> PACK_SHIFT_3RD) & MASK_BYTE);
        tagStr[TAG_CHAR_2ND] = static_cast<unsigned char>((axes[i].tag >> PACK_SHIFT_2ND) & MASK_BYTE);
        tagStr[TAG_CHAR_3RD] = static_cast<unsigned char>((axes[i].tag >> PACK_SHIFT_1ST) & MASK_BYTE);
        tagStr[TAG_CHAR_4TH] = static_cast<unsigned char>((axes[i].tag) & MASK_BYTE);
        tagStr[TAG_CHAR_END] = '\0'; // ending flag

        axis.axisTag = tagStr;
        axis.nameId = axes[i].name_id;
        axis.minValue = axes[i].min_value;
        axis.defaultValue = axes[i].default_value;
        axis.maxValue = axes[i].max_value;
        axis.isHidden = (axes[i].flags & HB_OT_VAR_AXIS_FLAG_HIDDEN) != 0;
        std::string axisTagForLanguage = "";
        for (auto& language : languages) {
            GetLocalName(axis.nameId, axisTagForLanguage, language);
            if (axisTagForLanguage.empty()) {
                continue;
            }
            axis.axisTagMapForLanguage[language] = axisTagForLanguage;
        }
        mAxisInfo.push_back(axis);
    }
}

void FontVariationInfo::GetLocalName(uint16_t nameId,
                                     std::string& result,
                                     const std::string& language)
{
    hb_ot_name_id_t hbNameId = static_cast<hb_ot_name_id_t>(nameId);
    if (hbNameId == HB_OT_NAME_ID_INVALID) {
        return;
    }
#ifdef CURRENT_OS_MAC
    return;
#endif
    hb_language_t languageTag = hb_language_from_string(language.c_str(), -1);
    unsigned int predictLen = hb_ot_name_get_utf8(mFace.get(),
                                                  hbNameId,
                                                  languageTag,
                                                  nullptr,
                                                  nullptr);
    predictLen += 1;
    std::vector<char> buffer(predictLen);
    unsigned int actualLen = hb_ot_name_get_utf8(mFace.get(),
                                                 hbNameId,
                                                 languageTag,
                                                 &predictLen,
                                                 buffer.data());
    result = std::string(buffer.data(), actualLen);
}

void FontVariationInfo::ExtractInstanceInfo(const std::vector<std::string>& languages)
{
    uint32_t instanceCount = hb_ot_var_get_named_instance_count(mFace.get());
    if (instanceCount == 0) {
        return;
    }
    uint32_t axisCount = hb_ot_var_get_axis_count(mFace.get());
    if (axisCount == 0) {
        return;
    }
    std::vector<float> defaultCoords(axisCount);
    for (uint32_t i = 0; i < axisCount; ++i) {
        defaultCoords[i] = mAxisInfo[i].defaultValue;
    }
    for (uint32_t i = 0; i < instanceCount; ++i) {
        FontInstanceInfo instance;
        std::string subfamilyName = "";
        std::string postScriptName = "";
        for (auto& language : languages) {
            GetLocalName(hb_ot_var_named_instance_get_subfamily_name_id(mFace.get(), i),
                         subfamilyName, language);
            GetLocalName(hb_ot_var_named_instance_get_postscript_name_id(mFace.get(), i),
                         postScriptName, language);
            if (!subfamilyName.empty()) {
                instance.subfamilyNameMapForLanguage[language] = subfamilyName;
            }
            if (!postScriptName.empty()) {
                instance.postScriptNameMapForLanguage[language] = postScriptName;
            }
        }
        std::vector<float> coords(axisCount);
        uint32_t coordsCount = axisCount;
        hb_ot_var_named_instance_get_design_coords(mFace.get(),
                                                   i,
                                                   &coordsCount,
                                                   coords.data());
        uint32_t cyclicCount = std::min(axisCount, coordsCount);
        for (uint32_t j = 0; j < cyclicCount; ++j) {
            const FontAxisInfo& axis = mAxisInfo[j];
            instance.coordinates[axis.axisTag] = coords[j];
        }
        // Check is is default
        instance.isDefault = true;
        for (unsigned int k = 0; k < cyclicCount; ++k) {
            if (coords[k] != defaultCoords[k]) {
                instance.isDefault = false;
                break;
            }
        }
        mInstanceInfo.push_back(instance);
    }
}

bool FontVariationInfo::IsVariableFont() const
{
    return mFace && hb_ot_var_has_data(mFace.get());
}
} // Drawing
} // Rosen
} // OHOS