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

class ColorPickerModifierTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

// Callback function for color picker
static void OnColorPicked(uint32_t color)
{
    // Color picked callback handler
}

/*
 * @tc.name: ColorPickerModifierTest_001
 * @tc.desc: Test SetColorPickerParams with normal values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set color picker parameters
    testNode->SetColorPickerParams(1, 0.5f, 0.1f);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorPickerModifierTest_002
 * @tc.desc: Test SetColorPickerParams with boundary values
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_002)
{
    std::vector<std::tuple<int, float, float>> paramsList = {
        { 0, 0.0f, 0.0f },         // minimum values
        { 1, 0.001f, 0.001f },     // near minimum
        { 100, 0.5f, 0.5f },       // normal values
        { 1000, 1.0f, 1.0f },      // maximum values
        { -1, -0.1f, -0.1f }       // negative values (edge case)
    };

    for (size_t i = 0; i < paramsList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetBackgroundColor(0xffff0000);

        auto [strategy, interval, threshold] = paramsList[i];
        testNode->SetColorPickerParams(strategy, interval, threshold);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ColorPickerModifierTest_003
 * @tc.desc: Test SetColorPickerCallback with valid callback (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_003)
{
    std::vector<int> strategies = { 0, 1, 2 };

    for (size_t row = 0; row < strategies.size(); row++) {
        for (size_t col = 0; col < 3; col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);

            testNode->SetColorPickerParams(strategies[row], 0.5f, 0.1f);

            // Register callback
            testNode->RegisterColorPickerCallback(OnColorPicked);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: ColorPickerModifierTest_004
 * @tc.desc: Test RegisterColorPickerCallback with null callback
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_004)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    testNode->SetColorPickerParams(1, 0.5f, 0.1f);

    // Register null callback
    testNode->RegisterColorPickerCallback(nullptr);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorPickerModifierTest_005
 * @tc.desc: Test UnregisterColorPickerCallback
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_005)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    testNode->SetColorPickerParams(1, 0.5f, 0.1f);

    // Register callback
    testNode->RegisterColorPickerCallback(OnColorPicked);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Unregister callback
    testNode->UnregisterColorPickerCallback();
}

/*
 * @tc.name: ColorPickerModifierTest_006
 * @tc.desc: Test HasColorPickerCallback
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_006)
{
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 50, 50, 400, 400 });
    testNode1->SetBackgroundColor(0xffff0000);
    testNode1->SetColorPickerParams(1, 0.5f, 0.1f);

    // Node with callback
    testNode1->RegisterColorPickerCallback(OnColorPicked);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Node without callback
    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({ 500, 50, 400, 400 });
    testNode2->SetBackgroundColor(0xff00ff00);
    testNode2->SetColorPickerParams(1, 0.5f, 0.1f);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);
}

/*
 * @tc.name: ColorPickerModifierTest_007
 * @tc.desc: Test SetColorPickerParams with different intervals (3x4 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_007)
{
    std::vector<float> intervals = { 0.0f, 0.1f, 0.5f, 1.0f };

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < intervals.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 280 + 50, (int)row * 350 + 50, 250, 300 });
            testNode->SetBackgroundColor(0xffff0000);

            testNode->SetColorPickerParams(row, intervals[col], 0.1f);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: ColorPickerModifierTest_008
 * @tc.desc: Test SetColorPickerParams with different thresholds
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_008)
{
    std::vector<float> thresholds = { 0.0f, 0.1f, 0.2f, 0.5f, 1.0f };

    for (size_t i = 0; i < thresholds.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetBackgroundColor(0xffff0000);

        testNode->SetColorPickerParams(1, 0.5f, thresholds[i]);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ColorPickerModifierTest_009
 * @tc.desc: Test Register/Unregister/Register callback cycle
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_009)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    testNode->SetColorPickerParams(1, 0.5f, 0.1f);

    // Register
    testNode->RegisterColorPickerCallback(OnColorPicked);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Unregister
    testNode->UnregisterColorPickerCallback();

    // Register again
    testNode->RegisterColorPickerCallback(OnColorPicked);

    // Unregister again
    testNode->UnregisterColorPickerCallback();
}

/*
 * @tc.name: ColorPickerModifierTest_010
 * @tc.desc: Test SetColorPickerParams with alpha (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7M1
 */
GRAPHIC_TEST(ColorPickerModifierTest, CONTENT_DISPLAY_TEST, ColorPickerModifierTest_010)
{
    std::vector<int> strategies = { 0, 1, 2 };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < strategies.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);

            testNode->SetColorPickerParams(strategies[row], 0.5f, 0.1f);
            testNode->RegisterColorPickerCallback(OnColorPicked);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

} // namespace OHOS::Rosen
