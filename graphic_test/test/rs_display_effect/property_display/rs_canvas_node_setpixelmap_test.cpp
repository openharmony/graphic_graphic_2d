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

class RSCanvasNodeSetPixelmapTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

// ============================================================================
// Part 1: Basic SetPixelmap Tests (基础测试)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_001
 * @tc.desc: Test SetPixelmap with valid PixelMap
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);
    ASSERT_NE(pixelMap, nullptr);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetPixelmap(pixelMap);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_002
 * @tc.desc: Test SetPixelmap with null PixelMap
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_002)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set null pixelmap
    testNode->SetPixelmap(nullptr);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_003
 * @tc.desc: Test SetPixelmap with different bounds sizes (4x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_003)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<Vector4f> boundsList = {
        { 50, 50, 100, 100 },
        { 200, 50, 150, 100 },
        { 400, 50, 200, 100 },
        { 50, 200, 100, 150 },
        { 200, 200, 200, 150 },
        { 450, 200, 250, 150 },
        { 50, 400, 150, 200 },
        { 250, 400, 200, 200 },
        { 500, 400, 250, 200 },
        { 50, 700, 300, 250 },
        { 400, 700, 350, 250 },
        { 800, 700, 300, 250 }
    };

    for (const auto& bounds : boundsList) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetPixelmap(pixelMap);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_004
 * @tc.desc: Test SetPixelmap with zero and negative bounds
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_004)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<Vector4f> zeroBounds = {
        { 50, 50, 0, 100 },      // width=0
        { 150, 50, 100, 0 },      // height=0
        { 300, 50, 0, 0 },        // both zero
        { 450, 50, -100, 100 },   // negative width
        { 50, 200, 100, -100 }   // negative height
    };

    for (const auto& bounds : zeroBounds) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetPixelmap(pixelMap);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_005
 * @tc.desc: Test SetPixelmap with out of screen bounds
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_005)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<Vector4f> outBounds = {
        { 1100, 1900, 200, 200 },   // bottom-right corner
        { 1000, 0, 300, 300 },       // right edge
        { 0, 1700, 300, 300 },       // bottom edge
        { 1100, 1100, 300, 300 }    // exceeding screen
    };

    for (const auto& bounds : outBounds) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetPixelmap(pixelMap);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// ============================================================================
