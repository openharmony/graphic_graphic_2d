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

#include "pen_test.h"

#include "draw/brush.h"
#include "draw/color.h"
#include "draw/path.h"
#include "draw/pen.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr static float MARGINE_SCALE_SIZE = 10.0f;
void PenTest::TestPenColor(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenColor");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetWidth(10); // the thickness of the pen is 10

    Rect rect(margin, margin, width - margin, height - margin);
    pen.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).DrawRect(rect);

    Rect rect1(rect.GetLeft() + margin, rect.GetTop() + margin,
        rect.GetRight() - margin, rect.GetBottom() - margin);
    // r = 80, g = 227, b = 194, a = 255 max < 255, 
    pen.SetARGB(80, 227, 194, 255);
    canvas.AttachPen(pen).DrawRect(rect1);

    Rect rect2(rect1.GetLeft() + margin, rect1.GetTop() + margin,
        rect1.GetRight() - margin, rect1.GetBottom() - margin);
    // r = 0.5, g = 0.5, b = 0.5, a = 1.0 max < 1.0
    pen.SetColor({0.5f, 0.5f, 0.5f, 1.0f}, ColorSpace::CreateSRGB());
    canvas.AttachPen(pen).DrawRect(rect2);

    Rect rect3(rect2.GetLeft() + margin, rect2.GetTop() + margin,
        rect2.GetRight() - margin, rect2.GetBottom() - margin);
    Color c(Drawing::Color::COLOR_YELLOW);
    pen.SetColor(c);
    canvas.AttachPen(pen).DrawRect(rect3);
}

void PenTest::TestPenAlpha(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenAlpha");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetWidth(10); // the thickness of the pen is 10

    Rect rect(margin, margin, width - margin, height - margin);
    pen.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen).DrawRect(rect);

    Rect rect1(rect.GetLeft() + margin, rect.GetTop() + margin,
        rect.GetRight() - margin, rect.GetBottom() - margin);
    // alpha value is 0.5
    pen.SetAlphaF(0.5f);
    canvas.AttachPen(pen).DrawRect(rect1);

    Rect rect2(rect1.GetLeft() + margin, rect1.GetTop() + margin,
        rect1.GetRight() - margin, rect1.GetBottom() - margin);
    // alpha value is 0x10
    pen.SetAlpha(0x10);
    canvas.AttachPen(pen).DrawRect(rect2);
}

void PenTest::TestPenWidth(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenWidth");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);

    Rect rect(margin, margin, width - margin, height - margin);
    pen.SetWidth(10); // the thickness of the pen is 10
    canvas.AttachPen(pen).DrawRect(rect);

    Rect rect1(rect.GetLeft() + margin, rect.GetTop() + margin,
        rect.GetRight() - margin, rect.GetBottom() - margin);
    pen.SetWidth(pen.GetWidth() * 2); // the thickness of the pen is 20
    canvas.AttachPen(pen).DrawRect(rect1);
}

void PenTest::TestPenMiterLimit(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenMiterLimit");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(20); // the thickness of the pen is 20

    // 0.2 of height
    Point a(rect.GetLeft(), rect.GetTop() + rect.GetHeight() * 2.0f / MARGINE_SCALE_SIZE);
    // 0.3 of height
    Point b(rect.GetLeft(), rect.GetTop() + rect.GetHeight() * 3.0f / MARGINE_SCALE_SIZE);
    // half of width and 0.25 of height
    Point c(rect.GetLeft() + rect.GetWidth() / 2.0f,
        rect.GetTop() + rect.GetHeight() * 2.5f / MARGINE_SCALE_SIZE);
    Path path;
    path.MoveTo(a.GetX(), a.GetY());
    path.LineTo(c.GetX(), c.GetY());
    path.LineTo(b.GetX(), b.GetY());
    canvas.AttachPen(pen).DrawPath(path);

    // 0.3 of height
    canvas.Translate(0, rect.GetHeight() * 3.0f / MARGINE_SCALE_SIZE);
    // miter limit is 10.0
    pen.SetMiterLimit(10.0f);
    canvas.AttachPen(pen).DrawPath(path);

    // 0.3 of height
    canvas.Translate(0, rect.GetHeight() * 3.0f / MARGINE_SCALE_SIZE);
    // miter limit is 5.0
    pen.SetMiterLimit(5.0f);
    canvas.AttachPen(pen).DrawPath(path);
}

