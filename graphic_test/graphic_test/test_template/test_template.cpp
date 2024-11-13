/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TestTemplateTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        auto size = GetScreenSize();
        SetSurfaceBounds({0, 0, size.x_ / 2.0f, size.y_ / 2.0f});
        SetSurfaceColor(RSColor(0xffff0000));
    }

    // called after each tests
    void AfterEach() override {}
};

/*
 * @tc.name: GRAPHIC_TEST_MACRO_TEST
 * @tc.desc: GRAPHIC_TEST with derived test class, will take surface capture
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TestTemplateTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_MACRO_TEST)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 500, 700});
    testNode->SetFrame({0, 0, 500, 700});
    testNode->SetBackgroundColor(0xffffff00);
    GetRootNode()->AddChild(testNode);

    // created node should be registered to preserve ref_count
    RegisterNode(testNode);
}

/*
 * @tc.name: GRAPHIC_TEST_MACRO_TEST
 * @tc.desc: GRAPHIC_TEST with default test class, will take surface capture
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CONTENT_DISPLAY_TEST, GRAPHIC_TEST_MACRO_TEST)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 500, 700});
    testNode->SetFrame({0, 0, 500, 700});
    testNode->SetBackgroundColor(0xffffff00);
    GetRootNode()->AddChild(testNode);

    // created node should be registered to preserve ref_count
    RegisterNode(testNode);
}

/*
 * @tc.name: GRAPHIC_TEST_N_MACRO_TEST
 * @tc.desc: GRAPHIC_TEST with derived test class, will not take surface capture
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(TestTemplateTest, CONTENT_DISPLAY_TEST, GRAPHIC_TEST_N_MACRO_TEST)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 500, 700});
    testNode->SetFrame({0, 0, 500, 700});
    testNode->SetBackgroundColor(0xffffff00);
    GetRootNode()->AddChild(testNode);

    // created node should be registered to preserve ref_count
    RegisterNode(testNode);
}

/*
 * @tc.name: GRAPHIC_TEST_N_MACRO_TEST
 * @tc.desc: GRAPHIC_TEST with default test class, will not take surface capture
 * @tc.type: FUNC
 */
GRAPHIC_N_TEST(CONTENT_DISPLAY_TEST, GRAPHIC_TEST_N_MACRO_TEST)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({0, 0, 500, 700});
    testNode->SetFrame({0, 0, 500, 700});
    testNode->SetBackgroundColor(0xffffff00);
    GetRootNode()->AddChild(testNode);

    // created node should be registered to preserve ref_count
    RegisterNode(testNode);
}

}