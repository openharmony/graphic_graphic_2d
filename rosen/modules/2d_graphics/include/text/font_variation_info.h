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

#ifndef FONT_VARIATION_INFO_H
#define FONT_VARIATION_INFO_H

#include <string>
#include <unordered_map>
#include <vector>

#include <hb.h>
#include <hb-ot.h>
#ifdef USE_M133_SKIA
#include "private/base/SkTemplates.h"
#else
#include "private/SkTemplates.h"
#endif

#include "typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct FontAxisInfo {
    // Map between aimed language and axisTagName.
    std::unordered_map<std::string, std::string> axisTagMapForLanguage;
    std::string axisTag = "";          // Tag identifying the design variation for the axis.
    float minValue = 0;                // The minimum coordinate value for the axis.
    float defaultValue = 0;            // The default coordinate value for the axis.
    float maxValue = 0;                // The maximum coordinate value for the axis.
    uint16_t nameId = 0;               // The name ID for entries in the 'name' table.
    bool isHidden = false;             // The axis should not be exposed directly in user interfaces.
};

struct FontInstanceInfo {
    std::unordered_map<std::string, std::string> subfamilyNameMapForLanguage;
    std::unordered_map<std::string, std::string> postScriptNameMapForLanguage;
    std::unordered_map<std::string, float> coordinates;
    bool isDefault = false;
};

class FontVariationInfo {
public:
    FontVariationInfo() = default;
    ~FontVariationInfo()
    {
        Cleanup();
    }

    /**
     * @brief            Returns variation axis info for given typeface.
     * @param typeface   given typeface.
     * @param languages  defined language to get specific axisTag.
     * @return           variation axis info for given typeface
     */
    static std::vector<FontAxisInfo> GenerateFontVariationAxisInfo(const std::shared_ptr<Typeface>& typeface,
                                                                   const std::vector<std::string>& languages);

    /**
     * @brief            Returns variation instance info for given typeface.
     * @param typeface   given typeface.
     * @param languages  defined language to get specific name.
     * @return           variation instance info for given typeface
     */
    static std::vector<FontInstanceInfo> GenerateFontVariationInstanceInfo(const std::shared_ptr<Typeface>& typeface,
                                                                           const std::vector<std::string>& languages);

private:
#ifdef USE_M133_SKIA
    using HBFace = std::unique_ptr<hb_face_t, SkFunctionObject<hb_face_destroy>>;
    using HBBlob = std::unique_ptr<hb_blob_t, SkFunctionObject<hb_blob_destroy>>;
#else
    template <typename T, typename P, P* p> using resource = std::unique_ptr<T, SkFunctionWrapper<P, p>>;
    using HBFace = resource<hb_face_t, decltype(hb_face_destroy), hb_face_destroy>;
    using HBBlob = resource<hb_blob_t, decltype(hb_blob_destroy), hb_blob_destroy>;
#endif

    bool LoadFromTypeface(const std::shared_ptr<Typeface>& typeface,
                          const std::vector<std::string>& languages,
                          bool needInstance);
    bool InitializedFace(const std::shared_ptr<Typeface>& typeface);
    bool IsVariableFont() const;
    void Cleanup();
    void ExtractAxisInfo(const std::vector<std::string>& languages);
    void GetLocalName(uint16_t nameId, std::string& result, const std::string& language);
    void ExtractInstanceInfo(const std::vector<std::string>& languages);

    HBFace mFace = nullptr;
    std::vector<FontAxisInfo> mAxisInfo = {};
    std::vector<FontInstanceInfo> mInstanceInfo = {};
};
} // Drawing
} // Rosen
} // OHOS
#endif // FONT_VARIATION_INFO_H