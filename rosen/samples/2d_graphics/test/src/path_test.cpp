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
static constexpr float MARGINE_SCALE_SIZE = 10.0f;
static constexpr float MATH_TWO = 2.0f;
void PathTest::TestDrawStar(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawStar");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    int len = rect.GetWidth() - rect.GetWidth() / 5.0f; // set len size
    // half of width
    Point a(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + rect.GetHeight() / 2.0f - len * std::cos(18.0f));
    Point c;
    Point d;

    // five star angle is 36
    d.SetX(a.GetX() - len * std::sin(18.0f));
    d.SetY(a.GetY() + len * std::cos(18.0f));

    c.SetX(a.GetX() + len * std::sin(18.0f));
    c.SetY(d.GetY());

    Point b;
    b.SetX(a.GetX() + (len / MATH_TWO)); // half of len, for point position calculation
    b.SetY(a.GetY() + std::sqrt((c.GetX() - d.GetX()) * (c.GetX() - d.GetX()) + (len / MATH_TWO) * (len / MATH_TWO)));

    Point e;
    e.SetX(a.GetX() - (len / MATH_TWO)); // half of len, for point position calculation
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
}

std::vector<PathTest::TestFunc> PathTest::PathTestCase()
{
    std::vector<TestFunc> testFuncVec;
    testFuncVec.push_back(TestDrawStar);
    testFuncVec.push_back(TestMoveTo);
    testFuncVec.push_back(TestLineTo);
    testFuncVec.push_back(TestArcTo);
    testFuncVec.push_back(TestCubicTo);
    testFuncVec.push_back(TestQuadTo);

    testFuncVec.push_back(TestAddRect);
    testFuncVec.push_back(TestAddOval);
    testFuncVec.push_back(TestAddArc);
    testFuncVec.push_back(TestAddPoly);
    testFuncVec.push_back(TestAddCircle);
    testFuncVec.push_back(TestAddRoundRect);
    testFuncVec.push_back(TestAddPath);

    testFuncVec.push_back(TestFillStyle);
    testFuncVec.push_back(TestFillStyle2);
    testFuncVec.push_back(TestFillStyle3);
    testFuncVec.push_back(TestFillStyle4);

    testFuncVec.push_back(TestOffset);
    testFuncVec.push_back(TestTransform);
    testFuncVec.push_back(TestOp);
    testFuncVec.push_back(TestOp2);
    testFuncVec.push_back(TestOp3);
    testFuncVec.push_back(TestOp4);
    testFuncVec.push_back(TestOp5);
    testFuncVec.push_back(TestClose);
    return testFuncVec;
}

void PathTest::TestMoveTo(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestMoveTo");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    path.AddRect(rect);
    path.MoveTo(rect.GetLeft(), rect.GetTop());
    path.LineTo(rect.GetRight(), rect.GetBottom());
    path.MoveTo(rect.GetRight(), rect.GetTop());
    path.LineTo(rect.GetLeft(), rect.GetBottom());
    // half of width
    path.MoveTo(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop());
    // half of width
    path.LineTo(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetBottom());
    path.MoveTo(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 2.0f);
    path.LineTo(rect.GetRight(), rect.GetTop() + rect.GetHeight() / 2.0f);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawPath(path);
}

void PathTest::TestLineTo(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestLineTo");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    std::vector<Point> linePoints;
    // a quarter of width
    linePoints.emplace_back(Point(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop()));
    // a quarter of width
    linePoints.emplace_back(Point(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetBottom()));
    // a quarter of width
    linePoints.emplace_back(Point(rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetTop()));
    // a quarter of width
    linePoints.emplace_back(Point(rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetBottom()));
    linePoints.emplace_back(Point(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 4.0f));
    linePoints.emplace_back(Point(rect.GetRight(), rect.GetTop() + rect.GetHeight() / 4.0f));
    linePoints.emplace_back(Point(rect.GetLeft(), rect.GetBottom() - rect.GetHeight() / 4.0f));
    linePoints.emplace_back(Point(rect.GetRight(), rect.GetBottom() - rect.GetHeight() / 4.0f));

    Path path;
    // two point one line
    for (size_t i = 0; i < linePoints.size(); i += 2) {
        path.MoveTo(linePoints.at(i).GetX(), linePoints.at(i).GetY());
        path.LineTo(linePoints.at(i + 1).GetX(), linePoints.at(i + 1).GetY());
    }

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawPath(path);
}

