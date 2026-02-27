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
#ifndef OHOS_TEXT_ANI_COMMON_H
#define OHOS_TEXT_ANI_COMMON_H

#include <ani.h>
#include <cstdint>

#include "utils/include/error_code.h"
namespace OHOS::Text::ANI {
#define ANI_WRAP_RETURN_C(name) (":C{" name "}")
#define ANI_WRAP_RETURN_E(name) (":E{" name "}")

/**
 * define namespace
 */
#define ANI_NAMESPACE_TEXT "@ohos.graphics.text.text"

/**
 * define ani
 */
#define TEXT_NATIVE_OBJ "nativeObj"
#define TEXT_BIND_NATIVE "bindNative"
#define TEXT_GET_NATIVE "getNative"
#define ANI_DOUBLE "std.core.Double"
#define ANI_INT "std.core.Int"
#define ANI_BOOLEAN "std.core.Boolean"
#define ANI_STRING "std.core.String"
#define ANI_MAP "std.core.Map"
#define ANI_ARRAY "std.core.Array"
#define ANI_BUSINESS_ERROR "@ohos.base.BusinessError"
#define ANI_GLOBAL_RESOURCE "global.resource.Resource"
/**
 * define interface
 */
#define ANI_INTERFACE_PARAGRAPH_STYLE "@ohos.graphics.text.text.ParagraphStyle"
#define ANI_INTERFACE_TEXT_STYLE "@ohos.graphics.text.text.TextStyle"
#define ANI_INTERFACE_STRUT_STYLE "@ohos.graphics.text.text.StrutStyle"
#define ANI_INTERFACE_TEXT_TAB "@ohos.graphics.text.text.TextTab"
#define ANI_INTERFACE_FONT_FEATURE "@ohos.graphics.text.text.FontFeature"
#define ANI_INTERFACE_FONT_VARIATION "@ohos.graphics.text.text.FontVariation"
#define ANI_INTERFACE_FONT_VARIATION_AXIS "@ohos.graphics.text.text.FontVariationAxis"
#define ANI_INTERFACE_FONT_VARIATION_INSTANCE "@ohos.graphics.text.text.FontVariationInstance"
#define ANI_INTERFACE_TEXTSHADOW "@ohos.graphics.text.text.TextShadow"
#define ANI_INTERFACE_RECT_STYLE "@ohos.graphics.text.text.RectStyle"
#define ANI_INTERFACE_TEXT_BOX "@ohos.graphics.text.text.TextBox"
#define ANI_INTERFACE_RANGE "@ohos.graphics.text.text.Range"
#define ANI_INTERFACE_PLACEHOLDER_SPAN "@ohos.graphics.text.text.PlaceholderSpan"
#define ANI_INTERFACE_TYPOGRAPHIC_BOUNDS "@ohos.graphics.text.text.TypographicBounds"
#define ANI_INTERFACE_POINT "@ohos.graphics.common2D.common2D.Point"
#define ANI_INTERFACE_RECT "@ohos.graphics.common2D.common2D.Rect"
#define ANI_INTERFACE_COLOR "@ohos.graphics.common2D.common2D.Color"
#define ANI_INTERFACE_POSITION_WITH_AFFINITY "@ohos.graphics.text.text.PositionWithAffinity"
#define ANI_INTERFACE_FONT_DESCRIPTOR "@ohos.graphics.text.text.FontDescriptor"
#define ANI_INTERFACE_DECORATION "@ohos.graphics.text.text.Decoration"
#define ANI_INTERFACE_FONT_METRICS "@ohos.graphics.drawing.drawing.FontMetrics"
#define ANI_INTERFACE_TEXT_RECT_SIZE "@ohos.graphics.text.text.TextRectSize"
#define ANI_INTERFACE_TEXT_LAYOUT_RESULT "@ohos.graphics.text.text.TextLayoutResult"
/**
 * define class
 */
#define ANI_CLASS_TEXT_RECT_SIZE "@ohos.graphics.text.text.TextRectSizeInternal"
#define ANI_CLASS_TEXT_LAYOUT_RESULT "@ohos.graphics.text.text.TextLayoutResultInternal"
#define ANI_CLASS_CLEANER "@ohos.graphics.text.text.Cleaner"
#define ANI_CLASS_PARAGRAPH "@ohos.graphics.text.text.Paragraph"
#define ANI_CLASS_PARAGRAPH_BUILDER "@ohos.graphics.text.text.ParagraphBuilder"
#define ANI_CLASS_LINEMETRICS "@ohos.graphics.text.text.LineMetricsInternal"
#define ANI_CLASS_RUNMETRICS "@ohos.graphics.text.text.RunMetricsInternal"
#define ANI_CLASS_TEXT_STYLE "@ohos.graphics.text.text.TextStyleInternal"
#define ANI_CLASS_FONT_COLLECTION "@ohos.graphics.text.text.FontCollection"
#define ANI_CLASS_FONT_FEATURE "@ohos.graphics.text.text.FontFeatureInternal"
#define ANI_CLASS_FONT_VARIATION "@ohos.graphics.text.text.FontVariationInternal"
#define ANI_CLASS_FONT_VARIATION_AXIS "@ohos.graphics.text.text.FontVariationAxisInternal"
#define ANI_CLASS_FONT_VARIATION_INSTANCE "@ohos.graphics.text.text.FontVariationInstanceInternal"
#define ANI_CLASS_CANVAS "@ohos.graphics.drawing.drawing.Canvas"
#define ANI_CLASS_PATH "@ohos.graphics.drawing.drawing.Path"
#define ANI_CLASS_FONT_METRICS "@ohos.graphics.drawing.drawing.FontMetricsInner"
#define ANI_CLASS_FONT "@ohos.graphics.drawing.drawing.Font"
#define ANI_CLASS_TEXTSHADOW "@ohos.graphics.text.text.TextShadowInternal"
#define ANI_CLASS_RECT_STYLE "@ohos.graphics.text.text.RectStyleInternal"
#define ANI_CLASS_DECORATION "@ohos.graphics.text.text.DecorationInternal"
#define ANI_CLASS_TEXT_BOX "@ohos.graphics.text.text.TextBoxInternal"
#define ANI_CLASS_POSITION_WITH_AFFINITY "@ohos.graphics.text.text.PositionWithAffinityInternal"
#define ANI_CLASS_RANGE "@ohos.graphics.text.text.RangeInternal"
#define ANI_CLASS_TEXT_LINE "@ohos.graphics.text.text.TextLine"
#define ANI_CLASS_LINE_TYPESET "@ohos.graphics.text.text.LineTypeset"
#define ANI_CLASS_RUN "@ohos.graphics.text.text.Run"
#define ANI_CLASS_TYPOGRAPHIC_BOUNDS "@ohos.graphics.text.text.TypographicBoundsInternal"
#define ANI_CLASS_FONT_DESCRIPTOR "@ohos.graphics.text.text.FontDescriptorInternal"
/**
 * define enum
 */
#define ANI_ENUM_FONT_WEIGHT "@ohos.graphics.text.text.FontWeight"
#define ANI_ENUM_FONT_STYLE "@ohos.graphics.text.text.FontStyle"
#define ANI_ENUM_TEXT_BASELINE "@ohos.graphics.text.text.TextBaseline"
#define ANI_ENUM_ELLIPSIS_MODE "@ohos.graphics.text.text.EllipsisMode"
#define ANI_ENUM_TEXT_DECORATION_TYPE "@ohos.graphics.text.text.TextDecorationType"
#define ANI_ENUM_TEXT_DECORATION_STYLE "@ohos.graphics.text.text.TextDecorationStyle"
#define ANI_ENUM_RECT_WIDTH_STYLE "@ohos.graphics.text.text.RectWidthStyle"
#define ANI_ENUM_RECT_HEIGHT_STYLE "@ohos.graphics.text.text.RectHeightStyle"
#define ANI_ENUM_TEXT_DIRECTION "@ohos.graphics.text.text.TextDirection"
#define ANI_ENUM_AFFINITY "@ohos.graphics.text.text.Affinity"
#define ANI_ENUM_SYSTEM_FONT_TYPE "@ohos.graphics.text.text.SystemFontType"
#define ANI_ENUM_PLACEHOLDER_ALIGNMENT "@ohos.graphics.text.text.PlaceholderAlignment"
#define ANI_ENUM_TEXT_ALIGN "@ohos.graphics.text.text.TextAlign"
#define ANI_ENUM_WORD_BREAK "@ohos.graphics.text.text.WordBreak"
#define ANI_ENUM_BREAK_STRATEGY "@ohos.graphics.text.text.BreakStrategy"
#define ANI_ENUM_TEXT_HEIGHT_BEHAVIOR "@ohos.graphics.text.text.TextHeightBehavior"
#define ANI_ENUM_FONT_WIDTH "@ohos.graphics.text.text.FontWidth"
#define ANI_ENUM_FONT_EDGING "@ohos.graphics.drawing.drawing.FontEdging"
#define ANI_ENUM_TEXT_VERTICAL_ALIGN "@ohos.graphics.text.text.TextVerticalAlign"
#define ANI_ENUM_TEXT_BADGE_TYPE "@ohos.graphics.text.text.TextBadgeType"
#define ANI_ENUM_TEXT_HIGH_CONTRAST "@ohos.graphics.text.text.TextHighContrast"
#define ANI_ENUM_TEXT_UNDEFINED_GLYPH_DISPLAY "@ohos.graphics.text.text.TextUndefinedGlyphDisplay"
#define ANI_ENUM_TEXT_LINE_HEIGHT_STYLE_TYPE "@ohos.graphics.text.text.LineHeightStyle"
#define ANI_ENUM_TEXT_ENCODING "@ohos.graphics.drawing.drawing.TextEncoding"

enum class TextErrorCode : int32_t {
    OK = 0,
    ERROR_NO_PERMISSION = 201,             // the value do not change. It is defined on all system
    ERROR_INVALID_PARAM = 401,             // the value do not change. It is defined on all system
    ERROR_DEVICE_NOT_SUPPORT = 801,        // the value do not change. It is defined on all system
    ERROR_ABNORMAL_PARAM_VALUE = 18600001, // the value do not change. It is defined on color manager system
    ERROR_NO_MEMORY = 8800100,             // no memory
};

using AniTextResult = MLB::TextResult<ani_object>;
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_COMMON_H