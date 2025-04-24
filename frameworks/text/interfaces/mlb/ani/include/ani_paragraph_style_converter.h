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

#ifndef OHOS_TEXT_ANI_PARAGRAPH_STYLE_CONVERTER_H
#define OHOS_TEXT_ANI_PARAGRAPH_STYLE_CONVERTER_H

#include <ani.h>
#include <memory>
#include <string>
#include <vector>

#include "typography.h"
#include "typography_style.h"

namespace OHOS::Text::ANI {
class AniParagraphStyleConverter final {
public:
    static std::unique_ptr<OHOS::Rosen::TypographyStyle> ParseParagraphStyleToNative(ani_env* env, ani_object obj);
    static void ParseParagraphStyleStrutStyleToNative(
        ani_env* env, ani_object obj, std::unique_ptr<OHOS::Rosen::TypographyStyle>& paragraphStyle);
    static void ParseTextTabToNative(ani_env* env, ani_object obj, OHOS::Rosen::TextTab& textTab);
    static void ParseFontFamiliesToNative(ani_env* env, ani_array obj, std::vector<std::string>& fontFamilies);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_PARAGRAPH_STYLE_CONVERTER_H
