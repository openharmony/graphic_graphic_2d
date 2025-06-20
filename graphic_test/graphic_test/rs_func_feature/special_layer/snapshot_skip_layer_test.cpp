/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "rs_graphic_test_utils.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
Vector4f DEFAULT_RECT1 = {0, 0, 1000, 1000};
Vector4f DEFAULT_RECT2 = {400, 400, 100, 100};
Vector4f DEFAULT_RECT3 = {0, 0, 200, 200};

class SnapshotSkipLayerTest : public RSGraphicTest  {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        SetSurfaceColor(RSColor(COLOR_RED));
    }

    // called after each tests
    void AfterEach() override {}
};
} //namespace

/*
 * @tc.name: Set_Snapshot_Skip_Layer_Test01
 * @tc.desc: test set snapshot skip layer func
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SnapshotSkipLayerTest, CONTENT_DISPLAY_TEST, Set_Snapshot_Skip_Layer_Test01)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSnapshotSkipLayer(true);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds(DEFAULT_RECT2);
    canvasNode0->SetBackgroundColor(COLOR_YELLOW);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds(DEFAULT_RECT3);
    canvasNode1->SetBackgroundColor(COLOR_RED);
    RegisterNode(canvasNode1);
    GetRootNode()->AddChild(canvasNode1);
}

/*
 * @tc.name: Set_Snapshot_Skip_Layer_Test02
 * @tc.desc: test set snapshot skip layer func
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SnapshotSkipLayerTest, CONTENT_DISPLAY_TEST, Set_Snapshot_Skip_Layer_Test02)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSnapshotSkipLayer(false);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds(DEFAULT_RECT2);
    canvasNode0->SetBackgroundColor(COLOR_YELLOW);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds(DEFAULT_RECT3);
    canvasNode1->SetBackgroundColor(COLOR_RED);
    RegisterNode(canvasNode1);
    GetRootNode()->AddChild(canvasNode1);
}
} //namespace