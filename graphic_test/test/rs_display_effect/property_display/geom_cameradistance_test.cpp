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
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, CameraDistance_Test_1)
{
    std::vector<float> zList = { -1, 0.0, 0.5, 1, 100.0 };
    for (int i = 0; i < zList.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 380, i * 380 + 40, 600, 600 });
        testNode->SetPivot(Vector2f(0.5, 0.5));
        testNode->SetRotation(45.0, 0, 45.0);
        testNode->SetCameraDistance(zList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, CameraDistance_Test_2)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 0, 0, 500, 500 });
    testNode->SetPivot(Vector2f(0.5, 0.5));
    testNode->SetRotation(45.0, 0, 45.0);
    testNode->SetCameraDistance(-50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, CameraDistance_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 0, 0, 500, 500 });
    testNode->SetPivot(Vector2f(0.5, 0.5));
    testNode->SetRotation(45.0, 0, 45.0);
    testNode->SetCameraDistance(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, CameraDistance_Test_4)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 0, 0, 500, 500 });
    testNode->SetPivot(Vector2f(0.5, 0.5));
    testNode->SetRotation(45.0, 0, 45.0);
    testNode->SetCameraDistance(0);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, CameraDistance_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 0, 0, 500, 500 });
    testNode->SetPivot(Vector2f(0.5, 0.5));
    testNode->SetRotation(45.0, 45.0, 0);
    testNode->SetCameraDistance(-50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, CameraDistance_Test_6)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 0, 0, 500, 500 });
    testNode->SetPivot(Vector2f(0.5, 0.5));
    testNode->SetRotation(180.0, 180.0, 0);
    testNode->SetCameraDistance(50);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(GeometryTest, CONTENT_DISPLAY_TEST, CameraDistance_Test_7)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 0, 0, 500, 500 });
    testNode->SetPivot(Vector2f(0.5, 0.5));
    testNode->SetRotation(90.0, 90.0, 90.0);
    testNode->SetCameraDistance(0);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}
} // namespace OHOS::Rosen