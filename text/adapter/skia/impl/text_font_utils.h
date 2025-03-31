/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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
#ifndef TEXT_FONT_UTILS
#define TEXT_FONT_UTILS

#include <vector>
#include <string>
#include <algorithm>

#include "modules/skparagraph/include/TextStyle.h"
#include "text_types.h"
#include "txt/text_style.h"
#include "text/font_style.h"
#include "SkFontArguments.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace skt = skia::textlayout;

class TextFontUtils {
public:
    static FontWeight GetTxtFontWeight(int fontWeight);

    static FontStyle GetTxtFontStyle(RSFontStyle::Slant slant);

    static RSFontStyle::Weight GetSkiaFontWeight(FontWeight spFontWeight);

    static RSFontStyle::Slant GetSkiaFontSlant(FontStyle spSlant);

    static RSFontStyle::Width GetSkiaFontWidth(FontWidth spFontWidth);

    static std::vector<SkString> GetSkiaFontfamilies(const std::vector<std::string>& spFontFamiles);

    static void MakeFontArguments(skt::TextStyle& skStyle, const FontVariations& fontVariations);

    static skt::TextShadow MakeTextShadow(const TextShadow& txtShadow);

    static RSFontStyle::Slant ConvertToRSFontSlant(FontStyle fontStyle);

    static int ConvertToSkFontWeight(FontWeight fontWeight);

    static RSFontStyle MakeFontStyle(FontWeight fontWeight, FontWidth fontWidth, FontStyle fontStyle);
};
}  // namespace SPText
}  // namespace Rosen
}  // namespace OHOS

#endif // SKIA_UTILS_H