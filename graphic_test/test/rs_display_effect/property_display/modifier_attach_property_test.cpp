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

class ModifierAttachPropertyTest : public RSGraphicTest {
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
 * @tc.name: ModifierAttachPropertyTest_001
 * @tc.desc: Test AttachProperty with valid property
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Create and attach property modifier
    auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    alphaModifier->SetAlpha(0.5f);
    testNode->AddModifier(alphaModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ModifierAttachPropertyTest_002
 * @tc.desc: Test AttachProperty with null property
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_002)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Attach null property
    std::shared_ptr<ModifierNG::RSAlphaModifier> nullModifier = nullptr;
    testNode->AddModifier(nullModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ModifierAttachPropertyTest_003
 * @tc.desc: Test AttachProperty with multiple properties (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_003)
{
    std::vector<float> alphaValues = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < alphaValues.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);

            // Attach alpha property
            auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
            alphaModifier->SetAlpha(alphaValues[col]);
            testNode->AddModifier(alphaModifier);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: ModifierAttachPropertyTest_004
 * @tc.desc: Test AttachProperty with rotation modifier
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_004)
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
    }
}

/*
 * @tc.name: ModifierAttachPropertyTest_005
 * @tc.desc: Test AttachProperty with scale modifier
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_005)
{
    std::vector<std::pair<float, float>> scaleValues = {
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f },
        { 2.0f, 2.0f }
    };

    for (size_t i = 0; i < scaleValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);

        auto scaleModifier = std::make_shared<ModifierNG::RSTransformModifier>();
        scaleModifier->SetScale({ scaleValues[i].first, scaleValues[i].second });
        testNode->AddModifier(scaleModifier);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ModifierAttachPropertyTest_006
 * @tc.desc: Test AttachProperty with translation modifier
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_006)
{
    std::vector<std::pair<float, float>> translateValues = {
        { 0, 0 },
        { 50, 0 },
        { 0, 50 },
        { 50, 50 }
    };

    for (size_t i = 0; i < translateValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);

        auto translateModifier = std::make_shared<ModifierNG::RSTransformModifier>();
        translateModifier->SetTranslate({ translateValues[i].first, translateValues[i].second });
        testNode->AddModifier(translateModifier);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ModifierAttachPropertyTest_007
 * @tc.desc: Test AttachProperty multiple properties on same node
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_007)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Attach multiple properties
    auto alphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    alphaModifier->SetAlpha(0.7f);
    auto rotationModifier = std::make_shared<ModifierNG::RSTransformModifier>();
    rotationModifier->SetRotation(45.0f);
    auto scaleModifier = std::make_shared<ModifierNG::RSTransformModifier>();
    scaleModifier->SetScale({ 1.2f, 1.2f });

    testNode->AddModifier(alphaModifier);
    testNode->AddModifier(rotationModifier);
    testNode->AddModifier(scaleModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ModifierAttachPropertyTest_008
 * @tc.desc: Test AttachProperty with parent-child hierarchy
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_008)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 600, 600 });
    parent->SetBackgroundColor(0xffff0000);

    // Attach property to parent
    auto parentAlphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    parentAlphaModifier->SetAlpha(0.8f);
    parent->AddModifier(parentAlphaModifier);

    // Add children with properties
    for (int i = 0; i < 4; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + (i % 2) * 300, 50 + (i / 2) * 300, 200, 200 });
        child->SetBackgroundColor(0xff00ff00);

        auto childAlphaModifier = std::make_shared<ModifierNG::RSAlphaModifier>();
        childAlphaModifier->SetAlpha(0.5f + i * 0.1f);
        child->AddModifier(childAlphaModifier);

        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
}

/*
 * @tc.name: ModifierAttachPropertyTest_009
 * @tc.desc: Test AttachProperty replace existing property
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_009)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Attach initial alpha property
    auto alphaModifier1 = std::make_shared<ModifierNG::RSAlphaModifier>();
    alphaModifier1->SetAlpha(0.5f);
    testNode->AddModifier(alphaModifier1);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Replace with new alpha property
    auto alphaModifier2 = std::make_shared<ModifierNG::RSAlphaModifier>();
    alphaModifier2->SetAlpha(0.8f);
    testNode->AddModifier(alphaModifier2);
}

/*
 * @tc.name: ModifierAttachPropertyTest_010
 * @tc.desc: Test AttachProperty with pivot (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L7
 */
GRAPHIC_TEST(ModifierAttachPropertyTest, CONTENT_DISPLAY_TEST, ModifierAttachPropertyTest_010)
{
    std::vector<std::pair<float, float>> pivotValues = {
        { 0.0f, 0.0f },
        { 0.5f, 0.5f },
        { 1.0f, 1.0f }
    };

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < pivotValues.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);

            auto pivotModifier = std::make_shared<ModifierNG::RSTransformModifier>();
            pivotModifier->SetPivot({ pivotValues[col].first, pivotValues[col].second });
            auto rotationModifier = std::make_shared<ModifierNG::RSTransformModifier>();
            rotationModifier->SetRotation(45.0f);

            testNode->AddModifier(pivotModifier);
            testNode->AddModifier(rotationModifier);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

} // namespace OHOS::Rosen
