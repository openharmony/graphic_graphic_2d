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

class SurfaceTextureExportTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/*
 * @tc.name: SurfaceTextureExportTest_001
 * @tc.desc: Test RSSurfaceNode::SetTextureExport with enabled state
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_001)
{
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Enable texture export
    surfaceNode->SetTextureExport(true);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: SurfaceTextureExportTest_002
 * @tc.desc: Test RSSurfaceNode::SetTextureExport with disabled state
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_002)
{
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Disable texture export
    surfaceNode->SetTextureExport(false);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: SurfaceTextureExportTest_003
 * @tc.desc: Test RSSurfaceNode::SetTextureExport toggle (3x4 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_003)
{
    std::vector<bool> exportStates = { true, false };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < exportStates.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            RSSurfaceNodeConfig config;
        auto surfaceNode = RSSurfaceNode::Create(config);
            surfaceNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            surfaceNode->SetBackgroundColor(0xffff0000);
            surfaceNode->SetAlpha(alphaList[col]);
            surfaceNode->SetTextureExport(exportStates[row]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);
        }
    }
}

/*
 * @tc.name: SurfaceTextureExportTest_004
 * @tc.desc: Test RSSurfaceNode::SetTextureExport state change
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_004)
{
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Initial state: enabled
    surfaceNode->SetTextureExport(true);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);

    // Toggle to disabled
    surfaceNode->SetTextureExport(false);

    // Toggle back to enabled
    surfaceNode->SetTextureExport(true);
}

/*
 * @tc.name: SurfaceTextureExportTest_005
 * @tc.desc: Test RSSurfaceNode::SetTextureExport with rotation
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_005)
{
    std::vector<float> rotationList = { 0, 45, 90, 135 };

    for (size_t i = 0; i < rotationList.size(); i++) {
        RSSurfaceNodeConfig config;
        auto surfaceNode = RSSurfaceNode::Create(config);
        surfaceNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetRotation(rotationList[i]);
        surfaceNode->SetTextureExport(i % 2 == 0);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: SurfaceTextureExportTest_006
 * @tc.desc: Test RSSurfaceNode::SetTextureExport with scale (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_006)
{
    std::vector<bool> exportStates = { true, false, true };
    std::vector<std::pair<float, float>> scaleList = {
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f }
    };

    for (size_t row = 0; row < exportStates.size(); row++) {
        for (size_t col = 0; col < scaleList.size(); col++) {
            RSSurfaceNodeConfig config;
        auto surfaceNode = RSSurfaceNode::Create(config);
            surfaceNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            surfaceNode->SetBackgroundColor(0xffff0000);
            surfaceNode->SetScale(scaleList[col].first, scaleList[col].second);
            surfaceNode->SetTextureExport(exportStates[row]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);
        }
    }
}

/*
 * @tc.name: SurfaceTextureExportTest_007
 * @tc.desc: Test RSSurfaceNode::SetTextureExport with zero bounds
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_007)
{
    std::vector<Vector4f> zeroBounds = {
        { 50, 50, 0, 100 },
        { 150, 50, 100, 0 },
        { 300, 50, 0, 0 }
    };

    for (const auto& bounds : zeroBounds) {
        RSSurfaceNodeConfig config;
        auto surfaceNode = RSSurfaceNode::Create(config);
        surfaceNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetTextureExport(true);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: SurfaceTextureExportTest_008
 * @tc.desc: Test RSSurfaceNode::SetTextureExport with effects
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_008)
{
    std::vector<bool> exportStates = { true, false };
    std::vector<float> blurRadius = { 0, 10, 20 };

    for (size_t row = 0; row < exportStates.size(); row++) {
        for (size_t col = 0; col < blurRadius.size(); col++) {
            RSSurfaceNodeConfig config;
        auto surfaceNode = RSSurfaceNode::Create(config);
            surfaceNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            surfaceNode->SetBackgroundColor(0xffff0000);
            surfaceNode->SetBackgroundBlurRadius(blurRadius[col]);
            surfaceNode->SetTextureExport(exportStates[row]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);
        }
    }
}

/*
 * @tc.name: SurfaceTextureExportTest_009
 * @tc.desc: Test RSSurfaceNode::SetTextureExport with alpha
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_009)
{
    std::vector<bool> exportStates = { true, false, true, false };
    std::vector<float> alphaList = { 0.0f, 0.3f, 0.6f, 1.0f };

    for (size_t i = 0; i < exportStates.size(); i++) {
        RSSurfaceNodeConfig config;
        auto surfaceNode = RSSurfaceNode::Create(config);
        surfaceNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetAlpha(alphaList[i]);
        surfaceNode->SetTextureExport(exportStates[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: SurfaceTextureExportTest_010
 * @tc.desc: Test RSSurfaceNode::SetTextureExport multiple surface nodes
 * @tc.type: FUNC
 * @tc.require: issueI7N7L9
 */
GRAPHIC_TEST(SurfaceTextureExportTest, CONTENT_DISPLAY_TEST, SurfaceTextureExportTest_010)
{
    for (int i = 0; i < 5; i++) {
        RSSurfaceNodeConfig config;
        auto surfaceNode = RSSurfaceNode::Create(config);
        surfaceNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        surfaceNode->SetBackgroundColor(0xffff0000 - i * 0x00200000);
        surfaceNode->SetTextureExport(i % 2 == 0);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

} // namespace OHOS::Rosen
