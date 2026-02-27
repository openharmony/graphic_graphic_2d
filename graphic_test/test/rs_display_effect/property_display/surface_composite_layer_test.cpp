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

class SurfaceCompositeLayerTest : public RSGraphicTest {
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
 * @tc.name: SurfaceCompositeLayerTest_001
 * @tc.desc: Test RSCanvasNode with different alpha values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_001)
{
    // Test different alpha values
    std::vector<float> alphaValues = { 0.0f, 0.3f, 0.6f, 1.0f };

    for (size_t i = 0; i < alphaValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetAlpha(alphaValues[i]);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_002
 * @tc.desc: Test RSCanvasNode with boundary alpha values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_002)
{
    std::vector<float> boundaryAlphas = {
        0.0f,        // minimum
        0.1f,        // near minimum
        0.5f,        // mid-range
        0.9f,        // near maximum
        1.0f,        // maximum
        -0.1f,       // negative (edge case)
        1.1f         // > 1.0 (edge case)
    };

    for (size_t i = 0; i < boundaryAlphas.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 140 + 50, 50, 130, 130 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetAlpha(boundaryAlphas[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_003
 * @tc.desc: Test RSCanvasNode with alpha and scale (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_003)
{
    std::vector<float> alphaValues = { 0.3f, 0.6f, 1.0f };
    std::vector<float> scaleList = { 0.5f, 1.0f, 1.5f };

    for (size_t row = 0; row < alphaValues.size(); row++) {
        for (size_t col = 0; col < scaleList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaValues[row]);
            testNode->SetScale(scaleList[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_004
 * @tc.desc: Test RSCanvasNode alpha update scenario
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_004)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set initial alpha
    testNode->SetAlpha(0.3f);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update to different value
    testNode->SetAlpha(0.6f);

    // Update again
    testNode->SetAlpha(1.0f);
}

/*
 * @tc.name: SurfaceCompositeLayerTest_005
 * @tc.desc: Test RSCanvasNode with rotation
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_005)
{
    std::vector<float> alphaValues = { 0.3f, 1.0f };
    std::vector<float> rotationList = { 0, 45, 90 };

    for (size_t row = 0; row < alphaValues.size(); row++) {
        for (size_t col = 0; col < rotationList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaValues[row]);
            testNode->SetRotation(rotationList[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_006
 * @tc.desc: Test RSCanvasNode with scale
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_006)
{
    std::vector<std::pair<float, float>> scaleList = {
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f },
        { 2.0f, 2.0f }
    };

    for (size_t i = 0; i < scaleList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetScale(scaleList[i].first, scaleList[i].second);
        testNode->SetAlpha((i % 2 == 0) ? 0.5f : 1.0f);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_007
 * @tc.desc: Test RSCanvasNode with zero bounds
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_007)
{
    std::vector<Vector4f> zeroBounds = {
        { 50, 50, 0, 100 },
        { 150, 50, 100, 0 },
        { 300, 50, 0, 0 }
    };

    for (size_t i = 0; i < zeroBounds.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ zeroBounds[i].x_, zeroBounds[i].y_,
            zeroBounds[i].z_, zeroBounds[i].w_ });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetAlpha(0.5f + i * 0.2f);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_008
 * @tc.desc: Test RSCanvasNode with effects
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_008)
{
    std::vector<float> alphaValues = { 0.3f, 1.0f };
    std::vector<float> blurRadius = { 0, 10, 20 };

    for (size_t row = 0; row < alphaValues.size(); row++) {
        for (size_t col = 0; col < blurRadius.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaValues[row]);
            testNode->SetBackgroundBlurRadius(blurRadius[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_009
 * @tc.desc: Test RSCanvasNode with pixelmap
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_009)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<float> alphaValues = { 0.3f, 0.6f, 1.0f, 0.5f, 0.8f };

    for (size_t i = 0; i < alphaValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 230 + 50, 50, 200, 200 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetAlpha(alphaValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_010
 * @tc.desc: Test RSCanvasNode multiple nodes
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_010)
{
    for (int i = 0; i < 6; i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 190 + 50, 50, 180, 180 });
        testNode->SetBackgroundColor(0xffff0000 - i * 0x00200000);
        testNode->SetAlpha((i % 3 + 1) * 0.3f);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
