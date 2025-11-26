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
namespace 
{
constexpr CacheKey TRANS_TO_REQUIRED_KEY{ANI_NAMESPACE_TEXT, "transToRequired",
    "C{@ohos.graphics.text.text.TextStyle}:C{@ohos.graphics.text.text.TextStyleR}"};
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
                                                        "C{" ANI_STRING "}" "C{" ANI_STRING "}"
                                                        "E{" ANI_ENUM_FONT_WEIGHT "}iizz:";
constexpr CacheKey FONT_DESCRIPTOR_KEY{ANI_CLASS_FONT_DESCRIPTOR, "<ctor>", FONT_DESCRIPTOR_SIGN};
constexpr CacheKey FONT_DESCRIPTOR_POST_SCRIPT_NAME_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>postScriptName", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_DESCRIPTOR_FULL_NAME_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>fullName", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_DESCRIPTOR_FONT_FAMILY_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>fontFamily", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_DESCRIPTOR_FONT_SUBFAMILY_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>fontSubfamily", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_DESCRIPTOR_WIDTH_KEY{ANI_INTERFACE_FONT_DESCRIPTOR, "<get>width", ANI_WRAP_RETURN_C(ANI_INT)};
constexpr CacheKey FONT_DESCRIPTOR_ITALIC_KEY{ANI_INTERFACE_FONT_DESCRIPTOR, "<get>italic", ANI_WRAP_RETURN_C(ANI_INT)};
constexpr CacheKey FONT_DESCRIPTOR_MONO_SPACE_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>monoSpace", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey FONT_DESCRIPTOR_SYMBOLIC_KEY{
    ANI_INTERFACE_FONT_DESCRIPTOR, "<get>symbolic", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
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

constexpr CacheKey TEXT_STYLE_R_COLOR_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};
constexpr CacheKey TEXT_STYLE_R_FONT_WEIGHT_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontWeight", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WEIGHT)};
constexpr CacheKey TEXT_STYLE_R_FONT_STYLE_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontStyle", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_STYLE)};
constexpr CacheKey TEXT_STYLE_R_BASELINE_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>baseline", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BASELINE)};
constexpr CacheKey TEXT_STYLE_R_FONT_FAMILIES_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontFamilies", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_R_FONT_SIZE_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>fontSize", ":d"};
constexpr CacheKey TEXT_STYLE_R_LETTER_SPACING_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>letterSpacing", ":d"};
constexpr CacheKey TEXT_STYLE_R_WORD_SPACING_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>wordSpacing", ":d"};
constexpr CacheKey TEXT_STYLE_R_HEIGHT_SCALE_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>heightScale", ":d"};
constexpr CacheKey TEXT_STYLE_R_HALF_LEADING_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>halfLeading", ":z"};
constexpr CacheKey TEXT_STYLE_R_HEIGHT_ONLY_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>heightOnly", ":z"};
constexpr CacheKey TEXT_STYLE_R_ELLIPSIS_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>ellipsis", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey TEXT_STYLE_R_ELLIPSIS_MODE_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>ellipsisMode", ANI_WRAP_RETURN_E(ANI_ENUM_ELLIPSIS_MODE)};
constexpr CacheKey TEXT_STYLE_R_LOCALE_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>locale", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey TEXT_STYLE_R_BASELINE_SHIFT_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>baselineShift", ":d"};
constexpr CacheKey TEXT_STYLE_R_BACKGROUND_RECT_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>backgroundRect", ANI_WRAP_RETURN_C(ANI_INTERFACE_RECT_STYLE)};
constexpr CacheKey TEXT_STYLE_R_DECORATION_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>decoration", ANI_WRAP_RETURN_C(ANI_INTERFACE_DECORATION)};
constexpr CacheKey TEXT_STYLE_R_TEXT_SHADOWS_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>textShadows", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_R_FONT_FEATURES_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontFeatures", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_R_FONT_VARIATIONS_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontVariations", ANI_WRAP_RETURN_C(ANI_ARRAY)};

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
constexpr CacheKey FONT_VARIATION_VALUE_KEY{ANI_INTERFACE_FONT_VARIATION, "<get>value", ":d"};

constexpr CacheKey RECT_STYLE_COLOR_KEY{ANI_INTERFACE_RECT_STYLE, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};
constexpr CacheKey RECT_STYLE_LEFT_TOP_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>leftTopRadius", ":d"};
constexpr CacheKey RECT_STYLE_RIGHT_TOP_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>rightTopRadius", ":d"};
constexpr CacheKey RECT_STYLE_RIGHT_BOTTOM_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>rightBottomRadius", ":d"};
constexpr CacheKey RECT_STYLE_LEFT_BOTTOM_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>leftBottomRadius", ":d"};

