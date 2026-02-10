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

class MaterialQualityLevelTest : public RSGraphicTest {
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
 * @tc.name: MaterialQualityLevelTest_001
 * @tc.desc: Test SetMaterialWithQualityLevel with normal values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set material with quality level
    testNode->SetMaterialWithQualityLevel(1);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: MaterialQualityLevelTest_002
 * @tc.desc: Test SetMaterialWithQualityLevel with boundary values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_002)
{
    std::vector<int> qualityLevels = {
        0,           // minimum
        1,           // low quality
        2,           // medium quality
        3,           // high quality
        10,          // very high
        100,         // extreme
        -1,          // negative (edge case)
        -100         // large negative
    };

    for (size_t i = 0; i < qualityLevels.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 140 + 50, 50, 130, 130 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetMaterialWithQualityLevel(qualityLevels[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: MaterialQualityLevelTest_003
 * @tc.desc: Test SetMaterialWithQualityLevel with alpha (3x4 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_003)
{
    std::vector<int> qualityLevels = { 1, 2, 3 };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < qualityLevels.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);
            testNode->SetMaterialWithQualityLevel(qualityLevels[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: MaterialQualityLevelTest_004
 * @tc.desc: Test SetMaterialWithQualityLevel update scenario
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_004)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set initial quality level
    testNode->SetMaterialWithQualityLevel(1);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update to different level
    testNode->SetMaterialWithQualityLevel(2);

    // Update again
    testNode->SetMaterialWithQualityLevel(3);
}

/*
 * @tc.name: MaterialQualityLevelTest_005
 * @tc.desc: Test SetMaterialWithQualityLevel with rotation (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_005)
{
    std::vector<int> qualityLevels = { 1, 2, 3 };
    std::vector<float> rotationList = { 0, 45, 90 };

    for (size_t row = 0; row < qualityLevels.size(); row++) {
        for (size_t col = 0; col < rotationList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetRotation(rotationList[col]);
            testNode->SetMaterialWithQualityLevel(qualityLevels[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: MaterialQualityLevelTest_006
 * @tc.desc: Test SetMaterialWithQualityLevel with scale
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_006)
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
        testNode->SetMaterialWithQualityLevel((i % 3) + 1);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: MaterialQualityLevelTest_007
 * @tc.desc: Test SetMaterialWithQualityLevel with zero bounds
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_007)
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
        testNode->SetMaterialWithQualityLevel(i + 1);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: MaterialQualityLevelTest_008
 * @tc.desc: Test SetMaterialWithQualityLevel with effects (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_008)
{
    std::vector<int> qualityLevels = { 1, 2, 3 };
    std::vector<float> blurRadius = { 0, 10, 20 };

    for (size_t row = 0; row < qualityLevels.size(); row++) {
        for (size_t col = 0; col < blurRadius.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetBackgroundFilterRadius(blurRadius[col]);
            testNode->SetMaterialWithQualityLevel(qualityLevels[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: MaterialQualityLevelTest_009
 * @tc.desc: Test SetMaterialWithQualityLevel with pixelmap
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_009)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<int> qualityLevels = { 1, 2, 3, 1, 2 };

    for (size_t i = 0; i < qualityLevels.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 230 + 50, 50, 200, 200 });
        testNode->SetPixelMap(pixelMap);
        testNode->SetMaterialWithQualityLevel(qualityLevels[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: MaterialQualityLevelTest_010
 * @tc.desc: Test SetMaterialWithQualityLevel with parent-child hierarchy
 * @tc.type: FUNC
 * @tc.require: issueI7N7M4
 */
GRAPHIC_TEST(MaterialQualityLevelTest, CONTENT_DISPLAY_TEST, MaterialQualityLevelTest_010)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 1100, 500 });
    parent->SetBackgroundColor(0xffff0000);
    parent->SetMaterialWithQualityLevel(1);

    // Add children with different quality levels
    for (int i = 0; i < 3; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + i * 350, 50, 300, 400 });
        child->SetBackgroundColor(0xff00ff00);
        child->SetMaterialWithQualityLevel(i + 1);
        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
}

} // namespace OHOS::Rosen
