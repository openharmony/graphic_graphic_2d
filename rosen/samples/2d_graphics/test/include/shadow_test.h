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

#ifndef SHADOW_TEST_H
#define SHADOW_TEST_H

#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "draw/pen.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ShadowTest {
public:
    using TestFunc = std::function<void(Canvas&, uint32_t, uint32_t)>;
    inline static ShadowTest& GetInstance()
    {
        static ShadowTest shadowTestCase;
        return shadowTestCase;
    }

    static void TestDrawShadow(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestDrawShadowStyle(Canvas& canvas, uint32_t width, uint32_t height);
    std::vector<TestFunc> ShadowTestCase();

private:
    ShadowTest() = default;
    virtual ~ShadowTest() = default;
};

void ShadowTest::TestDrawShadow(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawShadow");

    Path path;
    // Add oval to path, bounds of ellipse added is {200, 200, 600, 1000}.
    path.AddOval({ 200, 200, 600, 1000 });
    Point3 planeParams = { 540.0, 0.0, 600.0 };
    Point3 devLightPos = { 0, 0, 0 };
    scalar lightRadius = 0.5;
    Drawing::Color ambientColor = Drawing::Color::ColorQuadSetARGB(0, 0, 0, 0);
    Drawing::Color spotColor = Drawing::Color::COLOR_RED;
    ShadowFlags flag = ShadowFlags::TRANSPARENT_OCCLUDER;
    canvas.DrawShadow(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
    LOGI("------- TestDrawShadow");
}

void ShadowTest::TestDrawShadowStyle(Canvas& canvas, uint32_t width, uint32_t height)
{
    LOGI("+++++++ TestDrawShadowStyle");

    Path path;
    // Add oval to path, bounds of ellipse added is {200, 200, 600, 1000}.
    path.AddOval({ 200, 200, 600, 1000 });
    Point3 planeParams = { 540.0, 0.0, 600.0 };
    Point3 devLightPos = { 0, 0, 0 };
    scalar lightRadius = 0.5;
    Drawing::Color ambientColor = Drawing::Color::ColorQuadSetARGB(0, 0, 0, 0);
    Drawing::Color spotColor = Drawing::Color::COLOR_RED;
    ShadowFlags flag = ShadowFlags::TRANSPARENT_OCCLUDER;
    canvas.DrawShadowStyle(path, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag, true);
    LOGI("------- TestDrawShadowStyle");
}

std::vector<ShadowTest::TestFunc> ShadowTest::ShadowTestCase()
{
    std::vector<TestFunc> testFuncVec;
    testFuncVec.push_back(TestDrawShadow);
    testFuncVec.push_back(TestDrawShadowStyle);
    return testFuncVec;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SHADOW_TEST_H