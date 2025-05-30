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
#include "draw_rect_test.h"

namespace OHOS {
namespace Rosen {
void DrawRectTest::OnTestFunction(Drawing::Canvas* canvas)
{
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000); // color:red
    canvas->AttachBrush(brush);
    auto rect = Drawing::Rect(0, 0, 100, 100); // xy (0, 0), size 100*100
    canvas->DrawRect(rect);
    canvas->DetachBrush();
}
void DrawRectTest::OnTestPerformance(Drawing::Canvas* canvas)
{
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000); // color:red
    canvas->AttachBrush(brush);
    auto rect = Drawing::Rect(0, 0, 100, 100); // xy (0, 0), size 100*100
    for (int i = 0; i < testCount_; i++) {
        canvas->DrawRect(rect);
    }
    canvas->DetachBrush();
}
} // namespace Rosen
} // namespace OHOS