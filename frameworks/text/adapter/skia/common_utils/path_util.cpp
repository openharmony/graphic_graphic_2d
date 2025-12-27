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

#include "path_util.h"

#include "utils/text_log.h"
#include "utils/point.h"

namespace OHOS::Rosen::SPText {
namespace {
constexpr float EPSILON = 1e-6f;
constexpr int MIN_POINTS_FOR_CLOSED_PATH = 3;
}
/**
 * Determine the orientation of the triplet (p1, p2, p3).
 * Attention: the y-axis order is reversed.
 */
Orientation TextPathUtil::ComputeOrientation(
    const Drawing::Point& p1, const Drawing::Point& p2, const Drawing::Point& p3)
{
    float val = (p2.GetX() - p1.GetX()) * (p2.GetY() - p3.GetY()) - (p1.GetY() - p2.GetY()) * (p3.GetX() - p2.GetX());
    if (val > EPSILON) {
        return Orientation::COUNTER_CLOCKWISE;
    } else if (val < -EPSILON) {
        return Orientation::CLOCKWISE;
    } else {
        return Orientation::COLLINEAR;
    }
}

size_t TextPathUtil::FindMinPointIndex(const Drawing::Path& path)
{
    int pointsCount = path.CountPoints();
    if (pointsCount <= 0) {
        return 0;
    }
    
    size_t index = 0;
    Drawing::Point minPoint{FLT_MAX, FLT_MAX};
    for (size_t i = 0; i < static_cast<size_t>(pointsCount - 1); i++) {
        const auto& point = path.GetPoint(i);
        if (point.GetY() < minPoint.GetY()) {
            minPoint = point;
            index = i;
        } else if (point.GetY() == minPoint.GetY()) {
            if (point.GetX() < minPoint.GetX()) {
                minPoint = point;
                index = i;
            }
        }
    }
    return index;
}

bool TextPathUtil::IsPathClockwise(const Drawing::Path& path)
{
    int pointsCount = path.CountPoints();
    if (pointsCount < MIN_POINTS_FOR_CLOSED_PATH || !path.IsClosed(false)) {
        // Not enough points or not closed
        return false;
    }

    size_t index = FindMinPointIndex(path);

    Drawing::Point prePoint = path.GetPoint(index > 0 ? index - 1 : (pointsCount - 2)); // - 2 means the last point
    Drawing::Point nextPoint =
        path.GetPoint(index < (pointsCount - 2) ? index + 1 : 0);  // - 2 means the last point
    Orientation orientation = ComputeOrientation(prePoint, path.GetPoint(index), nextPoint);
    return orientation == Orientation::CLOCKWISE;
}

void TextPathUtil::ExtractOuterPath(const std::vector<Drawing::Path>& allPaths, Drawing::Path& outerPath)
{
    if (allPaths.empty()) {
        return;
    }
    if (allPaths.size() == 1) {
        // If there's only one path, it's the outer path
        outerPath = allPaths[0];
        TEXT_LOGD("Extracting outer path from single path");
    } else {
        for (size_t i = 0; i < allPaths.size(); i++) {
            const auto& path = allPaths[i];
            if (path.CountPoints() < MIN_POINTS_FOR_CLOSED_PATH || !path.IsClosed(false) || IsPathClockwise(path)) {
                outerPath.AddPath(path);
                TEXT_LOGD("Extracting outer path %{public}zu", i);
            }
        }
    }
}
}