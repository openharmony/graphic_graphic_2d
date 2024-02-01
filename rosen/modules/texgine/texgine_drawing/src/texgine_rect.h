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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_RECT_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_RECT_H

#include <memory>

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkRect.h>
#include <include/core/SkRRect.h>
#else
#include "drawing.h"
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineRect {
public:
    TexgineRect();

    /*
     * @brief Create TexgineRect
     * @param left The left boundary of TexgineRect
     * @param right The right boundary of TexgineRect
     * @param top The top boundary of TexgineRect
     * @param bottom The bottom boundary of TexgineRect
     */
    static TexgineRect MakeLTRB(float left, float top, float right, float bottom);

    /*
     * @brief Create TexgineRect
     * @param x The left boundary of TexgineRect
     * @param y The top boundary of TexgineRect
     * @param w The width of TexgineRect
     * @param h The height of TexgineRect
     */
    static TexgineRect MakeXYWH(float x, float y, float w, float h);

    /*
     * @brief Create TexgineRect at (0, 0, w, h)
     * @param w The width of TexgineRect
     * @param h The height of TexgineRect
     */
    static TexgineRect MakeWH(float w, float h);

    /*
     * @brief Create TexgineRect
     * @param x The left boundary of TexgineRect
     * @param y The top boundary of TexgineRect
     * @param w The width of TexgineRect
     * @param h The height of TexgineRect
     * @param skRadii The SkVector of TexgineRect
     */
    static TexgineRect MakeRRect(float x, float y, float w, float h, const SkVector skRadii[4]);

    /*
     * @brief Return SkRect that user init or set to TexgineRect
     */
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<SkRect> GetRect() const;
#else
    std::shared_ptr<RSRect> GetRect() const;
#endif

    /*
     * @brief Return SkRRect that user set to TexgineRect
     */
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<SkRRect> GetRRect() const;
#else
    std::shared_ptr<RSRoundRect> GetRRect() const;
#endif

    /*
     * @brief Sets SkRect to TexgineRect
     * @param rect SkRect user want
     */
#ifndef USE_ROSEN_DRAWING
    void SetRect(const SkRect &rect);
#else
    void SetRect(const RSRect &rect);
#endif

    /*
     * @brief Sets SkRRect to TexgineRect
     * @param rect SkRRect user want
     */
#ifndef USE_ROSEN_DRAWING
    void SetRRect(const SkRRect &rrect);
#else
    void SetRRect(const RSRoundRect &rrect);
#endif

    float *fLeft_ = nullptr;
    float *fTop_ = nullptr;
    float *fRight_ = nullptr;
    float *fBottom_ = nullptr;

private:
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<SkRect> rect_ = nullptr;
    std::shared_ptr<SkRRect> rrect_ = nullptr;
#else
    std::shared_ptr<RSRect> rect_ = nullptr;
    std::shared_ptr<RSRoundRect> rrect_ = nullptr;
#endif
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_RECT_H
