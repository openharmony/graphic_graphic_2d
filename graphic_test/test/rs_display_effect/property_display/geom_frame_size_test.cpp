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

/* SetFrameSize: normal values - matrix 3x3 (width x height) */
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
            testNode->SetFrameSize({widths[col], heights[row]});
            testNode->SetBackgroundColor(0xffff0000);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFrameSize: boundary values - zero size */
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
        testNode->SetFrameSize(zeroSizes[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrameSize: boundary values - maximum/minimum float */
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
        testNode->SetFrameSize(extremeSizes[i]);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrameSize: abnormal values - negative dimensions */
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
        testNode->SetFrameSize(negativeSizes[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrameSize: abnormal values - infinity and NaN */
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
        testNode->SetFrameSize(invalidSizes[i]);
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrameSize: aspect ratio variations - matrix 4x3 */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Aspect_Ratio_Matrix_4x3)
{
    std::vector<float> widths = {100, 200, 300, 400};
    std::vector<float> heights = {100, 200, 300};

    for (size_t row = 0; row < heights.size(); row++) {
        for (size_t col = 0; col < widths.size(); col++) {
            auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
                {static_cast<float>(col * 300 + 50),
                 static_cast<float>(row * 300 + 50), 200, 200});
            testNode->SetFrameSize({widths[col], heights[row]});
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFrameWidth: normal values - 4x1 matrix */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameWidth_Normal_Matrix_4x1)
{
    std::vector<float> widths = {0, 50, 100, 200};

    for (size_t i = 0; i < widths.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = 50;
        float y = static_cast<float>(i * 300 + 50);
        testNode->SetBounds({x, y, 300, 200});
        testNode->SetFrameWidth(widths[i]);
        testNode->SetBackgroundColor(0xff00ff00);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrameWidth: boundary and extreme values */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameWidth_Boundary_Extreme)
{
    std::vector<float> extremeWidths = {
        0,                                    // zero
        1,                                     // minimum positive
        1000,                            // large
        std::numeric_limits<float>::max(),        // max float
        std::numeric_limits<float>::min(),        // min positive
        std::numeric_limits<float>::epsilon(),    // epsilon
        -100,                            // negative
        -std::numeric_limits<float>::max(),       // max negative
        std::numeric_limits<float>::infinity(),   // positive infinity
        -std::numeric_limits<float>::infinity(),  // negative infinity
        std::numeric_limits<float>::quiet_NaN()   // NaN
    };

    for (size_t i = 0; i < extremeWidths.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 2) * 500 + 50);
        float y = static_cast<float>((i / 2) * 200 + 50);
        testNode->SetBounds({x, y, 300, 100});
        testNode->SetFrameWidth(extremeWidths[i]);
        testNode->SetBackgroundColor(0xffff0000);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrameHeight: normal values - 1x4 matrix */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameHeight_Normal_Matrix_1x4)
{
    std::vector<float> heights = {0, 50, 100, 200};

    for (size_t i = 0; i < heights.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>(i * 300 + 50);
        float y = 50;
        testNode->SetBounds({x, y, 200, 300});
        testNode->SetFrameHeight(heights[i]);
        testNode->SetBackgroundColor(0xff0000ff);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrameHeight: boundary and extreme values */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameHeight_Boundary_Extreme)
{
    std::vector<float> extremeHeights = {
        0,
        1,
        1000,
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::min(),
        std::numeric_limits<float>::epsilon(),
        -100,
        -std::numeric_limits<float>::max(),
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::quiet_NaN()
    };

    for (size_t i = 0; i < extremeHeights.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        float x = static_cast<float>((i % 2) * 200 + 50);
        float y = static_cast<float>((i / 2) * 500 + 50);
        testNode->SetBounds({x, y, 100, 300});
        testNode->SetFrameHeight(extremeHeights[i]);
        testNode->SetBackgroundColor(0xff00ff00);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* SetFrameSize: rapid sequential updates */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Rapid_Updates)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 400, 400});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    testNode->SetFrameSize({100, 100});
    testNode->SetFrameSize({200, 200});
    testNode->SetFrameSize({300, 300});
    testNode->SetFrameSize({100, 200});
    testNode->SetFrameSize({200, 100});
    testNode->SetFrameSize({0, 0});
    testNode->SetFrameSize({500, 500});
}

/* SetFrameWidth/Height: combined test matrix */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameWidthHeight_Combined_Matrix_3x3)
{
    std::vector<float> widths = {0, 100, 200};
    std::vector<float> heights = {0, 100, 200};

    for (size_t row = 0; row < heights.size(); row++) {
        for (size_t col = 0; col < widths.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            float x = static_cast<float>(col * 300 + 50);
            float y = static_cast<float>(row * 300 + 50);
            testNode->SetBounds({x, y, 200, 200});
            testNode->SetFrameWidth(widths[col]);
            testNode->SetFrameHeight(heights[row]);
            testNode->SetBackgroundColor(0xffffff00);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* SetFrameSize: size vs bounds relationship */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Bounds_Relationship)
{
    // Frame smaller than bounds
    auto testNode1 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {50, 50, 300, 300});
    testNode1->SetFrameSize({100, 100});
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Frame equal to bounds
    auto testNode2 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {400, 50, 300, 300});
    testNode2->SetFrameSize({300, 300});
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);

    // Frame larger than bounds
    auto testNode3 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {50, 400, 300, 300});
    testNode3->SetFrameSize({500, 500});
    GetRootNode()->AddChild(testNode3);
    RegisterNode(testNode3);

    // Frame extends beyond bounds
    auto testNode4 = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {400, 400, 300, 300});
    testNode4->SetFrameSize({1000, 1000});
    GetRootNode()->AddChild(testNode4);
    RegisterNode(testNode4);
}

/* SetFrameSize/Width/Height: interaction test */
GRAPHIC_TEST(FrameSizeTest, CONTENT_DISPLAY_TEST, FrameSize_Interaction)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/geom_test.jpg",
        {100, 100, 400, 400});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // SetFrameSize followed by SetFrameWidth
    testNode->SetFrameSize({200, 200});
    testNode->SetFrameWidth(300);

    // SetFrameHeight followed by SetFrameSize
    testNode->SetFrameHeight(100);
    testNode->SetFrameSize({150, 150});

    // SetFrameWidth followed by SetFrameHeight
    testNode->SetFrameWidth(250);
    testNode->SetFrameHeight(250);
}

} // namespace OHOS::Rosen
