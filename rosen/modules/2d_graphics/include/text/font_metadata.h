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

#ifndef FONT_METADATA_H
#define FONT_METADATA_H

#include <string>
#include <unordered_map>

#include <hb-ot.h>

#include "typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class OtNameId : uint8_t {
    FONT_FAMILY,
    FONT_SUBFAMILY,
    FULL_NAME,
    POSTSCRIPT_NAME,
    VERSION_STRING,
    MANUFACTURER,
    COPYRIGHT,
    TRADEMARK,
    LICENSE,
    COUNT
};

struct FontLegalInfo {
    std::string version = "";
    std::string manufacturer = "";
    std::string copyright = "";
    std::string trademark = "";
    std::string license = "";
};

struct FontIdentification {
    std::string fontFamily = "";
    std::string fontSubFamily = "";
    std::string fullName = "";
    std::string postScriptName = "";
};

class OtNameIdMapper {
public:
    static const std::unordered_map<OtNameId, hb_ot_name_id_t>& GetMapping();

    static hb_ot_name_id_t ToHarfBuzzNameId(OtNameId nameId)
    {
        const auto& mapping = GetMapping();
        const auto iterator = mapping.find(nameId);
        if (iterator != mapping.end()) {
            return iterator->second;
        }
        return HB_OT_NAME_ID_INVALID;
    }
};

class FontMetaDataCollector {
public:
    /**
     * @brief            Returns font legal information for given typeface.
     * @param typeface   given typeface.
     * @param languages  preferred languages.
     * @return           map between language and font legal information for given typeface.
     */
    static std::unordered_map<std::string, FontLegalInfo> GenerateFontLegalInfo(
        const std::shared_ptr<Typeface>& typeface, const std::vector<std::string>& languages);

    /**
     * @brief            Returns font name information for given typeface with assigned language.
     * @param typeface   given typeface.
     * @param languages  preferred languages.
     * @return           map between language and font name information for given typeface.
     */
    static std::unordered_map<std::string, FontIdentification> GenerateFontIdentification(
        const std::shared_ptr<Typeface>& typeface, const std::vector<std::string>& languages);
};
} // Drawing
} // Rosen
} // OHOS
#endif // FONT_METADATA_H