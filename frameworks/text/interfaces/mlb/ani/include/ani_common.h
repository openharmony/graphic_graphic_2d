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
#include <cstdint>

#ifndef OHOS_TEXT_ANI_COMMON_H
#define OHOS_TEXT_ANI_COMMON_H

namespace OHOS::Text::NAI {
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

enum class TextErrorCode : int32_t {
    ERROR = -1,
    OK = 0,
    ERROR_NO_PERMISSION = 201,             // the value do not change. It is defined on all system
    ERROR_INVALID_PARAM = 401,             // the value do not change. It is defined on all system
    ERROR_DEVICE_NOT_SUPPORT = 801,        // the value do not change. It is defined on all system
    ERROR_ABNORMAL_PARAM_VALUE = 18600001, // the value do not change. It is defined on color manager system
    ERROR_NO_MEMORY = 8800100,             // no memory
};
} // namespace OHOS::Text::NAI
#endif // OHOS_TEXT_ANI_COMMON_H