constexpr std::string_view TEXT_STYLE_SIGN =
    "C{" ANI_INTERFACE_DECORATION "}C{" ANI_INTERFACE_COLOR "}E{" ANI_ENUM_FONT_WEIGHT "}E{" ANI_ENUM_FONT_STYLE
    "}E{" ANI_ENUM_TEXT_BASELINE "}C{" ANI_ARRAY "}ddddzzC{" ANI_STRING "}E{" ANI_ENUM_ELLIPSIS_MODE "}C{" ANI_STRING
    "}dC{" ANI_ARRAY "}C{" ANI_ARRAY "}C{" ANI_INTERFACE_RECT_STYLE "}:";
constexpr CacheKey TEXT_STYLE_KEY{ANI_CLASS_TEXT_STYLE, "<ctor>", TEXT_STYLE_SIGN};

constexpr std::string_view TEXT_SHADOW_SIGN = "C{" ANI_INTERFACE_COLOR "}C{" ANI_INTERFACE_POINT "}d:";
constexpr CacheKey TEXTSHADOW_KEY{ANI_CLASS_TEXTSHADOW, "<ctor>", TEXT_SHADOW_SIGN};

constexpr std::string_view DECORATION_SIGN =
    "E{" ANI_ENUM_TEXT_DECORATION_TYPE "}C{" ANI_INTERFACE_COLOR "}E{" ANI_ENUM_TEXT_DECORATION_STYLE "}d:";
constexpr CacheKey DECORATION_KEY{ANI_CLASS_DECORATION, "<ctor>", DECORATION_SIGN};

constexpr CacheKey RECT_STYLE_KEY{ANI_CLASS_RECT_STYLE, "<ctor>", "C{" ANI_INTERFACE_COLOR "}dddd:"};

constexpr CacheKey FONT_FEATURE_KEY{ANI_CLASS_FONT_FEATURE, "<ctor>", "C{" ANI_STRING "}i:"};
constexpr CacheKey FONT_VARIATION_KEY{ANI_CLASS_FONT_VARIATION, "<ctor>", ":"};

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
constexpr CacheKey TEXT_BOX_KEY{
    ANI_CLASS_TEXT_BOX, "<ctor>", "C{" ANI_INTERFACE_RECT "}C{" ANI_ENUM_TEXT_DIRECTION "}:"};
constexpr CacheKey RANGE_KEY{ANI_CLASS_RANGE, "<ctor>", "ii:"};

constexpr CacheKey TYPOGRAPHIC_BOUNDS_KEY{ANI_CLASS_TYPOGRAPHIC_BOUNDS, "<ctor>", "dddd:"};

constexpr CacheKey LINE_TYPESET_GET_NATIVE_KEY{ANI_CLASS_LINE_TYPESET, TEXT_GET_NATIVE, ":l"};
} // namespace name
    
ani_namespace AniGlobalNamespace::text = nullptr;

ani_class AniGlobalClass::aniString = nullptr;
ani_class AniGlobalClass::aniArray = nullptr;
ani_class AniGlobalClass::aniMap = nullptr;
ani_class AniGlobalClass::aniDouble = nullptr;
ani_class AniGlobalClass::aniInt = nullptr;
ani_class AniGlobalClass::aniBoolean = nullptr;
ani_class AniGlobalClass::businessError = nullptr;
ani_class AniGlobalClass::globalResource = nullptr;
ani_class AniGlobalClass::fontCollection = nullptr;
ani_class AniGlobalClass::fontDescriptor = nullptr;
ani_class AniGlobalClass::paragraphBuilder = nullptr;
ani_class AniGlobalClass::paragraph = nullptr;
ani_class AniGlobalClass::run = nullptr;
ani_class AniGlobalClass::textLine = nullptr;
ani_class AniGlobalClass::lineTypeSet = nullptr;
ani_class AniGlobalClass::positionWithAffinity = nullptr;
ani_class AniGlobalClass::lineMetrics = nullptr;
ani_class AniGlobalClass::runMetrics = nullptr;
ani_class AniGlobalClass::font = nullptr;
ani_class AniGlobalClass::textBox = nullptr;
ani_class AniGlobalClass::range = nullptr;
ani_class AniGlobalClass::textStyle = nullptr;
ani_class AniGlobalClass::textShadow = nullptr;
ani_class AniGlobalClass::decoration = nullptr;
ani_class AniGlobalClass::rectStyle = nullptr;
ani_class AniGlobalClass::fontFeature = nullptr;
ani_class AniGlobalClass::fontVariation = nullptr;
ani_class AniGlobalClass::typographicBounds = nullptr;
ani_class AniGlobalClass::cleaner = nullptr;

ani_enum AniGlobalEnum::fontWeight = nullptr;
ani_enum AniGlobalEnum::affinity = nullptr;
ani_enum AniGlobalEnum::textDirection = nullptr;
ani_enum AniGlobalEnum::fontStyle = nullptr;
ani_enum AniGlobalEnum::textBaseline = nullptr;
ani_enum AniGlobalEnum::ellipsisMode = nullptr;
ani_enum AniGlobalEnum::textDecorationType = nullptr;
ani_enum AniGlobalEnum::textDecorationStyle = nullptr;