void PathTest::TestArcTo(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestArcTo");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;

    // half of width
    path.MoveTo(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 2.0f);
    // start angle is -90, sweep angle is 270
    path.ArcTo(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom(), -90, 270);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawRect(rect);

    pen.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).DrawPath(path);
}

void PathTest::TestCubicTo(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestCubicTo");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    path.MoveTo(rect.GetLeft(), rect.GetTop());
    path.CubicTo(rect.GetLeft(), rect.GetBottom(), rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom());

    Path path2;
    path2.MoveTo(rect.GetLeft(), rect.GetTop());
    path2.LineTo(rect.GetLeft(), rect.GetBottom());
    path2.LineTo(rect.GetRight(), rect.GetTop());
    path2.LineTo(rect.GetRight(), rect.GetBottom());

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawPath(path2);

    pen.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).DrawPath(path);
}

void PathTest::TestQuadTo(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestQuadTo");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    path.MoveTo(rect.GetLeft(), rect.GetTop());
    // half of width
    path.QuadTo(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetBottom(), rect.GetRight(), rect.GetTop());

    Path path2;
    path2.MoveTo(rect.GetLeft(), rect.GetTop());
    // half of width
    path2.LineTo(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetBottom());
    path2.LineTo(rect.GetRight(), rect.GetTop());

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawPath(path2);

    pen.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).DrawPath(path);
}

void PathTest::TestAddRect(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestAddRect");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    path.AddRect(rect);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    pen.SetCapStyle(Pen::CapStyle::SQUARE_CAP);
    scalar intervals[] = { 5, 20.0f }; // solid line length is 5, dotted line length is 20.0
    // intervals[] count is 2
    pen.SetPathEffect(PathEffect::CreateDashPathEffect(intervals, 2, 0));
    canvas.AttachPen(pen).DrawPath(path);

    // a quarter of height
    Rect rect2(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 4.0f,
        rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetBottom() - rect.GetHeight() / 4.0f);
    Path path2;
    path2.AddRect(rect2);

    Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.DetachPen().AttachBrush(brush).DrawPath(path2);
}

void PathTest::TestAddOval(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestAddOval");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    path.AddOval(rect);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    scalar intervals[] = { 5, 20.0f }; // solid line length is 5, dotted line length is 20.0
    // intervals[] count is 2
    pen.SetPathEffect(PathEffect::CreateDashPathEffect(intervals, 2, 0));
    canvas.AttachPen(pen).DrawPath(path);

    // a quarter of height
    Rect rect2(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 4.0f,
        rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetBottom() - rect.GetHeight() / 4.0f);
    Path path2;
    path2.AddOval(rect2);

    Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.DetachPen().AttachBrush(brush).DrawPath(path2);
}

void PathTest::TestAddArc(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestAddArc");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    // start angle is -180, sweep angle is 180
    path.AddArc(rect, -180, 180);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawPath(path);

    Path path2;
    // start angle is 0, sweep angle is 180
    path2.AddArc(rect, 0, 180);

    Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.DetachPen().AttachBrush(brush).DrawPath(path2);
}

void PathTest::TestAddPoly(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestAddPoly");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    // half of width
    std::vector<Point> points = { Point(rect.GetLeft(), rect.GetTop()),
        Point(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetBottom()), Point(rect.GetRight(), rect.GetTop()) };
    // points array count is 3
    path.AddPoly(points, 3, true);

    Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.DetachPen().AttachBrush(brush).DrawPath(path);
}

void PathTest::TestAddCircle(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestAddCircle");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // start radius is 10, radius length is increased by 20 at a time, not more than half the width
    for (size_t i = 10; i < rect.GetWidth() / 2.0f; i += 20) {
        Path path;
        // half of width and height
        path.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + rect.GetHeight() / 2.0f, i);
        Pen pen;
        pen.SetAntiAlias(true);
        pen.SetColor(Drawing::Color::COLOR_BLACK);
        pen.SetWidth(10); // The thickness of the pen is 10
        canvas.AttachPen(pen).DrawPath(path);
    }
}

