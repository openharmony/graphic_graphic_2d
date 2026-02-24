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

#ifndef OHOS_ROSEN_ANI_FONT_H
#define OHOS_ROSEN_ANI_FONT_H

#include "ani_drawing_utils.h"

#include "text/font.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniFont final {
public:
    explicit AniFont(std::shared_ptr<Font> font = nullptr) : font_(font) {}
    ~AniFont();

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);

    static ani_object GetMetrics(ani_env* env, ani_object obj);
    static ani_double GetSize(ani_env* env, ani_object obj);
    static ani_object GetTypeface(ani_env* env, ani_object obj);
    static void EnableSubpixel(ani_env* env, ani_object obj, ani_boolean isSubpixel);
    static void EnableEmbolden(ani_env* env, ani_object obj, ani_boolean isEmbolden);
    static void EnableLinearMetrics(ani_env* env, ani_object obj, ani_boolean isLinearMetrics);
    static void SetSize(ani_env* env, ani_object obj, ani_double alpha);
    static void SetTypeface(ani_env* env, ani_object obj, ani_object typeface);
    static ani_double MeasureSingleCharacter(ani_env* env, ani_object obj, ani_string text);
    static ani_double MeasureSingleCharacterWithFeatures(ani_env* env, ani_object obj, ani_string text,
        ani_array featuresobj);
    static ani_double MeasureText(ani_env* env, ani_object obj, ani_string aniText, ani_enum_item encoding);
    static void SetScaleX(ani_env* env, ani_object obj, ani_double scaleX);
    static void SetBaselineSnap(ani_env* env, ani_object obj, ani_boolean isBaselineSnap);
    static ani_boolean IsBaselineSnap(ani_env* env, ani_object obj);
    static void SetEmbeddedBitmaps(ani_env* env, ani_object obj, ani_boolean isEmbeddedBitmaps);
    static ani_boolean IsEmbeddedBitmaps(ani_env* env, ani_object obj);
    static void SetForceAutoHinting(ani_env* env, ani_object obj, ani_boolean isForceAutoHinting);
    static ani_object GetWidths(ani_env* env, ani_object obj, ani_array glyphs);
    static ani_object TextToGlyphs(ani_env* env, ani_object obj, ani_string aniText, ani_object glyphCount);
    static ani_boolean IsSubpixel(ani_env* env, ani_object obj);
    static ani_boolean IsLinearMetrics(ani_env* env, ani_object obj);
    static ani_double GetSkewX(ani_env* env, ani_object obj);
    static ani_boolean IsEmbolden(ani_env* env, ani_object obj);
    static void SetHinting(ani_env* env, ani_object obj, ani_enum_item hinting);
    static ani_int CountText(ani_env* env, ani_object obj, ani_string aniText);
    static void SetSkewX(ani_env* env, ani_object obj, ani_double skewX);
    static void SetEdging(ani_env* env, ani_object obj, ani_enum_item edging);
    static ani_boolean IsForceAutoHinting(ani_env* env, ani_object obj);
    static ani_double GetScaleX(ani_env* env, ani_object obj);
    static ani_enum_item GetHinting(ani_env* env, ani_object obj);
    static ani_enum_item GetEdging(ani_env* env, ani_object obj);
    static ani_object CreatePathForGlyph(ani_env* env, ani_object obj, ani_int index);
    static ani_object GetBounds(ani_env* env, ani_object obj, ani_array glyphs);
    static ani_object GetTextPath(ani_env* env, ani_object obj, ani_string aniText,
        ani_int byteLength, ani_double x, ani_double y);
    static void SetThemeFontFollowed(ani_env* env, ani_object obj, ani_boolean followed);
    static ani_boolean IsThemeFontFollowed(ani_env* env, ani_object obj);

    std::shared_ptr<Font> GetFont();

private:
    static ani_object FontTransferStatic(
        ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input);
    static ani_long GetFontAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    std::shared_ptr<Font>* GetFontPtrAddr();
    std::shared_ptr<Font> font_ = nullptr;
};

ani_object CreateAniFontMetrics(ani_env* env, const FontMetrics& fontMetrics);
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_FONT_H