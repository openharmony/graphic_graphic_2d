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

#include "camera_test.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr scalar POSITION_RIGHT = 500;
static constexpr scalar POSITION_BOTTOM = 500;
void CameraTest::TestCamera(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestCamera");

    Brush brush;
    Matrix m0;
    m0.Translate(width / 2.0, height / 2.0); // Set matrix m0 to translate by (width / 2.0, height / 2.0).
    canvas.SetMatrix(m0);

    Camera3D camera;
    camera.RotateXDegrees(-25);      // Set camera to rotate by degrees -25 at x-aixs.
    camera.RotateYDegrees(45);       // Set camera to rotate by degrees 45 at y-aixs.
    camera.Translate(-50, 50, 50);   // Set camera to translate by (-50, 50, 50).
    Drawing::Rect r(0, 0, POSITION_RIGHT, POSITION_BOTTOM); // rect is set to (fLeft, fTop, fRight, fBottom)

    canvas.Save();
    camera.Save();
    camera.RotateYDegrees(90); // Set camera to rotate by degrees 90 at y-aixs.
    Matrix m1;
    camera.ApplyToMatrix(m1);
    camera.Restore();
    canvas.ConcatMatrix(m1);
    brush.SetColor(Drawing::Color::COLOR_LTGRAY);
    canvas.AttachBrush(brush);
    canvas.DrawRect(r);
    canvas.Restore();
    LOGI("------- TestCamera");
}

std::vector<CameraTest::TestFunc> CameraTest::CameraTestCase()
{
    std::vector<TestFunc> testFuncVec;
    testFuncVec.push_back(TestCamera);
    return testFuncVec;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS