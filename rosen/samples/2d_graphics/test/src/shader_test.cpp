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

#include "shader_test.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void ShaderTest::TestDrawShader(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawShader");

    Brush brush;
    brush.SetColor(Drawing::Color::COLOR_BLUE);
    std::vector<ColorQuad> colors { Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE,
        Drawing::Color::COLOR_RED };
    std::vector<scalar> pos { 0.0, 0.5, 1.0 };
    // The start and end points for the gradient: {1200, 700}, {1300, 800}.
    auto e = ShaderEffect::CreateLinearGradient({ 1200, 700 }, { 1300, 800 }, colors, pos, TileMode::MIRROR);
    brush.SetShaderEffect(e);

    canvas.AttachBrush(brush);
    canvas.DrawRect({ 1200, 700, 1500, 1000 }); // rect is set to (fLeft, fTop, fRight, fBottom)
    LOGI("------- TestDrawShader");
}

std::vector<ShaderTest::TestFunc> ShaderTest::ShaderTestCase()
{
    std::vector<TestFunc> testFuncVec;
    testFuncVec.push_back(TestDrawShader);
    return testFuncVec;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS