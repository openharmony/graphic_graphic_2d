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

#include "matrix_test.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void MatrixTest::TestMatrix(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestMatrix");
    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    canvas.AttachBrush(brush);
    canvas.DrawRect({ 1200, 100, 1500, 400 }); // rect is set to (fLeft, fTop, fRight, fBottom)

    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachBrush(brush);
    Matrix m;
    // Set matrix to scale by 0.5 and 1.5, about a pivot point at (0, 0).
    m.Scale(0.5, 1.5, 0, 0);

    canvas.SetMatrix(m);
    canvas.DrawRect({ 1000, 0, 1300, 300 }); // rect is set to (fLeft, fTop, fRight, fBottom)

    Matrix n;
    /* Set matrix to:
        | 1  0  100 |
        | 0  1  300 |
        | 0  0   1  |
    */
    n.SetMatrix(1, 0, 100, 0, 1, 300, 0, 0, 1);
    brush.SetColor(Drawing::Color::COLOR_GREEN);
    canvas.AttachBrush(brush);
    canvas.SetMatrix(n);
    canvas.DrawRect({ 1200, 100, 1500, 400 }); // rect is set to (fLeft, fTop, fRight, fBottom)

    Matrix m1;
    Matrix m2;
    m1.Translate(100, 300); // Set matrix to translate by (100, 300).
    // Set matrix to rotate by degrees 45 about a pivot point at (0, 0).
    m2.Rotate(45, 0, 0);
    m = m1 * m2;

    brush.SetColor(Drawing::Color::COLOR_BLACK);
    canvas.AttachBrush(brush);
    canvas.SetMatrix(m);
    canvas.DrawRect({ 1200, 100, 1500, 400 }); // rect is set to (fLeft, fTop, fRight, fBottom)

    Matrix matrix;
    matrix.Translate(100, 100);              // 100 means offset size
    std::vector<Point> point = { { 0, 0 } }; // {0, 0} means point position
    matrix.MapPoints(point, point, point.size());
    for (size_t i = 0; i < point.size(); i++) {
        LOGI("mapped point fx = %{public}f, fy = %{public}f", point[i].GetX(), point[i].GetY());
    }

    LOGI("------- TestMatrix");
}

std::vector<MatrixTest::TestFunc> MatrixTest::MatrixTestCase()
{
    std::vector<TestFunc> testFuncVec;
    testFuncVec.push_back(TestMatrix);
    return testFuncVec;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS