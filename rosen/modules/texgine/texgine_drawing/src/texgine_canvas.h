/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_CANVAS_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_CANVAS_H

#include <memory>

#include "drawing.h"

#include "texgine_rect.h"
#include "texgine_paint.h"
#include "texgine_text_blob.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineCanvas {
public:
    /*
     * @brief Draws line segment from (x0, y0) to (x1, y1) using clip, SkMatrix, and SkPaint paint.
     */
    void DrawLine(double x0, double y0, double x1, double y1, const TexginePaint &paint);

    /*
     * @brief Draws SkRect rect using clip, SkMatrix, and SkPaint paint.
     */
    void DrawRect(const TexgineRect &rect, const TexginePaint &paint) const;

    /*
     * @brief Draws SkRRect rect using SkPaint paint.
     */
    void DrawRRect(const TexgineRect &rect, const TexginePaint &paint) const;

    /*
     * @brief Draws SkTextBlob blob at (x, y), using clip, SkMatrix, and SkPaint paint.
     */
    void DrawTextBlob(const std::shared_ptr<TexgineTextBlob> &blob, float x, float y, const TexginePaint &paint);

    /*
     * @brief Draws symbol at point, using clip, SkMatrix, and paint.
     */
    void DrawSymbol(const RSHMSymbolData &symbol, RSPoint locate, const TexginePaint &paint);

    /*
     * @brief Draws Path , using SkPath path and SkPaint paint.
     */
    void DrawPath(const RSPath &path, const TexginePaint &paint);

    /*
     * @brief Fills clip with color color using SkBlendMode::kSrc.
     *        This has the effect of replacing all pixels contained by clip with color.
     * @param color unpremultiplied ARGB
     */
    void Clear(uint32_t color) const;

    /*
     * @brief Saves SkMatrix and clip, and allocates a SkBitmap for subsequent drawing.
     */
    int Save() const;

    /*
     * @brief Translates SkMatrix by dx along the x-axis and dy along the y-axis.
     */
    void Translate(float dx, float dy) const;

    /*
     * @brief Returns the pointer of SkCanvas what user sets to TexgineCanvas
     */
    RSCanvas *GetCanvas() const;

    /*
     * @brief Removes changes to SkMatrix and clip since SkCanvas state was
              last saved. The state is removed from the stack.
    */
    void Restore() const;

    /*
     * @brief Sets SkCanvas to TexgineCanvas what user want
     */
    void SetCanvas(RSCanvas *canvas);

private:
    RSCanvas *canvas_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_CANVAS_H
