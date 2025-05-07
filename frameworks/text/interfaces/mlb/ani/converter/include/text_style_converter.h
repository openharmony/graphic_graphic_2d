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

#ifndef OHOS_TEXT_ANI_TEXT_STYLE_CONVERTER_H
#define OHOS_TEXT_ANI_TEXT_STYLE_CONVERTER_H

#include <ani.h>

#include "text_style.h"

namespace OHOS::Text::NAI {
using namespace OHOS::Rosen;
class TextStyleConverter {
public:
    static std::unique_ptr<TextStyle> ParseTextStyleToNative(ani_env* env, ani_object obj);
    static void ParseFontFeatureToNative(ani_env* env, ani_object obj, FontFeatures& fontFeatures);
    static void ParseFontVariationToNative(ani_env* env, ani_object obj, FontVariations& fontVariations);
    static void ParseTextShadowToNative(ani_env* env, ani_object obj, std::vector<TextShadow>& textShadow);
    static void ParseRectStyleToNative(ani_env* env, ani_object obj, RectStyle& rectStyle);
    static void ParseDecorationToNative(ani_env* env, ani_object obj, TextStyle& textStyle);

    static ani_object ParseTextStyleToAni(ani_env* env, const TextStyle& textStyle);
    static ani_object ParseTextShadowToAni(ani_env* env, const TextShadow& textShadow);
    static ani_object ParseDecorationToAni(ani_env* env, const TextStyle& textStyle);
    static ani_object ParseRectStyleToAni(ani_env* env, const RectStyle& rectStyle);
    static ani_object ParseFontFeaturesToAni(ani_env* env, const FontFeatures& fontFeatures);
    static ani_object ParseFontVariationsToAni(ani_env* env, const FontVariations& fontVariations);
};
} // namespace OHOS::Text::NAI
#endif // OHOS_TEXT_ANI_TEXT_STYLE_CONVERTER_H