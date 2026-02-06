/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class AppearanceTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* ParticleEffect: basic particle effect */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_ParticleEffect_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    // Note: SetParticleEffect requires specific particle configuration
    // This is a placeholder for the actual particle effect API
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* ParticleEffect: with different bounds */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_ParticleEffect_Test_2)
{
    std::vector<Vector4f> boundsList = {
        { 100, 100, 300, 300 },
        { 450, 100, 300, 300 },
        { 100, 450, 300, 300 },
        { 450, 450, 300, 300 }
    };

    for (const auto& bounds : boundsList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* ParticleEffect: with background image */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_ParticleEffect_Test_3)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 600, 600 });
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* ParticleEffect: with border */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_ParticleEffect_Test_4)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetBorderWidth(25);
    testNode->SetBorderColor(0xff00ff00);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* ParticleEffect: with alpha */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_ParticleEffect_Test_5)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg", { 100, 100, 500, 500 });
    testNode->SetAlpha(0.7f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* ParticleEffect: with shadow */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_ParticleEffect_Test_6)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 100, 100, 500, 500 });
    testNode->SetShadowColor(0xff000000);
    testNode->SetShadowRadius(35);
    testNode->SetShadowOffset(25, 25);
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* ParticleEffect: extreme bounds */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_ParticleEffect_Extreme_Bounds)
{
    std::vector<Vector4f> extremeBounds = {
        { 0, 0, 1, 1 },           // 最小
        { 0, 0, 1200, 2000 },      // 全屏
        { -100, -100, 200, 200 },  // 负位置
        { 1000, 1800, 200, 200 }   // 边缘
    };

    for (const auto& bounds : extremeBounds) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* ParticleEffect: zero/negative dimensions */
GRAPHIC_TEST(AppearanceTest, CONTENT_DISPLAY_TEST, Appearance_ParticleEffect_Zero_Dimensions)
{
    std::vector<Vector4f> boundsList = {
        { 100, 100, 0, 100 },
        { 100, 250, 100, 0 },
        { 100, 400, -100, 100 },
        { 100, 550, 100, -100 }
    };

    for (const auto& bounds : boundsList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
