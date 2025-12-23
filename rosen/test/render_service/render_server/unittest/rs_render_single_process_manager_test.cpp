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

#include "dfx/rs_service_dump_manager.h"
#include "dfx/rs_process_dump_manager.h"
#include "parameters.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "rs_render_single_process_manager.h"
#include "rs_render_pipeline.h"
#include "render_process/transaction/rs_service_to_render_connection.h"
#include "render_server/transaction/rs_render_to_service_connection.h"
#include "rs_composer_to_render_connection.h"
#include "rs_render_service.h"
#include "rs_render_to_composer_connection.h"
#include "rs_render_composer_agent.h"
#include "rs_render_composer_manager.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/rs_connect_to_render_process.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
ScreenId screenId = 1;
RSRenderService renderService;
}

class RSRenderSingleProcessManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderSingleProcessManagerTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService.Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}
void RSRenderSingleProcessManagerTest::TearDownTestCase() {}
void RSRenderSingleProcessManagerTest::SetUp() {}
void RSRenderSingleProcessManagerTest::TearDown() {}

/**
 * @tc.name: OnScreenConnectedTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderSingleProcessManagerTest, OnScreenConnectedTest, TestSize.Level1)
{
    renderService.renderProcessManager_->OnScreenDisconnected(screenId);
    renderService.renderProcessManager_->OnScreenRefresh(screenId);
    ASSERT_TRUE(renderService.renderProcessManager_);
}

/**
 * @tc.name: OnVirtualScreenConnectedTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderSingleProcessManagerTest, OnVirtualScreenConnectedTest, TestSize.Level1)
{
    renderService.renderProcessManager_->OnVirtualScreenDisconnected(screenId);
    renderService.renderProcessManager_->GetServiceToRenderConn(screenId);
    renderService.renderProcessManager_->GetServiceToRenderConns();
    renderService.renderProcessManager_->GetConnectToRenderConnection(screenId);
    ASSERT_TRUE(renderService.renderProcessManager_);
}
} // namespace OHOS::Rosen
