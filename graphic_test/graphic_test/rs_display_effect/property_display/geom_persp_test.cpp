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

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_1)
{
    float xList[] = { -1.0, 0.0, 2.0 };
    float yList[] = { -1.0, 0.0, 1.0 };
    for (int i = 0; i < 1; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 680 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 45.0);
        testNode->SetPerspX(xList[i]);
        testNode->SetPerspY(yList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_2)
{
    float xList[] = { -1.0, 0.0, 1 };
    float yList[] = { -1.0, 0.0, 0 };
    for (int i = 1; i < 3; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, (i - 1) * 680 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 45.0);
        testNode->SetPerspX(xList[i]);
        testNode->SetPerspY(yList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_3)
{
    float persp[] = { 0.8, 1.0 };
    for (int i = 0; i < 2; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 880 + 200, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(0.0, 0, 45.0);
        testNode->SetPerspW(persp[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, Geometry_Persp_Test_4)
{
    float perspz[] = { 0.8, 0.2 };
    float perspw[] = { 0.8, 1.0 };
    for (int i = 0; i < 2; i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 880 + 200, 600, 600 });
        testNode->SetRotation(Quaternion(0.0, 0.0, 0.382, 0.923));
        testNode->SetPerspZ(perspz[i]);
        testNode->SetPerspW(perspw[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}
} // namespace OHOS::Rosen