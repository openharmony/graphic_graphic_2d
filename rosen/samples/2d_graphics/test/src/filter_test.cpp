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

#include "filter_test.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void FilterTest::TestDrawFilter(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawFilter");

    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(Drawing::Color::COLOR_BLUE);
    pen.SetColor(pen.GetColor4f(), ColorSpace::CreateSRGBLinear());
    pen.SetAlpha(0x44); // alpha value is 0x44
    pen.SetWidth(10);   // The thickness of the pen is 10
    Filter filter;
    filter.SetColorFilter(ColorFilter::CreateBlendModeColorFilter(Drawing::Color::COLOR_RED, BlendMode::SRC_ATOP));
    // Radius of the Gaussian blur to apply is 10.
    filter.SetMaskFilter(MaskFilter::CreateBlurMaskFilter(BlurType::NORMAL, 10));
    pen.SetFilter(filter);
    canvas.AttachPen(pen);

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    brush.SetColor(brush.GetColor4f(), ColorSpace::CreateSRGBLinear());
    brush.SetAlpha(0x44); // alpha value is 0x44
    brush.SetBlendMode(BlendMode::SRC_ATOP);
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);
    canvas.DrawRect({ 1200, 300, 1500, 600 }); // rect is set to (fLeft, fTop, fRight, fBottom)

    canvas.DetachPen();
    canvas.DrawRect({ 1200, 700, 1500, 1000 }); // rect is set to (fLeft, fTop, fRight, fBottom)
    LOGI("------- TestDrawFilter");
}

std::vector<FilterTest::TestFunc> FilterTest::FilterTestCase()
{
    std::vector<TestFunc> testFuncVec;
    testFuncVec.push_back(TestDrawFilter);
    return testFuncVec;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS