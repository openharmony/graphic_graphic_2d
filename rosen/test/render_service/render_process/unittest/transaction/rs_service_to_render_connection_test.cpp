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

#include <iremote_broker.h>

#include "irs_render_to_composer_connection.h"
#include "parameters.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "rs_render_process_manager.h"
#include "rs_render_service.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_render_single_process_manager.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_service_to_render_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
sptr<RSServiceToRenderConnection> g_rsConn = nullptr;
}

class RSServiceToRenderConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSServiceToRenderConnectionTest::SetUpTestCase()
{
    auto runner = AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    auto renderPipeline = RSRenderPipeline::Create(handler, nullptr, nullptr);

    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService.Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    sptr<RSRenderPipelineAgent> renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline);
    g_rsConn = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
}
void RSServiceToRenderConnectionTest::TearDownTestCase() {}
void RSServiceToRenderConnectionTest::SetUp() {}
void RSServiceToRenderConnectionTest::TearDown() {}

/**
 * @tc.name: GetRealtimeRefreshRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, GetRealtimeRefreshRateTest, TestSize.Level1)
{
    EXPECT_GE(g_rsConn->GetRealtimeRefreshRate(INVALID_SCREEN_ID), 0);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, SetShowRefreshRateEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    int32_t type = 1;
    g_rsConn->SetShowRefreshRateEnabled(enabled, type);
    g_rsConn->SetShowRefreshRateEnabled(enabled1, type);
    ASSERT_TRUE(g_rsConn);
}

/**
 * @tc.name: GetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionTest, GetShowRefreshRateEnabledTest, TestSize.Level1)
{
    bool enabled = true;
    bool enabled1 = false;
    g_rsConn->GetShowRefreshRateEnabled(enabled);
    g_rsConn->GetShowRefreshRateEnabled(enabled1);
    ASSERT_TRUE(g_rsConn);
}
} // namespace OHOS::Rosen
