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

#ifndef UTIL_PATH_H
#define UTIL_PATH_H

#include <vector>

#include "draw/path.h"
#include "draw/path_iterator.h"
#include "utils/point.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class UtilsPath {
public:
    using BezierFunction = Point (*)(float t, const Point*);
    static scalar Distance(const Point& p1, const Point& p2);
    static void AddMove(const Point& toPoint, std::vector<Point>& approxPoints, std::vector<float>& approxLengths);
    static void AddLine(const Point& toPoint, std::vector<Point>& approxPoints, std::vector<float>& approxLengths);
    static Point CalculateQuadraticBezier(float t, const Point* points);
    static Point CalculateCubicBezier(float t, const Point* points);
    static void AddBezier(const Point* points, BezierFunction calculationBezier, std::vector<Point>& approxPoints,
        std::vector<float>& approxLengths, float errorSquared, bool doubleCheckDivision);
    static void AddConic(PathIter& pathIter, const Point* points, std::vector<Point>& approxPoints,
        std::vector<float>& approxLengths, float errorConic);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
