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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_FONT_STYLE_SET_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_FONT_STYLE_SET_H

#include <memory>

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkFontMgr.h>
#else
#include "drawing.h"
#endif

#include "texgine_font_style.h"
#include "texgine_string.h"
#include "texgine_typeface.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontStyleSet {
public:
#ifndef USE_ROSEN_DRAWING
    explicit TexgineFontStyleSet(SkFontStyleSet *set);
    ~TexgineFontStyleSet();
#else
    explicit TexgineFontStyleSet(RSFontStyleSet *set);
    ~TexgineFontStyleSet();
#endif

    /*
     * @brief Return the count of typeface
     */
    int Count() const;

    /*
     * @brief Get thr font style for the specified typeface
     * @param index The index of a typeface
     * @param style The style value that returned to the caller
     * @param name  The style name that returned to the caller
     */
    void GetStyle(const int index, std::shared_ptr<TexgineFontStyle> style, std::shared_ptr<TexgineString> name) const;

    /*
     * @brief To create a typeface
     * @param index The the index of the typeface in this font style set
     */
    std::shared_ptr<TexgineTypeface> CreateTypeface(int index);

    /*
     * @brief To get the closest matching typeface
     * @param pattern The style value to be matching
     */
    std::shared_ptr<TexgineTypeface> MatchStyle(const std::shared_ptr<TexgineFontStyle> pattern);

    /*
     * @brief Returns the SkFontStyleSet in TexgineFontStyleSet
     */
#ifndef USE_ROSEN_DRAWING
    SkFontStyleSet *GetFontStyleSet() const
#else
    RSFontStyleSet *GetFontStyleSet() const
#endif
    {
        return set_;
    }

private:
#ifndef USE_ROSEN_DRAWING
    SkFontStyleSet *set_ = nullptr;
#else
    RSFontStyleSet *set_ = nullptr;
#endif
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_FONT_STYLE_SET_H
