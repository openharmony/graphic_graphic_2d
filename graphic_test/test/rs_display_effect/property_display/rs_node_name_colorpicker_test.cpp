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

class RSNodeNameColorPickerTest : public RSGraphicTest {
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
 * @tc.name: RSNodeSetName_001
 * @tc.desc: Test SetNodeName with normal string
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetNodeName("TestNode");

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetName_002
 * @tc.desc: Test SetNodeName with empty string
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_002)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetNodeName("");

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetName_003
 * @tc.desc: Test SetNodeName with very long string
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_003)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetNodeName("ThisIsAVeryLongNodeNameForTestingPurposesWithLotsOfCharacters");

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetName_004
 * @tc.desc: Test SetNodeName with special characters
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_004)
{
    std::vector<std::string> names = {
        "Node_123",
        "Node-Test",
        "Node.Test",
        "Node@2026",
        "Node$Test",
        "Node&Test"
    };

    for (size_t i = 0; i < names.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 180 + 50, 50, 160, 200 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetNodeName(names[i]);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeSetName_005
 * @tc.desc: Test SetNodeName with Unicode characters
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_005)
{
    std::vector<std::string> names = {
        "节点A",
        "ノードB",
        "노드C",
        "Node测试"
    };

    for (size_t i = 0; i < names.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 200 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetNodeName(names[i]);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeSetName_006
 * @tc.desc: Test SetNodeName with numeric strings
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_006)
{
    std::vector<std::string> names = {
        "123",
        "0",
        "999999",
        "3.14",
        "0xFF"
    };

    for (size_t i = 0; i < names.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetNodeName(names[i]);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeSetName_007
 * @tc.desc: Test SetNodeName with spaces and tabs
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_007)
{
    std::vector<std::string> names = {
        "Node Name",
        " NodeName",
        "NodeName ",
        "Node  Name",
        "\tNodeName"
    };

    for (size_t i = 0; i < names.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetNodeName(names[i]);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeSetName_008
 * @tc.desc: Test SetNodeName with multiple updates
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_008)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set name multiple times
    testNode->SetNodeName("InitialName");
    testNode->SetNodeName("UpdatedName");
    testNode->SetNodeName("FinalName");

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetName_009
 * @tc.desc: Test SetNodeName with hierarchy
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_009)
{
    auto rootNode = RSCanvasNode::Create();
    rootNode->SetBounds({ 50, 50, 1000, 800 });
    rootNode->SetNodeName("RootNode");
    rootNode->SetBackgroundColor(0xffcccccc);

    auto childNode1 = RSCanvasNode::Create();
    childNode1->SetBounds({ 50, 50, 400, 300 });
    childNode1->SetNodeName("ChildNode1");
    childNode1->SetBackgroundColor(0xffff0000);

    auto childNode2 = RSCanvasNode::Create();
    childNode2->SetBounds({ 500, 50, 400, 300 });
    childNode2->SetNodeName("ChildNode2");
    childNode2->SetBackgroundColor(0xff00ff00);

    auto grandChildNode = RSCanvasNode::Create();
    grandChildNode->SetBounds({ 50, 350, 300, 200 });
    grandChildNode->SetNodeName("GrandChildNode");
    grandChildNode->SetBackgroundColor(0xff0000ff);

    rootNode->AddChild(childNode1);
    rootNode->AddChild(childNode2);
    childNode1->AddChild(grandChildNode);

    GetRootNode()->AddChild(rootNode);
    RegisterNode(rootNode);
}

/*
 * @tc.name: RSNodeSetName_010
 * @tc.desc: Test SetNodeName matrix with different naming patterns
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_010)
{
    std::vector<std::string> prefixes = { "Btn", "Lbl", "Img" };
    std::vector<std::string> suffixes = { "_Main", "_Sub", "_Aux" };

    for (size_t row = 0; row < prefixes.size(); row++) {
        for (size_t col = 0; col < suffixes.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetNodeName(prefixes[row] + std::to_string(col) + suffixes[col]);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_001
 * @tc.desc: Test SetColorPickerCallback with valid callback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetColorPickerCallback(OnColorPicked);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_002
 * @tc.desc: Test SetColorPickerCallback with null callback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_002)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetColorPickerCallback(nullptr);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_003
 * @tc.desc: Test SetColorPickerCallback with lambda
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_003)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    testNode->SetColorPickerCallback([](uint32_t color) {
        // Lambda callback
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_004
 * @tc.desc: Test SetColorPickerCallback multiple times
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_004)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set callback multiple times
    testNode->SetColorPickerCallback(OnColorPicked);
    testNode->SetColorPickerCallback([](uint32_t color) {
        // New callback
    });
    testNode->SetColorPickerCallback(nullptr);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_005
 * @tc.desc: Test SetColorPickerCallback with SetNodeName
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_005)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetNodeName("ColorPickerNode");
    testNode->SetColorPickerCallback(OnColorPicked);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_006
 * @tc.desc: Test SetColorPickerCallback matrix with different alpha values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_006)
{
    std::vector<float> alphas = { 0.2f, 0.5f, 1.0f };
    std::vector<ColorPickerCallback> callbacks = {
        OnColorPicked,
        [](uint32_t color) {},
        nullptr
    };

    for (size_t row = 0; row < alphas.size(); row++) {
        for (size_t col = 0; col < callbacks.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphas[row]);
            testNode->SetColorPickerCallback(callbacks[col]);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_007
 * @tc.desc: Test SetColorPickerCallback with different background colors
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_007)
{
    std::vector<uint32_t> colors = {
        0xffff0000, // Red
        0xff00ff00, // Green
        0xff0000ff, // Blue
        0xffffff00, // Yellow
        0xffff00ff  // Magenta
    };

    for (size_t i = 0; i < colors.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetBackgroundColor(colors[i]);
        testNode->SetNodeName("ColorNode_" + std::to_string(i));
        testNode->SetColorPickerCallback(OnColorPicked);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_008
 * @tc.desc: Test SetColorPickerCallback callback cycle (set-null-set)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_008)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Set callback
    testNode->SetColorPickerCallback(OnColorPicked);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Set to null
    testNode->SetColorPickerCallback(nullptr);

    // Set again
    testNode->SetColorPickerCallback(OnColorPicked);
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_009
 * @tc.desc: Test SetColorPickerCallback with named nodes in hierarchy
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_009)
{
    auto parentNode = RSCanvasNode::Create();
    parentNode->SetBounds({ 50, 50, 1000, 800 });
    parentNode->SetNodeName("ParentNode");
    parentNode->SetBackgroundColor(0xffcccccc);

    auto childNode1 = RSCanvasNode::Create();
    childNode1->SetBounds({ 50, 50, 400, 300 });
    childNode1->SetNodeName("ChildWithColorPicker");
    childNode1->SetBackgroundColor(0xffff0000);
    childNode1->SetColorPickerCallback(OnColorPicked);

    auto childNode2 = RSCanvasNode::Create();
    childNode2->SetBounds({ 500, 50, 400, 300 });
    childNode2->SetNodeName("ChildWithoutColorPicker");
    childNode2->SetBackgroundColor(0xff00ff00);

    parentNode->AddChild(childNode1);
    parentNode->AddChild(childNode2);

    GetRootNode()->AddChild(parentNode);
    RegisterNode(parentNode);
}

/*
 * @tc.name: RSNodeSetColorPickerCallback_010
 * @tc.desc: Test SetColorPickerCallback with different callback types
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetColorPickerCallback_010)
{
    std::vector<ColorPickerCallback> callbacks = {
        OnColorPicked,
        [](uint32_t color) {
            // Lambda 1
        },
        [](uint32_t color) {
            // Lambda 2
        },
        nullptr
    };

    for (size_t i = 0; i < callbacks.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 200 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetNodeName("CallbackTest_" + std::to_string(i));
        testNode->SetColorPickerCallback(callbacks[i]);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeSetName_011
 * @tc.desc: Test SetNodeName with RSNodeSetName_011
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_011)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetNodeName("TestNode_011");
    testNode->SetColorPickerCallback(OnColorPicked);
    testNode->SetNodeName("UpdatedNodeName_011");

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeSetName_012
 * @tc.desc: Test SetNodeName with very long string after callback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeNameColorPickerTest, CONTENT_DISPLAY_TEST, RSNodeSetName_012)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetColorPickerCallback(OnColorPicked);

    std::string longName(200, 'A');
    testNode->SetNodeName(longName);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

} // namespace OHOS::Rosen
