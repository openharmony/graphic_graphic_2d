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

#include "gtest/gtest.h"
#include "modifier_render_thread/rs_canvas_modifiers_draw_agent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSCanvasModifiersDrawAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasModifiersDrawAgentTest::SetUpTestCase() {}
void RSCanvasModifiersDrawAgentTest::TearDownTestCase() {}
void RSCanvasModifiersDrawAgentTest::SetUp() {}
void RSCanvasModifiersDrawAgentTest::TearDown() {}

HWTEST_F(RSCanvasModifiersDrawAgentTest, Constructor_InitializesCorrectly001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    EXPECT_NE(agent, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, SetCacheDir_Basic001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    agent->SetCacheDir("/test/cache");
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeCreate_Basic001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    NodeId nodeId = 12345;
    std::weak_ptr<RSRenderInterface> weakInterface;
    agent->OnNodeCreate(nodeId, weakInterface);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeRelease_Basic001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    NodeId nodeId = 12345;
    std::weak_ptr<RSRenderInterface> weakInterface;
    agent->OnNodeRelease(nodeId, weakInterface);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeStateChanged_InactiveState001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    NodeId nodeId = 67890;
    agent->OnNodeStateChanged(nodeId, RSNodeState::INACTIVE);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeStateChanged_ActiveState001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    NodeId nodeId = 67890;
    agent->OnNodeStateChanged(nodeId, RSNodeState::ACTIVE);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, ResetSurface_Basic001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    NodeId nodeId = 12345;
    agent->ResetSurface(nodeId, 100, 100, false, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, ResetSurface_SizeOutOfGpuLimit001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    NodeId nodeId = 12345;
    agent->ResetSurface(nodeId, 100, 100, true, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, GetBitmap_Basic001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    NodeId nodeId = 12345;
    Drawing::Bitmap bitmap;
    bool result = agent->GetBitmap(nodeId, bitmap);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, GetPixelMap_Basic001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    NodeId nodeId = 12345;
    std::shared_ptr<Media::PixelMap> pixelMap;
    Drawing::Rect rect(0, 0, 100, 100);
    bool result = agent->GetPixelMap(nodeId, pixelMap, &rect, nullptr);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, UpdateCanvasContent_Basic001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    NodeId nodeId = 12345;
    agent->UpdateCanvasContent(nodeId, nullptr);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, SubmitAndCollectCanvasBuffers_Basic001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    agent->SubmitAndCollectCanvasBuffers();
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, SwapTransactionConfigList_Basic001, TestSize.Level1)
{
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>();
    std::vector<RSTransactionConfig> configs;
    agent->SwapTransactionConfigList(configs);
    EXPECT_EQ(configs.size(), 0);
}
} // namespace Rosen
} // namespace OHOS
