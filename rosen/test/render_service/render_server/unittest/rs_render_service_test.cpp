/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include <parameters.h>
#include "rs_render_service.h"
#include "rs_render_process_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "vsync/vsync_manager.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen.h"
#include "rs_render_composer_manager.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "feature/hyper_graphic_manager/hgm_context.h"
#include "hgm_core.h"
#include "dfx/rs_service_dumper.h"
#include "dfx/rs_service_dump_manager.h"
#include "rs_game_frame_handler.h"
#include "graphic_feature_param_manager.h"
#include "vsync_distributor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int32_t INVALID_FD = -1;
constexpr int32_t VALID_FD = 1;
constexpr ScreenId TEST_SCREEN_ID = 1;
constexpr NodeId TEST_NODE_ID = 100;
}

class RSRenderServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline sptr<RSRenderService> renderService_ = nullptr;
    static inline sptr<RSScreenManager> screenManager_ = nullptr;
    static inline sptr<RSVsyncManager> vsyncManager_ = nullptr;
};

void RSRenderServiceTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService_ = sptr<RSRenderService>::MakeSptr();
    screenManager_ = sptr<RSScreenManager>::MakeSptr();
    vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
}

void RSRenderServiceTest::TearDownTestCase()
{
    renderService_ = nullptr;
    screenManager_ = nullptr;
    vsyncManager_ = nullptr;
}

void RSRenderServiceTest::SetUp() {}

void RSRenderServiceTest::TearDown() {}

HWTEST_F(RSRenderServiceTest, InitRenderServerConfig001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    renderService_->InitRenderServerConfig();
    ASSERT_NE(renderService_->renderModeConfig_, nullptr);
}

HWTEST_F(RSRenderServiceTest, InitCCMConfig001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    renderService_->InitCCMConfig();
    EXPECT_TRUE(true);
}

HWTEST_F(RSRenderServiceTest, CoreComponentsInit001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->InitRenderServerConfig();
    bool result = renderService_->CoreComponentsInit();
    ASSERT_TRUE(result);
}

HWTEST_F(RSRenderServiceTest, HgmInit001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    renderService_->vsyncManager_->init(renderService_->screenManager_);
    renderService_->InitRenderServerConfig();
    renderService_->HgmInit();
    EXPECT_TRUE(true);
}

HWTEST_F(RSRenderServiceTest, FeatureComponentInit001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    renderService_->vsyncManager_->init(renderService_->screenManager_);
    renderService_->FeatureComponentInit();
    ASSERT_NE(renderService_->rsDumper_, nullptr);
    ASSERT_NE(renderService_->rsDumpManager_, nullptr);
}

HWTEST_F(RSRenderServiceTest, RenderProcessManagerInit001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    renderService_->InitRenderServerConfig();
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    renderService_->vsyncManager_->init(renderService_->screenManager_);
    renderService_->InitRenderServerConfig();
    renderService_->rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService_->handler_);
    if (auto mgr = HgmCore::Instance().GetFrameRateMgr()) {
        auto func = [](bool forceUpdate, ScreenId activeScreenId) {};
        auto rsDistributor = sptr<VSyncDistributor>::MakeSptr("rs");
        auto appDistributor = sptr<VSyncDistributor>::MakeSptr("app");
        auto eventRunner = AppExecFwk::EventRunner::Create(false);
        auto handler = std::make_shared<AppExecFwk::EventHandler>(eventRunner);
        renderService_->hgmContext_ = std::make_shared<HgmContext>(
            handler, mgr, func, appDistributor, rsDistributor);
    }
    renderService_->RenderProcessManagerInit();
    ASSERT_NE(renderService_->renderProcessManager_, nullptr);
}

HWTEST_F(RSRenderServiceTest, SAMgrRegister001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    bool result = renderService_->SAMgrRegister();
    ASSERT_FALSE(result);
}

HWTEST_F(RSRenderServiceTest, SAMgrRegister002, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    OHOS::system::SetParameter("bootevent.samgr.ready", "true");
    bool result = renderService_->SAMgrRegister();
    ASSERT_TRUE(result);
}

HWTEST_F(RSRenderServiceTest, Run001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    renderService_->Run();
    EXPECT_TRUE(true);
}

HWTEST_F(RSRenderServiceTest, Run002, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->runner_ = runner;
    auto thread = std::make_unique<std::thread>([renderService = renderService_] { renderService->Run(); });
    thread->detach();
    EXPECT_TRUE(true);
}

HWTEST_F(RSRenderServiceTest, RegisterRenderProcessConnection001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto result = renderService_->RegisterRenderProcessConnection();
    ASSERT_NE(result, nullptr);
}

HWTEST_F(RSRenderServiceTest, GetConnection001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    ASSERT_NE(token, nullptr);
    auto result = renderService_->GetConnection(token);
    EXPECT_EQ(result.first, nullptr);
    EXPECT_EQ(result.second, nullptr);
}

