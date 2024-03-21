/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "draw_path.h"

namespace OHOS {
namespace Rosen {
void DrawPathTest::OnTestFunctionCpu(Drawing::Canvas* canvas)
{
    Drawing::Pen pen;
    pen.SetColor(0xFFFF0000);
    pen.SetWidth(10.0f);
    canvas->AttachPen(pen);
    Drawing::Path path;
    path.MoveTo(0, 0); // from (0, 0)
    path.LineTo(300, 300); // to (300, 300)
    canvas->DrawPath(path);
    canvas->DetachPen();
}
void DrawPathTest::OnTestFunctionGpuUpScreen(Drawing::Canvas* canvas)
{
    Drawing::Path path;
    path.MoveTo(200, 200); // from (200, 200)
    path.LineTo(500, 500); // to (500, 500)
    canvas->DrawPath(path);
}
void DrawPathTest::OnTestPerformance(Drawing::Canvas* canvas)
{
    Drawing::Pen pen;
    pen.SetColor(0xFFFF0000);
    pen.SetWidth(10.0f);
    canvas->AttachPen(pen);
    Drawing::Path path;
    path.MoveTo(0, 0); // from (0, 0)
    path.LineTo(300, 300); // to (300, 300)
    for (int i = 0; i < testCount_; i++) {
        canvas->DrawPath(path);
    }
    canvas->DetachPen();
}
} // namespace Rosen
} // namespace OHOS