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

#ifndef OHOS_TEXT_ANI_TEXT_LINE_H
#define OHOS_TEXT_ANI_TEXT_LINE_H

#include <ani.h>
#include <memory>

#include "text_line_base.h"
#include "typography.h"

namespace OHOS::Text::ANI {
class AniTextLine final {
public:
    static ani_status AniInit(ani_vm* vm, uint32_t* result);
    static ani_object CreateTextLine(ani_env* env, std::unique_ptr<Rosen::TextLineBase>& textLine);
    static void SetParagraph(ani_env* env, ani_object textLine, std::unique_ptr<Rosen::Typography>& paragraph);

private:
    static ani_double GetGlyphCount(ani_env* env, ani_object object);
    static ani_object GetTextRange(ani_env* env, ani_object object);
    static ani_object GetGlyphRuns(ani_env* env, ani_object object);
    static void Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y);
    static ani_object CreateTruncatedLine(
        ani_env* env, ani_object object, ani_double width, ani_object ellipsisMode, ani_object ellipsis);
    static ani_object GetTypographicBounds(ani_env* env, ani_object object);
    static ani_object GetImageBounds(ani_env* env, ani_object object);
    static ani_double GetTrailingSpaceWidth(ani_env* env, ani_object object);
    static ani_double GetStringIndexForPosition(ani_env* env, ani_object object, ani_object point);
    static ani_double GetOffsetForStringIndex(ani_env* env, ani_object object, ani_double index);
    static void EnumerateCaretOffsets(ani_env* env, [[maybe_unused]] ani_object object, ani_fn_object callback);
    static ani_double GetAlignmentOffset(
        ani_env* env, ani_object object, ani_double alignmentFactor, ani_double alignmentWidth);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_TEXT_LINE_H