void PathTest::TestAddRoundRect(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestAddRoundRect");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    // the corner radius is 30.0
    path.AddRoundRect(rect, 30.0f, 30.0f);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawPath(path);

    // a quarter of height
    Rect rect2(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 4.0f,
        rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetBottom() - rect.GetHeight() / 4.0f);
    Path path2;
    // the corner radius is 30.0
    path2.AddRoundRect(rect2, 30.0f, 30.0f);

    Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.DetachPen().AttachBrush(brush).DrawPath(path2);
}

void PathTest::TestAddPath(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestAddPath");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    path.AddRect(rect);

    // a quarter of height
    Rect rect2(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 4.0f,
        rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetBottom() - rect.GetHeight() / 4.0f);
    Path path2;
    // the corner radius is 30.0
    path2.AddRoundRect(rect2, 30.0f, 30.0f);

    Path destPath;
    destPath.AddPath(path);
    destPath.AddPath(path2);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawPath(destPath);
}

void PathTest::TestFillStyle(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestFillStyle_EVENTODD");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a quarter of height
    Rect rect1(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom() - rect.GetHeight() / 4.0f);
    Rect rect2(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 4.0f, rect.GetRight(), rect.GetBottom());
    Path path;
    path.AddRect(rect1);
    path.AddRect(rect2);
    path.SetFillStyle(PathFillType::EVENTODD);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);

    Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).AttachBrush(brush).DrawPath(path);
}

void PathTest::TestFillStyle2(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestFillStyle_INVERSE_EVENTODD");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a quarter of height
    Rect rect1(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom() - rect.GetHeight() / 4.0f);
    Rect rect2(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 4.0f, rect.GetRight(), rect.GetBottom());
    Path path;
    path.AddRect(rect1);
    path.AddRect(rect2);
    path.SetFillStyle(PathFillType::INVERSE_EVENTODD);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);

    Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).AttachBrush(brush).DrawPath(path);
}

void PathTest::TestFillStyle3(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestFillStyle_WINDING");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a quarter of height
    Rect rect1(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom() - rect.GetHeight() / 4.0f);
    Rect rect2(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 4.0f, rect.GetRight(), rect.GetBottom());
    Path path;
    path.AddRect(rect1);
    path.AddRect(rect2);
    path.SetFillStyle(PathFillType::WINDING);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);

    Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).AttachBrush(brush).DrawPath(path);
}

void PathTest::TestFillStyle4(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestFillStyle_INVERSE_WINDING");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a quarter of height
    Rect rect1(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom() - rect.GetHeight() / 4.0f);
    Rect rect2(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 4.0f, rect.GetRight(), rect.GetBottom());
    Path path;
    path.AddRect(rect1);
    path.AddRect(rect2);
    path.SetFillStyle(PathFillType::INVERSE_WINDING);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);

    Brush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).AttachBrush(brush).DrawPath(path);
}

void PathTest::TestOffset(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestOffset");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a quarter of width and height
    Rect rect2(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop(), rect.GetRight() - rect.GetWidth() / 4.0f,
        rect.GetBottom() - rect.GetHeight() / 4.0f);
    Path path;
    path.AddRect(rect2);
    // a quarter of height
    path.Offset(0, rect.GetHeight() / 4.0f);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawRect(rect2);
    pen.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).DrawPath(path);
}

void PathTest::TestTransform(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestTransform");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path;
    // half of height
    path.MoveTo(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + rect.GetHeight() / 2.0f);
    // half of height
    path.LineTo(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + (rect.GetHeight() - rect.GetHeight()) / 2.0f);
    // half of height
    path.LineTo(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 2.0f);
    Matrix m;
    // rotate angle is 36
    m.Rotate(36, rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + rect.GetHeight() / 2.0f);
    // transform 10 times
    for (size_t i = 0; i < 10; i++) {
        path.Transform(m);
        Pen pen;
        pen.SetAntiAlias(true);
        pen.SetColor(Drawing::Color::COLOR_BLACK);
        canvas.AttachPen(pen).DrawPath(path);
    }
}

void PathTest::TestOp(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestOp_DIFFERENCE");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a third of height
    const uint32_t radius = rect.GetHeight() / 3.0f;
    Path path1;
    // half of width
    path1.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + radius, radius);
    Path path2;
    // half of width
    path2.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetBottom() - radius, radius);
    Path path;

    if (path.Op(path1, path2, PathOp::DIFFERENCE)) {
        Pen pen;
        pen.SetAntiAlias(true);
        pen.SetWidth(10); // The thickness of the pen is 10
        pen.SetColor(Drawing::Color::COLOR_BLACK);

        Brush brush;
        brush.SetAntiAlias(true);
        brush.SetColor(Drawing::Color::COLOR_RED);
        canvas.AttachBrush(brush).AttachPen(pen).DrawPath(path);
    }
}

