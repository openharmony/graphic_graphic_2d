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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PATH_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PATH_H

#include <memory>

#include "drawing.h"

#include "texgine_rect.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
enum class TexginePathDirection {
    K_CW,  // clockwise direction for adding closed contours
    K_CCW, // counter-clockwise direction for adding closed contours
};

struct TexginePoint {
    float fX;
    float fY;
};

class TexginePath {
public:
    /*
     * @brief Returns the SkPath that user sets to TexginePath or TexginePath`s own SkPath
     */
    std::shared_ptr<RSPath> GetPath() const;

    /*
     * @brief Sets SkPath to TexginePath
     */
    void SetPath(const std::shared_ptr<RSPath> path);

    /*
     * @brief Adds oval to path, Oval is upright ellipse bounded by SkRect oval with
     *        radii equal to half oval width and half oval height
     * @param oval The boundary of the added ellipse
     * @param dir TexginePathDirection to wind ellipse
     * @return reference to SkPath
     */
    TexginePath &AddOval(const TexgineRect &oval, TexginePathDirection dir = TexginePathDirection::K_CW);

    /*
     * @brief Add contour start point at SkPoint p
     * @param p contour start
     * @return reference to SkPath
     */
    TexginePath &MoveTo(const TexginePoint &p);

    /*
     * @brief Add a quadrilateral from the last point to SkPoint p1 to SkPoint p2
     * @param p1 Control the SkPoint of added quadrilaterals
     * @param p2 The end of SkPoint added quad
     */
    TexginePath &QuadTo(const TexginePoint &p1, const TexginePoint &p2);

    /*
     * @brief Adds line from last point to SkPoint p
     * @param  end SkPoint of added line
     */
    TexginePath &LineTo(const TexginePoint &p);

private:
    std::shared_ptr<RSPath> path_ = std::make_shared<RSPath>();
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PATH_H
