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

#include "customized/random_rs_path.h"

#include <memory>

#include "common/rs_vector2.h"
#include "draw/path.h"
#include "random/random_data.h"
#include "random/random_engine.h"
#include "render/rs_path.h"
#include "utils/point.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace {
Drawing::PathDirection GetRandomDrawingPathDirection()
{
    static constexpr int DRAWING_PATH_DIRECTION_INDEX_MAX = 1;
    return static_cast<Drawing::PathDirection>(RandomEngine::GetRandomIndex(DRAWING_PATH_DIRECTION_INDEX_MAX));
}
} // namespace

Drawing::Path RandomRSPath::GetRandomDrawingPath()
{
    Drawing::Path path;
    Drawing::Point pt1 = RandomData::GetRandomDrawingPoint();
    Drawing::Point pt2 = RandomData::GetRandomDrawingPoint();
    Drawing::scalar startAngle = RandomData::GetRandomFloat();
    Drawing::scalar sweepAngle = RandomData::GetRandomFloat();
    Drawing::Point endPt = RandomData::GetRandomDrawingPoint();
    path.ArcTo(pt1.x_, pt1.y_, pt2.x_, pt2.y_, startAngle, sweepAngle);
    path.CubicTo(pt1, pt2, endPt);
    auto rect = RandomData::GetRandomOptionalDrawingRect();
    if (rect.has_value()) {
        Drawing::Point radiusPt = RandomData::GetRandomDrawingPoint();
        path.AddRoundRect(rect.value(), radiusPt.x_, radiusPt.y_, GetRandomDrawingPathDirection());
    }
    path.QuadTo(pt1, endPt);
    path.ConicTo(pt1.x_, pt1.y_, endPt.x_, endPt.y_, sweepAngle);
    rect = RandomData::GetRandomOptionalDrawingRect();
    if (rect.has_value()) {
        path.AddOval(rect.value(), GetRandomDrawingPathDirection());
    }
    path.RMoveTo(pt1.x_, pt1.y_);
    path.RLineTo(pt1.x_, pt1.y_);
    rect = RandomData::GetRandomOptionalDrawingRect();
    if (rect.has_value()) {
        path.AddArc(rect.value(), startAngle, sweepAngle);
    }
    path.RCubicTo(pt1.x_, pt1.y_, pt2.x_, pt2.y_, endPt.x_, endPt.y_);
    path.RQuadTo(pt1.x_, pt1.y_, pt2.x_, pt2.y_);
    Drawing::scalar radius = RandomData::GetRandomFloat();
    path.AddCircle(pt1.x_, pt1.y_, radius, GetRandomDrawingPathDirection());
    return path;
}

std::shared_ptr<RSPath> RandomRSPath::GetRandomRSPath()
{
    Drawing::Path path = GetRandomDrawingPath();
    return RSPath::CreateRSPath(path);
}
} // namespace Rosen
} // namespace OHOS
