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

#include "text/font_properties_query.h"

#include <vector>
#include <memory>
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
FontProperties FontPropertiesQuery::GenerateFontProperties(const std::shared_ptr<Typeface>& typeface)
{
    FontProperties properties;
    if (!typeface) {
        LOGE("Drawing_Text [GenerateFontProperties] typeface is nullptr!");
        return properties;
    }

    // weight, width, slant from FontStyle
    FontStyle fontStyle = typeface->GetFontStyle();
    properties.weight = fontStyle.GetWeight();
    properties.width = fontStyle.GetWidth();
    properties.slant = fontStyle.GetSlant();

    // bold, italic from Typeface
    properties.bold = typeface->GetBold();
    properties.italic = typeface->GetItalic();

    properties.monospace = typeface->GetMonospace();
    properties.colorglyphs = typeface->IsColored();

    return properties;
}

FontProperties FontPropertiesQuery::GenerateFontProperties(const std::string& fontPath)
{
    if (fontPath.empty()) {
        LOGE("Drawing_Text [GenerateFontProperties] fontPath is empty!");
        return {};
    }
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile(fontPath.c_str());
    if (!typeface) {
        LOGE("Drawing_Text [GenerateFontProperties] Failed to create typeface from file: %s", fontPath.c_str());
        return {};
    }
    return GenerateFontProperties(typeface);
}

FontProperties FontPropertiesQuery::GenerateFontProperties(const std::vector<uint8_t>& fontData)
{
    if (fontData.empty()) {
        LOGE("Drawing_Text [GenerateFontProperties] fontData is empty!");
        return {};
    }
    auto stream = std::make_unique<Drawing::MemoryStream>(fontData.data(), fontData.size());
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    return GenerateFontProperties(typeface);
}

} // Drawing
} // Rosen
} // OHOS