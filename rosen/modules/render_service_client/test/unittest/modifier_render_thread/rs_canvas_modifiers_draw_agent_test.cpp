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
#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_interface.h"
#include <chrono>
#include <future>

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
    static std::shared_ptr<RSRenderInterface> CreateRenderInterface();
};

std::shared_ptr<RSRenderInterface> RSCanvasModifiersDrawAgentTest::CreateRenderInterface()
{
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    auto connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    if (connectToRender == nullptr) {
        return nullptr;
    }
    return std::make_shared<RSRenderInterface>(connectToRender);
}

void RSCanvasModifiersDrawAgentTest::SetUpTestCase() {}
void RSCanvasModifiersDrawAgentTest::TearDownTestCase() {}
void RSCanvasModifiersDrawAgentTest::SetUp() {}
void RSCanvasModifiersDrawAgentTest::TearDown() {}

HWTEST_F(RSCanvasModifiersDrawAgentTest, Constructor_InitializesCorrectly001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    EXPECT_NE(agent, nullptr);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, SetCacheDir_Basic001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    agent->SetCacheDir("/test/cache");
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeCreate_Basic001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    NodeId nodeId = 12345;
    agent->OnNodeCreate(nodeId, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeRelease_Basic001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    NodeId nodeId = 12345;
    agent->OnNodeRelease(nodeId);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeStateChanged_InactiveState001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    NodeId nodeId = 67890;
    agent->OnNodeStateChanged(nodeId, RSNodeState::INACTIVE);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeStateChanged_ActiveState001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    NodeId nodeId = 67890;
    agent->OnNodeStateChanged(nodeId, RSNodeState::ACTIVE);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, ResetSurface_Basic001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    NodeId nodeId = 12345;
    agent->ResetSurface(nodeId, 100, 100, false);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, ResetSurface_SizeOutOfGpuLimit001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    NodeId nodeId = 12345;
    agent->ResetSurface(nodeId, 100, 100, true);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, GetBitmap_Basic001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    NodeId nodeId = 12345;
    Drawing::Bitmap bitmap;
    bool result = agent->GetBitmap(nodeId, bitmap);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, GetPixelMap_Basic001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    NodeId nodeId = 12345;
    std::shared_ptr<Media::PixelMap> pixelMap;
    Drawing::Rect rect(0, 0, 100, 100);
    bool result = agent->GetPixelMap(nodeId, pixelMap, &rect, nullptr);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, UpdateCanvasContent_Basic001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    NodeId nodeId = 12345;
    agent->UpdateCanvasContent(nodeId, nullptr);
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, SubmitAndCollectCanvasBuffers_Basic001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    agent->SubmitAndCollectCanvasBuffers();
    EXPECT_NE(agent->canvasModifiersDraw_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawAgentTest, SyncAndWait_Basic001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    auto result = agent->SyncAndWait();
    EXPECT_EQ(result, nullptr);
}

// WaitAllTasksFinish when thread not started: RemoveCleanFreeBuffersTask returns false,
// so PostSyncTask(ReleaseResource) is skipped — no crash, no-op
HWTEST_F(RSCanvasModifiersDrawAgentTest, WaitAllTasksFinish_ThreadNotStarted001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    EXPECT_FALSE(agent->canvasModifiersDraw_->threadStarted_.load());
    ASSERT_NO_FATAL_FAILURE(agent->WaitAllTasksFinish());
    EXPECT_FALSE(agent->canvasModifiersDraw_->threadStarted_.load());
}

// Destroy: delegates to canvasModifiersDraw_->Destroy(), stops thread and clears handler_/runner_
HWTEST_F(RSCanvasModifiersDrawAgentTest, Destroy_StopsThread001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    agent->canvasModifiersDraw_->StartThread();
    ASSERT_TRUE(agent->canvasModifiersDraw_->threadStarted_.load());
    ASSERT_NO_FATAL_FAILURE(agent->Destroy());
    EXPECT_FALSE(agent->canvasModifiersDraw_->threadStarted_.load());
    EXPECT_EQ(agent->canvasModifiersDraw_->handler_, nullptr);
    EXPECT_EQ(agent->canvasModifiersDraw_->runner_, nullptr);
}

// OnNodeCreate forwards nodeId and colorSpace to the worker thread and creates
// a drawable entry in drawableMap_ (verifies the new colorSpace parameter is
// correctly forwarded through the agent's async PostTask).
HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeCreate_ForwardsColorSpace001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    agent->canvasModifiersDraw_->StartThread();
    agent->canvasModifiersDraw_->canvasModifiersContext_->cacheDir_ = "/data/local/tmp";
    agent->canvasModifiersDraw_->canvasModifiersContext_->GetGpuContext();
    if (agent->canvasModifiersDraw_->canvasModifiersContext_->gpuContext_ == nullptr) {
        return; // GPU not available in test environment
    }
    NodeId nodeId = 20001;
    agent->OnNodeCreate(nodeId, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    auto future = agent->canvasModifiersDraw_->ScheduleTask(
        [agent, nodeId]() { return agent->canvasModifiersDraw_->drawableMap_.count(nodeId) > 0; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
    agent->Destroy();
}

// OnNodeCreate with a non-default colorSpace (DISPLAY_P3) — covers forwarding
// of a non-SRGB gamut value through the agent lambda capture.
HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeCreate_NonDefaultColorSpace001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    agent->canvasModifiersDraw_->StartThread();
    agent->canvasModifiersDraw_->canvasModifiersContext_->cacheDir_ = "/data/local/tmp";
    agent->canvasModifiersDraw_->canvasModifiersContext_->GetGpuContext();
    if (agent->canvasModifiersDraw_->canvasModifiersContext_->gpuContext_ == nullptr) {
        return;
    }
    NodeId nodeId = 20002;
    agent->OnNodeCreate(nodeId, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    auto future = agent->canvasModifiersDraw_->ScheduleTask(
        [agent, nodeId]() { return agent->canvasModifiersDraw_->drawableMap_.count(nodeId) > 0; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
    agent->Destroy();
}

// ResetSurface no longer takes a colorSpace parameter — verify it still
// forwards correctly and sets needForceSubmit_ on the worker thread.
HWTEST_F(RSCanvasModifiersDrawAgentTest, ResetSurface_ForwardsWithoutColorSpace001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    agent->canvasModifiersDraw_->StartThread();
    agent->canvasModifiersDraw_->canvasModifiersContext_->cacheDir_ = "/data/local/tmp";
    agent->canvasModifiersDraw_->canvasModifiersContext_->GetGpuContext();
    if (agent->canvasModifiersDraw_->canvasModifiersContext_->gpuContext_ == nullptr) {
        return;
    }
    NodeId nodeId = 20003;
    agent->OnNodeCreate(nodeId, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    // Wait for OnNodeCreate to complete
    auto waitFuture = agent->canvasModifiersDraw_->ScheduleTask(
        [agent, nodeId]() { return agent->canvasModifiersDraw_->drawableMap_.count(nodeId) > 0; });
    ASSERT_EQ(waitFuture.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    ASSERT_TRUE(waitFuture.get());
    agent->ResetSurface(nodeId, 100, 100, false);
    auto future = agent->canvasModifiersDraw_->ScheduleTask(
        [agent]() { return agent->canvasModifiersDraw_->needForceSubmit_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
    agent->Destroy();
}

// OnNodeStateChanged posts an async task; wait for the worker thread to drain the
// queue so the whole lambda (including the forward call) is executed.
HWTEST_F(RSCanvasModifiersDrawAgentTest, OnNodeStateChanged_TaskCompletes001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    agent->canvasModifiersDraw_->StartThread();
    NodeId nodeId = 30001;
    agent->OnNodeStateChanged(nodeId, RSNodeState::INACTIVE);
    auto future = agent->canvasModifiersDraw_->ScheduleTask([]() { return true; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
    agent->Destroy();
}

// SubmitAndCollectCanvasBuffers posts an async task; wait for the worker thread to
// drain the queue so the whole lambda (including the forward call) is executed.
HWTEST_F(RSCanvasModifiersDrawAgentTest, SubmitAndCollectCanvasBuffers_TaskCompletes001, TestSize.Level1)
{
    auto renderInterface = CreateRenderInterface();
    ASSERT_NE(renderInterface, nullptr);
    auto agent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    agent->canvasModifiersDraw_->StartThread();
    agent->SubmitAndCollectCanvasBuffers();
    auto future = agent->canvasModifiersDraw_->ScheduleTask([]() { return true; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
    agent->Destroy();
}
} // namespace Rosen
} // namespace OHOS
