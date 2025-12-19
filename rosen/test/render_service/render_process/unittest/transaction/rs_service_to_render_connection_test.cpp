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
#include "transaction/rs_service_to_render_connection.h"
#include "rs_render_process_manager.h"
#include "rs_render_service.h"
#include "screen_manager/rs_screen_manager.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "irs_render_to_composer_connection.h"
#include "rs_render_process_agent.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "rs_render_single_process_manager.h"


namespace OHOS::Rosen {
class RSServiceToRenderConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline sptr<RSIServiceToRenderConnection> rsConn_ = nullptr;
};

void RSServiceToRenderConnectionTest::SetUpTestCase()
{
    auto runner = AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    auto renderPipeline = RSRenderPipeline::Create(handler, nullptr, nullptr);
    RSRenderService renderService;
    auto rsRenderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService);
    sptr<RSRenderServiceAgent> renderPipelineAgent = new sptr<RSRenderServiceAgent>::MakeSptr(renderPipeline);
    rsConn_ = sptr<RSServiceToRenderConnection>::MakeSptr(rsRenderServiceAgent, renderPipelineAgent);
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
    EXEPECT_GE(rsConn_->GetRealtimeRefreshRate(INVALID_SCREEN_ID), 0);
    ASSERT_TRUE(rsConn_);
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
    rsConn_->SetShowRefreshRateEnabled(enabled, type);
    rsConn_->SetShowRefreshRateEnabled(enabled1, type);
    ASSERT_TRUE(rsConn_);
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
    int32_t type = 1;
    rsConn_->GetShowRefreshRateEnabled(enabled, type);
    rsConn_->GetShowRefreshRateEnabled(enabled1, type);
    ASSERT_TRUE(rsConn_);
}
} // namespace OHOS::Rosen
