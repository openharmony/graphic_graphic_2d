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

#include "path_effect_test.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void PathEffectTest::TestDrawDashPathEffect(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawDashPathEffect");

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_RED);
    pen.SetWidth(10); // The thickness of the pen is 10
    scalar vals[] = { 10.0, 20.0 };
    // Number of elements in the intervals array is 2; offset into the intervals array is 25.
    pen.SetPathEffect(PathEffect::CreateDashPathEffect(vals, 2, 25));
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    canvas.AttachBrush(brush);
    canvas.DrawRect({ 1200, 300, 1500, 600 }); // rect is set to (fLeft, fTop, fRight, fBottom)

    canvas.DetachPen();
    canvas.DrawRect({ 1200, 700, 1500, 1000 }); // rect is set to (fLeft, fTop, fRight, fBottom)
    LOGI("------- TestDrawDashPathEffect");
}

std::vector<PathEffectTest::TestFunc> PathEffectTest::PathEffectTestCase()
{
    std::vector<TestFunc> testFuncVec;
    testFuncVec.push_back(TestDrawDashPathEffect);
    return testFuncVec;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS