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

#include <gtest/gtest.h>

#include <refbase.h>
#include <iremote_broker.h>

#include "parameters.h"
#include "transaction/rs_render_to_service_connection.h"
#include "rs_render_process_manager.h"
#include "rs_render_service.h"
#include "screen_manager/rs_screen_manager.h"
#include "rs_render_to_composer_connection_proxy.h"
#include "irs_render_to_composer_connection.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
static inline sptr<RSRenderToServiceConnection> rsConn_ = nullptr;
}
class RSRenderToServiceConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderToServiceConnectionTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", false);
    renderservice.Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    sptr<RSRenderServiceAgent> renderServiceAgent_ = sptr<RSRenderServiceAgent>::MakeSptr(renderService);
    sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent =
        sptr<RSRenderProcessManagerAgent>::MakeSptr(renderservice.renderProcessManager_);
    rsConn_ = sptr<RSRenderToServiceConnection>::MakeSptr(renderServiceAgent, renderProcessManagerAgent);
}
void RSRenderToServiceConnectionTest::TearDownTestCase() {}
void RSRenderToServiceConnectionTest::SetUp() {}
void RSRenderToServiceConnectionTest::TearDown() {}

/**
 * @tc.name: GetRealtimeRefreshRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderToServiceConnectionTest, GetRealtimeRefreshRateTest, TestSize.Level1)
{
    uint64_t timestamp = 1;
    uint64_t vsyncId = 1;
    std::unordered_set<ScreenId> screenIds = {1, 2, 3};
    sptr<HgmProcessToServiceInfo> processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
    ASSERT_TRUE(rsConn_->NotifyRenderServiceProcessHgmFrameRate(timestamp, vsyncId, screenIds, processToServiceInfo));
}
} // namespace OHOS::Rosen
