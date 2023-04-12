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

#include <include/core/SkFontMgr.h>

#include "texgine_font_style.h"
#include "texgine_font_style_set.h"
#include "texgine_typeface.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontManager {
public:
    static std::shared_ptr<TexgineFontManager> RefDefault();
    TexgineFontManager();
    sk_sp<SkFontMgr> GetFontMgr();
    void SetFontMgr(sk_sp<SkFontMgr> mgr);
    std::shared_ptr<TexgineTypeface> MatchFamilyStyleCharacter(const char familyName[], TexgineFontStyle &style,
                                          const char *bcp47[], int bcp47Count, int32_t character);
    std::shared_ptr<TexgineFontStyleSet> MatchFamily(const char familyName[]);

private:
    sk_sp<SkFontMgr> fontMgr_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_FONT_MANAGER_H