// Part 2: SetPixelmap with Alpha (与透明度结合)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Alpha_001
 * @tc.desc: Test SetPixelmap with different alpha values (3x4 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Alpha_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<float> alphaList = { 0.0f, 0.3f, 0.5f, 0.7f, 1.0f };

    for (size_t i = 0; i < alphaList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetAlpha(alphaList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Alpha_002
 * @tc.desc: Test SetPixelmap with extreme alpha values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Alpha_002)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<float> extremeAlphas = { -1.0f, -0.5f, 0.0f, 0.001f, 0.5f, 1.0f, 1.5f, 2.0f };

    for (size_t i = 0; i < extremeAlphas.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 140 + 50, 50, 130, 130 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetAlpha(extremeAlphas[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Alpha_003
 * @tc.desc: Test SetPixelmap alpha with background layer (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Alpha_003)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    // Background layer
    auto bgNode = RSCanvasNode::Create();
    bgNode->SetBounds({ 50, 50, 1100, 1100 });
    bgNode->SetBackgroundColor(0xffffffff);
    GetRootNode()->AddChild(bgNode);

    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetPixelmap(pixelMap);
            testNode->SetAlpha(alphaList[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

// ============================================================================
// Part 3: SetPixelmap with Rotation (与旋转结合)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Rotation_001
 * @tc.desc: Test SetPixelmap with rotation (4x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Rotation_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<float> rotationList = { 0, 45, 90, 135, 180, 270 };
    std::vector<float> alphaList = { 0.5f, 1.0f };

    for (size_t row = 0; row < alphaList.size(); row++) {
        for (size_t col = 0; col < rotationList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 200 + 50, (int)row * 400 + 50, 180, 180 });
            testNode->SetPixelmap(pixelMap);
            testNode->SetRotation(rotationList[col]);
            testNode->SetAlpha(alphaList[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Rotation_002
 * @tc.desc: Test SetPixelmap with extreme rotation values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Rotation_002)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<float> extremeRotations = { -360, -270, -180, -90, -45, 0, 45, 90, 180, 270, 360 };

    for (size_t i = 0; i < extremeRotations.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 110 + 50, 50, 100, 100 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetRotation(extremeRotations[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// ============================================================================
// Part 4: SetPixelmap with Scale (与缩放结合)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Scale_001
 * @tc.desc: Test SetPixelmap with scale (3x4 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Scale_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<std::pair<float, float>> scaleList = {
        { 0.1f, 0.1f },
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f },
        { 2.0f, 2.0f },
        { 3.0f, 3.0f },
        { 0.5f, 1.5f },
        { 1.5f, 0.5f },
        { -1.0f, 1.0f },
        { 1.0f, -1.0f },
        { -1.0f, -1.0f },
        { -2.0f, -2.0f }
    };

    for (size_t i = 0; i < scaleList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 90 + 50, 50, 80, 80 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetScale(scaleList[i].first, scaleList[i].second);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Scale_002
 * @tc.desc: Test SetPixelmap with extreme scale values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Scale_002)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<std::pair<float, float>> extremeScales = {
        { 0.0f, 0.0f },      // zero scale
        { 0.01f, 0.01f },    // very small
        { 10.0f, 10.0f },    // very large
        { 100.0f, 100.0f }  // extremely large
    };

    for (size_t i = 0; i < extremeScales.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetScale(extremeScales[i].first, extremeScales[i].second);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// ============================================================================
// Part 5: SetPixelmap with Translation (与平移结合)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Translation_001
 * @tc.desc: Test SetPixelmap with translation (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Translation_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<std::pair<float, float>> translateList = {
        { 0, 0 },
        { 50, 0 },
        { 0, 50 },
        { 50, 50 },
        { 100, 100 },
        { -50, -50 },
        { -100, 0 },
        { 0, -100 },
        { -50, 50 }
    };

    for (size_t i = 0; i < translateList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 130 + 50, 50, 120, 120 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetTranslate(Vector2f(translateList[i].first, translateList[i].second));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// ============================================================================
// Part 6: SetPixelmap Update Scenarios (更新场景)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Update_001
 * @tc.desc: Test SetPixelmap update - same node multiple times
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Update_001)
{
    auto pixelMap1 = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                    Media::AllocatorType::SHARE_MEM_ALLOC);
    auto pixelMap2 = DecodePixelMap("/data/local/tmp/geom_test.jpg",
                                    Media::AllocatorType::SHARE_MEM_ALLOC);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetPixelmap(pixelMap1); // Initial pixelmap

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update to different pixelmap
    testNode->SetPixelmap(pixelMap2);

    // Update again to first pixelmap
    testNode->SetPixelmap(pixelMap1);
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Update_002
 * @tc.desc: Test SetPixelmap with null update sequence
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Update_002)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetPixelmap(pixelMap);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Update to null
    testNode->SetPixelmap(nullptr);

    // Update back to pixelmap
    testNode->SetPixelmap(pixelMap);
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Update_003
 * @tc.desc: Test SetPixelmap update with property changes (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Update_003)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    for (int i = 0; i < 9; i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 50 + (i % 3) * 380, 50 + (i / 3) * 380, 300, 300 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetAlpha(0.5f + i * 0.05f);
        testNode->SetRotation(i * 30.0f);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// ============================================================================
// Part 7: SetPixelmap with Parent-Child Hierarchy (父子层级)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Hierarchy_001
 * @tc.desc: Test SetPixelmap in parent-child hierarchy (3 levels)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Hierarchy_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    // Root node
    auto root = RSCanvasNode::Create();
    root->SetBounds({ 50, 50, 1100, 1100 });
    root->SetBackgroundColor(0xffff0000);

    // Level 1 children
    for (int i = 0; i < 3; i++) {
        auto level1 = RSCanvasNode::Create();
        level1->SetBounds({ 50 + i * 350, 50, 330, 330 });
        level1->SetBackgroundColor(0xff00ff00);
        level1->SetPixelmap(pixelMap);
        level1->SetAlpha(0.7f);

        // Level 2 children
        for (int j = 0; j < 2; j++) {
            auto level2 = RSCanvasNode::Create();
            level2->SetBounds({ 50 + j * 160, 50, 150, 250 });
            level2->SetBackgroundColor(0xff0000ff);
            level2->SetPixelmap(pixelMap);
            level2->SetAlpha(0.8f);
            level1->AddChild(level2);
        }

        root->AddChild(level1);
    }

    GetRootNode()->AddChild(root);
    RegisterNode(root);
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Hierarchy_002
 * @tc.desc: Test SetPixelmap with overlapping children
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Hierarchy_002)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 600, 600 });
    parent->SetBackgroundColor(0xffff0000);

    // Create overlapping children with pixelmap
    for (int i = 0; i < 4; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 100 + i * 100, 100 + i * 50, 250, 250 });
        child->SetPixelmap(pixelMap);
        child->SetAlpha(0.7f - i * 0.1f);
        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
}

// ============================================================================
// Part 8: SetPixelmap with Multiple Properties (多属性组合)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_MultiProps_001
 * @tc.desc: Test SetPixelmap with rotation, scale, translation, alpha (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_MultiProps_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<float> rotations = { 0, 45, 90 };
    std::vector<std::pair<float, float>> scales = { { 0.8f, 0.8f }, { 1.0f, 1.0f }, { 1.2f, 1.2f } };

    for (size_t row = 0; row < rotations.size(); row++) {
        for (size_t col = 0; col < scales.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetPixelmap(pixelMap);
            testNode->SetRotation(rotations[row]);
            testNode->SetScale(scales[col].first, scales[col].second);
            testNode->SetTranslate(Vector2f(10.0f, 10.0f));
            testNode->SetAlpha(0.8f);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_MultiProps_002
 * @tc.desc: Test SetPixelmap with pivot and rotation (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_MultiProps_002)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<std::pair<float, float>> pivots = {
        { 0.0f, 0.0f },
        { 0.5f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 0.5f },
        { 0.5f, 0.5f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.3f, 0.7f },
        { 0.7f, 0.3f }
    };

    for (size_t i = 0; i < pivots.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 130 + 50, 50, 120, 120 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetPivot(pivots[i].first, pivots[i].second);
        testNode->SetRotation(45.0f);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// ============================================================================
// Part 9: SetPixelmap with Effects (特效结合)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Effects_001
 * @tc.desc: Test SetPixelmap with background blur (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Effects_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<float> blurRadius = { 0, 10, 20 };
    std::vector<float> alphaList = { 0.5f, 0.8f, 1.0f };

    for (size_t row = 0; row < blurRadius.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetPixelmap(pixelMap);
            testNode->SetBackgroundBlurRadius(blurRadius[row]);
            testNode->SetAlpha(alphaList[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Effects_002
 * @tc.desc: Test SetPixelmap with shadow radius (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Effects_002)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<float> shadowRadius = { 0, 10, 20 };
    std::vector<float> offsetX = { 0, 5, 10 };

    for (size_t row = 0; row < shadowRadius.size(); row++) {
        for (size_t col = 0; col < offsetX.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetPixelmap(pixelMap);
            testNode->SetShadowRadius(shadowRadius[row]);
            testNode->SetShadowOffsetX(offsetX[col]);
            testNode->SetShadowOffsetY(5.0f);
            testNode->SetShadowColor(0x80000000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Effects_003
 * @tc.desc: Test SetPixelmap with corner radius (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Effects_003)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<float> radiusValues = { 0, 10, 20, 30, 40, 50, 60, 80, 100 };

    for (size_t i = 0; i < radiusValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 130 + 50, 50, 120, 120 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetCornerRadius(radiusValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// ============================================================================
// Part 10: SetPixelmap Special Scenarios (特殊场景)
// ============================================================================

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Special_001
 * @tc.desc: Test SetPixelmap with very large bounds
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Special_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::vector<Vector4f> largeBounds = {
        { 0, 0, 1200, 2000 },      // Full screen
        { 0, 0, 2400, 4000 },      // 2x screen
        { -1000, -1000, 2200, 2200 }, // Negative position, large size
        { 500, 500, 3000, 3000 }     // Very large size
    };

    for (const auto& bounds : largeBounds) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ bounds.x_, bounds.y_, bounds.z_, bounds.w_ });
        testNode->SetPixelmap(pixelMap);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Special_002
 * @tc.desc: Test SetPixelmap with rapid updates
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Special_002)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });

    // Rapid updates
    for (int i = 0; i < 5; i++) {
        testNode->SetPixelmap(pixelMap);
        testNode->SetAlpha(i * 0.2f);
    }

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Special_003
 * @tc.desc: Test SetPixelmap on SurfaceNode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Special_003)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    auto surfaceNode = RSCanvasNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetPixelmap(pixelMap);
    surfaceNode->SetAlpha(0.8f);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSCanvasNodeSetPixelmapTest_Special_004
 * @tc.desc: Test SetPixelmap with frame position changes (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSCanvasNodeSetPixelmapTest, CONTENT_DISPLAY_TEST, RSCanvasNodeSetPixelmapTest_Special_004)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    std::pair<float, float> framePositions[] = {
        { 0, 0 },
        { 100, 0 },
        { 0, 100 },
        { 200, 200 },
        { -50, -50 },
        { 300, 100 },
        { 100, 300 },
        { -100, 200 },
        { 400, -100 }
    };

    for (size_t i = 0; i < 9; i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ 50, 50, 200, 200 });
        testNode->SetPixelmap(pixelMap);
        testNode->SetFrame({ framePositions[i].first, framePositions[i].second, 200, 200 });
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen
