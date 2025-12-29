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

#include <refbase.h>
#include <iremote_broker.h>

#include "parameters.h"
#include "rs_render_service_agent.h"
#include "rs_render_process_manager.h"
#include "rs_render_service.h"
#include "screen_manager/rs_screen_manager.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "rs_render_to_composer_connection.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "rs_render_composer_agent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
static inline sptr<RSRenderServiceAgent> rsAgent_ = nullptr;
}
class RenderServiceAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RenderServiceAgentTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService.Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    rsAgent_ = sptr<RSRenderServiceAgent>::MakeSptr(renderService);
}

void RenderServiceAgentTest::TearDownTestCase() {}
void RenderServiceAgentTest::SetUp() {}
void RenderServiceAgentTest::TearDown() {}

/**
 * @tc.name: HandleTouchEventTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RenderServiceAgentTest, HandleTouchEventTest, TestSize.Level1)
{
    int32_t touchStatus = 1;
    int32_t touchCnt = 1;
    rsAgent_->HandleTouchEvent(touchStatus, touchCnt);
    ASSERT_TRUE(rsAgent_);
}

/**
 * @tc.name: ProcessHgmFrameRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RenderServiceAgentTest, ProcessHgmFrameRateTest, TestSize.Level1)
{
    int32_t timeStamp = 1;
    int32_t vsyncId = 1;
    sptr<HgmProcessToServiceInfo> processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
    sptr<HgmServiceToProcessInfo> serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    rsAgent_->ProcessHgmFrameRate(timeStamp, vsyncId, processToServiceInfo, serviceToProcessInfo);
    ASSERT_TRUE(rsAgent_);
}
} // namespace OHOS::Rosen
