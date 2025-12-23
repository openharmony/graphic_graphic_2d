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

#include "parameters.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "rs_render_process_manager_agent.h"
#include "rs_render_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
static inline sptr<RSRenderProcessManagerAgent> rsManager_ = nullptr;
static inline sptr<RSRenderProcessManagerAgent> rsManager1_ = nullptr;
}
class RSRenderProcessManagerAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderProcessManagerAgentTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService.Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    rsManager_ = sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService.renderProcessManager_);
    rsManager1_ = sptr<RSRenderProcessManagerAgent>::MakeSptr(nullptr);
}

void RSRenderProcessManagerAgentTest::TearDownTestCase() {}
void RSRenderProcessManagerAgentTest::SetUp() {}
void RSRenderProcessManagerAgentTest::TearDown() {}

/**
 * @tc.name: GetServiceToRenderConnTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderProcessManagerAgentTest, GetServiceToRenderConnTest, TestSize.Level1)
{
    ScreenId screenId = 1;
    rsManager_->GetServiceToRenderConn(screenId);
    rsManager_->GetServiceToRenderConns();
    ASSERT_EQ(rsManager1_->GetServiceToRenderConn(screenId), nullptr);
    ASSERT_TRUE(rsManager1_->GetServiceToRenderConns().empty());
    ASSERT_TRUE(rsManager_);
}
} // namespace OHOS::Rosen