ani_method AniGlobalMethod::map = nullptr;
ani_method AniGlobalMethod::mapSet = nullptr;
ani_method AniGlobalMethod::businessErrorCtor = nullptr;
ani_method AniGlobalMethod::arrayCtor = nullptr;
ani_method AniGlobalMethod::arraySet = nullptr;
ani_method AniGlobalMethod::doubleCtor = nullptr;
ani_method AniGlobalMethod::doubleGet = nullptr;
ani_method AniGlobalMethod::intCtor = nullptr;
ani_method AniGlobalMethod::intGet = nullptr;
ani_method AniGlobalMethod::booleanCtor = nullptr;
ani_method AniGlobalMethod::booleanGet = nullptr;
ani_method AniGlobalMethod::fontCollection = nullptr;
ani_method AniGlobalMethod::fontCollectionGetNative = nullptr;
ani_method AniGlobalMethod::paragraphBuilderGetNative = nullptr;
ani_method AniGlobalMethod::paragraph = nullptr;
ani_method AniGlobalMethod::paragraphGetNative = nullptr;
ani_method AniGlobalMethod::paragraphBuilderCtor = nullptr;
ani_method AniGlobalMethod::run = nullptr;
ani_method AniGlobalMethod::runGetNative = nullptr;
ani_method AniGlobalMethod::textLine = nullptr;
ani_method AniGlobalMethod::textLineGetNative = nullptr;
ani_method AniGlobalMethod::lineTypesetCtor = nullptr;
ani_method AniGlobalMethod::lineTypesetGetNative = nullptr;
ani_method AniGlobalMethod::fontDescriptorCtor = nullptr;
ani_method AniGlobalMethod::canvasGetNative = nullptr;
ani_method AniGlobalMethod::lineMetricsCtor = nullptr;
ani_method AniGlobalMethod::fontDescriptorGetPostScriptName = nullptr;
ani_method AniGlobalMethod::fontDescriptorGetFullName = nullptr;
ani_method AniGlobalMethod::fontDescriptorGetFontFamily = nullptr;
ani_method AniGlobalMethod::fontDescriptorGetFontSubfamily = nullptr;
ani_method AniGlobalMethod::fontDescriptorGetWidth = nullptr;
ani_method AniGlobalMethod::fontDescriptorGetItalic = nullptr;
ani_method AniGlobalMethod::fontDescriptorGetMonoSpace = nullptr;
ani_method AniGlobalMethod::fontDescriptorGetSymbolic = nullptr;
ani_method AniGlobalMethod::positionWithAffinity = nullptr;
ani_method AniGlobalMethod::paragraphStyleMaxLines = nullptr;
ani_method AniGlobalMethod::paragraphStyleTextStyle = nullptr;
ani_method AniGlobalMethod::paragraphStyleTextDirection = nullptr;
ani_method AniGlobalMethod::paragraphStyleAlign = nullptr;
ani_method AniGlobalMethod::paragraphStyleWordBreak = nullptr;
ani_method AniGlobalMethod::paragraphStyleBreakStrategy = nullptr;
ani_method AniGlobalMethod::paragraphStyleTextHeightBehavior = nullptr;
ani_method AniGlobalMethod::paragraphStyleStrutStyle = nullptr;
ani_method AniGlobalMethod::paragraphStyleTab = nullptr;
ani_method AniGlobalMethod::strutStyleFontStyle = nullptr;
ani_method AniGlobalMethod::strutStyleFontWidth = nullptr;
ani_method AniGlobalMethod::strutStyleFontWeight = nullptr;
ani_method AniGlobalMethod::strutStyleFontSize = nullptr;
ani_method AniGlobalMethod::strutStyleHeight = nullptr;
ani_method AniGlobalMethod::strutStyleLeading = nullptr;
ani_method AniGlobalMethod::strutStyleForceHeight = nullptr;
ani_method AniGlobalMethod::strutStyleEnabled = nullptr;
ani_method AniGlobalMethod::strutStyleHeightOverride = nullptr;
ani_method AniGlobalMethod::strutStyleHalfLeading = nullptr;
ani_method AniGlobalMethod::strutStyleFontFamilies = nullptr;
ani_method AniGlobalMethod::textTabAlignment = nullptr;
ani_method AniGlobalMethod::textTabLocation = nullptr;

