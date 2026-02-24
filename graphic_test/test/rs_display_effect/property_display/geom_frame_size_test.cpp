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

class FrameSizeTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/* SetFrame: normal values - matrix 3x3 (width x height) */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Normal_Matrix_3x3)
{
    std::vector<float> widths = {100, 200, 300};
    std::vector<float> heights = {100, 200, 300};

    for (size_t row = 0; row < heights.size(); row++) {
        for (size_t col = 0; col < widths.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            float x = static_cast<float>(col * 400 + 50);
            float y = static_cast<float>(row * 400 + 50);
            testNode->SetBounds({x, y, 400, 400});
            testNode->SetFrame(x, y, widths[col], heights[row]);
            testNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFrame: boundary values - zero size */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Boundary_Zero)
{
    std::vector<Vector2f> zeroSizes = {
        {0, 0},
        {0, 100},
        {100, 0}
    };

    for (size_t i = 0; i < zeroSizes.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {50, static_cast<float>(i * 300 + 50), 300, 300});
        testNode->SetFrame(50, static_cast<float>(i * 300 + 50), zeroSizes[i].x_, zeroSizes[i].y_);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: boundary values - maximum/minimum float */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Boundary_Extreme)
{
    std::vector<Vector2f> extremeSizes = {
        {1, 1},                    // minimum positive size
        {1000, 1000},  // large size
        {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()}, // max float
        {std::numeric_limits<float>::min(), std::numeric_limits<float>::min()}, // min positive float
        {std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::epsilon()} // epsilon
    };

    for (size_t i = 0; i < extremeSizes.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 2) * 600 + 50);
        float y = static_cast<float>((i / 2) * 600 + 50);
        testNode->SetBounds({x, y, 500, 500});
        testNode->SetFrame(x, y, extremeSizes[i].x_, extremeSizes[i].y_);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: abnormal values - negative dimensions */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Abnormal_Negative)
{
    std::vector<Vector2f> negativeSizes = {
        {-100, 100},
        {100, -100},
        {-200, -200},
        {-500, -500}
    };

    for (size_t i = 0; i < negativeSizes.size(); i++) {
        auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
            {50, static_cast<float>(i * 300 + 50), 300, 300});
        testNode->SetFrame(50, static_cast<float>(i * 300 + 50), negativeSizes[i].x_, negativeSizes[i].y_);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: abnormal values - infinity and NaN */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Abnormal_Invalid_Float)
{
    std::vector<Vector2f> invalidSizes = {
        {std::numeric_limits<float>::infinity(), 100},
        {100, std::numeric_limits<float>::infinity()},
        {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()},
        {-std::numeric_limits<float>::infinity(), 100},
        {std::numeric_limits<float>::quiet_NaN(), 100},
        {100, std::numeric_limits<float>::quiet_NaN()}
    };

    for (size_t i = 0; i < invalidSizes.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 2) * 600 + 50);
        float y = static_cast<float>((i / 2) * 600 + 50);
        testNode->SetBounds({x, y, 500, 500});
        testNode->SetFrame(x, y, invalidSizes[i].x_, invalidSizes[i].y_);
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrame: aspect ratio variations - matrix 4x3 */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Aspect_Ratio_Matrix_4x3)
{
    std::vector<float> widths = {100, 200, 300, 400};
    std::vector<float> heights = {100, 200, 300};

    for (size_t row = 0; row < heights.size(); row++) {
        for (size_t col = 0; col < widths.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 50),
                 static_cast<float>(row * 300 + 50), 200, 200});
            testNode->SetFrame(static_cast<float>(col * 300 + 50),
                           static_cast<float>(row * 300 + 50), widths[col], heights[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFrame: rapid sequential updates */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Rapid_Updates)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 400, 400});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    testNode->SetFrame(100, 100, 100, 100);
    testNode->SetFrame(100, 100, 200, 200);
    testNode->SetFrame(100, 100, 300, 300);
    testNode->SetFrame(100, 100, 100, 200);
    testNode->SetFrame(100, 100, 200, 100);
    testNode->SetFrame(100, 100, 0, 0);
    testNode->SetFrame(100, 100, 500, 500);
}

/* SetFrame: size vs bounds relationship */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Bounds_Relationship)
{
    // Frame smaller than bounds
    auto testNode1 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {50, 50, 300, 300});
    testNode1->SetFrame(50, 50, 100, 100);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Frame equal to bounds
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {400, 50, 300, 300});
    testNode2->SetFrame(400, 50, 300, 300);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);

    // Frame larger than bounds
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {50, 400, 300, 300});
    testNode3->SetFrame(50, 400, 500, 500);
    GetRootNode()->AddChild(testNode3);
    RegisterNode(testNode3);

    // Frame extends beyond bounds
    auto testNode4 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {400, 400, 300, 300});
    testNode4->SetFrame(400, 400, 1000, 1000);
    GetRootNode()->AddChild(testNode4);
    RegisterNode(testNode4);
}

} // namespace OHOS::Rosen
