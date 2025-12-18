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
#include "rs_render_process_manager_agent.h"
#include "rs_render_multi_process_manager.h"
#include "rs_render_service.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"

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
}

void RSRenderProcessManagerAgentTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService.Init_V2();
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    rsManager_ = sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService.renderProcessManager_);
    rsManager1_ = sptr<RSRenderProcessManagerAgent>::MakeSptr(nullptr);
}

void RSRenderProcessManagerAgentTest::TearDownTestCase() {}
void RSRenderProcessManagerAgentTest::SetUp() {}
void RSRenderProcessManagerAgentTest::TearDown() {}

/**
 * @tc.name: HandleHwcPackageEventTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderSingleProcessManagerTest, HandleHwcPackageEventTest, TestSize.Level1)
{
    uint32_t listSize = 1;
    std::vector<std::string> packageList = {"a", "b"};
    rsManager_->HandleHwcPackageEvent(listSize, packageList);
    ASSERT_TRUE(rsManager_);
}

/**
 * @tc.name: RegisterTypefaceTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderSingleProcessManagerTest, RegisterTypefaceTest, TestSize.Level1)
{
    uint64_t globalUniqueId = 1;
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;
    ASSERT_TRUE(rsManager_->RegisterTypeface(globalUniqueId, typeface));
    ASSERT_TRUE(rsManager_->UnRegisterTypeface(globalUniqueId));
    ASSERT_FALSE(rsManager1_->RegisterTypeface(globalUniqueId, typeface));
    ASSERT_FALSE(rsManager1_->UnRegisterTypeface(globalUniqueId));
}

/**
 * @tc.name: GetServiceToRenderConnTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderSingleProcessManagerTest, GetServiceToRenderConnTest, TestSize.Level1)
{
    ScreenId screenId = 1;
    rsManager_->GetServiceToRenderConn(screenId);
    rsManager_->GetServiceToRenderConns();
    ASSERT_TRUE(rsManager_);
}
}