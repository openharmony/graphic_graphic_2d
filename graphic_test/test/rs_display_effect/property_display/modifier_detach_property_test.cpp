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

class ModifierDetachPropertyTest : public RSGraphicTest {
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
 * @tc.name: ModifierDetachPropertyTest_001
 * @tc.desc: Test DetachProperty with valid property
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Attach property
    auto alphaModifier = std::make_shared<RSAlphaModifier>(0.5f);
    testNode->AttachProperty(alphaModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Detach property
    testNode->DetachProperty(alphaModifier);
}

/*
 * @tc.name: ModifierDetachPropertyTest_002
 * @tc.desc: Test DetachProperty with null property
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_002)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Detach null property
    std::shared_ptr<RSAlphaModifier> nullModifier = nullptr;
    testNode->DetachProperty(nullModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ModifierDetachPropertyTest_003
 * @tc.desc: Test DetachProperty with multiple properties (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_003)
{
    std::vector<float> alphaValues = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < alphaValues.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);

            auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaValues[col]);
            testNode->AttachProperty(alphaModifier);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);

            // Detach property
            testNode->DetachProperty(alphaModifier);
        }
    }
}

/*
 * @tc.name: ModifierDetachPropertyTest_004
 * @tc.desc: Test DetachProperty non-existent property
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_004)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Create modifier but don't attach
    auto alphaModifier = std::make_shared<RSAlphaModifier>(0.5f);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Try to detach non-existent property
    testNode->DetachProperty(alphaModifier);
}

/*
 * @tc.name: ModifierDetachPropertyTest_005
 * @tc.desc: Test DetachProperty with rotation modifier
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_005)
{
    std::vector<float> rotationValues = { 0, 45, 90, 135 };

    for (size_t i = 0; i < rotationValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);

        auto rotationModifier = std::make_shared<RSRotationModifier>(rotationValues[i]);
        testNode->AttachProperty(rotationModifier);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);

        testNode->DetachProperty(rotationModifier);
    }
}

/*
 * @tc.name: ModifierDetachPropertyTest_006
 * @tc.desc: Test DetachProperty attach/detach cycle
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_006)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    auto alphaModifier = std::make_shared<RSAlphaModifier>(0.5f);

    // Attach
    testNode->AttachProperty(alphaModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Detach
    testNode->DetachProperty(alphaModifier);

    // Attach again
    testNode->AttachProperty(alphaModifier);

    // Detach again
    testNode->DetachProperty(alphaModifier);
}

/*
 * @tc.name: ModifierDetachPropertyTest_007
 * @tc.desc: Test DetachProperty with parent-child hierarchy
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_007)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 600, 600 });
    parent->SetBackgroundColor(0xffff0000);

    auto parentAlphaModifier = std::make_shared<RSAlphaModifier>(0.8f);
    parent->AttachProperty(parentAlphaModifier);

    // Add children with properties
    std::vector<std::shared_ptr<RSAlphaModifier>> childModifiers;
    for (int i = 0; i < 4; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + (i % 2) * 300, 50 + (i / 2) * 300, 200, 200 });
        child->SetBackgroundColor(0xff00ff00);

        auto childAlphaModifier = std::make_shared<RSAlphaModifier>(0.5f + i * 0.1f);
        child->AttachProperty(childAlphaModifier);
        childModifiers.push_back(childAlphaModifier);

        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);

    // Detach all child properties
    for (auto& modifier : childModifiers) {
        for (auto& child : parent->GetChildren()) {
            auto node = std::static_pointer_cast<RSCanvasNode>(child);
            node->DetachProperty(modifier);
        }
    }
}

/*
 * @tc.name: ModifierDetachPropertyTest_008
 * @tc.desc: Test DetachProperty with scale
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_008)
{
    std::vector<std::pair<float, float>> scaleValues = {
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f }
    };

    for (size_t i = 0; i < scaleValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 300, 300 });
        testNode->SetBackgroundColor(0xffff0000);

        auto scaleModifier = std::make_shared<RSScaleModifier>(
            scaleValues[i].first, scaleValues[i].second);
        testNode->AttachProperty(scaleModifier);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);

        testNode->DetachProperty(scaleModifier);
    }
}

/*
 * @tc.name: ModifierDetachPropertyTest_009
 * @tc.desc: Test DetachProperty after multiple attach
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_009)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Attach same property multiple times
    auto alphaModifier = std::make_shared<RSAlphaModifier>(0.5f);
    testNode->AttachProperty(alphaModifier);
    testNode->AttachProperty(alphaModifier);
    testNode->AttachProperty(alphaModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Detach once
    testNode->DetachProperty(alphaModifier);
}

/*
 * @tc.name: ModifierDetachPropertyTest_010
 * @tc.desc: Test DetachProperty with translation (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierDetachPropertyTest, CONTENT_DISPLAY_TEST, ModifierDetachPropertyTest_010)
{
    std::vector<std::pair<float, float>> translateValues = {
        { 0, 0 },
        { 50, 0 },
        { 0, 50 }
    };

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < translateValues.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);

            auto translateModifier = std::make_shared<RSTranslationModifier>(
                translateValues[col].first, translateValues[col].second);
            testNode->AttachProperty(translateModifier);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);

            testNode->DetachProperty(translateModifier);
        }
    }
}

} // namespace OHOS::Rosen