ani_method AniGlobalMethod::textStyleCtor = nullptr;
ani_method AniGlobalMethod::textStyleRColor = nullptr;
ani_method AniGlobalMethod::textStyleRFontWeight = nullptr;
ani_method AniGlobalMethod::textStyleRFontStyle = nullptr;
ani_method AniGlobalMethod::textStyleRBaseline = nullptr;
ani_method AniGlobalMethod::textStyleRFontFamilies = nullptr;
ani_method AniGlobalMethod::textStyleRFontSize = nullptr;
ani_method AniGlobalMethod::textStyleRLetterSpacing = nullptr;
ani_method AniGlobalMethod::textStyleRWordSpacing = nullptr;
ani_method AniGlobalMethod::textStyleRHeightScale = nullptr;
ani_method AniGlobalMethod::textStyleRHalfLeading = nullptr;
ani_method AniGlobalMethod::textStyleRHeightOnly = nullptr;
ani_method AniGlobalMethod::textStyleREllipsis = nullptr;
ani_method AniGlobalMethod::textStyleREllipsisMode = nullptr;
ani_method AniGlobalMethod::textStyleRLocale = nullptr;
ani_method AniGlobalMethod::textStyleRBaselineShift = nullptr;
ani_method AniGlobalMethod::textStyleRDecoration = nullptr;
ani_method AniGlobalMethod::textStyleRTextShadows = nullptr;
ani_method AniGlobalMethod::textStyleRFontFeatures = nullptr;
ani_method AniGlobalMethod::textStyleRFontVariations = nullptr;
ani_method AniGlobalMethod::textStyleRBackgroundRect = nullptr;
ani_method AniGlobalMethod::decorationCtor = nullptr;
ani_method AniGlobalMethod::decorationDecorationType = nullptr;
ani_method AniGlobalMethod::decorationDecorationStyle = nullptr;
ani_method AniGlobalMethod::decorationDecorationThicknessScale = nullptr;
ani_method AniGlobalMethod::decorationDecorationColor = nullptr;
ani_method AniGlobalMethod::pointX = nullptr;
ani_method AniGlobalMethod::pointY = nullptr;
ani_method AniGlobalMethod::textShadowCtor = nullptr;
ani_method AniGlobalMethod::textShadowBlurRadius = nullptr;
ani_method AniGlobalMethod::textShadowColor = nullptr;
ani_method AniGlobalMethod::textShadowPoint = nullptr;
ani_method AniGlobalMethod::fontFeatureCtor = nullptr;
ani_method AniGlobalMethod::fontFeatureName = nullptr;
ani_method AniGlobalMethod::fontFeatureValue = nullptr;
ani_method AniGlobalMethod::fontVariationCtor = nullptr;
ani_method AniGlobalMethod::fontVariationAxis = nullptr;
ani_method AniGlobalMethod::fontVariationValue = nullptr;
ani_method AniGlobalMethod::rectStyleCtor = nullptr;
ani_method AniGlobalMethod::rectStyleColor = nullptr;
ani_method AniGlobalMethod::rectStyleLeftTopRadius = nullptr;
ani_method AniGlobalMethod::rectStyleRightTopRadius = nullptr;
ani_method AniGlobalMethod::rectStyleRightBottomRadius = nullptr;
ani_method AniGlobalMethod::rectStyleLeftBottomRadius = nullptr;
ani_method AniGlobalMethod::pathGetNative = nullptr;
ani_method AniGlobalMethod::placeholderSpanWidth = nullptr;
ani_method AniGlobalMethod::placeholderSpanHeight = nullptr;
ani_method AniGlobalMethod::placeholderSpanAlign = nullptr;
ani_method AniGlobalMethod::placeholderSpanBaseline = nullptr;
ani_method AniGlobalMethod::placeholderSpanBaselineOffset = nullptr;
ani_method AniGlobalMethod::globalResourceId = nullptr;
ani_method AniGlobalMethod::globalResourceBundleName = nullptr;
ani_method AniGlobalMethod::globalResourceModuleName = nullptr;
ani_method AniGlobalMethod::globalResourceParams = nullptr;
ani_method AniGlobalMethod::globalResourceType = nullptr;
ani_method AniGlobalMethod::runMetricsCtor = nullptr;
ani_method AniGlobalMethod::fontCtor = nullptr;
ani_method AniGlobalMethod::rangeCtor = nullptr;
ani_method AniGlobalMethod::rangeStart = nullptr;
ani_method AniGlobalMethod::rangeEnd = nullptr;
ani_method AniGlobalMethod::textBoxCtor = nullptr;
ani_method AniGlobalMethod::typographicBoundsCtor = nullptr;

ani_function AniGlobalFunction::transToRequired = nullptr;

void InitNamespace(ani_env* env)
{
    AniGlobalNamespace::text = ANI_FIND_NAMESPACE(env, ANI_NAMESPACE_TEXT);
}

