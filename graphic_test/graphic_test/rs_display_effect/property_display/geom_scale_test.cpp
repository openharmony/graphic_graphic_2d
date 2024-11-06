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

#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class GeometryTest : public RSGraphicTest {
private:
    const int screenWidth = 1260;
    const int screenHeight = 2720;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSurfaceBounds({ 0, 0, screenWidth, screenHeight });
    }
};

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_1)
{
    float zList[] = { 1, 0.5, 2 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            auto testNode =
                SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { j * 420 + 110, i * 420 + 110, 200, 200 });
            testNode->SetPivot(Vector2f(0.5, 0.5));
            testNode->SetScaleX(zList[i]);
            testNode->SetScaleY(zList[j]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Scale_Test_2)
{
    float scales[] = { 1, 0.5, 2 };
    for (int j = 0; j < 3; j++) {
        auto testNode =
            SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, j * 780 + 100, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetScaleZ(scales[j]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen