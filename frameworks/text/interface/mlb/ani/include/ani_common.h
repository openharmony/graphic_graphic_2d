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

#include <cstdint>

namespace OHOS::Text::ANI {
#define ANI_WRAP_RETURN_C(name) ([]() -> const char* {                                                                 \
    static const std::string wrapped = ":C{" + std::string(name) + "}";                                                \
    return wrapped.c_str();                                                                                            \
}())

#define ANI_WRAP_RETURN_E(name) ([]() -> const char* {                                                                 \
    static const std::string wrapped = ":E{" + std::string(name) + "}";                                                \
    return wrapped.c_str();                                                                                            \
}())

/**
 * define namespace
 */
constexpr const char* ANI_NAMESPACE_TEXT = "@ohos.graphics.text.text";

/**
 * define ani
 */
constexpr const char* NATIVE_OBJ = "nativeObj";
constexpr const char* BIND_NATIVE = "bindNative";
constexpr const char* ANI_DOUBLE = "std.core.Double";
constexpr const char* ANI_INT = "std.core.Int";
constexpr const char* ANI_BOOLEAN = "std.core.Boolean";
constexpr const char* ANI_STRING = "std.core.String";
constexpr const char* ANI_MAP = "std.core.Map";
constexpr const char* ANI_ARRAY = "escompat.Array";
constexpr const char* ANI_BUSINESS_ERROR = "@ohos.base.BusinessError";
constexpr const char* ANI_GLOBAL_RESOURCE = "global.resource.Resource";
/**
 * define interface
 */
constexpr const char* ANI_INTERFACE_PARAGRAPH_STYLE = "@ohos.graphics.text.text.ParagraphStyle";
constexpr const char* ANI_INTERFACE_TEXT_STYLE = "@ohos.graphics.text.text.TextStyle";
constexpr const char* ANI_INTERFACE_STRUT_STYLE = "@ohos.graphics.text.text.StrutStyle";
constexpr const char* ANI_INTERFACE_TEXT_TAB = "@ohos.graphics.text.text.TextTab";
constexpr const char* ANI_INTERFACE_FONT_FEATURE = "@ohos.graphics.text.text.FontFeature";
constexpr const char* ANI_INTERFACE_FONT_VARIATION = "@ohos.graphics.text.text.FontVariation";
constexpr const char* ANI_INTERFACE_TEXTSHADOW = "@ohos.graphics.text.text.TextShadow";
constexpr const char* ANI_INTERFACE_RECT_STYLE = "@ohos.graphics.text.text.RectStyle";
constexpr const char* ANI_INTERFACE_TEXT_BOX = "@ohos.graphics.text.text.TextBox";
constexpr const char* ANI_INTERFACE_RANGE = "@ohos.graphics.text.text.Range";
constexpr const char* ANI_INTERFACE_PLACEHOLDER_SPAN = "@ohos.graphics.text.text.PlaceholderSpan";
constexpr const char* ANI_INTERFACE_TYPOGRAPHIC_BOUNDS = "@ohos.graphics.text.text.TypographicBounds";
constexpr const char* ANI_INTERFACE_POINT = "@ohos.graphics.common2D.common2D.Point";
constexpr const char* ANI_INTERFACE_RECT = "@ohos.graphics.common2D.common2D.Rect";
constexpr const char* ANI_INTERFACE_COLOR = "@ohos.graphics.common2D.common2D.Color";
constexpr const char* ANI_INTERFACE_POSITION_WITH_AFFINITY = "@ohos.graphics.text.text.PositionWithAffinity";
constexpr const char* ANI_INTERFACE_FONT_DESCRIPTOR = "@ohos.graphics.text.text.FontDescriptor";
constexpr const char* ANI_INTERFACE_DECORATION = "@ohos.graphics.text.text.Decoration";
constexpr const char* ANI_INTERFACE_FONT_METRICS = "@ohos.graphics.drawing.drawing.FontMetrics";
/**
 * define class
 */
constexpr const char* ANI_CLASS_CLEANER = "@ohos.graphics.text.text.Cleaner";
constexpr const char* ANI_CLASS_PARAGRAPH = "@ohos.graphics.text.text.Paragraph";
constexpr const char* ANI_CLASS_PARAGRAPH_BUILDER = "@ohos.graphics.text.text.ParagraphBuilder";
constexpr const char* ANI_CLASS_LINEMETRICS = "@ohos.graphics.text.text.LineMetricsInternal";
constexpr const char* ANI_CLASS_RUNMETRICS = "@ohos.graphics.text.text.RunMetricsInternal";
constexpr const char* ANI_CLASS_TEXT_STYLE = "@ohos.graphics.text.text.TextStyleInternal";
constexpr const char* ANI_CLASS_FONT_COLLECTION = "@ohos.graphics.text.text.FontCollection";
constexpr const char* ANI_CLASS_FONT_FEATURE = "@ohos.graphics.text.text.FontFeatureInternal";
constexpr const char* ANI_CLASS_FONT_VARIATION = "@ohos.graphics.text.text.FontVariationInternal";
constexpr const char* ANI_CLASS_CANVAS = "@ohos.graphics.drawing.drawing.Canvas";
constexpr const char* ANI_CLASS_PATH = "@ohos.graphics.drawing.drawing.Path";
constexpr const char* ANI_CLASS_FONT_METRICS = "@ohos.graphics.drawing.drawing.FontMetricsInner";
constexpr const char* ANI_CLASS_FONT = "@ohos.graphics.drawing.drawing.Font";
constexpr const char* ANI_CLASS_TEXTSHADOW = "@ohos.graphics.text.text.TextShadowInternal";
constexpr const char* ANI_CLASS_RECT_STYLE = "@ohos.graphics.text.text.RectStyleInternal";
constexpr const char* ANI_CLASS_DECORATION = "@ohos.graphics.text.text.DecorationInternal";
constexpr const char* ANI_CLASS_TEXT_BOX = "@ohos.graphics.text.text.TextBoxInternal";
constexpr const char* ANI_CLASS_POSITION_WITH_AFFINITY = "@ohos.graphics.text.text.PositionWithAffinityInternal";
constexpr const char* ANI_CLASS_RANGE = "@ohos.graphics.text.text.RangeInternal";
constexpr const char* ANI_CLASS_TEXT_LINE = "@ohos.graphics.text.text.TextLine";
constexpr const char* ANI_CLASS_LINE_TYPESET = "@ohos.graphics.text.text.LineTypeset";
constexpr const char* ANI_CLASS_RUN = "@ohos.graphics.text.text.Run";
constexpr const char* ANI_CLASS_TYPOGRAPHIC_BOUNDS = "@ohos.graphics.text.text.TypographicBoundsInternal";
constexpr const char* ANI_CLASS_FONT_DESCRIPTOR = "@ohos.graphics.text.text.FontDescriptorInternal";
/**
 * define enum
 */
constexpr const char* ANI_ENUM_FONT_WEIGHT = "@ohos.graphics.text.text.FontWeight";
constexpr const char* ANI_ENUM_FONT_STYLE = "@ohos.graphics.text.text.FontStyle";
constexpr const char* ANI_ENUM_TEXT_BASELINE = "@ohos.graphics.text.text.TextBaseline";
constexpr const char* ANI_ENUM_ELLIPSIS_MODE = "@ohos.graphics.text.text.EllipsisMode";
constexpr const char* ANI_ENUM_TEXT_DECORATION_TYPE = "@ohos.graphics.text.text.TextDecorationType";
constexpr const char* ANI_ENUM_TEXT_DECORATION_STYLE = "@ohos.graphics.text.text.TextDecorationStyle";
constexpr const char* ANI_ENUM_RECT_WIDTH_STYLE = "@ohos.graphics.text.text.RectWidthStyle";
constexpr const char* ANI_ENUM_RECT_HEIGHT_STYLE = "@ohos.graphics.text.text.RectHeightStyle";
constexpr const char* ANI_ENUM_TEXT_DIRECTION = "@ohos.graphics.text.text.TextDirection";
constexpr const char* ANI_ENUM_AFFINITY = "@ohos.graphics.text.text.Affinity";
constexpr const char* ANI_ENUM_SYSTEM_FONT_TYPE = "@ohos.graphics.text.text.SystemFontType";
constexpr const char* ANI_ENUM_PLACEHOLDER_ALIGNMENT = "@ohos.graphics.text.text.PlaceholderAlignment";
constexpr const char* ANI_ENUM_TEXT_ALIGN = "@ohos.graphics.text.text.TextAlign";
constexpr const char* ANI_ENUM_WORD_BREAK = "@ohos.graphics.text.text.WordBreak";
constexpr const char* ANI_ENUM_BREAK_STRATEGY = "@ohos.graphics.text.text.BreakStrategy";
constexpr const char* ANI_ENUM_TEXT_HEIGHT_BEHAVIOR = "@ohos.graphics.text.text.TextHeightBehavior";
constexpr const char* ANI_ENUM_FONT_WIDTH = "@ohos.graphics.text.text.FontWidth";

enum class TextErrorCode : int32_t {
    OK = 0,
    ERROR_NO_PERMISSION = 201,             // the value do not change. It is defined on all system
    ERROR_INVALID_PARAM = 401,             // the value do not change. It is defined on all system
    ERROR_DEVICE_NOT_SUPPORT = 801,        // the value do not change. It is defined on all system
    ERROR_ABNORMAL_PARAM_VALUE = 18600001, // the value do not change. It is defined on color manager system
    ERROR_NO_MEMORY = 8800100,             // no memory
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_COMMON_H