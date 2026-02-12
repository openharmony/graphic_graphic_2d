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

#include "common/rs_vector2.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "modifier_ng/appearance/rs_alpha_modifier.h"
#include "modifier_ng/geometry/rs_transform_modifier.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class ModifierRemoveModifierTest : public RSGraphicTest {
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
 * @tc.name: ModifierRemoveModifierTest_001
 * @tc.desc: Test RemoveModifier with valid property
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Add modifier
    auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    alphaModifier->SetAlpha(0.5f);
    testNode->AddModifier(alphaModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Remove modifier
    testNode->RemoveModifier(alphaModifier);
}

/*
 * @tc.name: ModifierRemoveModifierTest_002
 * @tc.desc: Test RemoveModifier with null property
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_002)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Detach null property
    std::shared_ptr<ModifierNG::RSAlphaModifier> nullModifier = nullptr;
    testNode->RemoveModifier(nullModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ModifierRemoveModifierTest_003
 * @tc.desc: Test RemoveModifier with multiple properties (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_003)
{
    std::vector<float> alphaValues = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < alphaValues.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);

            auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
            alphaModifier->SetAlpha(alphaValues[col]);
            testNode->AddModifier(alphaModifier);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);

            // Detach property
            testNode->RemoveModifier(alphaModifier);
        }
    }
}

/*
 * @tc.name: ModifierRemoveModifierTest_004
 * @tc.desc: Test RemoveModifier non-existent property
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_004)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Create modifier but don't attach
    auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    alphaModifier->SetAlpha(0.5f);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Try to detach non-existent property
    testNode->RemoveModifier(alphaModifier);
}

/*
 * @tc.name: ModifierRemoveModifierTest_005
 * @tc.desc: Test RemoveModifier with rotation modifier
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_005)
{
    std::vector<float> rotationValues = { 0, 45, 90, 135 };

    for (size_t i = 0; i < rotationValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);

        auto rotationModifier = std::make_shared<ModifierNG::RSTransformModifier>();
        rotationModifier->SetRotation(rotationValues[i]);
        testNode->AddModifier(rotationModifier);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);

        testNode->RemoveModifier(rotationModifier);
    }
}

/*
 * @tc.name: ModifierRemoveModifierTest_006
 * @tc.desc: Test RemoveModifier attach/detach cycle
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_006)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    alphaModifier->SetAlpha(0.5f);

    // Attach
    testNode->AddModifier(alphaModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Detach
    testNode->RemoveModifier(alphaModifier);

    // Attach again
    testNode->AddModifier(alphaModifier);

    // Detach again
    testNode->RemoveModifier(alphaModifier);
}

/*
 * @tc.name: ModifierRemoveModifierTest_007
 * @tc.desc: Test RemoveModifier with parent-child hierarchy
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_007)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 600, 600 });
    parent->SetBackgroundColor(0xffff0000);

    auto parentAlphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    parentAlphaModifier->SetAlpha(0.8f);
    parent->AddModifier(parentAlphaModifier);

    // Add children with properties
    std::vector<std::shared_ptr<ModifierNG::RSAlphaModifier>> childModifiers;
    for (int i = 0; i < 4; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + (i % 2) * 300, 50 + (i / 2) * 300, 200, 200 });
        child->SetBackgroundColor(0xff00ff00);

        auto childAlphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
        childAlphaModifier->SetAlpha(0.5f + i * 0.1f);
        child->AddModifier(childAlphaModifier);
        childModifiers.push_back(childAlphaModifier);

        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);

    // Remove all child modifiers
    for (auto& modifier : childModifiers) {
        for (auto& child : parent->GetChildren()) {
            auto node = std::static_pointer_cast<RSCanvasNode>(child.lock());
            if (node) {
                node->RemoveModifier(modifier);
            }
        }
    }
}

/*
 * @tc.name: ModifierRemoveModifierTest_008
 * @tc.desc: Test RemoveModifier with scale
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_008)
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

        auto scaleModifier = std::make_shared<ModifierNG::RSTransformModifier>();
        scaleModifier->SetScale({ scaleValues[i].first, scaleValues[i].second });
        testNode->AddModifier(scaleModifier);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);

        testNode->RemoveModifier(scaleModifier);
    }
}

/*
 * @tc.name: ModifierRemoveModifierTest_009
 * @tc.desc: Test RemoveModifier after multiple attach
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_009)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Attach same property multiple times
    auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    alphaModifier->SetAlpha(0.5f);
    testNode->AddModifier(alphaModifier);
    testNode->AddModifier(alphaModifier);
    testNode->AddModifier(alphaModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Detach once
    testNode->RemoveModifier(alphaModifier);
}

/*
 * @tc.name: ModifierRemoveModifierTest_010
 * @tc.desc: Test RemoveModifier with translation (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L8
 */
GRAPHIC_TEST(ModifierRemoveModifierTest, CONTENT_DISPLAY_TEST, ModifierRemoveModifierTest_010)
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

            auto translateModifier = std::make_shared<ModifierNG::RSTransformModifier>();
            translateModifier->SetTranslate({ translateValues[col].first, translateValues[col].second });
            testNode->AddModifier(translateModifier);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);

            testNode->RemoveModifier(translateModifier);
        }
    }
}

} // namespace OHOS::Rosen
