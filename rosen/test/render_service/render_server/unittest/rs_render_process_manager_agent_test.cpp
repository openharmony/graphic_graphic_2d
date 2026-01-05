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
sptr<RSRenderProcessManagerAgent> g_rsManager = nullptr;
sptr<RSRenderProcessManagerAgent> g_rsManager1 = nullptr;
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
    g_rsManager = sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService.renderProcessManager_);
    g_rsManager1 = sptr<RSRenderProcessManagerAgent>::MakeSptr(nullptr);
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
    g_rsManager->GetServiceToRenderConn(screenId);
    g_rsManager->GetServiceToRenderConns();
    ASSERT_EQ(g_rsManager1->GetServiceToRenderConn(screenId), nullptr);
    ASSERT_TRUE(g_rsManager1->GetServiceToRenderConns().empty());
    ASSERT_TRUE(g_rsManager);
}
} // namespace OHOS::Rosen