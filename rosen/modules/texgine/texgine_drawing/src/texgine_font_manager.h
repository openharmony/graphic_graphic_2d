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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_MANAGER_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_MANAGER_H

#include <memory>

#include "drawing.h"

#include "texgine_font_style.h"
#include "texgine_font_style_set.h"
#include "texgine_typeface.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontManager {
public:
    TexgineFontManager();

    /*
     * @brief Get default font manager
     */
    static std::shared_ptr<TexgineFontManager> RefDefault();

    /*
     * @brief Get the font manager
     */
    std::shared_ptr<RSFontMgr> GetFontMgr() const;

    void SetFontMgr(const std::shared_ptr<RSFontMgr> mgr);

    /*
     * @brief Use the system fallback to find a typeface for the given character.
     * @param familyName The family names of user setting
     *        style        The font style of user setting
     *        bcp47[]      is a combination of ISO 639, 15924, and 3166-1 codes,
     *                     so it is fine to just pass a ISO 639 here
     *        bcp47Count   The length of bcp47[]
     *        character    The character what you want to match typeface
     * @return Will return nullptr if no family can be found
     */
    std::shared_ptr<TexgineTypeface> MatchFamilyStyleCharacter(const std::string &familyName,
        const TexgineFontStyle &style, const char* bcp47[], int bcp47Count, int32_t character);

    /*
     * @brief Get the font style set
     * @param familyName The family names of user setting
     * @return Never return nullptr, will return an empty set if the name is NULL
     */
    std::shared_ptr<TexgineFontStyleSet> MatchFamily(const std::string &familyName);

    std::shared_ptr<TexgineTypeface> MatchFamilyStyle(const std::string &familyName,
        const TexgineFontStyle &style);

private:
    std::shared_ptr<RSFontMgr> fontMgr_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_MANAGER_H