void InitClass(ani_env* env)
{
    AniGlobalClass::aniString = ANI_FIND_CLASS(env, ANI_STRING);
    AniGlobalClass::aniArray = ANI_FIND_CLASS(env, ANI_ARRAY);
    AniGlobalClass::aniMap = ANI_FIND_CLASS(env, ANI_MAP);
    AniGlobalClass::aniDouble = ANI_FIND_CLASS(env, ANI_DOUBLE);
    AniGlobalClass::aniInt = ANI_FIND_CLASS(env, ANI_INT);
    AniGlobalClass::aniBoolean = ANI_FIND_CLASS(env, ANI_BOOLEAN);
    AniGlobalClass::businessError = ANI_FIND_CLASS(env, ANI_BUSINESS_ERROR);
    AniGlobalClass::globalResource = ANI_FIND_CLASS(env, ANI_GLOBAL_RESOURCE);
    AniGlobalClass::fontCollection = ANI_FIND_CLASS(env, ANI_CLASS_FONT_COLLECTION);
    AniGlobalClass::fontDescriptor = ANI_FIND_CLASS(env, ANI_CLASS_FONT_DESCRIPTOR);
    AniGlobalClass::paragraphBuilder = ANI_FIND_CLASS(env, ANI_CLASS_PARAGRAPH_BUILDER);
    AniGlobalClass::paragraph = ANI_FIND_CLASS(env, ANI_CLASS_PARAGRAPH);
    AniGlobalClass::run = ANI_FIND_CLASS(env, ANI_CLASS_RUN);
    AniGlobalClass::textLine = ANI_FIND_CLASS(env, ANI_CLASS_TEXT_LINE);
    AniGlobalClass::lineTypeSet = ANI_FIND_CLASS(env, ANI_CLASS_LINE_TYPESET);
    AniGlobalClass::positionWithAffinity = ANI_FIND_CLASS(env, ANI_CLASS_POSITION_WITH_AFFINITY);
    AniGlobalClass::lineMetrics = ANI_FIND_CLASS(env, ANI_CLASS_LINEMETRICS);
    AniGlobalClass::runMetrics = ANI_FIND_CLASS(env, ANI_CLASS_RUNMETRICS);
    AniGlobalClass::font = ANI_FIND_CLASS(env, ANI_CLASS_FONT);
    AniGlobalClass::textBox = ANI_FIND_CLASS(env, ANI_CLASS_TEXT_BOX);
    AniGlobalClass::range = ANI_FIND_CLASS(env, ANI_CLASS_RANGE);
    AniGlobalClass::textStyle = ANI_FIND_CLASS(env, ANI_CLASS_TEXT_STYLE);
    AniGlobalClass::textShadow = ANI_FIND_CLASS(env, ANI_CLASS_TEXTSHADOW);
    AniGlobalClass::decoration = ANI_FIND_CLASS(env, ANI_CLASS_DECORATION);
    AniGlobalClass::rectStyle = ANI_FIND_CLASS(env, ANI_CLASS_RECT_STYLE);
    AniGlobalClass::fontFeature = ANI_FIND_CLASS(env, ANI_CLASS_FONT_FEATURE);
    AniGlobalClass::fontVariation = ANI_FIND_CLASS(env, ANI_CLASS_FONT_VARIATION);
    AniGlobalClass::typographicBounds = ANI_FIND_CLASS(env, ANI_CLASS_TYPOGRAPHIC_BOUNDS);
    AniGlobalClass::cleaner = ANI_FIND_CLASS(env, ANI_CLASS_CLEANER);
}

void InitEnum(ani_env* env)
{
    AniGlobalEnum::fontWeight = ANI_FIND_ENUM(env, ANI_ENUM_FONT_WEIGHT);
    AniGlobalEnum::affinity = ANI_FIND_ENUM(env, ANI_ENUM_AFFINITY);
    AniGlobalEnum::textDirection = ANI_FIND_ENUM(env, ANI_ENUM_TEXT_DIRECTION);
    AniGlobalEnum::fontStyle = ANI_FIND_ENUM(env, ANI_ENUM_FONT_STYLE);
    AniGlobalEnum::textBaseline = ANI_FIND_ENUM(env, ANI_ENUM_TEXT_BASELINE);
    AniGlobalEnum::ellipsisMode = ANI_FIND_ENUM(env, ANI_ENUM_ELLIPSIS_MODE);
    AniGlobalEnum::textDecorationType = ANI_FIND_ENUM(env, ANI_ENUM_TEXT_DECORATION_TYPE);
    AniGlobalEnum::textDecorationStyle = ANI_FIND_ENUM(env, ANI_ENUM_TEXT_DECORATION_STYLE);
}