void PenTest::TestPenCapStyle(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenCapStyle");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(30); // the thickness of the pen is 30

    // a quarter of width and height
    Point a(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 4.0f);
    // a quarter of width and height
    Point b(rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 4.0f);
    pen.SetCapStyle(Pen::CapStyle::FLAT_CAP);
    canvas.AttachPen(pen).DrawLine(a, b);

    // a quarter of width and half height
    Point c(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 2.0f);
    // a quarter of width and half height
    Point d(rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 2.0f);
    pen.SetCapStyle(Pen::CapStyle::SQUARE_CAP);
    canvas.AttachPen(pen).DrawLine(c, d);

    // a quarter of width and height
    Point e(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetBottom() - rect.GetHeight() / 4.0f);
    // a quarter of width and height
    Point f(rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetBottom() - rect.GetHeight() / 4.0f);
    pen.SetCapStyle(Pen::CapStyle::ROUND_CAP);
    canvas.AttachPen(pen).DrawLine(e, f);
}

void PenTest::TestPenJoinStyle(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenJoinStyle");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(20); // the thickness of the pen is 20

    Rect rect(margin, margin, width - margin, height - margin);
    pen.SetJoinStyle(Pen::JoinStyle::MITER_JOIN);
    canvas.AttachPen(pen).DrawRect(rect);

    Rect rect1(rect.GetLeft() + margin, rect.GetTop() + margin,
        rect.GetRight() - margin, rect.GetBottom() - margin);
    pen.SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);
    canvas.AttachPen(pen).DrawRect(rect1);

    Rect rect2(rect1.GetLeft() + margin, rect1.GetTop() + margin,
        rect1.GetRight() - margin, rect1.GetBottom() - margin);
    pen.SetJoinStyle(Pen::JoinStyle::BEVEL_JOIN);
    canvas.AttachPen(pen).DrawRect(rect2);
}

void PenTest::TestPenBlendMode(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenBlendMode");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetWidth(30); // the thickness of the pen is 30

    // a quarter of width and height
    Rect rect1(rect.GetLeft(), rect.GetTop() + rect.GetHeight() / 4.0f,
        rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetBottom() - rect.GetHeight() / 4.0f);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetBlendMode(Drawing::BlendMode::SRC_OVER);
    canvas.AttachPen(pen).DrawRect(rect1);

    Pen blenderPen;
    blenderPen.SetAntiAlias(true);
    blenderPen.SetWidth(30); // the thickness of the pen is 30
    // a quarter of width and height
    Rect rect2(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 4.0f,
        rect.GetRight(), rect.GetBottom() - rect.GetHeight() / 4.0f);
    blenderPen.SetColor(Drawing::Color::COLOR_BLUE);
    blenderPen.SetBlendMode(Drawing::BlendMode::SRC_OUT);
    canvas.AttachPen(blenderPen).DrawRect(rect2);
}

void PenTest::TestPenAntiAlias(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenAntiAlias");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Rect rect(margin, margin, width - margin, height - margin);

    Pen pen;
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(30); // the thickness of the pen is 30

    // a quarter of width and height
    Point a(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 4.0f);
    Point b(rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetTop() + rect.GetHeight() / 4.0f);
    Point e(rect.GetLeft() + rect.GetWidth() / 4.0f, rect.GetBottom() - rect.GetHeight() / 4.0f);
    Point f(rect.GetRight() - rect.GetWidth() / 4.0f, rect.GetBottom() - rect.GetHeight() / 4.0f);

    pen.SetAntiAlias(true);
    canvas.AttachPen(pen).DrawLine(a, f);

    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetAntiAlias(false);
    canvas.AttachPen(pen).DrawLine(b, e);
}

