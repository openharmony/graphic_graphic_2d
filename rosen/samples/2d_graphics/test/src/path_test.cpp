/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "path_test.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void PathTest::TestDrawPath(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPath");
    Path path1;
    // Add beginning of contour at point {200, 200}.
    path1.MoveTo(200, 200);
    // Add quad from last point towards (600, 200), to (600, 600).
    path1.QuadTo(600, 200, 600, 600);
    path1.Close();

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_GRAY);
    pen.SetWidth(10); // The thickness of the pen is 10
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    canvas.AttachBrush(brush);

    Path path2;
    // Add oval to path, bounds of ellipse added is {200, 200, 600, 1000}.
    path2.AddOval({ 200, 200, 600, 1000 });

    Path dest;
    dest.Op(path1, path2, PathOp::UNION);
    canvas.DrawPath(dest);
    LOGI("+++++++ TestDrawPath");
}

void PathTest::TestDrawPathPro(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawPathPro");
    int len = 300;     // set len size
    Point a(500, 500); // point at {500, 500}

    Point c;
    Point d;

    d.SetX(a.GetX() - len * std::sin(18.0f));
    d.SetY(a.GetY() + len * std::cos(18.0f));

    c.SetX(a.GetX() + len * std::sin(18.0f));
    c.SetY(d.GetY());

    Point b;
    b.SetX(a.GetX() + (len / 2.0)); // half of len, for point position calculation
    b.SetY(a.GetY() + std::sqrt((c.GetX() - d.GetX()) * (c.GetX() - d.GetX()) + (len / 2.0) * (len / 2.0)));

    Point e;
    e.SetX(a.GetX() - (len / 2.0)); // half of len, for point position calculation
    e.SetY(b.GetY());

    Path path;
    path.MoveTo(a.GetX(), a.GetY());
    path.LineTo(b.GetX(), b.GetY());
    path.LineTo(c.GetX(), c.GetY());
    path.LineTo(d.GetX(), d.GetY());
    path.LineTo(e.GetX(), e.GetY());
    path.Close();

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(10); // The thickness of the pen is 10
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    canvas.AttachBrush(brush);

    canvas.AttachPen(pen).AttachBrush(brush).DrawPath(path);
    LOGI("+++++++ TestDrawPathPro");
}

std::vector<PathTest::TestFunc> PathTest::PathTestCase()
{
    std::vector<TestFunc> testFuncVec;
    testFuncVec.push_back(TestDrawPath);
    testFuncVec.push_back(TestDrawPathPro);
    return testFuncVec;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS