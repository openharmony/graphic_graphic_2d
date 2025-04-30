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

#include "text_style.h"
#include "text/font_metrics.h"
#include "typography.h"
#include "typography_style.h"

namespace OHOS::Text::NAI {
using namespace OHOS::Rosen;
inline constexpr const char* NATIVE_OBJ = "nativeObj";
inline constexpr const char* ANI_OBJECT = "Lstd/core/Double;";
inline constexpr const char* ANI_INT = "Lstd/core/Int;";
inline constexpr const char* ANI_BOOLEAN = "Lstd/core/Boolean;";
inline constexpr const char* ANI_MAP = "Lescompat/Map;";
inline constexpr const char* ANI_ARRAY = "Lescompat/Array;";
inline constexpr const char* ANI_CLASS_CLEANER = "L@ohos/graphics/text/text/Cleaner;";
inline constexpr const char* ANI_CLASS_PARAGRAPH = "L@ohos/graphics/text/text/Paragraph;";
inline constexpr const char* ANI_CLASS_PARAGRAPH_BUILDER = "L@ohos/graphics/text/text/ParagraphBuilder;";
inline constexpr const char* ANI_CLASS_LINEMETRICS_I = "L@ohos/graphics/text/text/LineMetricsInternal;";
inline constexpr const char* ANI_CLASS_RUNMETRICS_I = "L@ohos/graphics/text/text/RunMetricsInternal;";
inline constexpr const char* ANI_CLASS_PARAGRAPH_STYLE = "L@ohos/graphics/text/text/ParagraphStyle;";
inline constexpr const char* ANI_CLASS_TEXT_STYLE = "L@ohos/graphics/text/text/TextStyle;";
inline constexpr const char* ANI_CLASS_TEXT_STYLE_I = "L@ohos/graphics/text/text/TextStyleInternal;";
inline constexpr const char* ANI_CLASS_FONT_COLLECTION = "L@ohos/graphics/text/text/FontCollection;";
inline constexpr const char* ANI_CLASS_FONT_FEATURE = "L@ohos/graphics/text/text/FontFeature;";
inline constexpr const char* ANI_CLASS_FONT_FEATURE_I = "L@ohos/graphics/text/text/FontFeatureInternal;";
inline constexpr const char* ANI_CLASS_FONT_VARIATION = "L@ohos/graphics/text/text/FontVariation;";
inline constexpr const char* ANI_CLASS_FONT_VARIATION_I = "L@ohos/graphics/text/text/FontVariationInternal;";
inline constexpr const char* ANI_CLASS_CANVAS = "L@ohos/graphics/drawing/drawing/Canvas;";
inline constexpr const char* ANI_CLASS_PATH = "L@ohos/graphics/drawing/drawing/Path;";
inline constexpr const char* ANI_CLASS_FONT_METRICS = "L@ohos/graphics/drawing/drawing/FontMetrics;";
inline constexpr const char* ANI_CLASS_TEXTSHADOW = "L@ohos/graphics/text/text/TextShadow;";
inline constexpr const char* ANI_CLASS_TEXTSHADOW_I = "L@ohos/graphics/text/text/TextShadowInternal;";
inline constexpr const char* ANI_CLASS_RECT_STYLE = "L@ohos/graphics/text/text/RectStyle;";
inline constexpr const char* ANI_CLASS_RECT_STYLE_I = "L@ohos/graphics/text/text/RectStyleInternal;";
inline constexpr const char* ANI_CLASS_DECORATION_I = "L@ohos/graphics/text/text/DecorationInternal;";

inline constexpr const char* ANI_ENUM_FONT_WEIGHT = "L@ohos/graphics/text/text/FontWeight;";
inline constexpr const char* ANI_ENUM_FONT_STYLE = "L@ohos/graphics/text/text/FontStyle;";
inline constexpr const char* ANI_ENUM_TEXT_BASELINE = "L@ohos/graphics/text/text/TextBaseline;";
inline constexpr const char* ANI_ENUM_ELLIPSIS_MODE = "L@ohos/graphics/text/text/EllipsisMode;";
inline constexpr const char* ANI_ENUM_TEXT_DECORATION_TYPE = "L@ohos/graphics/text/text/TextDecorationType;";
inline constexpr const char* ANI_ENUM_TEXT_DECORATION_STYLE = "L@ohos/graphics/text/text/TextDecorationStyle;";

class AniCommon {
public:
    static std::unique_ptr<TypographyStyle> ParseParagraphStyleToNative(ani_env* env, ani_object obj);
    static std::unique_ptr<TextStyle> ParseTextStyleToNative(ani_env* env, ani_object obj);
    static void ParseParagraphStyleStrutStyleToNative(ani_env* env, ani_object obj,
                                              std::unique_ptr<TypographyStyle>& paragraphStyle);
    static void ParseDrawingColorToNative(ani_env* env, ani_object obj, Drawing::Color& color);
    static void ParseFontFeatureToNative(ani_env* env, ani_object obj, FontFeatures& fontFeatures);
    static void ParseFontVariationToNative(ani_env* env, ani_object obj, FontVariations& fontVariations);
    static void ParseTextTabToNative(ani_env* env, ani_object obj, TextTab& textTab);
    static void ParseFontFamiliesToNative(ani_env* env, ani_array_ref obj, std::vector<std::string>& fontFamilies);
    static void ParseTextShadowToNative(ani_env* env, ani_object obj, std::vector<TextShadow>& textShadow);
    static void ParseTextColorToNative(ani_env* env, ani_object obj, const std::string& str, Drawing::Color& colorSrc);
    static void ParseRectStyleToNative(ani_env* env, ani_object obj, RectStyle& rectStyle);

    static ani_object ParseTextStyleToAni(ani_env* env, const TextStyle& textStyle);
    static ani_object ParseFontMetricsToAni(ani_env* env, const Drawing::FontMetrics& fontMetrics);
    static ani_object ParseRunMetricsToAni(ani_env* env, const std::map<size_t, RunMetrics>& map);
    static ani_object ParseLineMetricsToAni(ani_env* env, const LineMetrics& lineMetrics);
    static ani_object ParseTextShadowToAni(ani_env* env, const TextShadow& textShadow);
    static ani_object ParseDecorationToAni(ani_env* env, const TextStyle& textStyle);
    static ani_object ParseRectStyleToAni(ani_env* env, const RectStyle& rectStyle);
    static ani_object ParseFontFeaturesToAni(ani_env* env, const FontFeatures& fontFeatures);
    static ani_object ParseFontVariationsToAni(ani_env* env, const FontVariations& fontVariations);
};
} // namespace OHOS::Text::NAI
#endif // OHOS_ROSEN_ANI_COMMON_H