void PenTest::TestPenPathEffect(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenPathEffect");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(20); // The thickness of the pen is 20

    Rect rect(margin, margin, width - margin, height - margin);
    canvas.AttachPen(pen).DrawRect(rect);

    Rect rect1(rect.GetLeft() + margin, rect.GetTop() + margin,
        rect.GetRight() - margin, rect.GetBottom() - margin);
    scalar vals[] = { 10.0, 20.0 };
    // number of elements in the intervals array is 2; offset into the intervals array is 25.
    pen.SetPathEffect(PathEffect::CreateDashPathEffect(vals, 2, 25));
    canvas.AttachPen(pen).DrawRect(rect1);

    Rect rect2(rect1.GetLeft() + margin, rect1.GetTop() + margin,
        rect1.GetRight() - margin, rect1.GetBottom() - margin);
    Path shapePath;
    shapePath.MoveTo(0, 0);
    shapePath.LineTo(0, 20);
    shapePath.LineTo(20, 0);
    shapePath.Close();
    // advance is 20 and phase is 0
    pen.SetPathEffect(PathEffect::CreatePathDashEffect(shapePath, 20, 0, PathDashStyle::MORPH));
    canvas.AttachPen(pen).DrawRect(rect2);
}

void PenTest::TestPenFilter(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenFilter");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(20); // the thickness of the pen is 20

    Rect rect(margin, margin, width - margin, height - margin);

    Rect rect1(rect.GetLeft() + margin, rect.GetTop() + margin,
        rect.GetRight() - margin, rect.GetBottom() - margin);
    Filter filter;
    // radius of the gaussian blur to apply is 10
    filter.SetMaskFilter(MaskFilter::CreateBlurMaskFilter(BlurType::NORMAL, 10));
    pen.SetFilter(filter);
    canvas.AttachPen(pen).DrawRect(rect1);

    Rect rect2(rect1.GetLeft() + margin, rect1.GetTop() + margin,
        rect1.GetRight() - margin, rect1.GetBottom() - margin);
    filter.SetMaskFilter(nullptr);
    pen.SetFilter(filter);
    canvas.AttachPen(pen).DrawRect(rect2);
}

void PenTest::TestPenReset(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestPenReset");
    const uint32_t margin = static_cast<uint32_t>(width / MARGINE_SCALE_SIZE);
    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(20); // The thickness of the pen is 20

    Rect rect(margin, margin, width - margin, height - margin);

    Rect rect1(rect.GetLeft() + margin, rect.GetTop() + margin,
        rect.GetRight() - margin, rect.GetBottom() - margin);
    Filter filter;
    // radius of the gaussian blur to apply is 10.
    filter.SetMaskFilter(MaskFilter::CreateBlurMaskFilter(BlurType::NORMAL, 10));
    pen.SetFilter(filter);
    canvas.AttachPen(pen).DrawRect(rect1);

    Rect rect2(rect1.GetLeft() + margin, rect1.GetTop() + margin,
        rect1.GetRight() - margin, rect1.GetBottom() - margin);
    pen.Reset();
    canvas.AttachPen(pen).DrawRect(rect2);
}

std::vector<PenTest::TestFunc> PenTest::PenTestCase()
{
    std::vector<TestFunc> testFuncVec;
    testFuncVec.push_back(TestPenColor);
    testFuncVec.push_back(TestPenAlpha);
    testFuncVec.push_back(TestPenWidth);
    testFuncVec.push_back(TestPenMiterLimit);
    testFuncVec.push_back(TestPenCapStyle);
    testFuncVec.push_back(TestPenJoinStyle);
    testFuncVec.push_back(TestPenBlendMode);
    testFuncVec.push_back(TestPenAntiAlias);
    testFuncVec.push_back(TestPenPathEffect);
    testFuncVec.push_back(TestPenFilter);
    testFuncVec.push_back(TestPenReset);
    return testFuncVec;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS