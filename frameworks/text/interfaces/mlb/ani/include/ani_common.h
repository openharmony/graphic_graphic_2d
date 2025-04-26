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

#ifndef OHOS_ROSEN_ANI_COMMON_H
#define OHOS_ROSEN_ANI_COMMON_H

#include <ani.h>

#include "typography_style.h"
namespace OHOS::Text::NAI {
using namespace OHOS::Rosen;
inline constexpr const char* NATIVE_OBJ = "nativeObj";
inline constexpr const char* ANI_CLASS_CLEANER = "L@ohos/graphics/text/text/Cleaner;";
inline constexpr const char* ANI_CLASS_PARAGRAPH = "L@ohos/graphics/text/text/Paragraph;";
inline constexpr const char* ANI_CLASS_PARAGRAPH_BUILDER = "L@ohos/graphics/text/text/ParagraphBuilder;";
inline constexpr const char* ANI_CLASS_LINEMETRICS_I = "L@ohos/graphics/text/text/LineMetricsInternal;";
inline constexpr const char* ANI_CLASS_RUNMETRICS_I = "L@ohos/graphics/text/text/RunMetricsInternal;";
inline constexpr const char* ANI_CLASS_PARAGRAPH_STYLE = "L@ohos/graphics/text/text/ParagraphStyle;";
inline constexpr const char* ANI_CLASS_TEXT_STYLE = "L@ohos/graphics/text/text/TextStyle;";
inline constexpr const char* ANI_CLASS_TEXT_STYLE_I = "L@ohos/graphics/text/text/TextStyleInternal;";
inline constexpr const char* ANI_CLASS_FONT_COLLECTION = "L@ohos/graphics/text/text/FontCollection;";
inline constexpr const char* ANI_CLASS_FONTFEATURE = "L@ohos/graphics/text/text/FontFeature;";
inline constexpr const char* ANI_CLASS_CANVAS = "L@ohos/graphics/drawing/drawing/Canvas;";
inline constexpr const char* ANI_CLASS_PATH = "L@ohos/graphics/drawing/drawing/Path;";
inline constexpr const char* ANI_CLASS_FONT_METRICS = "L@ohos/graphics/drawing/drawing/FontMetrics;";
inline constexpr const char* ANI_CLASS_TEXTSHADOW = "L@ohos/graphics/text/text/TextShadow;";

class AniCommon {
public:
    static std::unique_ptr<TypographyStyle> ParseParagraphStyle(ani_env* env, ani_object obj);
    static std::unique_ptr<TextStyle> ParseTextStyle(ani_env* env, ani_object obj);
    static void ParseDrawingColor(ani_env* env, ani_object obj, Drawing::Color& color);
    static void ParseFontFeature(ani_env* env, ani_object obj, FontFeatures& fontFeatures);
    static void SetParagraphStyleStrutStyle(ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle);
    static void SetParagraphStyleTab(ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle);
    static void SetFontFamilies(ani_env* env, ani_array_ref obj, std::vector<std::string>& paragraphStyle);
    static void ParseTextShadow(ani_env* env, ani_object obj, std::vector<TextShadow>& textShadow);
    static void SetTextShadowColor(ani_env* env, ani_object obj, const std::string& str, Drawing::Color& colorSrc);
};
} // namespace OHOS::Text::NAI
#endif // OHOS_ROSEN_ANI_COMMON_H