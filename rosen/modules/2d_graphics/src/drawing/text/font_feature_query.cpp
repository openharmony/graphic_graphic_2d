/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "text/font_feature_query.h"

#include <vector>
#include <memory>
#include <set>

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

static constexpr uint32_t FEATURE_TAG_MAX_LENGTH = 1024;
static constexpr uint32_t UNICODE_MAX_STRING_LENGTH = 10;

std::vector<std::string> FontFeatureQuery::GenerateFontSupportedFeatures(const std::shared_ptr<Typeface>& typeface)
{
    if (!typeface) {
        LOGE("Drawing_Text [GenerateFontSupportedFeatures] typeface is nullptr!");
        return {};
    }
    HBFace hbFace = FontHarfbuzz::CreateHbFace(*typeface);
    if (!hbFace) {
        LOGE("Drawing_Text [GenerateFontSupportedFeatures] hbFace is nullptr!");
        return {};
    }
    std::set<std::string> featureSet;
    unsigned int featureCount = FEATURE_TAG_MAX_LENGTH;
    std::vector<hb_tag_t> tags(featureCount);
    hb_tag_t tableTags[] = { HB_TAG('G', 'S', 'U', 'B'), HB_TAG('G', 'P', 'O', 'S') };
    for (auto tag : tableTags) {
        hb_ot_layout_table_get_feature_tags(hbFace.get(), tag, HB_DIRECTION_INVALID, &featureCount, tags.data());
        if (featureCount == 0) {
            continue;
        }
        std::vector<hb_tag_t> featureTags(featureCount);
        hb_ot_layout_table_get_feature_tags(hbFace.get(), tag, 0, &featureCount, featureTags.data());
        for (unsigned int i = 0; i < featureCount; ++i) {
            char tagStr[UNICODE_MAX_STRING_LENGTH] = {0};
            hb_tag_to_string(featureTags[i], tagStr);
            featureSet.insert(tagStr);
        }
    }
    return std::vector<std::string>(featureSet.begin(), featureSet.end());
}

std::vector<std::string> FontFeatureQuery::GenerateFontSupportedFeatures(const std::string& fontPath)
{
    if (fontPath.empty()) {
        LOGE("Drawing_Text [GenerateFontSupportedFeatures] fontPath is empty!");
        return {};
    }
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile(fontPath.c_str());
    return GenerateFontSupportedFeatures(typeface);
}

std::vector<std::string> FontFeatureQuery::GenerateFontSupportedFeatures(const std::vector<uint8_t>& fontData)
{
    if (fontData.empty()) {
        LOGE("Drawing_Text [GenerateFontSupportedFeatures] fontData is empty!");
        return {};
    }
    auto stream = std::make_unique<Drawing::MemoryStream>(fontData.data(), fontData.size());
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    return GenerateFontSupportedFeatures(typeface);
}
} // Drawing
} // Rosen
} // OHOS