void PathTest::TestOp2(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestOp_INTERSECT");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a third of height
    const uint32_t radius = rect.GetHeight() / 3.0f;
    Path path1;
    // half of width
    path1.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + radius, radius);
    Path path2;
    // half of width
    path2.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetBottom() - radius, radius);
    Path path;

    if (path.Op(path1, path2, PathOp::INTERSECT)) {
        Pen pen;
        pen.SetAntiAlias(true);
        pen.SetWidth(10); // The thickness of the pen is 10
        pen.SetColor(Drawing::Color::COLOR_BLACK);

        Brush brush;
        brush.SetAntiAlias(true);
        brush.SetColor(Drawing::Color::COLOR_RED);
        canvas.AttachBrush(brush).AttachPen(pen).DrawPath(path);
    }
}

void PathTest::TestOp3(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestOp_REVERSE_DIFFERENCE");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a third of height
    const uint32_t radius = rect.GetHeight() / 3.0f;
    Path path1;
    // half of width
    path1.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + radius, radius);
    Path path2;
    // half of width
    path2.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetBottom() - radius, radius);
    Path path;

    if (path.Op(path1, path2, PathOp::REVERSE_DIFFERENCE)) {
        Pen pen;
        pen.SetAntiAlias(true);
        pen.SetWidth(10); // The thickness of the pen is 10
        pen.SetColor(Drawing::Color::COLOR_BLACK);

        Brush brush;
        brush.SetAntiAlias(true);
        brush.SetColor(Drawing::Color::COLOR_RED);
        canvas.AttachBrush(brush).AttachPen(pen).DrawPath(path);
    }
}

void PathTest::TestOp4(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestOp_UNION");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a third of height
    const uint32_t radius = rect.GetHeight() / 3.0f;
    Path path1;
    // half of width
    path1.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + radius, radius);
    Path path2;
    // half of width
    path2.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetBottom() - radius, radius);
    Path path;

    if (path.Op(path1, path2, PathOp::UNION)) {
        Pen pen;
        pen.SetAntiAlias(true);
        pen.SetWidth(10); // The thickness of the pen is 10
        pen.SetColor(Drawing::Color::COLOR_BLACK);

        Brush brush;
        brush.SetAntiAlias(true);
        brush.SetColor(Drawing::Color::COLOR_RED);
        canvas.AttachBrush(brush).AttachPen(pen).DrawPath(path);
    }
}

void PathTest::TestOp5(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestOp_XOR");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    // a third of height
    const uint32_t radius = rect.GetHeight() / 3.0f;
    Path path1;
    // half of width
    path1.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + radius, radius);
    Path path2;
    // half of width
    path2.AddCircle(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetBottom() - radius, radius);
    Path path;

    if (path.Op(path1, path2, PathOp::XOR)) {
        Pen pen;
        pen.SetAntiAlias(true);
        pen.SetWidth(10); // The thickness of the pen is 10
        pen.SetColor(Drawing::Color::COLOR_BLACK);

        Brush brush;
        brush.SetAntiAlias(true);
        brush.SetColor(Drawing::Color::COLOR_RED);
        canvas.AttachBrush(brush).AttachPen(pen).DrawPath(path);
    }
}

void PathTest::TestClose(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestClose");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);
    Path path1;
    // half of width
    path1.MoveTo(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop());
    path1.LineTo(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 2.0f);
    path1.LineTo(rect.GetRight(), rect.GetTop() + rect.GetHeight() / 2.0f);
    Path path2;
    // half of width and height
    path2.MoveTo(rect.GetLeft() + rect.GetWidth() / 2.0f, rect.GetTop() + rect.GetHeight() / 2.0f);
    path2.LineTo(rect.GetLeft(), rect.GetBottom());
    path2.LineTo(rect.GetRight(), rect.GetBottom());
    path2.Close();

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachPen(pen).DrawPath(path1);
    pen.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).DrawPath(path2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS