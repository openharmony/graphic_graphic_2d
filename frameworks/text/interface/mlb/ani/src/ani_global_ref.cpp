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

#include "ani_global_ref.h"

#include "ani_common.h"
#include "ani_cache_manager.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
namespace {
constexpr std::string_view RUNMETRICS_SIGN = "C{" ANI_INTERFACE_TEXT_STYLE "}C{" ANI_INTERFACE_FONT_METRICS "}:";
constexpr CacheKey RUNMETRICS_KEY{ANI_CLASS_RUNMETRICS, "<ctor>", RUNMETRICS_SIGN};
constexpr CacheKey MAP_SET_KEY{ANI_MAP, "set", "YY:C{std.core.Map}"};
constexpr CacheKey BUSINESS_ERROR_KEY{ANI_BUSINESS_ERROR, "<ctor>", "C{std.core.String}C{escompat.ErrorOptions}:"};
constexpr CacheKey ARRAY_KEY{ANI_ARRAY, "<ctor>", "i:"};
constexpr CacheKey MAP_KEY{ANI_MAP, "<ctor>", ":"};
constexpr CacheKey DOUBLE_KEY{ANI_DOUBLE, "<ctor>", "d:"};
constexpr CacheKey INT_KEY{ANI_INT, "<ctor>", "i:"};
constexpr CacheKey BOOLEAN_KEY{ANI_BOOLEAN, "<ctor>", "z:"};
constexpr CacheKey DOUBLE_GET_KEY{ANI_DOUBLE, "toDouble", ":d"};
constexpr CacheKey INT_GET_KEY{ANI_INT, "toInt", ":i"};
constexpr CacheKey BOOLEAN_GET_KEY{ANI_BOOLEAN, "toBoolean", ":z"};
constexpr CacheKey FONT_COLLECTION_KEY{ANI_CLASS_FONT_COLLECTION, "<ctor>", "l:"};
constexpr CacheKey FONT_COLLECTION_GET_NATIVE_KEY{ANI_CLASS_FONT_COLLECTION, TEXT_GET_NATIVE, ":l"};
constexpr CacheKey PARAGRAPH_BUILDER_GET_NATIVE_KEY{ANI_CLASS_PARAGRAPH_BUILDER, TEXT_GET_NATIVE, ":l"};
constexpr CacheKey PARAGRAPH_KEY{ANI_CLASS_PARAGRAPH, "<ctor>", ":"};
constexpr CacheKey PARAGRAPH_GET_NATIVE_KEY{ANI_CLASS_PARAGRAPH, TEXT_GET_NATIVE, ":l"};
constexpr CacheKey RUN_KEY{ANI_CLASS_RUN, "<ctor>", ":"};
constexpr CacheKey RUN_GET_NATIVE_KEY{ANI_CLASS_RUN, TEXT_GET_NATIVE, ":l"};
constexpr CacheKey TEXT_LINE_KEY{ANI_CLASS_TEXT_LINE, "<ctor>", ":"};
constexpr CacheKey TEXT_LINE_GET_NATIVE_KEY{ANI_CLASS_TEXT_LINE, TEXT_GET_NATIVE, ":l"};

constexpr const std::string_view FONT_DESCRIPTOR_SIGN = "C{" ANI_STRING "}" "C{" ANI_STRING "}" "C{" ANI_STRING "}"
    "C{" ANI_STRING "}" "C{" ANI_STRING "}" "E{" ANI_ENUM_FONT_WEIGHT "}iizz" "C{" ANI_STRING "}" "C{" ANI_STRING "}"
    "C{" ANI_STRING "}" "C{" ANI_STRING "}" "C{" ANI_STRING "}" "C{" ANI_STRING "}" "C{" ANI_STRING "}"
    "C{" ANI_STRING "}" "C{" ANI_STRING "}" "iC{" ANI_ARRAY "}C{" ANI_ARRAY "}:";
constexpr CacheKey FONT_DESCRIPTOR_KEY{ANI_CLASS_FONT_DESCRIPTOR, "<ctor>", FONT_DESCRIPTOR_SIGN};
constexpr CacheKey FONT_DESCRIPTOR_GET_PATH_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>path", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_DESCRIPTOR_POST_SCRIPT_NAME_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>postScriptName", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_DESCRIPTOR_FULL_NAME_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>fullName", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_DESCRIPTOR_FONT_FAMILY_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>fontFamily", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_DESCRIPTOR_FONT_SUBFAMILY_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>fontSubfamily", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_DESCRIPTOR_GET_WEIGHT_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>weight", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WEIGHT)};
constexpr CacheKey FONT_DESCRIPTOR_WIDTH_KEY{ANI_INTERFACE_FONT_DESCRIPTOR, "<get>width", ANI_WRAP_RETURN_C(ANI_INT)};
constexpr CacheKey FONT_DESCRIPTOR_ITALIC_KEY{ANI_INTERFACE_FONT_DESCRIPTOR, "<get>italic", ANI_WRAP_RETURN_C(ANI_INT)};
constexpr CacheKey FONT_DESCRIPTOR_MONO_SPACE_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>monoSpace", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey FONT_DESCRIPTOR_SYMBOLIC_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>symbolic", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey FONT_DESCRIPTOR_VARIATION_AXIS_RECORDS_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>variationAxisRecords", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey FONT_DESCRIPTOR_VARIATION_INSTANCE_RECORDS_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>variationInstanceRecords", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey LINE_TYPESET_KEY{ANI_CLASS_LINE_TYPESET, "<ctor>", ":"};
constexpr CacheKey PARAGRAPH_BUILDER_KEY{ANI_CLASS_PARAGRAPH_BUILDER, "<ctor>", ":"};
constexpr CacheKey CANVAS_GET_NATIVE_KEY{ANI_CLASS_CANVAS, TEXT_GET_NATIVE, ":l"};
constexpr CacheKey PATH_GET_NATIVE_KEY{ANI_CLASS_PATH, TEXT_GET_NATIVE, ":l"};
constexpr CacheKey ARRAY_SET_KEY{ANI_ARRAY, "$_set", "iY:"};
constexpr CacheKey POSITION_WITH_AFFINITY_KEY{ANI_CLASS_POSITION_WITH_AFFINITY, "<ctor>", "iE{" ANI_ENUM_AFFINITY "}:"};

constexpr CacheKey PARAGRAPH_STYLE_MAX_LINES_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>maxLines", ANI_WRAP_RETURN_C(ANI_INT)};
constexpr CacheKey PARAGRAPH_STYLE_TEXT_STYLE_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>textStyle", ANI_WRAP_RETURN_C(ANI_INTERFACE_TEXT_STYLE)};
constexpr CacheKey PARAGRAPH_STYLE_TEXT_DIRECTION_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>textDirection", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DIRECTION)};
constexpr CacheKey PARAGRAPH_STYLE_ALIGN_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>align", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_ALIGN)};
constexpr CacheKey PARAGRAPH_STYLE_WORD_BREAK_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>wordBreak", ANI_WRAP_RETURN_E(ANI_ENUM_WORD_BREAK)};
constexpr CacheKey PARAGRAPH_STYLE_BREAK_STRATEGY_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>breakStrategy", ANI_WRAP_RETURN_E(ANI_ENUM_BREAK_STRATEGY)};
constexpr CacheKey PARAGRAPH_STYLE_TEXT_HEIGHT_BEHAVIOR_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>textHeightBehavior", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_HEIGHT_BEHAVIOR)};
constexpr CacheKey PARAGRAPH_STYLE_STRUT_STYLE_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>strutStyle", ANI_WRAP_RETURN_C(ANI_INTERFACE_STRUT_STYLE)};
constexpr CacheKey PARAGRAPH_STYLE_TAB_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>tab", ANI_WRAP_RETURN_C(ANI_INTERFACE_TEXT_TAB)};
constexpr CacheKey PARAGRAPH_STYLE_TRAILING_SPACE_OPTIMIZED_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>trailingSpaceOptimized", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey PARAGRAPH_STYLE_AUTO_SPACE_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>autoSpace", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey PARAGRAPH_STYLE_COMPRESS_HEAD_PUNCTUATION_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>compressHeadPunctuation", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey PARAGRAPH_STYLE_VERTICAL_ALIGN_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>verticalAlign", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_VERTICAL_ALIGN)};
constexpr CacheKey PARAGRAPH_STYLE_INCLUDE_FONT_PADDING_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>includeFontPadding", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey PARAGRAPH_STYLE_FALLBACK_LINE_SPACING_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>fallbackLineSpacing", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey PARAGRAPH_STYLE_LINE_SPACING_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>lineSpacing", ANI_WRAP_RETURN_C(ANI_DOUBLE)};

constexpr CacheKey STRUT_STYLE_FONT_STYLE_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>fontStyle", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_STYLE)};
constexpr CacheKey STRUT_STYLE_FONT_WIDTH_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>fontWidth", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WIDTH)};
constexpr CacheKey STRUT_STYLE_FONT_WEIGHT_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>fontWeight", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WEIGHT)};
constexpr CacheKey STRUT_STYLE_FONT_SIZE_KEY{ANI_INTERFACE_STRUT_STYLE, "<get>fontSize", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey STRUT_STYLE_HEIGHT_KEY{ANI_INTERFACE_STRUT_STYLE, "<get>height", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey STRUT_STYLE_LEADING_KEY{ANI_INTERFACE_STRUT_STYLE, "<get>leading", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey STRUT_STYLE_FORCE_HEIGHT_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>forceHeight", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey STRUT_STYLE_ENABLED_KEY{ANI_INTERFACE_STRUT_STYLE, "<get>enabled", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey STRUT_STYLE_HEIGHT_OVERRIDE_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>heightOverride", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey STRUT_STYLE_HALF_LEADING_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>halfLeading", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey STRUT_STYLE_FONT_FAMILIES_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>fontFamilies", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_TAB_ALIGNMENT_KEY{
    ANI_INTERFACE_TEXT_TAB, "<get>alignment", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_ALIGN)};
constexpr CacheKey TEXT_TAB_LOCATION_KEY{ANI_INTERFACE_TEXT_TAB, "<get>location", ":d"};
constexpr CacheKey LINEMETRICS_KEY{ANI_CLASS_LINEMETRICS, "<ctor>", "iiddddddidC{std.core.Map}:"};

constexpr CacheKey TEXT_STYLE_COLOR_KEY{ANI_INTERFACE_TEXT_STYLE, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};
constexpr CacheKey TEXT_STYLE_FONT_WEIGHT_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>fontWeight", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WEIGHT)};
constexpr CacheKey TEXT_STYLE_FONT_WIDTH_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>fontWidth", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WIDTH)};
constexpr CacheKey TEXT_STYLE_FONT_STYLE_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>fontStyle", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_STYLE)};
constexpr CacheKey TEXT_STYLE_BASELINE_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>baseline", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BASELINE)};
constexpr CacheKey TEXT_STYLE_FONT_FAMILIES_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>fontFamilies", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_FONT_SIZE_KEY{ANI_INTERFACE_TEXT_STYLE, "<get>fontSize", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey TEXT_STYLE_LETTER_SPACING_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>letterSpacing", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey TEXT_STYLE_WORD_SPACING_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>wordSpacing", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey TEXT_STYLE_HEIGHT_SCALE_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>heightScale", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey TEXT_STYLE_HALF_LEADING_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>halfLeading", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey TEXT_STYLE_HEIGHT_ONLY_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>heightOnly", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey TEXT_STYLE_ELLIPSIS_KEY{ANI_INTERFACE_TEXT_STYLE, "<get>ellipsis", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey TEXT_STYLE_ELLIPSIS_MODE_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>ellipsisMode", ANI_WRAP_RETURN_E(ANI_ENUM_ELLIPSIS_MODE)};
constexpr CacheKey TEXT_STYLE_LOCALE_KEY{ANI_INTERFACE_TEXT_STYLE, "<get>locale", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey TEXT_STYLE_BASELINE_SHIFT_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>baselineShift", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey TEXT_STYLE_MAX_LINE_HEIGHT_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>lineHeightMaximum", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey TEXT_STYLE_MIN_LINE_HEIGHT_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>lineHeightMinimum", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey TEXT_STYLE_BACKGROUND_RECT_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>backgroundRect", ANI_WRAP_RETURN_C(ANI_INTERFACE_RECT_STYLE)};
constexpr CacheKey TEXT_STYLE_DECORATION_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>decoration", ANI_WRAP_RETURN_C(ANI_INTERFACE_DECORATION)};
constexpr CacheKey TEXT_STYLE_TEXT_SHADOWS_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>textShadows", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_FONT_FEATURES_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>fontFeatures", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_FONT_VARIATIONS_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>fontVariations", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_BADGE_TYPE_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>badgeType", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BADGE_TYPE)};
constexpr CacheKey TEXT_STYLE_LINE_HEIGHT_STYLE_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>lineHeightStyle", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_LINE_HEIGHT_STYLE_TYPE)};
constexpr CacheKey TEXT_STYLE_FONT_EDGING_KEY{
    ANI_INTERFACE_TEXT_STYLE, "<get>fontEdging", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_EDGING)};

constexpr CacheKey DECORATION_DECORATION_TYPE_KEY{
    ANI_INTERFACE_DECORATION, "<get>textDecoration", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DECORATION_TYPE)};
constexpr CacheKey DECORATION_DECORATION_STYLE_KEY{
    ANI_INTERFACE_DECORATION, "<get>decorationStyle", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DECORATION_STYLE)};
constexpr CacheKey DECORATION_DECORATION_THICKNESS_SCALE_KEY{
    ANI_INTERFACE_DECORATION, "<get>decorationThicknessScale", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey DECORATION_DECORATION_COLOR_KEY{
    ANI_INTERFACE_DECORATION, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};

constexpr CacheKey POINT_X_KEY{ANI_INTERFACE_POINT, "<get>x", ":d"};
constexpr CacheKey POINT_Y_KEY{ANI_INTERFACE_POINT, "<get>y", ":d"};

constexpr CacheKey TEXTSHADOW_BLUR_RADIUS_KEY{
    ANI_INTERFACE_TEXTSHADOW, "<get>blurRadius", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey TEXTSHADOW_COLOR_KEY{ANI_INTERFACE_TEXTSHADOW, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};
constexpr CacheKey TEXTSHADOW_POINT_KEY{ANI_INTERFACE_TEXTSHADOW, "<get>point", ANI_WRAP_RETURN_C(ANI_INTERFACE_POINT)};

constexpr CacheKey FONT_FEATURE_NAME_KEY{ANI_INTERFACE_FONT_FEATURE, "<get>name", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_FEATURE_VALUE_KEY{ANI_INTERFACE_FONT_FEATURE, "<get>value", ":i"};

constexpr CacheKey FONT_VARIATION_AXIS_KEY{ANI_INTERFACE_FONT_VARIATION, "<get>axis", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_VARIATION_VALUE_KEY{ANI_INTERFACE_FONT_VARIATION, "<get>value", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey FONT_VARIATION_IS_NORMALIZED_KEY{
    ANI_INTERFACE_FONT_VARIATION, "<get>isNormalized", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};

constexpr std::string_view FONT_VARIATION_AXIS_SIGN = "C{" ANI_STRING "}dddiC{" ANI_STRING "}C{" ANI_STRING "}:";
constexpr CacheKey FONT_VARIATION_AXIS_KEY_CTOR{ANI_CLASS_FONT_VARIATION_AXIS, "<ctor>", FONT_VARIATION_AXIS_SIGN};
constexpr CacheKey FONT_VARIATION_AXIS_KEY_GETTER{ANI_INTERFACE_FONT_VARIATION_AXIS, "<get>key", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_VARIATION_AXIS_MIN_VALUE_KEY{ANI_INTERFACE_FONT_VARIATION_AXIS, "<get>minValue", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey FONT_VARIATION_AXIS_MAX_VALUE_KEY{ANI_INTERFACE_FONT_VARIATION_AXIS, "<get>maxValue", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey FONT_VARIATION_AXIS_DEFAULT_VALUE_KEY{ANI_INTERFACE_FONT_VARIATION_AXIS, "<get>defaultValue", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey FONT_VARIATION_AXIS_FLAGS_KEY{ANI_INTERFACE_FONT_VARIATION_AXIS, "<get>flags", ANI_WRAP_RETURN_C(ANI_INT)};
constexpr CacheKey FONT_VARIATION_AXIS_NAME_KEY{ANI_INTERFACE_FONT_VARIATION_AXIS, "<get>name", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_VARIATION_AXIS_LOCAL_NAME_KEY{ANI_INTERFACE_FONT_VARIATION_AXIS, "<get>localName", ANI_WRAP_RETURN_C(ANI_STRING)};

constexpr std::string_view FONT_VARIATION_INSTANCE_SIGN = "C{" ANI_STRING "}C{" ANI_STRING "}C{" ANI_ARRAY "}:";
constexpr CacheKey FONT_VARIATION_INSTANCE_KEY_CTOR{ANI_CLASS_FONT_VARIATION_INSTANCE, "<ctor>", FONT_VARIATION_INSTANCE_SIGN};
constexpr CacheKey FONT_VARIATION_INSTANCE_NAME_KEY{ANI_INTERFACE_FONT_VARIATION_INSTANCE, "<get>name", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_VARIATION_INSTANCE_LOCAL_NAME_KEY{ANI_INTERFACE_FONT_VARIATION_INSTANCE, "<get>localName", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_VARIATION_INSTANCE_COORDINATES_KEY{ANI_INTERFACE_FONT_VARIATION_INSTANCE, "<get>coordinates", ANI_WRAP_RETURN_C(ANI_ARRAY)};

constexpr CacheKey RECT_STYLE_COLOR_KEY{ANI_INTERFACE_RECT_STYLE, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};
constexpr CacheKey RECT_STYLE_LEFT_TOP_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>leftTopRadius", ":d"};
constexpr CacheKey RECT_STYLE_RIGHT_TOP_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>rightTopRadius", ":d"};
constexpr CacheKey RECT_STYLE_RIGHT_BOTTOM_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>rightBottomRadius", ":d"};
constexpr CacheKey RECT_STYLE_LEFT_BOTTOM_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>leftBottomRadius", ":d"};

constexpr std::string_view TEXT_STYLE_SIGN =
    "C{" ANI_INTERFACE_DECORATION "}C{" ANI_INTERFACE_COLOR "}E{" ANI_ENUM_FONT_WEIGHT "}E{" ANI_ENUM_FONT_STYLE
    "}E{" ANI_ENUM_TEXT_BASELINE "}C{" ANI_ARRAY "}ddddzzC{" ANI_STRING "}E{" ANI_ENUM_ELLIPSIS_MODE "}C{" ANI_STRING
    "}dC{" ANI_ARRAY "}C{" ANI_ARRAY "}C{" ANI_INTERFACE_RECT_STYLE "}E{" ANI_ENUM_TEXT_BADGE_TYPE
    "}ddE{" ANI_ENUM_TEXT_LINE_HEIGHT_STYLE_TYPE "}E{" ANI_ENUM_FONT_WIDTH "}E{" ANI_ENUM_FONT_EDGING "}:";
constexpr CacheKey TEXT_STYLE_KEY{ANI_CLASS_TEXT_STYLE, "<ctor>", TEXT_STYLE_SIGN};

constexpr std::string_view TEXT_SHADOW_SIGN = "C{" ANI_INTERFACE_COLOR "}C{" ANI_INTERFACE_POINT "}d:";
constexpr CacheKey TEXTSHADOW_KEY{ANI_CLASS_TEXTSHADOW, "<ctor>", TEXT_SHADOW_SIGN};

constexpr std::string_view DECORATION_SIGN =
    "E{" ANI_ENUM_TEXT_DECORATION_TYPE "}C{" ANI_INTERFACE_COLOR "}E{" ANI_ENUM_TEXT_DECORATION_STYLE "}d:";
constexpr CacheKey DECORATION_KEY{ANI_CLASS_DECORATION, "<ctor>", DECORATION_SIGN};

constexpr CacheKey RECT_STYLE_KEY{ANI_CLASS_RECT_STYLE, "<ctor>", "C{" ANI_INTERFACE_COLOR "}dddd:"};

constexpr CacheKey FONT_FEATURE_KEY{ANI_CLASS_FONT_FEATURE, "<ctor>", "C{" ANI_STRING "}i:"};
constexpr CacheKey FONT_VARIATION_KEY{ANI_CLASS_FONT_VARIATION, "<ctor>", "C{" ANI_STRING "}d:"};

constexpr CacheKey FONT_KEY{ANI_CLASS_FONT, "<ctor>", ":"};

constexpr CacheKey PLACEHOLDER_SPAN_WIDTH_KEY{ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>width", ":d"};
constexpr CacheKey PLACEHOLDER_SPAN_HEIGHT_KEY{ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>height", ":d"};
constexpr CacheKey PLACEHOLDER_SPAN_ALIGN_KEY{
    ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>align", ANI_WRAP_RETURN_E(ANI_ENUM_PLACEHOLDER_ALIGNMENT)};
constexpr CacheKey PLACEHOLDER_SPAN_BASELINE_KEY{
    ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>baseline", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BASELINE)};
constexpr CacheKey PLACEHOLDER_SPAN_BASELINE_OFFSET_KEY{ANI_INTERFACE_PLACEHOLDER_SPAN, "<get>baselineOffset", ":d"};

constexpr CacheKey GLOBAL_RESOURCE_ID_KEY{ANI_GLOBAL_RESOURCE, "<get>id", ":l"};
constexpr CacheKey GLOBAL_RESOURCE_BUNDLE_NAME_KEY{
    ANI_GLOBAL_RESOURCE, "<get>bundleName", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey GLOBAL_RESOURCE_MODULE_NAME_KEY{
    ANI_GLOBAL_RESOURCE, "<get>moduleName", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey GLOBAL_RESOURCE_PARAMS_KEY{ANI_GLOBAL_RESOURCE, "<get>params", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey GLOBAL_RESOURCE_TYPE_KEY{ANI_GLOBAL_RESOURCE, "<get>type", ANI_WRAP_RETURN_C(ANI_INT)};

constexpr CacheKey RANGE_START_KEY{ANI_INTERFACE_RANGE, "<get>start", ":i"};
constexpr CacheKey RANGE_END_KEY{ANI_INTERFACE_RANGE, "<get>end", ":i"};
constexpr CacheKey TEXT_RECT_SIZE_WIDTH_KEY{ANI_INTERFACE_TEXT_RECT_SIZE, "<get>width", ":d"};
constexpr CacheKey TEXT_RECT_SIZE_HEIGHT_KEY{ANI_INTERFACE_TEXT_RECT_SIZE, "<get>height", ":d"};
constexpr CacheKey TEXT_RECT_SIZE_KEY{ANI_CLASS_TEXT_RECT_SIZE, "<ctor>", "dd:"};
constexpr CacheKey TEXT_BOX_KEY{
    ANI_CLASS_TEXT_BOX, "<ctor>", "C{" ANI_INTERFACE_RECT "}C{" ANI_ENUM_TEXT_DIRECTION "}:"};
constexpr CacheKey RANGE_KEY{ANI_CLASS_RANGE, "<ctor>", "ii:"};

constexpr CacheKey TYPOGRAPHIC_BOUNDS_KEY{ANI_CLASS_TYPOGRAPHIC_BOUNDS, "<ctor>", "dddd:"};

constexpr std::string_view TEXT_LAYOUT_RESULT_SIGN = "C{std.core.Array}C{" ANI_INTERFACE_TEXT_RECT_SIZE "}:";
constexpr CacheKey TEXT_LAYOUT_RESULT_KEY{ANI_CLASS_TEXT_LAYOUT_RESULT, "<ctor>", TEXT_LAYOUT_RESULT_SIGN};

constexpr CacheKey LINE_TYPESET_GET_NATIVE_KEY{ANI_CLASS_LINE_TYPESET, TEXT_GET_NATIVE, ":l"};
} // namespace

void AniGlobalNamespace::Init(ani_env* env)
{
    text = AniFindNamespace(env, ANI_NAMESPACE_TEXT);
}

void AniGlobalClass::Init(ani_env* env)
{
    aniString = AniFindClass(env, ANI_STRING);
    aniArray = AniFindClass(env, ANI_ARRAY);
    aniMap = AniFindClass(env, ANI_MAP);
    aniDouble = AniFindClass(env, ANI_DOUBLE);
    aniInt = AniFindClass(env, ANI_INT);
    aniBoolean = AniFindClass(env, ANI_BOOLEAN);
    businessError = AniFindClass(env, ANI_BUSINESS_ERROR);
    globalResource = AniFindClass(env, ANI_GLOBAL_RESOURCE);
    fontCollection = AniFindClass(env, ANI_CLASS_FONT_COLLECTION);
    fontDescriptor = AniFindClass(env, ANI_CLASS_FONT_DESCRIPTOR);
    paragraphBuilder = AniFindClass(env, ANI_CLASS_PARAGRAPH_BUILDER);
    paragraph = AniFindClass(env, ANI_CLASS_PARAGRAPH);
    run = AniFindClass(env, ANI_CLASS_RUN);
    textLine = AniFindClass(env, ANI_CLASS_TEXT_LINE);
    lineTypeSet = AniFindClass(env, ANI_CLASS_LINE_TYPESET);
    positionWithAffinity = AniFindClass(env, ANI_CLASS_POSITION_WITH_AFFINITY);
    lineMetrics = AniFindClass(env, ANI_CLASS_LINEMETRICS);
    runMetrics = AniFindClass(env, ANI_CLASS_RUNMETRICS);
    font = AniFindClass(env, ANI_CLASS_FONT);
    textBox = AniFindClass(env, ANI_CLASS_TEXT_BOX);
    range = AniFindClass(env, ANI_CLASS_RANGE);
    textStyle = AniFindClass(env, ANI_CLASS_TEXT_STYLE);
    textShadow = AniFindClass(env, ANI_CLASS_TEXTSHADOW);
    decoration = AniFindClass(env, ANI_CLASS_DECORATION);
    rectStyle = AniFindClass(env, ANI_CLASS_RECT_STYLE);
    fontFeature = AniFindClass(env, ANI_CLASS_FONT_FEATURE);
    fontVariation = AniFindClass(env, ANI_CLASS_FONT_VARIATION);
    fontVariationAxis = AniFindClass(env, ANI_CLASS_FONT_VARIATION_AXIS);
    fontVariationInstance = AniFindClass(env, ANI_CLASS_FONT_VARIATION_INSTANCE);

    if (fontVariationAxis == nullptr) {
        TEXT_LOGE("AniGlobalClass::Init: Failed to find FontVariationAxis class");
    } else {
        TEXT_LOGI("AniGlobalClass::Init: FontVariationAxis class found successfully");
    }

    if (fontVariationInstance == nullptr) {
        TEXT_LOGE("AniGlobalClass::Init: Failed to find FontVariationInstance class");
    } else {
        TEXT_LOGI("AniGlobalClass::Init: FontVariationInstance class found successfully");
    }

    typographicBounds = AniFindClass(env, ANI_CLASS_TYPOGRAPHIC_BOUNDS);
    cleaner = AniFindClass(env, ANI_CLASS_CLEANER);
    canvas = AniFindClass(env, ANI_CLASS_CANVAS);
    paragraphStyle = AniFindClass(env, ANI_INTERFACE_PARAGRAPH_STYLE);
    strutStyle = AniFindClass(env, ANI_INTERFACE_STRUT_STYLE);
    textTab = AniFindClass(env, ANI_INTERFACE_TEXT_TAB);
    point = AniFindClass(env, ANI_INTERFACE_POINT);
    path = AniFindClass(env, ANI_CLASS_PATH);
    placeholderSpan = AniFindClass(env, ANI_INTERFACE_PLACEHOLDER_SPAN);
    textLayoutResult = AniFindClass(env, ANI_CLASS_TEXT_LAYOUT_RESULT);
    textRectSize = AniFindClass(env, ANI_CLASS_TEXT_RECT_SIZE);
}

void AniGlobalEnum::Init(ani_env* env)
{
    fontWeight = AniFindEnum(env, ANI_ENUM_FONT_WEIGHT);
    fontWidth = AniFindEnum(env, ANI_ENUM_FONT_WIDTH);
    fontEdging = AniFindEnum(env, ANI_ENUM_FONT_EDGING);
    affinity = AniFindEnum(env, ANI_ENUM_AFFINITY);
    textDirection = AniFindEnum(env, ANI_ENUM_TEXT_DIRECTION);
    fontStyle = AniFindEnum(env, ANI_ENUM_FONT_STYLE);
    textBaseline = AniFindEnum(env, ANI_ENUM_TEXT_BASELINE);
    ellipsisMode = AniFindEnum(env, ANI_ENUM_ELLIPSIS_MODE);
    textDecorationType = AniFindEnum(env, ANI_ENUM_TEXT_DECORATION_TYPE);
    textDecorationStyle = AniFindEnum(env, ANI_ENUM_TEXT_DECORATION_STYLE);
    textBadgeType = AniFindEnum(env, ANI_ENUM_TEXT_BADGE_TYPE);
    lineHeightStyle = AniFindEnum(env, ANI_ENUM_TEXT_LINE_HEIGHT_STYLE_TYPE);
}

void AniGlobalMethod::Init(ani_env* env)
{
    InitBaseMethod(env);
    InitCommonMethod(env);
    InitFontDescriptorMethod(env);
    InitParagraphStyleMethod(env);
    InitStrutStyleMethod(env);
    InitTextStyleMethod(env);
    InitDecorationMethod(env);
    InitTextShadowMethod(env);
    InitFontFeatureMethod(env);
    InitFontVariationMethod(env);
    InitFontVariationAxisMethod(env);
    InitFontVariationInstanceMethod(env);
    InitRectStyleMethod(env);
    InitPlaceholderMethod(env);
    InitGlobalResourceMethod(env);
    InitRangeMethod(env);
    InitPointMethod(env);
    InitTextTabMethod(env);
    InitTextLayoutResultMethod(env);
    InitTextRectSizeMethod(env);
}

void AniGlobalMethod::InitBaseMethod(ani_env* env)
{
    map = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniMap, MAP_KEY);
    mapSet = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniMap, MAP_SET_KEY);
    businessErrorCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().businessError, BUSINESS_ERROR_KEY);
    arrayCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniArray, ARRAY_KEY);
    arraySet = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniArray, ARRAY_SET_KEY);
    doubleCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniDouble, DOUBLE_KEY);
    doubleGet = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniDouble, DOUBLE_GET_KEY);
    intCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniInt, INT_KEY);
    intGet = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniInt, INT_GET_KEY);
    booleanCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniBoolean, BOOLEAN_KEY);
    booleanGet = AniClassFindMethod(env, AniGlobalClass::GetInstance().aniBoolean, BOOLEAN_GET_KEY);
}

void AniGlobalMethod::InitCommonMethod(ani_env* env)
{
    fontCollection = AniClassFindMethod(env, AniGlobalClass::GetInstance().fontCollection, FONT_COLLECTION_KEY);
    fontCollectionGetNative =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontCollection, FONT_COLLECTION_GET_NATIVE_KEY);
    paragraphBuilderGetNative =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphBuilder, PARAGRAPH_BUILDER_GET_NATIVE_KEY);
    paragraph = AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraph, PARAGRAPH_KEY);
    paragraphGetNative = AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraph, PARAGRAPH_GET_NATIVE_KEY);
    paragraphBuilderCtor =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphBuilder, PARAGRAPH_BUILDER_KEY);
    run = AniClassFindMethod(env, AniGlobalClass::GetInstance().run, RUN_KEY);
    runGetNative = AniClassFindMethod(env, AniGlobalClass::GetInstance().run, RUN_GET_NATIVE_KEY);
    textLine = AniClassFindMethod(env, AniGlobalClass::GetInstance().textLine, TEXT_LINE_KEY);
    textLineGetNative = AniClassFindMethod(env, AniGlobalClass::GetInstance().textLine, TEXT_LINE_GET_NATIVE_KEY);
    lineTypesetCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().lineTypeSet, LINE_TYPESET_KEY);
    lineTypesetGetNative =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().lineTypeSet, LINE_TYPESET_GET_NATIVE_KEY);
    canvasGetNative = AniClassFindMethod(env, AniGlobalClass::GetInstance().canvas, CANVAS_GET_NATIVE_KEY);
    lineMetricsCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().lineMetrics, LINEMETRICS_KEY);
    positionWithAffinity =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().positionWithAffinity, POSITION_WITH_AFFINITY_KEY);
    pathGetNative = AniClassFindMethod(env, AniGlobalClass::GetInstance().path, PATH_GET_NATIVE_KEY);
    runMetricsCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().runMetrics, RUNMETRICS_KEY);
    fontCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().font, FONT_KEY);
    textBoxCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().textBox, TEXT_BOX_KEY);
    typographicBoundsCtor =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().typographicBounds, TYPOGRAPHIC_BOUNDS_KEY);
}

void AniGlobalMethod::InitFontDescriptorMethod(ani_env* env)
{
    fontDescriptorCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_KEY);
    fontDescriptorGetPath =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_GET_PATH_KEY);
    fontDescriptorGetPostScriptName =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_POST_SCRIPT_NAME_KEY);
    fontDescriptorGetFullName =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_FULL_NAME_KEY);
    fontDescriptorGetFontFamily =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_FONT_FAMILY_KEY);
    fontDescriptorGetFontSubfamily =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_FONT_SUBFAMILY_KEY);
    fontDescriptorGetWeight =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_GET_WEIGHT_KEY);
    fontDescriptorGetWidth =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_WIDTH_KEY);
    fontDescriptorGetItalic =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_ITALIC_KEY);
    fontDescriptorGetMonoSpace =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_MONO_SPACE_KEY);
    fontDescriptorGetSymbolic =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_SYMBOLIC_KEY);
    fontDescriptorGetVariationAxisRecords =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_VARIATION_AXIS_RECORDS_KEY);
    fontDescriptorGetVariationInstanceRecords =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontDescriptor, FONT_DESCRIPTOR_VARIATION_INSTANCE_RECORDS_KEY);
}
void AniGlobalMethod::InitParagraphStyleMethod(ani_env* env)
{
    paragraphStyleMaxLines =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_MAX_LINES_KEY);
    paragraphStyleTextStyle =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_TEXT_STYLE_KEY);
    paragraphStyleTextDirection =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_TEXT_DIRECTION_KEY);
    paragraphStyleAlign =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_ALIGN_KEY);
    paragraphStyleWordBreak =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_WORD_BREAK_KEY);
    paragraphStyleBreakStrategy =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_BREAK_STRATEGY_KEY);
    paragraphStyleTextHeightBehavior =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_TEXT_HEIGHT_BEHAVIOR_KEY);
    paragraphStyleStrutStyle =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_STRUT_STYLE_KEY);
    paragraphStyleTab =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_TAB_KEY);
    paragraphStyleTrailingSpaceOptimized = AniClassFindMethod(
        env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_TRAILING_SPACE_OPTIMIZED_KEY);
    paragraphStyleAutoSpace = AniClassFindMethod(
        env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_AUTO_SPACE_KEY);
    paragraphStyleCompressHeadPunctuation = AniClassFindMethod(
        env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_COMPRESS_HEAD_PUNCTUATION_KEY);
    paragraphStyleVerticalAlign = AniClassFindMethod(
        env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_VERTICAL_ALIGN_KEY);
    paragraphStyleIncludeFontPadding = AniClassFindMethod(
        env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_INCLUDE_FONT_PADDING_KEY);
    paragraphStyleFallbackLineSpacing = AniClassFindMethod(
        env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_FALLBACK_LINE_SPACING_KEY);
    paragraphStyleLineSpacing = AniClassFindMethod(
        env, AniGlobalClass::GetInstance().paragraphStyle, PARAGRAPH_STYLE_LINE_SPACING_KEY);
}
void AniGlobalMethod::InitStrutStyleMethod(ani_env* env)
{
    strutStyleFontStyle = AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_FONT_STYLE_KEY);
    strutStyleFontWidth = AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_FONT_WIDTH_KEY);
    strutStyleFontWeight =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_FONT_WEIGHT_KEY);
    strutStyleFontSize = AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_FONT_SIZE_KEY);
    strutStyleHeight = AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_HEIGHT_KEY);
    strutStyleLeading = AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_LEADING_KEY);
    strutStyleForceHeight =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_FORCE_HEIGHT_KEY);
    strutStyleEnabled = AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_ENABLED_KEY);
    strutStyleHeightOverride =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_HEIGHT_OVERRIDE_KEY);
    strutStyleHalfLeading =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_HALF_LEADING_KEY);
    strutStyleFontFamilies =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().strutStyle, STRUT_STYLE_FONT_FAMILIES_KEY);
}
void AniGlobalMethod::InitTextStyleMethod(ani_env* env)
{
    textStyleCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_KEY);
    textStyleColor = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_COLOR_KEY);
    textStyleFontWeight = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_FONT_WEIGHT_KEY);
    textStyleFontWidth = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_FONT_WIDTH_KEY);
    textStyleFontStyle = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_FONT_STYLE_KEY);
    textStyleBaseline = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_BASELINE_KEY);
    textStyleFontFamilies =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_FONT_FAMILIES_KEY);
    textStyleFontSize = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_FONT_SIZE_KEY);
    textStyleLetterSpacing =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_LETTER_SPACING_KEY);
    textStyleWordSpacing =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_WORD_SPACING_KEY);
    textStyleHeightScale =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_HEIGHT_SCALE_KEY);
    textStyleHalfLeading =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_HALF_LEADING_KEY);
    textStyleHeightOnly = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_HEIGHT_ONLY_KEY);
    textStyleEllipsis = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_ELLIPSIS_KEY);
    textStyleEllipsisMode =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_ELLIPSIS_MODE_KEY);
    textStyleLocale = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_LOCALE_KEY);
    textStyleBaselineShift =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_BASELINE_SHIFT_KEY);
    textStyleDecoration = AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_DECORATION_KEY);
    textStyleTextShadows =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_TEXT_SHADOWS_KEY);
    textStyleFontFeatures =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_FONT_FEATURES_KEY);
    textStyleFontVariations =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_FONT_VARIATIONS_KEY);
    textStyleBackgroundRect =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_BACKGROUND_RECT_KEY);
    textStyleBadgeType =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_BADGE_TYPE_KEY);
    textStyleLineHeightStyle =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_LINE_HEIGHT_STYLE_KEY);
    textStyleFontEdging =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_FONT_EDGING_KEY);
    textStyleMaxLineHeight =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_MAX_LINE_HEIGHT_KEY);
    textStyleMinLineHeight =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textStyle, TEXT_STYLE_MIN_LINE_HEIGHT_KEY);
}

void AniGlobalMethod::InitDecorationMethod(ani_env* env)
{
    decorationCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().decoration, DECORATION_KEY);
    decorationDecorationType =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().decoration, DECORATION_DECORATION_TYPE_KEY);
    decorationDecorationStyle =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().decoration, DECORATION_DECORATION_STYLE_KEY);
    decorationDecorationThicknessScale =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().decoration, DECORATION_DECORATION_THICKNESS_SCALE_KEY);
    decorationDecorationColor =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().decoration, DECORATION_DECORATION_COLOR_KEY);
}

void AniGlobalMethod::InitTextShadowMethod(ani_env* env)
{
    textShadowCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().textShadow, TEXTSHADOW_KEY);
    textShadowBlurRadius =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().textShadow, TEXTSHADOW_BLUR_RADIUS_KEY);
    textShadowColor = AniClassFindMethod(env, AniGlobalClass::GetInstance().textShadow, TEXTSHADOW_COLOR_KEY);
    textShadowPoint = AniClassFindMethod(env, AniGlobalClass::GetInstance().textShadow, TEXTSHADOW_POINT_KEY);
}

void AniGlobalMethod::InitFontFeatureMethod(ani_env* env)
{
    fontFeatureCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().fontFeature, FONT_FEATURE_KEY);
    fontFeatureName = AniClassFindMethod(env, AniGlobalClass::GetInstance().fontFeature, FONT_FEATURE_NAME_KEY);
    fontFeatureValue = AniClassFindMethod(env, AniGlobalClass::GetInstance().fontFeature, FONT_FEATURE_VALUE_KEY);
}

