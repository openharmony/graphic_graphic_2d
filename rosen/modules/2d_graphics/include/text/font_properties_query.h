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

#ifndef FONT_PROPERTIES_QUERY_H
#define FONT_PROPERTIES_QUERY_H

#include <string>
#include <vector>

#include "text/font_style.h"
#include "typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

struct DRAWING_API FontProperties {
    int32_t weight = FontStyle::NORMAL_WEIGHT;     // Font weight
    int32_t width = FontStyle::NORMAL_WIDTH;       // Font width
    FontStyle::Slant slant = FontStyle::UPRIGHT_SLANT;  // Font slant
    bool bold = false;                         // Font is bold
    bool italic = false;                       // Font is italic
    bool monospace = false;                    // Font is monospace (fixed pitch)
    bool colorglyphs = false;                  // Font has color glyphs (Embed Bitmap/COLR/CPAL/SVG)
};

class DRAWING_API FontPropertiesQuery {
public:
    /**
     * @brief            Returns all supported and valid font properties in given typeface.
     * @param typeface   given typeface.
     * @return           FontProperties of supported and valid font properties in given typeface
     */
    static FontProperties GenerateFontProperties(const std::shared_ptr<Typeface>& typeface);

    /**
     * @brief            Returns all supported and valid font properties in given fontPath.
     * @param fontPath   given fontPath.
     * @return           FontProperties of supported and valid font properties in given fontPath
     */
    static FontProperties GenerateFontProperties(const std::string& fontPath);

    /**
     * @brief            Returns all supported and valid font properties in given fontData.
     * @param fontData   given fontData.
     * @return           FontProperties of supported and valid font properties in given fontData
     */
    static FontProperties GenerateFontProperties(const std::vector<uint8_t>& fontData);
};
} // Drawing
} // Rosen
} // OHOS
#endif // FONT_PROPERTIES_QUERY_H