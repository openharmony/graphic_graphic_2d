/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_UTILS_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_UTILS_H

#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
class RSDrawableUtils {
public:
    static Drawing::RoundRect RRect2DrawingRRect(const RRect& rr)
    {
        Drawing::Rect rect = Drawing::Rect(
            rr.rect_.left_, rr.rect_.top_, rr.rect_.left_ + rr.rect_.width_, rr.rect_.top_ + rr.rect_.height_);

        // set radius for all 4 corner of RRect
        constexpr uint32_t NUM_OF_CORNERS_IN_RECT = 4;
        std::vector<Drawing::Point> radii(NUM_OF_CORNERS_IN_RECT);
        for (uint32_t i = 0; i < NUM_OF_CORNERS_IN_RECT; i++) {
            radii.at(i).SetX(rr.radius_[i].x_);
            radii.at(i).SetY(rr.radius_[i].y_);
        }
        return Drawing::RoundRect(rect, radii);
    }

    static Drawing::Rect Rect2DrawingRect(const RectF& r)
    {
        return Drawing::Rect(r.left_, r.top_, r.left_ + r.width_, r.top_ + r.height_);
    }

    static RRect GetRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool& isOutline)
    {
        if (!border) {
            return RRect();
        }

        return isOutline ?
            RRect(properties.GetRRect().rect_.MakeOutset(border->GetWidthFour()), border->GetRadiusFour()) :
            properties.GetRRect();
    }

    static RRect GetInnerRRectForDrawingBorder(const RSProperties& properties, const std::shared_ptr<RSBorder>& border,
        const bool& isOutline)
    {
        if (!border) {
            return RRect();
        }
        return isOutline ? properties.GetRRect() : properties.GetInnerRRect();
    }
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_UTILS_H