void AniGlobalMethod::InitFontVariationMethod(ani_env* env)
{
    fontVariationCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().fontVariation, FONT_VARIATION_KEY);
    fontVariationAxis = AniClassFindMethod(env, AniGlobalClass::GetInstance().fontVariation, FONT_VARIATION_AXIS_KEY);
    fontVariationValue = AniClassFindMethod(env, AniGlobalClass::GetInstance().fontVariation, FONT_VARIATION_VALUE_KEY);
    fontVariationIsNormalized =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().fontVariation, FONT_VARIATION_IS_NORMALIZED_KEY);
}

void AniGlobalMethod::InitRectStyleMethod(ani_env* env)
{
    rectStyleCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().rectStyle, RECT_STYLE_KEY);
    rectStyleColor = AniClassFindMethod(env, AniGlobalClass::GetInstance().rectStyle, RECT_STYLE_COLOR_KEY);
    rectStyleLeftTopRadius =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().rectStyle, RECT_STYLE_LEFT_TOP_RADIUS_KEY);
    rectStyleRightTopRadius =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().rectStyle, RECT_STYLE_RIGHT_TOP_RADIUS_KEY);
    rectStyleRightBottomRadius =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().rectStyle, RECT_STYLE_RIGHT_BOTTOM_RADIUS_KEY);
    rectStyleLeftBottomRadius =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().rectStyle, RECT_STYLE_LEFT_BOTTOM_RADIUS_KEY);
}

