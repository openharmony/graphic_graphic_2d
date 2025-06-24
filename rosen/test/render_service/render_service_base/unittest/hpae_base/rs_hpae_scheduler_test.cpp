/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "gtest/gtest.h"
#include <parameter.h>
#include <parameters.h>
#include <unistd.h>
#include "param/sys_param.h" 
#include "hpae_base/rs_hpae_scheduler.h"
#include "hpae_base/rs_hpae_base_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpaeSchedulerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::string hpaeSwitch;
    static inline std::string hpaeAaeSwitch;
};

void RSHpaeSchedulerTest::SetUpTestCase()
{
    hpaeSwitch = OHOS::system::GetParameter("debug.graphic.hpae.blur.enabled", "0");
    hpaeAaeSwitch = OHOS::system::GetParameter("rosen.graphic.hpae.blur.aae.enabled", "0");
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", "1");
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", "1"); 
}
void RSHpaeSchedulerTest::TearDownTestCase() {
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", hpaeSwitch);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", hpaeAaeSwitch);
}
void RSHpaeSchedulerTest::SetUp() {}
void RSHpaeSchedulerTest::TearDown() {}

/**
 * @tc.name: ResetTest
 * @tc.desc: Verify function Reset
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeSchedulerTest, ResetTest, TestSize.Level1)
{
    RSHpaeScheduler hpaeScheduler;
    hpaeScheduler.hpaeFrameId_ = 1;
    hpaeScheduler.Reset();
    EXPECT_EQ(hpaeScheduler.hpaeFrameId_, 0);
}

/**
 * @tc.name: CacheHpaeItemTest
 * @tc.desc: Verify function CacheHpaeItem
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeSchedulerTest, CacheHpaeItemTest, TestSize.Level1)
{
    RSHpaeScheduler hpaeScheduler;
    HpaeBackgroundCacheItem item;
    item.gpFrameId_ = 1;
    hpaeScheduler.CacheHpaeItem(item);
    EXPECT_EQ(hpaeScheduler.cachedItem_.gpFrameId_, 1);
}

/**
 * @tc.name: GetCacheHpaeItemTest
 * @tc.desc: Verify function GetCacheHpaeItem
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeSchedulerTest, GetCachedHpaeItemTest, TestSize.Level1)
{
    RSHpaeScheduler hpaeScheduler;
    HpaeBackgroundCacheItem item;
    item.gpFrameId_ = 1;
    hpaeScheduler.CacheHpaeItem(item);
    auto outItem = hpaeScheduler.GetCachedHpaeItem();
    EXPECT_EQ(outItem.gpFrameId_, 1);
}

/**
 * @tc.name: SetHpaeFrameIdTest
 * @tc.desc: Verify function SetHpaeFrameId
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeSchedulerTest, SetHpaeFrameIdTest, TestSize.Level1)
{
    RSHpaeScheduler hpaeScheduler;
    hpaeScheduler.SetHpaeFrameId(1);
    EXPECT_EQ(hpaeScheduler.hpaeFrameId_, 1);
}

/**
 * @tc.name: GetHpaeFrameIdTest
 * @tc.desc: Verify function GetHpaeFrameId
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeSchedulerTest, GetHpaeFrameIdTest, TestSize.Level1)
{
    RSHpaeScheduler hpaeScheduler;
    hpaeScheduler.SetHpaeFrameId(1);
    auto frameId = hpaeScheduler.GetHpaeFrameId();
    EXPECT_EQ(frameId, 1);
}

/**
 * @tc.name: SetToWaitBuildTaskTest
 * @tc.desc: Verify function SetToWaitBuildTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeSchedulerTest, SetToWaitBuildTaskTest, TestSize.Level1)
{
    RSHpaeScheduler hpaeScheduler;
    hpaeScheduler.buildTaskDone_ = true;
    hpaeScheduler.SetToWaitBuildTask();
    EXPECT_FALSE(hpaeScheduler.buildTaskDone_);
}

/**
 * @tc.name: NotifyBuildTaskDoneTest
 * @tc.desc: Verify function NotifyBuildTaskDone
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeSchedulerTest, NotifyBuildTaskDoneTest, TestSize.Level1)
{
    RSHpaeScheduler hpaeScheduler;
    hpaeScheduler.buildTaskDone_ = false;
    hpaeScheduler.NotifyBuildTaskDone();
    EXPECT_TRUE(hpaeScheduler.buildTaskDone_);
}

/**
 * @tc.name: WaitBuildTaskTest
 * @tc.desc: Verify function WaitBuildTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeSchedulerTest, WaitBuildTaskTest, TestSize.Level1)
{
    RSHpaeScheduler hpaeScheduler;
    hpaeScheduler.SetToWaitBuildTask();
    hpaeScheduler.WaitBuildTask();
    EXPECT_TRUE(hpaeScheduler.buildTaskDone_);
}

/**
 * @tc.name: WaitBuildTaskTest01
 * @tc.desc: Verify function WaitBuildTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeSchedulerTest, WaitBuildTaskTest01, TestSize.Level1)
{
    RSHpaeScheduler::GetInstance().buildTaskDone_ = true;
    RSHpaeScheduler::GetInstance().WaitBuildTask();
    EXPECT_TRUE(RSHpaeScheduler::GetInstance().buildTaskDone_);
}

} // namespace Rosen
} // namespace OHOS