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

#ifndef OHOS_TEXT_ANI_PARAGRAPH_H
#define OHOS_TEXT_ANI_PARAGRAPH_H

#include <ani.h>
#include <memory>
#include <vector>

#include "typography.h"

namespace OHOS::Text::ANI {
class AniParagraph final {
public:
    static ani_object SetTypography(ani_env* env, OHOS::Rosen::Typography* typography);
    static std::vector<ani_native_function> InitMethods(ani_env* env);
    static ani_status AniInit(ani_vm* vm, uint32_t* result);

private:
    static void LayoutSync(ani_env* env, ani_object object, ani_double width);
    static ani_object LayoutWithConstraints(ani_env* env, ani_object object, ani_object constraint);
    static void Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y);
    static void PaintOnPath(
        ani_env* env, ani_object object, ani_object canvas, ani_object path, ani_double hOffset, ani_double vOffset);
    static ani_double GetMaxWidth(ani_env* env, ani_object object);
    static ani_double GetHeight(ani_env* env, ani_object object);
    static ani_double GetLongestLine(ani_env* env, ani_object object);
    static ani_double GetLongestLineWithIndent(ani_env* env, ani_object object);
    static ani_double GetMinIntrinsicWidth(ani_env* env, ani_object object);
    static ani_double GetMaxIntrinsicWidth(ani_env* env, ani_object object);
    static ani_double GetAlphabeticBaseline(ani_env* env, ani_object object);
    static ani_double GetIdeographicBaseline(ani_env* env, ani_object object);
    static ani_object GetRectsForRange(
        ani_env* env, ani_object object, ani_object range, ani_object widthStyle, ani_object heightStyle);
    static ani_object GetRectsForPlaceholders(ani_env* env, ani_object object);
    static ani_object GetGlyphPositionAtCoordinate(ani_env* env, ani_object object, ani_double x, ani_double y);
    static ani_object GetWordBoundary(ani_env* env, ani_object object, ani_int offset);
    static ani_int GetLineCount(ani_env* env, ani_object object);
    static ani_double GetLineHeight(ani_env* env, ani_object object, ani_int line);
    static ani_double GetLineWidth(ani_env* env, ani_object object, ani_int line);
    static ani_boolean DidExceedMaxLines(ani_env* env, ani_object object);
    static ani_ref GetTextLines(ani_env* env, ani_object object);
    static ani_object GetActualTextRange(
        ani_env* env, ani_object object, ani_int lineNumber, ani_boolean includeSpaces);
    static ani_ref GetLineMetrics(ani_env* env, ani_object object);
    static ani_object GetLineMetricsAt(ani_env* env, ani_object object, ani_int lineNumber);
    static void UpdateColor(ani_env* env, ani_object object, ani_object color);
    static void UpdateDecoration(ani_env* env, ani_object object, ani_object decoration);
    static ani_object GetCharacterRangeForGlyphRange(ani_env* env, ani_object object,
        ani_object glyphRange, ani_object encoding);
    static ani_object GetGlyphRangeForCharacterRange(ani_env* env, ani_object object,
        ani_object characterRange, ani_object encoding);
    static ani_object GetCharacterPositionAtCoordinate(ani_env* env, ani_object object,
        ani_double x, ani_double y, ani_object encoding);
    static ani_object NativeTransferStatic(ani_env* env, ani_class cls, ani_object input);
    static ani_object NativeTransferDynamic(ani_env* env, ani_class cls, ani_long nativeObj);

    // Helper functions for code refactoring
    static ani_status SetArrayBoundaryElement(ani_env* env, ani_object array, int index,
        const Rosen::Boundary& range, const char* errorMsg);

    std::shared_ptr<OHOS::Rosen::Typography> typography_{nullptr};
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_PARAGRAPH_H