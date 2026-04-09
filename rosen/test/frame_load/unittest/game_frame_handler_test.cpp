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

#include <atomic>
#include <gtest/gtest.h>

#include "frame_report.h"
#include "hgm_core.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "rs_game_frame_handler.h"
#include "rs_render_service.h"
#include "vsync_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t UNI_RENDER_VSYNC_OFFSET = 5000000; // ns
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_DELAY_MODE = -3300000; // ns
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_TEST_VALUE = 8000000; // ns
}
class RsGameFrameHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsGameFrameHandlerTest::SetUpTestCase() {}
void RsGameFrameHandlerTest::TearDownTestCase() {}
void RsGameFrameHandlerTest::SetUp() {}
void RsGameFrameHandlerTest::TearDown() {}

/**
 * @tc.name: Init001
 * @tc.desc: test Init with valid callbacks
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, Init001, TestSize.Level1)
{
    auto handleFunc = [](int64_t rsOffset, int64_t appOffset) {};
    auto getCoreInfoFunc = [](bool& isLtpoEnabled, bool& isVsyncOffsetCustomized, bool& isDelayMode, int64_t& rsOffset,
        int64_t& appOffset) {
        isLtpoEnabled = false;
        isVsyncOffsetCustomized = true;
        isDelayMode = false;
        rsOffset = UNI_RENDER_VSYNC_OFFSET_TEST_VALUE;
        appOffset = UNI_RENDER_VSYNC_OFFSET_TEST_VALUE;
    };
    sptr<RsGameFrameHandler> handler = sptr<RsGameFrameHandler>::MakeSptr(handleFunc, getCoreInfoFunc);
    EXPECT_NE(handler, nullptr);
    EXPECT_NE(handler->handleGameSceneChangedFunc_, nullptr);
    EXPECT_NE(handler->getCoreInfoFunc_, nullptr);
}

/**
 * @tc.name: HandleGameSceneChanged001
 * @tc.desc: test HandleGameSceneChanged when LTPO is enabled and vsync offset is not customized.
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, HandleGameSceneChanged001, TestSize.Level1)
{
    sptr<RSRenderService> renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSVsyncManager> vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    sptr<RSScreenManager> screenManager = sptr<RSScreenManager>::MakeSptr();
    EXPECT_NE(renderService, nullptr);
    EXPECT_NE(vsyncManager, nullptr);
    EXPECT_NE(screenManager, nullptr);
    bool ret = vsyncManager->init(screenManager);
    EXPECT_TRUE(ret);
    renderService->vsyncManager_ = vsyncManager;
    EXPECT_NE(renderService->vsyncManager_, nullptr);
    EXPECT_NE(vsyncManager->rsVSyncController_, nullptr);
    EXPECT_NE(vsyncManager->appVSyncController_, nullptr);
    auto& hgmCore = HgmCore::Instance();
    if (!hgmCore.GetLtpoEnabled()) {
        if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            if (hgmCore.IsDelayMode()) {
                EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET_DELAY_MODE);
                EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET_DELAY_MODE);
            } else {
                EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET);
                EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET);
            }
        }
    }
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    EXPECT_NE(handler, nullptr);
    int64_t rsOffset = vsyncManager->rsVSyncController_->GetPhaseOffset();
    int64_t appOffset = vsyncManager->appVSyncController_->GetPhaseOffset();
    hgmCore.isLtpoMode_.store(true);
    hgmCore.isVsyncOffsetCustomized_.store(false);
    handler->HandleGameSceneChanged();
    EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), rsOffset);
    EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), appOffset);
}

/**
 * @tc.name: HandleGameSceneChanged002
 * @tc.desc: test HandleGameSceneChanged when LTPO is disabled and VSync offset is not customized
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, HandleGameSceneChanged002, TestSize.Level1)
{
    sptr<RSRenderService> renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSVsyncManager> vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    sptr<RSScreenManager> screenManager = sptr<RSScreenManager>::MakeSptr();
    EXPECT_NE(renderService, nullptr);
    EXPECT_NE(vsyncManager, nullptr);
    EXPECT_NE(screenManager, nullptr);
    bool ret = vsyncManager->init(screenManager);
    EXPECT_TRUE(ret);
    renderService->vsyncManager_ = vsyncManager;
    EXPECT_NE(renderService->vsyncManager_, nullptr);
    EXPECT_NE(vsyncManager->rsVSyncController_, nullptr);
    EXPECT_NE(vsyncManager->appVSyncController_, nullptr);
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    EXPECT_NE(handler, nullptr);
    int64_t rsOffset = vsyncManager->rsVSyncController_->GetPhaseOffset();
    int64_t appOffset = vsyncManager->appVSyncController_->GetPhaseOffset();
    auto& hgmCore = HgmCore::Instance();
    hgmCore.isLtpoMode_.store(false);
    hgmCore.isVsyncOffsetCustomized_.store(false);
    handler->HandleGameSceneChanged();
    EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), rsOffset);
    EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), appOffset);
}

/**
 * @tc.name: HandleGameSceneChanged003
 * @tc.desc: test HandleGameSceneChanged when LTPO is enabled and VSync offset is customized
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, HandleGameSceneChanged003, TestSize.Level1)
{
    sptr<RSRenderService> renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSVsyncManager> vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    sptr<RSScreenManager> screenManager = sptr<RSScreenManager>::MakeSptr();
    EXPECT_NE(renderService, nullptr);
    EXPECT_NE(vsyncManager, nullptr);
    EXPECT_NE(screenManager, nullptr);
    bool ret = vsyncManager->init(screenManager);
    EXPECT_TRUE(ret);
    renderService->vsyncManager_ = vsyncManager;
    EXPECT_NE(renderService->vsyncManager_, nullptr);
    EXPECT_NE(vsyncManager->rsVSyncController_, nullptr);
    EXPECT_NE(vsyncManager->appVSyncController_, nullptr);
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    EXPECT_NE(handler, nullptr);
    int64_t rsOffset = vsyncManager->rsVSyncController_->GetPhaseOffset();
    int64_t appOffset = vsyncManager->appVSyncController_->GetPhaseOffset();
    auto& hgmCore = HgmCore::Instance();
    hgmCore.isLtpoMode_.store(true);
    hgmCore.isVsyncOffsetCustomized_.store(true);
    handler->HandleGameSceneChanged();
    EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), rsOffset);
    EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), appOffset);
}

/**
 * @tc.name: HandleGameSceneChanged004
 * @tc.desc: test HandleGameSceneChanged when LTPO is disabled and VSync offset is customized
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, HandleGameSceneChanged004, TestSize.Level1)
{
    sptr<RSRenderService> renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSVsyncManager> vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    sptr<RSScreenManager> screenManager = sptr<RSScreenManager>::MakeSptr();
    EXPECT_NE(renderService, nullptr);
    EXPECT_NE(vsyncManager, nullptr);
    EXPECT_NE(screenManager, nullptr);
    bool ret = vsyncManager->init(screenManager);
    EXPECT_TRUE(ret);
    renderService->vsyncManager_ = vsyncManager;
    EXPECT_NE(renderService->vsyncManager_, nullptr);
    EXPECT_NE(vsyncManager->rsVSyncController_, nullptr);
    EXPECT_NE(vsyncManager->appVSyncController_, nullptr);
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    EXPECT_NE(handler, nullptr);
    int64_t rsOffset = vsyncManager->rsVSyncController_->GetPhaseOffset();
    int64_t appOffset = vsyncManager->appVSyncController_->GetPhaseOffset();
    auto& hgmCore = HgmCore::Instance();
    hgmCore.isLtpoMode_.store(false);
    hgmCore.isVsyncOffsetCustomized_.store(true);
    handler->HandleGameSceneChanged();
    EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), rsOffset);
    EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), appOffset);
}

/**
 * @tc.name: HandleGameSceneChanged005
 * @tc.desc: test HandleGameSceneChanged when game scene hasn't changed
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, HandleGameSceneChanged005, TestSize.Level1)
{
    sptr<RSRenderService> renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSVsyncManager> vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    sptr<RSScreenManager> screenManager = sptr<RSScreenManager>::MakeSptr();
    EXPECT_NE(renderService, nullptr);
    EXPECT_NE(vsyncManager, nullptr);
    EXPECT_NE(screenManager, nullptr);
    bool ret = vsyncManager->init(screenManager);
    EXPECT_TRUE(ret);
    renderService->vsyncManager_ = vsyncManager;
    EXPECT_NE(renderService->vsyncManager_, nullptr);
    EXPECT_NE(vsyncManager->rsVSyncController_, nullptr);
    EXPECT_NE(vsyncManager->appVSyncController_, nullptr);
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    EXPECT_NE(handler, nullptr);
    int64_t rsOffset = vsyncManager->rsVSyncController_->GetPhaseOffset();
    int64_t appOffset = vsyncManager->appVSyncController_->GetPhaseOffset();
    auto& hgmCore = HgmCore::Instance();
    hgmCore.isLtpoMode_.store(false);
    hgmCore.isVsyncOffsetCustomized_.store(true);
    handler->hasGameScene_.store(false);
    FrameReport::GetInstance().activelyPid_.store(0);
    handler->HandleGameSceneChanged();
    EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), rsOffset);
    EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), appOffset);
}

/**
 * @tc.name: HandleGameSceneChanged006
 * @tc.desc: test HandleGameSceneChanged when entering game scene (non-delay mode)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, HandleGameSceneChanged006, TestSize.Level1)
{
    sptr<RSRenderService> renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSVsyncManager> vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    sptr<RSScreenManager> screenManager = sptr<RSScreenManager>::MakeSptr();
    EXPECT_NE(renderService, nullptr);
    EXPECT_NE(vsyncManager, nullptr);
    EXPECT_NE(screenManager, nullptr);
    bool ret = vsyncManager->init(screenManager);
    EXPECT_TRUE(ret);
    renderService->vsyncManager_ = vsyncManager;
    EXPECT_NE(renderService->vsyncManager_, nullptr);
    EXPECT_NE(vsyncManager->rsVSyncController_, nullptr);
    EXPECT_NE(vsyncManager->appVSyncController_, nullptr);
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    EXPECT_NE(handler, nullptr);
    auto& hgmCore = HgmCore::Instance();
    hgmCore.isLtpoMode_.store(false);
    hgmCore.isVsyncOffsetCustomized_.store(true);
    hgmCore.isDelayMode_ = false;
    hgmCore.rsPhaseOffset_.store(UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    hgmCore.appPhaseOffset_.store(UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    handler->hasGameScene_.store(false);
    FrameReport::GetInstance().activelyPid_.store(1);
    EXPECT_EQ(RSUniRenderJudgement::uniRenderEnabledType_, UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
    handler->HandleGameSceneChanged();
    EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
}

/**
 * @tc.name: HandleGameSceneChanged007
 * @tc.desc: test HandleGameSceneChanged when entering game scene (delay mode)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, HandleGameSceneChanged007, TestSize.Level1)
{
    sptr<RSRenderService> renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSVsyncManager> vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    sptr<RSScreenManager> screenManager = sptr<RSScreenManager>::MakeSptr();
    EXPECT_NE(renderService, nullptr);
    EXPECT_NE(vsyncManager, nullptr);
    EXPECT_NE(screenManager, nullptr);
    bool ret = vsyncManager->init(screenManager);
    EXPECT_TRUE(ret);
    renderService->vsyncManager_ = vsyncManager;
    EXPECT_NE(renderService->vsyncManager_, nullptr);
    EXPECT_NE(vsyncManager->rsVSyncController_, nullptr);
    EXPECT_NE(vsyncManager->appVSyncController_, nullptr);
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    EXPECT_NE(handler, nullptr);
    auto& hgmCore = HgmCore::Instance();
    hgmCore.isLtpoMode_.store(false);
    hgmCore.isVsyncOffsetCustomized_.store(true);
    hgmCore.isDelayMode_ = true;
    hgmCore.rsPhaseOffset_.store(UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    hgmCore.appPhaseOffset_.store(UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    handler->hasGameScene_.store(false);
    FrameReport::GetInstance().activelyPid_.store(1);
    EXPECT_EQ(RSUniRenderJudgement::uniRenderEnabledType_, UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
    handler->HandleGameSceneChanged();
    EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
}

/**
 * @tc.name: HandleGameSceneChanged008
 * @tc.desc: test HandleGameSceneChanged when exiting game scene in delay mode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, HandleGameSceneChanged008, TestSize.Level1)
{
    sptr<RSRenderService> renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSVsyncManager> vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    sptr<RSScreenManager> screenManager = sptr<RSScreenManager>::MakeSptr();
    EXPECT_NE(renderService, nullptr);
    EXPECT_NE(vsyncManager, nullptr);
    EXPECT_NE(screenManager, nullptr);
    bool ret = vsyncManager->init(screenManager);
    EXPECT_TRUE(ret);
    renderService->vsyncManager_ = vsyncManager;
    EXPECT_NE(renderService->vsyncManager_, nullptr);
    EXPECT_NE(vsyncManager->rsVSyncController_, nullptr);
    EXPECT_NE(vsyncManager->appVSyncController_, nullptr);
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    EXPECT_NE(handler, nullptr);
    auto& hgmCore = HgmCore::Instance();
    hgmCore.isLtpoMode_.store(false);
    hgmCore.isVsyncOffsetCustomized_.store(true);
    hgmCore.isDelayMode_ = true;
    hgmCore.rsPhaseOffset_.store(UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    hgmCore.appPhaseOffset_.store(UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    handler->hasGameScene_.store(true);
    FrameReport::GetInstance().activelyPid_.store(0);
    EXPECT_EQ(RSUniRenderJudgement::uniRenderEnabledType_, UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
    handler->HandleGameSceneChanged();
    EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET_DELAY_MODE);
    EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET_DELAY_MODE);
}

/**
 * @tc.name: HandleGameSceneChanged009
 * @tc.desc: test HandleGameSceneChanged when exiting game scene(not in delay mode)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsGameFrameHandlerTest, HandleGameSceneChanged009, TestSize.Level1)
{
    sptr<RSRenderService> renderService = sptr<RSRenderService>::MakeSptr();
    sptr<RSVsyncManager> vsyncManager = sptr<RSVsyncManager>::MakeSptr();
    sptr<RSScreenManager> screenManager = sptr<RSScreenManager>::MakeSptr();
    EXPECT_NE(renderService, nullptr);
    EXPECT_NE(vsyncManager, nullptr);
    EXPECT_NE(screenManager, nullptr);
    bool ret = vsyncManager->init(screenManager);
    EXPECT_TRUE(ret);
    renderService->vsyncManager_ = vsyncManager;
    EXPECT_NE(renderService->vsyncManager_, nullptr);
    EXPECT_NE(vsyncManager->rsVSyncController_, nullptr);
    EXPECT_NE(vsyncManager->appVSyncController_, nullptr);
    renderService->InitGameFrameHandler();
    auto& handler = renderService->GetGameFrameHandler();
    EXPECT_NE(handler, nullptr);
    auto& hgmCore = HgmCore::Instance();
    hgmCore.isLtpoMode_.store(false);
    hgmCore.isVsyncOffsetCustomized_.store(true);
    hgmCore.isDelayMode_ = false;
    hgmCore.rsPhaseOffset_.store(UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    hgmCore.appPhaseOffset_.store(UNI_RENDER_VSYNC_OFFSET_TEST_VALUE);
    handler->hasGameScene_.store(true);
    FrameReport::GetInstance().activelyPid_.store(0);
    EXPECT_EQ(RSUniRenderJudgement::uniRenderEnabledType_, UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
    handler->HandleGameSceneChanged();
    EXPECT_EQ(vsyncManager->rsVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET);
    EXPECT_EQ(vsyncManager->appVSyncController_->GetPhaseOffset(), UNI_RENDER_VSYNC_OFFSET);
}
} // namespace Rosen
} // namespace OHOS
