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

#include <include/core/SkFontMgr.h>

#include "texgine_font_style.h"
#include "texgine_string.h"
#include "texgine_typeface.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontStyleSet {
public:
    TexgineFontStyleSet(SkFontStyleSet *set);
    ~TexgineFontStyleSet();

    int Count();
    void GetStyle(int index, std::shared_ptr<TexgineFontStyle> style, std::shared_ptr<TexgineString> name);
    std::shared_ptr<TexgineTypeface> CreateTypeface(int index);
    std::shared_ptr<TexgineTypeface> MatchStyle(std::shared_ptr<TexgineFontStyle> pattern);
    static std::shared_ptr<TexgineFontStyleSet> CreateEmpty();

    SkFontStyleSet *Get() const
    {
        return set_;
    }

private:
    SkFontStyleSet *set_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_FONT_STYLE_SET_H
