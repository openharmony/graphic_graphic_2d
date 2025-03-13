/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "path_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "draw/path.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_ARRAY_SIZE = 5000;
constexpr size_t DIRECTION_SIZE = 2;
constexpr size_t FILLTYPE_SIZE = 4;
constexpr size_t MATRIXFLAG_SIZE = 3;
constexpr size_t PATH_OP_SIZE = 5;
} // namespace
namespace Drawing {
bool PathAddFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    Rect rect = GetObject<Rect>();
    Rect oval = GetObject<Rect>();
    path.AddRect(rect, GetObject<PathDirection>());
    path.AddOval(oval, GetObject<PathDirection>());
    path.Close();
    return true;
}

bool PathOpFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path1;
    Path ending;
    Path out;
    uint32_t weight = GetObject<uint32_t>();
    path1.Interpolate(ending, weight, out);

    uint32_t px = GetObject<uint32_t>();
    uint32_t py = GetObject<uint32_t>();
    path1.LineTo(px, py);

    Path op;
    op.MoveTo(px, py);

    Path path2;
    path2.Op(op, path1, static_cast<PathOp>(GetObject<uint32_t>() % PATH_OP_SIZE));
    path1.Close();
    path2.Reset();
    return true;
}

/*
 * 测试以下 Path 接口：
 * 1. Path()
 * 2. Path(const Path&)
 * 3. GetDrawingType()
 * 4. ConvertToSVGString()
 * 5. operator=
 */
bool PathFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    Path pathTwo = Path(path);
    path.GetDrawingType();
    path.ConvertToSVGString();
    path = pathTwo;
    return true;
}

/*
 * 测试以下 Path 接口：
 * 1. Path()
 * 2. ArcTo(...)
 * 3. ArcTo(...)
 * 4. ArcTo(...)
 * 5. CubicTo(...)
 * 6. CubicTo(...)
 * 7. QuadTo(...)
 * 8. QuadTo(...)
 * 9. ConicTo(...)
 * 10. RMoveTo(...)
 * 11. RLineTo(...)
 * 12. RArcTo(...)
 * 13. RCubicTo(...)
 * 14. RQuadTo(...)
 * 15. RConicTo(...)
 * 16. GetPositionAndTangent(...)
 */
bool PathFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    scalar ptOneX = GetObject<scalar>();
    scalar ptOneY = GetObject<scalar>();
    scalar ptTwoX = GetObject<scalar>();
    scalar ptTwoY = GetObject<scalar>();
    scalar startAngle = GetObject<scalar>();
    scalar sweepAngle = GetObject<scalar>();
    path.ArcTo(ptOneX, ptOneY, ptTwoX, ptTwoY, startAngle, sweepAngle);
    Point ptOne { ptOneX, ptOneY };
    Point ptTwo { ptTwoX, ptTwoY };
    path.ArcTo(ptOne, ptTwo, startAngle, sweepAngle);
    uint32_t direction = GetObject<uint32_t>();
    path.ArcTo(ptOneX, ptOneY, startAngle, static_cast<PathDirection>(direction % DIRECTION_SIZE), ptTwoX, ptTwoY);
    scalar endPtX = GetObject<scalar>();
    scalar endPtY = GetObject<scalar>();
    path.CubicTo(ptOneX, ptOneY, ptTwoX, ptTwoY, endPtX, endPtY);
    Point ptEnd { endPtX, endPtY };
    path.CubicTo(ptOne, ptTwo, ptEnd);
    path.QuadTo(ptOneX, ptOneY, endPtX, endPtY);
    path.QuadTo(ptOne, ptEnd);
    path.ConicTo(ptOneX, ptOneY, endPtX, endPtY, sweepAngle);
    path.RMoveTo(ptOneX, ptOneY);
    path.RLineTo(ptOneX, ptOneY);
    path.RArcTo(ptOneX, ptOneY, startAngle, static_cast<PathDirection>(direction % DIRECTION_SIZE), ptTwoX, ptTwoY);
    path.RCubicTo(ptOneX, ptOneY, ptTwoX, ptTwoY, endPtX, endPtY);
    path.RQuadTo(ptOneX, ptOneY, ptTwoX, ptTwoY);
    path.RConicTo(ptOneX, ptOneY, endPtX, endPtY, sweepAngle);
    scalar distance = GetObject<scalar>();
    bool forceClosed = GetObject<bool>();
    path.GetPositionAndTangent(distance, ptOne, ptTwo, forceClosed);
    return true;
}

/*
 * 测试以下 Path 接口：
 * 1. Path()
 * 2. AddRect(...)
 * 3. AddRect(...)
 * 4. AddRect(...)
 * 5. AddOval(...)
 * 6. AddOval(...)
 * 7. AddArc(...)
 * 8. AddCircle(...)
 * 9. AddRoundRect(...)
 * 10. AddRoundRect(...)
 */
bool PathFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    Rect rect { left, top, right, bottom };
    uint32_t direction = GetObject<uint32_t>();
    path.AddRect(rect, static_cast<PathDirection>(direction % DIRECTION_SIZE));
    uint32_t start = GetObject<uint32_t>();
    path.AddRect(rect, start, static_cast<PathDirection>(direction % DIRECTION_SIZE));
    path.AddRect(left, top, right, bottom, static_cast<PathDirection>(direction % DIRECTION_SIZE));
    path.AddOval(rect, static_cast<PathDirection>(direction % DIRECTION_SIZE));
    path.AddOval(rect, start, static_cast<PathDirection>(direction % DIRECTION_SIZE));
    scalar startAngle = GetObject<scalar>();
    scalar sweepAngle = GetObject<scalar>();
    path.AddArc(rect, startAngle, sweepAngle);

    scalar ptx = GetObject<scalar>();
    scalar pty = GetObject<scalar>();
    scalar radius = GetObject<scalar>();
    path.AddCircle(ptx, pty, radius, static_cast<PathDirection>(direction % DIRECTION_SIZE));
    path.AddRoundRect(rect, ptx, pty, static_cast<PathDirection>(direction % DIRECTION_SIZE));
    RoundRect roundRect;
    path.AddRoundRect(roundRect, static_cast<PathDirection>(direction % DIRECTION_SIZE));
    return true;
}

/*
 * 测试以下 Path 接口：
 * 1. Path()
 * 2. AddPath(...)
 * 3. AddPath(...)
 * 4. AddPath(...)
 * 5. Contains(...)
 * 6. ReverseAddPath(...)
 * 7. GetBounds()
 * 8. SetFillStyle(...)
 * 9. BuildFromInterpolate(...)
 * 10. Transform(...)
 * 11. TransformWithPerspectiveClip(...)
 * 12. Offset(...)
 * 13. Offset(...)
 * 14. IsValid()
 * 15. GetLength(...)
 * 16. IsClosed(...)
 * 17. GetMatrix(...)
 * 18. Serialize()
 * 19. Deserialize(...)
 */
bool PathFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    Path src;
    Path ending;
    scalar ptx = GetObject<scalar>();
    scalar pty = GetObject<scalar>();
    scalar weight = GetObject<scalar>();
    uint32_t mode = GetObject<uint32_t>();
    path.AddPath(src, ptx, pty, static_cast<PathAddMode>(mode % DIRECTION_SIZE));
    path.AddPath(src, static_cast<PathAddMode>(mode % DIRECTION_SIZE));
    Matrix matrix;
    path.AddPath(src, matrix, static_cast<PathAddMode>(mode % DIRECTION_SIZE));
    path.Contains(ptx, pty);
    path.ReverseAddPath(src);
    path.GetBounds();
    uint32_t fillstyle = GetObject<uint32_t>();
    path.SetFillStyle(static_cast<PathFillType>(fillstyle % FILLTYPE_SIZE));
    path.BuildFromInterpolate(src, ending, weight);
    path.Transform(matrix);
    bool applyPerspectiveClip = GetObject<bool>();
    path.TransformWithPerspectiveClip(matrix, &src, applyPerspectiveClip);
    path.Offset(ptx, pty);
    path.Offset(&src, ptx, pty);
    path.IsValid();
    bool forceClosed = GetObject<bool>();
    path.GetLength(forceClosed);
    path.IsClosed(forceClosed);
    float distance = GetObject<float>();
    uint32_t flags = GetObject<uint32_t>();
    path.GetMatrix(forceClosed, distance, &matrix, static_cast<PathMeasureMatrixFlags>(flags % MATRIXFLAG_SIZE));
    path.Serialize();
    path.Deserialize(nullptr);
    return true;
}

/*
 * 测试以下 Path 接口：
 * 1. Path()
 * 2. BuildFromSVGString(...)
 */
bool PathFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    uint32_t count = GetObject<uint32_t>() % MAX_ARRAY_SIZE + 1;
    char str[count];
    for (size_t i = 0; i < count; i++) {
        str[i] = GetObject<char>();
    }
    str[count - 1] = '\0';
    path.BuildFromSVGString(str);
    return true;
}

/*
 * 测试以下 Path 接口：
 * 1. Path()
 * 2. AddPoly(...)
 */
bool PathFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    std::vector<Point> points;
    scalar ptOneX = GetObject<scalar>();
    scalar ptOneY = GetObject<scalar>();
    scalar ptTwoX = GetObject<scalar>();
    scalar ptTwoY = GetObject<scalar>();
    Point ptOne { ptOneX, ptOneY };
    Point ptTwo { ptTwoX, ptTwoY };
    points.push_back(ptOne);
    points.push_back(ptTwo);
    int count = points.size();
    bool close = GetObject<bool>();
    path.AddPoly(points, count, close);
    return true;
}

/*
 * 测试以下 Path 接口：
 * 1. GetSegment(...)
 */
bool PathFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    path.MoveTo(GetObject<scalar>(), GetObject<scalar>());
    path.LineTo(GetObject<scalar>(), GetObject<scalar>());
    path.LineTo(GetObject<scalar>(), GetObject<scalar>());
    Path newPath;
    path.GetSegment(GetObject<scalar>(), GetObject<scalar>(), &newPath, GetObject<bool>(), GetObject<bool>());
    path.GetSegment(GetObject<scalar>(), GetObject<scalar>(), nullptr, GetObject<bool>(), GetObject<bool>());
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PathAddFuzzTest(data, size);
    OHOS::Rosen::Drawing::PathOpFuzzTest(data, size);
    OHOS::Rosen::Drawing::PathFuzzTest001(data, size);
    OHOS::Rosen::Drawing::PathFuzzTest002(data, size);
    OHOS::Rosen::Drawing::PathFuzzTest003(data, size);
    OHOS::Rosen::Drawing::PathFuzzTest004(data, size);
    OHOS::Rosen::Drawing::PathFuzzTest005(data, size);
    OHOS::Rosen::Drawing::PathFuzzTest006(data, size);
    OHOS::Rosen::Drawing::PathFuzzTest007(data, size);
    return 0;
}