void AniGlobalMethod::InitPlaceholderMethod(ani_env* env)
{
    placeholderSpanWidth =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().placeholderSpan, PLACEHOLDER_SPAN_WIDTH_KEY);
    placeholderSpanHeight =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().placeholderSpan, PLACEHOLDER_SPAN_HEIGHT_KEY);
    placeholderSpanAlign =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().placeholderSpan, PLACEHOLDER_SPAN_ALIGN_KEY);
    placeholderSpanBaseline =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().placeholderSpan, PLACEHOLDER_SPAN_BASELINE_KEY);
    placeholderSpanBaselineOffset =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().placeholderSpan, PLACEHOLDER_SPAN_BASELINE_OFFSET_KEY);
}

void AniGlobalMethod::InitGlobalResourceMethod(ani_env* env)
{
    globalResourceId = AniClassFindMethod(env, AniGlobalClass::GetInstance().globalResource, GLOBAL_RESOURCE_ID_KEY);
    globalResourceBundleName =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().globalResource, GLOBAL_RESOURCE_BUNDLE_NAME_KEY);
    globalResourceModuleName =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().globalResource, GLOBAL_RESOURCE_MODULE_NAME_KEY);
    globalResourceParams =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().globalResource, GLOBAL_RESOURCE_PARAMS_KEY);
    globalResourceType =
        AniClassFindMethod(env, AniGlobalClass::GetInstance().globalResource, GLOBAL_RESOURCE_TYPE_KEY);
}

void AniGlobalMethod::InitRangeMethod(ani_env* env)
{
    rangeCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().range, RANGE_KEY);
    rangeStart = AniClassFindMethod(env, AniGlobalClass::GetInstance().range, RANGE_START_KEY);
    rangeEnd = AniClassFindMethod(env, AniGlobalClass::GetInstance().range, RANGE_END_KEY);
}

void AniGlobalMethod::InitPointMethod(ani_env* env)
{
    pointX = AniClassFindMethod(env, AniGlobalClass::GetInstance().point, POINT_X_KEY);
    pointY = AniClassFindMethod(env, AniGlobalClass::GetInstance().point, POINT_Y_KEY);
}
void AniGlobalMethod::InitTextTabMethod(ani_env* env)
{
    textTabAlignment = AniClassFindMethod(env, AniGlobalClass::GetInstance().textTab, TEXT_TAB_ALIGNMENT_KEY);
    textTabLocation = AniClassFindMethod(env, AniGlobalClass::GetInstance().textTab, TEXT_TAB_LOCATION_KEY);
}

void AniGlobalMethod::InitTextLayoutResultMethod(ani_env* env)
{
    textLayoutResultCtor = AniClassFindMethod(
        env, AniGlobalClass::GetInstance().textLayoutResult, TEXT_LAYOUT_RESULT_KEY);
}

void AniGlobalMethod::InitTextRectSizeMethod(ani_env* env)
{
    textRectSizeCtor = AniClassFindMethod(env, AniGlobalClass::GetInstance().textRectSize, TEXT_RECT_SIZE_KEY);
    textRectSizeWidth = AniClassFindMethod(env, AniGlobalClass::GetInstance().textRectSize, TEXT_RECT_SIZE_WIDTH_KEY);
    textRectSizeHeight = AniClassFindMethod(env, AniGlobalClass::GetInstance().textRectSize, TEXT_RECT_SIZE_HEIGHT_KEY);
}

ani_status InitAniGlobalRef(ani_vm* vm)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ret;
    }
    AniGlobalNamespace::GetInstance().Init(env);
    AniGlobalClass::GetInstance().Init(env);
    AniGlobalEnum::GetInstance().Init(env);
    AniGlobalMethod::GetInstance().Init(env);
    return ANI_OK;
}
} // namespace OHOS::Text::ANI