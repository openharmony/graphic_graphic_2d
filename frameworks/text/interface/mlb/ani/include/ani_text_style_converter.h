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
#include <memory>
#include <vector>

#include "text_style.h"

namespace OHOS::Text::ANI {
class AniTextStyleConverter final {
public:
    static ani_status ParseTextStyleToNative(ani_env* env, ani_object obj, OHOS::Rosen::TextStyle& textStyle);
    static void ParseFontFeatureToNative(ani_env* env, ani_object obj, OHOS::Rosen::FontFeatures& fontFeatures);
    static void ParseFontVariationToNative(ani_env* env, ani_object obj, OHOS::Rosen::FontVariations& fontVariations);
    static void ParseTextShadowToNative(ani_env* env, ani_object obj, std::vector<OHOS::Rosen::TextShadow>& textShadow);
    static void ParseRectStyleToNative(ani_env* env, ani_object obj, OHOS::Rosen::RectStyle& rectStyle);
    static void ParseDecorationToNative(ani_env* env, ani_object obj, OHOS::Rosen::TextStyle& textStyle);

    static ani_object ParseTextStyleToAni(ani_env* env, const OHOS::Rosen::TextStyle& textStyle);
    static ani_object ParseTextShadowToAni(ani_env* env, const OHOS::Rosen::TextShadow& textShadow);
    static ani_object ParseDecorationToAni(ani_env* env, const OHOS::Rosen::TextStyle& textStyle);
    static ani_object ParseRectStyleToAni(ani_env* env, const OHOS::Rosen::RectStyle& rectStyle);
    static ani_object ParseFontFeaturesToAni(ani_env* env, const OHOS::Rosen::FontFeatures& fontFeatures);
    static ani_object ParseFontVariationsToAni(ani_env* env, const OHOS::Rosen::FontVariations& fontVariations);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_TEXT_STYLE_CONVERTER_H