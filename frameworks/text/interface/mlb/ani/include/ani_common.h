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
/**
 * define namespace
 */
constexpr const char* ANI_NAMESPACE_TEXT = "L@ohos/graphics/text/text;";

/**
 * define ani
 */
constexpr const char* NATIVE_OBJ = "nativeObj";
constexpr const char* ANI_OBJECT = "Lstd/core/Double;";
constexpr const char* ANI_INT = "Lstd/core/Int;";
constexpr const char* ANI_BOOLEAN = "Lstd/core/Boolean;";
constexpr const char* ANI_STRING = "Lstd/core/String;";
constexpr const char* ANI_MAP = "Lescompat/Map;";
constexpr const char* ANI_ARRAY = "Lescompat/Array;";
/**
 * define interface
 */
constexpr const char* ANI_INTERFACE_PARAGRAPH_STYLE = "L@ohos/graphics/text/text/ParagraphStyle;";
constexpr const char* ANI_INTERFACE_TEXT_STYLE = "L@ohos/graphics/text/text/TextStyle;";
constexpr const char* ANI_INTERFACE_FONT_FEATURE = "L@ohos/graphics/text/text/FontFeature;";
constexpr const char* ANI_INTERFACE_FONT_VARIATION = "L@ohos/graphics/text/text/FontVariation;";
constexpr const char* ANI_INTERFACE_TEXTSHADOW = "L@ohos/graphics/text/text/TextShadow;";
constexpr const char* ANI_INTERFACE_RECT_STYLE = "L@ohos/graphics/text/text/RectStyle;";
constexpr const char* ANI_INTERFACE_FONT_DESCRIPTOR = "L@ohos/graphics/text/text/FontDescriptor;";
/**
 * define class
 */
constexpr const char* ANI_CLASS_CLEANER = "L@ohos/graphics/text/text/Cleaner;";
constexpr const char* ANI_CLASS_PARAGRAPH = "L@ohos/graphics/text/text/Paragraph;";
constexpr const char* ANI_CLASS_PARAGRAPH_BUILDER = "L@ohos/graphics/text/text/ParagraphBuilder;";
constexpr const char* ANI_CLASS_LINEMETRICS = "L@ohos/graphics/text/text/LineMetricsInternal;";
constexpr const char* ANI_CLASS_RUNMETRICS = "L@ohos/graphics/text/text/RunMetricsInternal;";
constexpr const char* ANI_CLASS_TEXT_STYLE = "L@ohos/graphics/text/text/TextStyleInternal;";
constexpr const char* ANI_CLASS_FONT_COLLECTION = "L@ohos/graphics/text/text/FontCollection;";
constexpr const char* ANI_CLASS_FONT_FEATURE = "L@ohos/graphics/text/text/FontFeatureInternal;";
constexpr const char* ANI_CLASS_FONT_VARIATION = "L@ohos/graphics/text/text/FontVariationInternal;";
constexpr const char* ANI_CLASS_CANVAS = "L@ohos/graphics/drawing/drawing/Canvas;";
constexpr const char* ANI_CLASS_PATH = "L@ohos/graphics/drawing/drawing/Path;";
constexpr const char* ANI_CLASS_FONT_METRICS = "L@ohos/graphics/drawing/drawing/FontMetricsInner;";
constexpr const char* ANI_CLASS_TEXTSHADOW = "L@ohos/graphics/text/text/TextShadowInternal;";
constexpr const char* ANI_CLASS_RECT_STYLE = "L@ohos/graphics/text/text/RectStyleInternal;";
constexpr const char* ANI_CLASS_DECORATION = "L@ohos/graphics/text/text/DecorationInternal;";
constexpr const char* ANI_CLASS_FONT_DESCRIPTOR = "L@ohos/graphics/text/text/FontDescriptorInternal;";

/**
 * define enum
 */
constexpr const char* ANI_ENUM_FONT_WEIGHT = "L@ohos/graphics/text/text/FontWeight;";
constexpr const char* ANI_ENUM_FONT_STYLE = "L@ohos/graphics/text/text/FontStyle;";
constexpr const char* ANI_ENUM_TEXT_BASELINE = "L@ohos/graphics/text/text/TextBaseline;";
constexpr const char* ANI_ENUM_ELLIPSIS_MODE = "L@ohos/graphics/text/text/EllipsisMode;";
constexpr const char* ANI_ENUM_TEXT_DECORATION_TYPE = "L@ohos/graphics/text/text/TextDecorationType;";
constexpr const char* ANI_ENUM_TEXT_DECORATION_STYLE = "L@ohos/graphics/text/text/TextDecorationStyle;";
constexpr const char* ANI_ENUM_SYSTEM_FONT_TYPE = "L@ohos/graphics/text/text/SystemFontType;";

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