HWTEST_F(RSRenderServiceTest, CreateConnection001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    sptr<RSIConnectionToken> token = nullptr;
    auto result = renderService_->CreateConnection(token);
    EXPECT_EQ(result.first, nullptr);
    EXPECT_EQ(result.second, nullptr);
}

HWTEST_F(RSRenderServiceTest, CreateConnection002, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    renderService_->vsyncManager_->init(renderService_->screenManager_);
    renderService_->InitRenderServerConfig();
    renderService_->rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService_->handler_);
    if (auto mgr = HgmCore::Instance().GetFrameRateMgr()) {
        auto func = [](bool forceUpdate, ScreenId activeScreenId) {};
        auto rsDistributor = sptr<VSyncDistributor>::MakeSptr("rs");
        auto appDistributor = sptr<VSyncDistributor>::MakeSptr("app");
        auto eventRunner = AppExecFwk::EventRunner::Create(false);
        auto handler = std::make_shared<AppExecFwk::EventHandler>(eventRunner);
        renderService_->hgmContext_ = std::make_shared<HgmContext>(
            handler, mgr, func, appDistributor, rsDistributor);
    }
    renderService_->RenderProcessManagerInit();
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    auto result = renderService_->CreateConnection(token);
    ASSERT_NE(result.first, nullptr);
    ASSERT_NE(result.second, nullptr);
}

HWTEST_F(RSRenderServiceTest, RemoveConnection001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    sptr<RSIConnectionToken> token = nullptr;
    bool result = renderService_->RemoveConnection(token);
    ASSERT_FALSE(result);
}

HWTEST_F(RSRenderServiceTest, RemoveConnection002, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    bool result = renderService_->RemoveConnection(token);
    ASSERT_FALSE(result);
}

HWTEST_F(RSRenderServiceTest, Dump001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService_->handler_);
    renderService_->rsDumper_ = std::make_shared<RSServiceDumper>(
        renderService_->handler_, renderService_->screenManager_, renderService_->rsRenderComposerManager_);
    renderService_->rsDumpManager_ = std::make_shared<RSServiceDumpManager>();
    renderService_->rsDumper_->RsDumpInit(renderService_->rsDumpManager_);
    std::vector<std::u16string> args;
    int fd = INVALID_FD;
    int result = renderService_->Dump(fd, args);
    EXPECT_NE(result, OHOS::NO_ERROR);
}

HWTEST_F(RSRenderServiceTest, Dump002, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService_->handler_);
    renderService_->rsDumper_ = std::make_shared<RSServiceDumper>(
        renderService_->handler_, renderService_->screenManager_, renderService_->rsRenderComposerManager_);
    renderService_->rsDumpManager_ = std::make_shared<RSServiceDumpManager>();
    renderService_->rsDumper_->RsDumpInit(renderService_->rsDumpManager_);
    std::vector<std::u16string> args;
    int fd = VALID_FD;
    int result = renderService_->Dump(fd, args);
    EXPECT_NE(result, OHOS::NO_ERROR);
}

HWTEST_F(RSRenderServiceTest, GetRefreshInfoToSP001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService_->handler_);
    std::string dumpString;
    renderService_->GetRefreshInfoToSP(dumpString, TEST_NODE_ID);
    EXPECT_TRUE(true);
}

HWTEST_F(RSRenderServiceTest, FpsDump001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService_->handler_);
    std::string dumpString;
    std::string arg = "fps";
    renderService_->FpsDump(dumpString, arg);
    EXPECT_TRUE(true);
}

HWTEST_F(RSRenderServiceTest, HandlePowerStatus001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService_->handler_);
    ScreenPowerStatus status = POWER_STATUS_OFF;
    renderService_->HandlePowerStatus(TEST_SCREEN_ID, status);
    EXPECT_TRUE(true);
}

HWTEST_F(RSRenderServiceTest, GetProcessInfo001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    renderService_->vsyncManager_->init(renderService_->screenManager_);
    renderService_->rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(renderService_->handler_);
    sptr<IRemoteObject> vsyncToken = nullptr;
    auto result = renderService_->GetProcessInfo(TEST_SCREEN_ID, vsyncToken);
    EXPECT_EQ(result.first, nullptr);
}

HWTEST_F(RSRenderServiceTest, InitGameFrameHandler001, TestSize.Level1)
{
    ASSERT_NE(renderService_, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    renderService_->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    renderService_->screenManager_ = sptr<RSScreenManager>::MakeSptr();
    renderService_->vsyncManager_ = sptr<RSVsyncManager>::MakeSptr();
    renderService_->vsyncManager_->init(renderService_->screenManager_);
    renderService_->InitGameFrameHandler();
    ASSERT_NE(renderService_->rsGameFrameHandler_, nullptr);
}

} // namespace OHOS::Rosen