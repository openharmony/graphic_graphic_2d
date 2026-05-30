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
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class DoubleSidedTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/**
 * Scene 1: Parent Y180, Child 0, child doubleSided=false
 * Expected: child not visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene1_ParentY180_Child0_False)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationY(180.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(true);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(0.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(false);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}

/**
 * Scene 2: Parent Y180, Child Y180, child doubleSided=false
 * Expected: child visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene2_ParentY180_ChildY180_False)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationY(180.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(true);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(180.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(false);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}

/**
 * Scene 3: Parent Y180, Child 0, child doubleSided=true
 * Expected: child visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene3_ParentY180_Child0_True)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationY(180.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(true);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(0.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(true);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}

/**
 * Scene 4: Parent 0, Child Y180, child doubleSided=false
 * Expected: child NOT visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene4_Parent0_ChildY180_False)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationY(0.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(true);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(180.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(false);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}

/**
 * Scene 5: Parent X180, Child 0, child doubleSided=false
 * Expected: child NOT visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene5_ParentX180_Child0_False)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationX(180.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(true);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(0.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(false);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}

/**
 * Scene 6: Parent 0, Child 0
 * Expected: child visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene6_Parent0_Child0)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationX(0.0f);
    parentNode->SetRotationY(0.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(true);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(0.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(false);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}

/**
 * Scene 7: Parent 0, Child Y100, child doubleSided=false
 * Expected: child NOT visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene7_Parent0_ChildY100_False)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationY(0.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(true);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(100.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(false);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}

/**
 * Scene 8: Parent doubleSided=false, Parent Y180, Child Y0, child doubleSided=true
 * Expected: parent child NOT visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene8_ParentFalse_ParentY180_Child0)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationY(180.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(false);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(0.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(true);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}

/**
 * Scene 9: Parent doubleSided=true, Parent 0, Child Y180, child doubleSided=false
 * Expected: child NOT visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene9_ParentTrue_Parent0_ChildY180_False)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationY(0.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(true);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(180.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(false);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}

/**
 * Scene 10: Parent doubleSided=false, Parent Y100, Child Y0, child doubleSided=true
 * Expected: parent child NOT visible.
 */
GRAPHIC_TEST(DoubleSidedTest, CONTENT_DISPLAY_TEST, DoubleSided_Scene9_ParentTrue_Parent0_ChildY180_True)
{
    auto parentNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    parentNode->SetBounds({ 200, 200, 800, 800 });
    parentNode->SetRotationY(100.0f);
    parentNode->SetBackgroundColor(0xFFFF8800);
    parentNode->SetDoubleSidedEnabled(false);
    GetRootNode()->AddChild(parentNode);

    auto childNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    childNode->SetBounds({ 100, 100, 600, 600 });
    childNode->SetRotationY(0.0f);
    childNode->SetBackgroundColor(0xFF007DFF);
    childNode->SetDoubleSidedEnabled(true);
    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode);
}
} // namespace OHOS::Rosen
