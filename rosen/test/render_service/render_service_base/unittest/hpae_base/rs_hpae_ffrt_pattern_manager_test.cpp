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
#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_base_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpaeFfrtPatternManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::string hpaeSwitch;
    static inline std::string hpaeAaeSwitch;
};

void RSHpaeFfrtPatternManagerTest::SetUpTestCase()
{
    hpaeSwitch = OHOS::system::GetParameter("debug.graphic.hpae.blur.enabled", "0");
    hpaeAaeSwitch = OHOS::system::GetParameter("rosen.graphic.hpae.blur.aae.enabled", "0");
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", "1");
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", "1");
}
void RSHpaeFfrtPatternManagerTest::TearDownTestCase()
{
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", hpaeSwitch);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", hpaeAaeSwitch);
}
void RSHpaeFfrtPatternManagerTest::SetUp() {}
void RSHpaeFfrtPatternManagerTest::TearDown() {}

/**
 * @tc.name: UpdatedTest
 * @tc.desc: Verify function IsUpdated/SetUpdatedFlag/ResetUpdatedFlag/SetThreadId/IsThreadIdMatch
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, UpdatedTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    ffrtManager.SetThreadId();
    ffrtManager.SetUpdatedFlag();
    bool updated = ffrtManager.IsUpdated();
    EXPECT_TRUE(updated);

    ffrtManager.ResetUpdatedFlag();
    updated = ffrtManager.IsUpdated();
    EXPECT_FALSE(updated);
}

/**
 * @tc.name: MHCRequestEGraphTest
 * @tc.desc: Verify function MHCRequestEGraph
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCRequestEGraphTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    bool ret = ffrtManager.MHCRequestEGraph(0);
    EXPECT_TRUE(ret);

    ffrtManager.g_instance = nullptr;
    ret = ffrtManager.MHCRequestEGraph(0);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MHCWaitTest
 * @tc.desc: Verify function MHCWait
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCWaitTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    bool ret = ffrtManager.MHCWait(0, MHC_PatternTaskName::BLUR_GPU0);
    EXPECT_TRUE(ret);

    ffrtManager.g_instance = nullptr;
    ret = ffrtManager.MHCWait(0, MHC_PatternTaskName::BLUR_GPU0);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MHCGetVulkanTaskWaitEventTest
 * @tc.desc: Verify function MHCGetVulkanTaskWaitEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCGetVulkanTaskWaitEventTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    uint16_t ret = ffrtManager.MHCGetVulkanTaskWaitEvent(0, MHC_PatternTaskName::BLUR_GPU0);
    EXPECT_EQ(ret, 1);

    ffrtManager.g_instance = nullptr;
    ret = ffrtManager.MHCGetVulkanTaskWaitEvent(0, MHC_PatternTaskName::BLUR_GPU0);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: MHCGetVulkanTaskNotifyEventTest
 * @tc.desc: Verify function MHCGetVulkanTaskNotifyEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCGetVulkanTaskNotifyEventTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    uint16_t ret = ffrtManager.MHCGetVulkanTaskNotifyEvent(0, MHC_PatternTaskName::BLUR_GPU0);
    EXPECT_EQ(ret, 2);

    ffrtManager.g_instance = nullptr;
    ret = ffrtManager.MHCGetVulkanTaskNotifyEvent(0, MHC_PatternTaskName::BLUR_GPU0);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: MHCReleaseEGraphTest
 * @tc.desc: Verify function MHCReleaseEGraph
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCReleaseEGraphTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    bool ret = ffrtManager.MHCReleaseEGraph(0);
    EXPECT_TRUE(ret);

    ffrtManager.g_instance = nullptr;
    ret = ffrtManager.MHCReleaseEGraph(0);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MHCReleaseAllTest
 * @tc.desc: Verify function MHCReleaseAll
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCReleaseAllTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    ffrtManager.MHCReleaseAll();
    EXPECT_NE(ffrtManager.g_instance, nullptr);

    ffrtManager.g_instance = nullptr;
    ffrtManager.MHCReleaseAll();
    EXPECT_EQ(ffrtManager.g_instance, nullptr);
}

/**
 * @tc.name: MHCSubmitTaskTest
 * @tc.desc: Verify function MHCSubmitTask
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCSubmitTaskTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    auto preFunc = [=]() {
    };
    auto afterFunc = [=]() {
    };

    bool ret = ffrtManager.MHCSubmitTask(0, MHC_PatternTaskName::BLUR_GPU0, preFunc, nullptr, 0, afterFunc);
    EXPECT_TRUE(ret);

    ffrtManager.g_instance = nullptr;
    ret = ffrtManager.MHCSubmitTask(0, MHC_PatternTaskName::BLUR_GPU0, preFunc, nullptr, 0, afterFunc);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MHCDlOpenTest
 * @tc.desc: Verify function MHCDlOpen
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCDlOpenTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    bool ret = ffrtManager.MHCDlOpen();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: MHCCheckTest
 * @tc.desc: Verify function MHCCheck
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCCheckTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    ffrtManager.g_instance = nullptr;
    bool ret = ffrtManager.MHCCheck("MHCCheck", 0);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MHCGraphPatternInitTest
 * @tc.desc: Verify function MHCGraphPatternInit
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHpaeFfrtPatternManagerTest, MHCGraphPatternInitTest, TestSize.Level1)
{
    RSHpaeFfrtPatternManager ffrtManager;
    bool ret = ffrtManager.MHCGraphPatternInit(5);
    EXPECT_TRUE(ret);
}

} // namespace Rosen
} // namespace OHOS