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

#include "utils_path.h"

#include <map>

#include "include/core/SkPoint.h"
#include "src/core/SkGeometry.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"

#include "draw/path.h"
#include "utils/log.h"
#include "utils/point.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr float QUADRATIC_BINOMIAL_COEFF = 2.0f;
static constexpr int CONIC_CTRL_POINT_COUNT = 4;
static constexpr int CUBIC_BEZIER_BINOMIAL = 3;
static constexpr int MAX_BEZIER_SEGMENTS = 5000000; // 5000000 is a safe upper bound.
static constexpr int MID_T_DIVISOR = 2;
static constexpr int QUADRATIC_BEZIER_POINT_COUNT = 3;
static constexpr int QUADRATIC_STRIDE = 2;

float ComputeQuadratic(float param, float start, float control, float end)
{
    float invParam = 1 - param;
    float invSqr = invParam * invParam;
    float crossTerm = QUADRATIC_BINOMIAL_COEFF * invParam * param;
    float paramSqr = param * param;
    return invSqr * start + crossTerm * control + paramSqr * end;
}

static float ComputeCubic(float param, float start, float control1, float control2, float end)
{
    float invParam = 1 - param;
    float invSqr = invParam * invParam;
    float invCubed = invSqr * invParam;
    float paramSqr = param * param;
    float paramCubed = paramSqr * param;
    return invCubed * start + CUBIC_BEZIER_BINOMIAL * invSqr * param * control1 +
        CUBIC_BEZIER_BINOMIAL * invParam * paramSqr * control2 + paramCubed * end;
}

static bool ShouldSubdivide(const Point* controlPoints, UtilsPath::BezierFunction calculationBezier, float startT,
    const Point& startPt, float endT, const Point& endPt, float& midParam, Point& midPt, float maxError)
{
    midParam = (startT + endT) / MID_T_DIVISOR;
    midPt = calculationBezier(midParam, controlPoints);
    float avgX = (startPt.GetX() + endPt.GetX()) / MID_T_DIVISOR;
    float avgY = (startPt.GetY() + endPt.GetY()) / MID_T_DIVISOR;
    float dx = midPt.GetX() - avgX;
    float dy = midPt.GetY() - avgY;
    float error = dx * dx + dy * dy;
    return error > maxError;
}

scalar UtilsPath::Distance(const Point& p1, const Point& p2)
{
    return SkPoint::Distance(reinterpret_cast<const SkPoint &>(p1), reinterpret_cast<const SkPoint &>(p2));
}

void UtilsPath::AddMove(const Point& point, std::vector<Point>& approxPoints, std::vector<float>& approxLengths)
{
    float length = approxLengths.empty() ? 0.0f : approxLengths.back();
    approxPoints.push_back(point);
    approxLengths.push_back(length);
}

void UtilsPath::AddLine(const Point& point, std::vector<Point>& approxPoints, std::vector<float>& approxLengths)
{
    if (approxPoints.empty()) {
        approxPoints.push_back(Point(0, 0));
        approxLengths.push_back(0);
    } else if (approxPoints.back() == point) {
        return;
    }
    float length = approxLengths.back() + Distance(approxPoints.back(), point);
    approxPoints.push_back(point);
    approxLengths.push_back(length);
}

Point UtilsPath::CalculateQuadraticBezier(float t, const Point* points)
{
    // 0, 1, 2 are the control points of a quad bezier curve.
    float x = ComputeQuadratic(t, points[0].GetX(), points[1].GetX(), points[2].GetX());
    float y = ComputeQuadratic(t, points[0].GetY(), points[1].GetY(), points[2].GetY());
    return Point(x, y);
}

Point UtilsPath::CalculateCubicBezier(float t, const Point* points)
{
    // 0, 1, 2, 3 are the control points of a cubic bezier curve.
    float x = ComputeCubic(t, points[0].GetX(), points[1].GetX(), points[2].GetX(), points[3].GetX());
    float y = ComputeCubic(t, points[0].GetY(), points[1].GetY(), points[2].GetY(), points[3].GetY());
    return Point(x, y);
}

void UtilsPath::AddBezier(const Point* points, BezierFunction calculationBezier, std::vector<Point>& approxPoints,
    std::vector<float>& approxLengths, float errorSquared, bool doubleCheck)
{
    using TimePointMap = std::map<float, Point>;
    TimePointMap timeToPoints;
    timeToPoints[0] = calculationBezier(0.0f, points);
    timeToPoints[1] = calculationBezier(1.0f, points);
    auto it = timeToPoints.begin();
    auto nextIt = std::next(it);
    size_t count = 0;
    while (nextIt != timeToPoints.end()) {
        count++;
        if (count > MAX_BEZIER_SEGMENTS) {
            LOGW("UtilsPath::AddBezier count too large.");
            break;
        }
        float midT;
        Point midPt;
        bool split = ShouldSubdivide(points, calculationBezier, it->first, it->second, nextIt->first, nextIt->second,
            midT, midPt, errorSquared);
        if (!split && doubleCheck) {
            float qT;
            Point qPt;
            split = ShouldSubdivide(points, calculationBezier, it->first, it->second, midT, midPt, qT, qPt,
                errorSquared);
            if (split) {
                doubleCheck = false;
            }
        }
        if (split) {
            nextIt = timeToPoints.insert(it, { midT, midPt });
        } else {
            ++it;
            nextIt = std::next(it);
        }
    }
    for (const auto& [_, pt] : timeToPoints) {
        AddLine(pt, approxPoints, approxLengths);
    }
}

void UtilsPath::AddConic(PathIter& pathIter, const Point* points, std::vector<Point>& approxPoints,
    std::vector<float>& approxLengths, float errorConic)
{
    SkAutoConicToQuads converter;
    SkPoint skPoints[CONIC_CTRL_POINT_COUNT];
    for (int i = 0; i < CONIC_CTRL_POINT_COUNT; ++i) {
        skPoints[i] = SkPoint::Make(points[i].GetX(), points[i].GetY());
    }
    const SkPoint* quads = converter.computeQuads(skPoints, pathIter.ConicWeight(), errorConic);
    if (quads == nullptr) {
        LOGE("UtilsPath::AddConic quads is nullptr.");
        return;
    }
    for (int i = 0; i < converter.countQuads(); ++i) {
        Point quadPoints[QUADRATIC_BEZIER_POINT_COUNT];
        for (int j = 0; j < QUADRATIC_BEZIER_POINT_COUNT; ++j) {
            quadPoints[j] = Point(quads[i * QUADRATIC_STRIDE + j].x(), quads[i * QUADRATIC_STRIDE + j].y());
        }
        UtilsPath::AddBezier(quadPoints, UtilsPath::CalculateQuadraticBezier, approxPoints, approxLengths, errorConic,
            false);
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