void InitMethod(ani_env* env)
{
    AniGlobalMethod::map = ANI_CLASS_FIND_METHOD(env, MAP_KEY);
    AniGlobalMethod::mapSet = ANI_CLASS_FIND_METHOD(env, MAP_SET_KEY);
    AniGlobalMethod::businessErrorCtor = ANI_CLASS_FIND_METHOD(env, BUSINESS_ERROR_KEY);
    AniGlobalMethod::arrayCtor = ANI_CLASS_FIND_METHOD(env, ARRAY_KEY);
    AniGlobalMethod::arraySet = ANI_CLASS_FIND_METHOD(env, ARRAY_SET_KEY);
    AniGlobalMethod::doubleCtor = ANI_CLASS_FIND_METHOD(env, DOUBLE_KEY);
    AniGlobalMethod::doubleGet = ANI_CLASS_FIND_METHOD(env, DOUBLE_GET_KEY);
    AniGlobalMethod::intCtor = ANI_CLASS_FIND_METHOD(env, INT_KEY);
    AniGlobalMethod::intGet = ANI_CLASS_FIND_METHOD(env, INT_GET_KEY);
    AniGlobalMethod::booleanCtor = ANI_CLASS_FIND_METHOD(env, BOOLEAN_KEY);
    AniGlobalMethod::booleanGet = ANI_CLASS_FIND_METHOD(env, BOOLEAN_GET_KEY);
    AniGlobalMethod::fontCollection = ANI_CLASS_FIND_METHOD(env, FONT_COLLECTION_KEY);
    AniGlobalMethod::fontCollectionGetNative = ANI_CLASS_FIND_METHOD(env, FONT_COLLECTION_GET_NATIVE_KEY);
    AniGlobalMethod::paragraphBuilderGetNative = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_BUILDER_GET_NATIVE_KEY);
    AniGlobalMethod::paragraph = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_KEY);
    AniGlobalMethod::paragraphGetNative = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_GET_NATIVE_KEY);
    AniGlobalMethod::paragraphBuilderCtor = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_BUILDER_KEY);
    AniGlobalMethod::run = ANI_CLASS_FIND_METHOD(env, RUN_KEY);
    AniGlobalMethod::runGetNative = ANI_CLASS_FIND_METHOD(env, RUN_GET_NATIVE_KEY);
    AniGlobalMethod::textLine = ANI_CLASS_FIND_METHOD(env, TEXT_LINE_KEY);
    AniGlobalMethod::textLineGetNative = ANI_CLASS_FIND_METHOD(env, TEXT_LINE_GET_NATIVE_KEY);
    AniGlobalMethod::lineTypesetCtor = ANI_CLASS_FIND_METHOD(env, LINE_TYPESET_KEY);
    AniGlobalMethod::lineTypesetGetNative = ANI_CLASS_FIND_METHOD(env, LINE_TYPESET_GET_NATIVE_KEY);
    AniGlobalMethod::fontDescriptorCtor = ANI_CLASS_FIND_METHOD(env, FONT_DESCRIPTOR_KEY);
    AniGlobalMethod::canvasGetNative = ANI_CLASS_FIND_METHOD(env, CANVAS_GET_NATIVE_KEY);
    AniGlobalMethod::lineMetricsCtor = ANI_CLASS_FIND_METHOD(env, LINEMETRICS_KEY);
    AniGlobalMethod::fontDescriptorGetPostScriptName = ANI_CLASS_FIND_METHOD(env, FONT_DESCRIPTOR_POST_SCRIPT_NAME_KEY);
    AniGlobalMethod::fontDescriptorGetFullName = ANI_CLASS_FIND_METHOD(env, FONT_DESCRIPTOR_FULL_NAME_KEY);
    AniGlobalMethod::fontDescriptorGetFontFamily = ANI_CLASS_FIND_METHOD(env, FONT_DESCRIPTOR_FONT_FAMILY_KEY);
    AniGlobalMethod::fontDescriptorGetFontSubfamily = ANI_CLASS_FIND_METHOD(env, FONT_DESCRIPTOR_FONT_SUBFAMILY_KEY);
    AniGlobalMethod::fontDescriptorGetWidth = ANI_CLASS_FIND_METHOD(env, FONT_DESCRIPTOR_WIDTH_KEY);
    AniGlobalMethod::fontDescriptorGetItalic = ANI_CLASS_FIND_METHOD(env, FONT_DESCRIPTOR_ITALIC_KEY);
    AniGlobalMethod::fontDescriptorGetMonoSpace = ANI_CLASS_FIND_METHOD(env, FONT_DESCRIPTOR_MONO_SPACE_KEY);
    AniGlobalMethod::fontDescriptorGetSymbolic = ANI_CLASS_FIND_METHOD(env, FONT_DESCRIPTOR_SYMBOLIC_KEY);
    AniGlobalMethod::positionWithAffinity = ANI_CLASS_FIND_METHOD(env, POSITION_WITH_AFFINITY_KEY);
    AniGlobalMethod::paragraphStyleMaxLines = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_STYLE_MAX_LINES_KEY);
    AniGlobalMethod::paragraphStyleTextStyle = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_STYLE_TEXT_STYLE_KEY);
    AniGlobalMethod::paragraphStyleTextDirection = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_STYLE_TEXT_DIRECTION_KEY);
    AniGlobalMethod::paragraphStyleAlign = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_STYLE_ALIGN_KEY);
    AniGlobalMethod::paragraphStyleWordBreak = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_STYLE_WORD_BREAK_KEY);
    AniGlobalMethod::paragraphStyleBreakStrategy = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_STYLE_BREAK_STRATEGY_KEY);
    AniGlobalMethod::paragraphStyleTextHeightBehavior = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_STYLE_TEXT_HEIGHT_BEHAVIOR_KEY);
    AniGlobalMethod::paragraphStyleStrutStyle = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_STYLE_STRUT_STYLE_KEY);
    AniGlobalMethod::paragraphStyleTab = ANI_CLASS_FIND_METHOD(env, PARAGRAPH_STYLE_TAB_KEY);
    AniGlobalMethod::strutStyleFontStyle = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_FONT_STYLE_KEY);
    AniGlobalMethod::strutStyleFontWidth = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_FONT_WIDTH_KEY);
    AniGlobalMethod::strutStyleFontWeight = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_FONT_WEIGHT_KEY);
    AniGlobalMethod::strutStyleFontSize = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_FONT_SIZE_KEY);
    AniGlobalMethod::strutStyleHeight = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_HEIGHT_KEY);
    AniGlobalMethod::strutStyleLeading = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_LEADING_KEY);
    AniGlobalMethod::strutStyleForceHeight = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_FORCE_HEIGHT_KEY);
    AniGlobalMethod::strutStyleEnabled = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_ENABLED_KEY);
    AniGlobalMethod::strutStyleHeightOverride = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_HEIGHT_OVERRIDE_KEY);
    AniGlobalMethod::strutStyleHalfLeading = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_HALF_LEADING_KEY);
    AniGlobalMethod::strutStyleFontFamilies = ANI_CLASS_FIND_METHOD(env, STRUT_STYLE_FONT_FAMILIES_KEY);
    AniGlobalMethod::textTabAlignment = ANI_CLASS_FIND_METHOD(env, TEXT_TAB_ALIGNMENT_KEY);
    AniGlobalMethod::textTabLocation = ANI_CLASS_FIND_METHOD(env, TEXT_TAB_LOCATION_KEY);

    AniGlobalMethod::textStyleCtor = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_KEY);
    AniGlobalMethod::textStyleRColor = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_COLOR_KEY);
    AniGlobalMethod::textStyleRFontWeight = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_FONT_WEIGHT_KEY);
    AniGlobalMethod::textStyleRFontStyle = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_FONT_STYLE_KEY);
    AniGlobalMethod::textStyleRBaseline = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_BASELINE_KEY);
    AniGlobalMethod::textStyleRFontFamilies = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_FONT_FAMILIES_KEY);
    AniGlobalMethod::textStyleRFontSize = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_FONT_SIZE_KEY);
    AniGlobalMethod::textStyleRLetterSpacing = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_LETTER_SPACING_KEY);
    AniGlobalMethod::textStyleRWordSpacing = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_WORD_SPACING_KEY);
    AniGlobalMethod::textStyleRHeightScale = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_HEIGHT_SCALE_KEY);
    AniGlobalMethod::textStyleRHalfLeading = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_HALF_LEADING_KEY);
    AniGlobalMethod::textStyleRHeightOnly = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_HEIGHT_ONLY_KEY);
    AniGlobalMethod::textStyleREllipsis = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_ELLIPSIS_KEY);
    AniGlobalMethod::textStyleREllipsisMode = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_ELLIPSIS_MODE_KEY);
    AniGlobalMethod::textStyleRLocale = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_LOCALE_KEY);
    AniGlobalMethod::textStyleRBaselineShift = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_BASELINE_SHIFT_KEY);
    AniGlobalMethod::textStyleRDecoration = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_DECORATION_KEY);
    AniGlobalMethod::textStyleRTextShadows = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_TEXT_SHADOWS_KEY);
    AniGlobalMethod::textStyleRFontFeatures = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_FONT_FEATURES_KEY);
    AniGlobalMethod::textStyleRFontVariations = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_FONT_VARIATIONS_KEY);
    AniGlobalMethod::textStyleRBackgroundRect = ANI_CLASS_FIND_METHOD(env, TEXT_STYLE_R_BACKGROUND_RECT_KEY);
    AniGlobalMethod::decorationCtor = ANI_CLASS_FIND_METHOD(env, DECORATION_KEY);
    AniGlobalMethod::decorationDecorationType = ANI_CLASS_FIND_METHOD(env, DECORATION_DECORATION_TYPE_KEY);
    AniGlobalMethod::decorationDecorationStyle = ANI_CLASS_FIND_METHOD(env, DECORATION_DECORATION_STYLE_KEY);
    AniGlobalMethod::decorationDecorationThicknessScale =ANI_CLASS_FIND_METHOD(env, DECORATION_DECORATION_THICKNESS_SCALE_KEY);
    AniGlobalMethod::decorationDecorationColor = ANI_CLASS_FIND_METHOD(env, DECORATION_DECORATION_COLOR_KEY);
    AniGlobalMethod::pointX = ANI_CLASS_FIND_METHOD(env, POINT_X_KEY);
    AniGlobalMethod::pointY = ANI_CLASS_FIND_METHOD(env, POINT_Y_KEY);
    AniGlobalMethod::textShadowCtor = ANI_CLASS_FIND_METHOD(env, TEXTSHADOW_KEY);
    AniGlobalMethod::textShadowBlurRadius = ANI_CLASS_FIND_METHOD(env, TEXTSHADOW_BLUR_RADIUS_KEY);
    AniGlobalMethod::textShadowColor = ANI_CLASS_FIND_METHOD(env, TEXTSHADOW_COLOR_KEY);
    AniGlobalMethod::textShadowPoint = ANI_CLASS_FIND_METHOD(env, TEXTSHADOW_POINT_KEY);
    AniGlobalMethod::fontFeatureCtor = ANI_CLASS_FIND_METHOD(env, FONT_FEATURE_KEY);
    AniGlobalMethod::fontFeatureName = ANI_CLASS_FIND_METHOD(env, FONT_FEATURE_NAME_KEY);
    AniGlobalMethod::fontFeatureValue = ANI_CLASS_FIND_METHOD(env, FONT_FEATURE_VALUE_KEY);
    AniGlobalMethod::fontVariationCtor = ANI_CLASS_FIND_METHOD(env, FONT_VARIATION_KEY);
    AniGlobalMethod::fontVariationAxis = ANI_CLASS_FIND_METHOD(env, FONT_VARIATION_AXIS_KEY);
    AniGlobalMethod::fontVariationValue = ANI_CLASS_FIND_METHOD(env, FONT_VARIATION_VALUE_KEY);
    AniGlobalMethod::rectStyleCtor = ANI_CLASS_FIND_METHOD(env, RECT_STYLE_KEY);
    AniGlobalMethod::rectStyleColor = ANI_CLASS_FIND_METHOD(env, RECT_STYLE_COLOR_KEY);
    AniGlobalMethod::rectStyleLeftTopRadius = ANI_CLASS_FIND_METHOD(env, RECT_STYLE_LEFT_TOP_RADIUS_KEY);
    AniGlobalMethod::rectStyleRightTopRadius = ANI_CLASS_FIND_METHOD(env, RECT_STYLE_RIGHT_TOP_RADIUS_KEY);
    AniGlobalMethod::rectStyleRightBottomRadius = ANI_CLASS_FIND_METHOD(env, RECT_STYLE_RIGHT_BOTTOM_RADIUS_KEY);
    AniGlobalMethod::rectStyleLeftBottomRadius = ANI_CLASS_FIND_METHOD(env, RECT_STYLE_LEFT_BOTTOM_RADIUS_KEY);

    AniGlobalMethod::pathGetNative = ANI_CLASS_FIND_METHOD(env, PATH_GET_NATIVE_KEY);

    AniGlobalMethod::placeholderSpanWidth = ANI_CLASS_FIND_METHOD(env, PLACEHOLDER_SPAN_WIDTH_KEY);
    AniGlobalMethod::placeholderSpanHeight = ANI_CLASS_FIND_METHOD(env, PLACEHOLDER_SPAN_HEIGHT_KEY);
    AniGlobalMethod::placeholderSpanAlign = ANI_CLASS_FIND_METHOD(env, PLACEHOLDER_SPAN_ALIGN_KEY);
    AniGlobalMethod::placeholderSpanBaseline = ANI_CLASS_FIND_METHOD(env, PLACEHOLDER_SPAN_BASELINE_KEY);
    AniGlobalMethod::placeholderSpanBaselineOffset = ANI_CLASS_FIND_METHOD(env, PLACEHOLDER_SPAN_BASELINE_OFFSET_KEY);

    AniGlobalMethod::globalResourceId = ANI_CLASS_FIND_METHOD(env, GLOBAL_RESOURCE_ID_KEY);
    AniGlobalMethod::globalResourceBundleName = ANI_CLASS_FIND_METHOD(env, GLOBAL_RESOURCE_BUNDLE_NAME_KEY);
    AniGlobalMethod::globalResourceModuleName = ANI_CLASS_FIND_METHOD(env, GLOBAL_RESOURCE_MODULE_NAME_KEY);
    AniGlobalMethod::globalResourceParams = ANI_CLASS_FIND_METHOD(env, GLOBAL_RESOURCE_PARAMS_KEY);
    AniGlobalMethod::globalResourceType = ANI_CLASS_FIND_METHOD(env, GLOBAL_RESOURCE_TYPE_KEY);

    AniGlobalMethod::runMetricsCtor = ANI_CLASS_FIND_METHOD(env, RUNMETRICS_KEY);
    AniGlobalMethod::fontCtor = ANI_CLASS_FIND_METHOD(env, FONT_KEY);

    AniGlobalMethod::rangeCtor = ANI_CLASS_FIND_METHOD(env, RANGE_KEY);
    AniGlobalMethod::rangeStart = ANI_CLASS_FIND_METHOD(env, RANGE_START_KEY);
    AniGlobalMethod::rangeEnd = ANI_CLASS_FIND_METHOD(env, RANGE_END_KEY);

    AniGlobalMethod::textBoxCtor = ANI_CLASS_FIND_METHOD(env, TEXT_BOX_KEY);
    AniGlobalMethod::typographicBoundsCtor = ANI_CLASS_FIND_METHOD(env, TYPOGRAPHIC_BOUNDS_KEY);

}


void InitFunction(ani_env* env)
{
    AniGlobalFunction::transToRequired = ANI_NAMESPACE_FIND_FUNCTION(env, TRANS_TO_REQUIRED_KEY);
}

ani_status InitAniGlobalRef(ani_vm* vm)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ret;
    }
    InitNamespace(env);
    InitClass(env);
    InitEnum(env);
    InitMethod(env);
    InitFunction(env);
    return ANI_OK;
}
} // namespace OHOS::Text::ANI