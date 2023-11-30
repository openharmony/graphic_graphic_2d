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

#include <cmath>
#include <utility>

#include "utils/round_rect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static double ComputeMinScale(double rad1, double rad2, double limit, double curMin)
{
    if ((rad1 + rad2) > limit) {
        return std::min(curMin, limit / (rad1 + rad2));
    }
    return curMin;
}

static void ComputeRadii(double limit, double scale, float& radiiA, float& radiiB)
{
    radiiA = static_cast<float>(static_cast<double>(radiiA * scale));
    radiiB = static_cast<float>(static_cast<double>(radiiB * scale));

    if (radiiA + radiiB > limit) {
        float* minRadius = &radiiA;
        float* maxRadius = &radiiB;

        if (*minRadius > *maxRadius) {
            using std::swap;
            swap(minRadius, maxRadius);
        }

        float newMinRadius = *minRadius;

        float newMaxRadius = static_cast<float>(limit - newMinRadius);

        while (newMaxRadius + newMinRadius > limit) {
            newMaxRadius = nextafterf(newMaxRadius, 0.0f);
        }
        *maxRadius = newMaxRadius;
    }
}

void RoundRect::AdjustRadiiX(double limit, double scale, CornerPos cornerPosA, CornerPos cornerPosB)
{
    float radiiA = radiusXY_[cornerPosA].GetX();
    float radiiB = radiusXY_[cornerPosB].GetX();

    ComputeRadii(limit, scale, radiiA, radiiB);

    radiusXY_[cornerPosA].SetX(radiiA);
    radiusXY_[cornerPosB].SetX(radiiB);
}

void RoundRect::AdjustRadiiY(double limit, double scale, CornerPos cornerPosA, CornerPos cornerPosB)
{
    float radiiA = radiusXY_[cornerPosA].GetY();
    float radiiB = radiusXY_[cornerPosB].GetY();

    ComputeRadii(limit, scale, radiiA, radiiB);

    radiusXY_[cornerPosA].SetY(radiiA);
    radiusXY_[cornerPosB].SetY(radiiB);
}

bool RoundRect::ClampToZero()
{
    bool allCornersSquare = true;

    for (int i = 0; i < CORNER_NUMBER; ++i) {
        if (radiusXY_[i].GetX() <= 0 || radiusXY_[i].GetY() <= 0) {
            radiusXY_[i].SetX(0);
            radiusXY_[i].SetY(0);
        } else {
            allCornersSquare = false;
        }
    }

    return allCornersSquare;
}

bool RoundRect::ScaleRadii()
{
    double scale = 1.0;
    double width = static_cast<double>(rect_.GetWidth());
    double height = static_cast<double>(rect_.GetHeight());
    if (width <= 0 || height <= 0) {
        return false;
    }

    scale = ComputeMinScale(radiusXY_[TOP_LEFT_POS].GetX(), radiusXY_[TOP_RIGHT_POS].GetX(), width, scale);
    scale = ComputeMinScale(radiusXY_[TOP_RIGHT_POS].GetY(), radiusXY_[BOTTOM_RIGHT_POS].GetY(), height, scale);
    scale = ComputeMinScale(radiusXY_[BOTTOM_RIGHT_POS].GetX(), radiusXY_[BOTTOM_LEFT_POS].GetX(), width, scale);
    scale = ComputeMinScale(radiusXY_[BOTTOM_LEFT_POS].GetY(), radiusXY_[TOP_LEFT_POS].GetY(), height, scale);
    if (scale < 1.0) {
        AdjustRadiiX(width, scale, TOP_LEFT_POS, TOP_RIGHT_POS);
        AdjustRadiiY(height, scale, TOP_RIGHT_POS, BOTTOM_RIGHT_POS);
        AdjustRadiiX(width, scale, BOTTOM_RIGHT_POS, BOTTOM_LEFT_POS);
        AdjustRadiiY(height, scale, BOTTOM_LEFT_POS, TOP_LEFT_POS);
    }

    ClampToZero();

    return scale < 1.0;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS