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
 * @tc.desc: Test SetCompositeType with different types
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_001)
{
    // Test different composite types
    std::vector<int> compositeTypes = { 0, 1, 2, 3 };

    for (size_t i = 0; i < compositeTypes.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        surfaceNode->SetBackgroundColor(0xffff0000);

        // Set composite type
        surfaceNode->SetCompositeType(compositeTypes[i]);

        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_002
 * @tc.desc: Test SetCompositeType with boundary values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_002)
{
    std::vector<int> boundaryTypes = {
        0,           // minimum
        1,           // normal
        10,          // mid-range
        100,         // high
        INT_MAX,     // max value
        -1,          // negative (edge case)
        -100         // large negative
    };

    for (size_t i = 0; i < boundaryTypes.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ (int)i * 160 + 50, 50, 150, 150 });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetCompositeType(boundaryTypes[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_003
 * @tc.desc: Test SetCompositeType with alpha (3x4 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_003)
{
    std::vector<int> compositeTypes = { 0, 1, 2 };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < compositeTypes.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto surfaceNode = RSSurfaceNode::Create();
            surfaceNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            surfaceNode->SetBackgroundColor(0xffff0000);
            surfaceNode->SetCompositeType(compositeTypes[row]);
            surfaceNode->SetAlpha(alphaList[col]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);
        }
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_004
 * @tc.desc: Test SetCompositeType update scenario
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_004)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Set initial composite type
    surfaceNode->SetCompositeType(0);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);

    // Update to different type
    surfaceNode->SetCompositeType(1);

    // Update again
    surfaceNode->SetCompositeType(2);
}

/*
 * @tc.name: SurfaceCompositeLayerTest_005
 * @tc.desc: Test SetCompositeType with rotation
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_005)
{
    std::vector<int> compositeTypes = { 0, 1, 2 };
    std::vector<float> rotationList = { 0, 45, 90 };

    for (size_t row = 0; row < compositeTypes.size(); row++) {
        for (size_t col = 0; col < rotationList.size(); col++) {
            auto surfaceNode = RSSurfaceNode::Create();
            surfaceNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            surfaceNode->SetBackgroundColor(0xffff0000);
            surfaceNode->SetCompositeType(compositeTypes[row]);
            surfaceNode->SetRotation(rotationList[col]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);
        }
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_006
 * @tc.desc: Test SetCompositeType with scale
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
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetScale(scaleList[i].first, scaleList[i].second);
        surfaceNode->SetCompositeType(i % 3);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_007
 * @tc.desc: Test SetCompositeType with zero bounds
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
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ zeroBounds[i].x_, zeroBounds[i].y_,
            zeroBounds[i].z_, zeroBounds[i].w_ });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetCompositeType(i);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_008
 * @tc.desc: Test SetCompositeType with effects
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_008)
{
    std::vector<int> compositeTypes = { 0, 1, 2 };
    std::vector<float> blurRadius = { 0, 10, 20 };

    for (size_t row = 0; row < compositeTypes.size(); row++) {
        for (size_t col = 0; col < blurRadius.size(); col++) {
            auto surfaceNode = RSSurfaceNode::Create();
            surfaceNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            surfaceNode->SetBackgroundColor(0xffff0000);
            surfaceNode->SetCompositeType(compositeTypes[row]);
            surfaceNode->SetBackgroundFilterRadius(blurRadius[col]);
            GetRootNode()->AddChild(surfaceNode);
            RegisterNode(surfaceNode);
        }
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_009
 * @tc.desc: Test SetCompositeType with pixelmap
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_009)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<int> compositeTypes = { 0, 1, 2, 0, 1 };

    for (size_t i = 0; i < compositeTypes.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ (int)i * 230 + 50, 50, 200, 200 });
        surfaceNode->SetPixelMap(pixelMap);
        surfaceNode->SetCompositeType(compositeTypes[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: SurfaceCompositeLayerTest_010
 * @tc.desc: Test SetCompositeType multiple surface nodes
 * @tc.type: FUNC
 * @tc.require: issueI7N7M0
 */
GRAPHIC_TEST(SurfaceCompositeLayerTest, CONTENT_DISPLAY_TEST, SurfaceCompositeLayerTest_010)
{
    for (int i = 0; i < 6; i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ (int)i * 190 + 50, 50, 180, 180 });
        surfaceNode->SetBackgroundColor(0xffff0000 - i * 0x00200000);
        surfaceNode->SetCompositeType(i % 3);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

} // namespace OHOS::Rosen
