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
#include "hpae_base/rs_hpae_hianimation.h"
#include "hpae_base/rs_hpae_base_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpaeHianimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::string hpaeSwitch;
    static inline std::string hpaeAaeSwitch;

    static inline hianimation_algo_device_t *mockHianimationDevice_;
};

static constexpr int32_t HIANIMATION_SUCC = 0;
static constexpr int32_t HIANIMATION_FAIL = -1;

bool MockHianimationInputCheck(const struct BlurImgParam *imgInfo, const struct HaeNoiseValue *noisePara)
{
    return true;
}
int32_t MockHianimationAlgoInit(uint32_t imgWeight, uint32_t imgHeight, float maxSigma, uint32_t format)
{
    return HIANIMATION_SUCC;
}
int32_t MockHianimationAlgoDeInit()
{
    return HIANIMATION_SUCC;
}
int32_t MockHianimationBuildTask(const struct HaeBlurBasicAttr *basicInfo,
    const struct HaeBlurEffectAttr *effectInfo, uint32_t *outTaskId, void **outTaskPtr)
{
    return HIANIMATION_SUCC;
}
int32_t MockHianimationBuildTaskFail(const struct HaeBlurBasicAttr *basicInfo,
    const struct HaeBlurEffectAttr *effectInfo, uint32_t *outTaskId, void **outTaskPtr)
{
    return HIANIMATION_FAIL;
}
int32_t MockHianimationSyncProcess(const struct HaeBlurBasicAttr *basicInfo,
    const struct HaeBlurEffectAttr *effectInfo)
{
    return HIANIMATION_SUCC;
}
int32_t MockHianimationDestroyTask(uint32_t taskId)
{
    return HIANIMATION_SUCC;
}

void RSHpaeHianimationTest::SetUpTestCase()
{
    mockHianimationDevice_ = new hianimation_algo_device_t;
    mockHianimationDevice_->hianimationInputCheck = MockHianimationInputCheck;
    mockHianimationDevice_->hianimationAlgoInit = MockHianimationAlgoInit;
    mockHianimationDevice_->hianimationAlgoDeInit = MockHianimationAlgoDeInit;
    mockHianimationDevice_->hianimationBuildTask = MockHianimationBuildTask;
    mockHianimationDevice_->hianimationSyncProcess = MockHianimationSyncProcess;
    mockHianimationDevice_->hianimationDestroyTask = MockHianimationDestroyTask;

    hpaeSwitch = OHOS::system::GetParameter("debug.graphic.hpae.blur.enabled", "0");
    hpaeAaeSwitch = OHOS::system::GetParameter("rosen.graphic.hpae.blur.aae.enabled", "0");
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", "1");
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", "1");
}
void RSHpaeHianimationTest::TearDownTestCase()
{
    delete mockHianimationDevice_;
    mockHianimationDevice_ = nullptr;
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", hpaeSwitch);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", hpaeAaeSwitch);
}
void RSHpaeHianimationTest::SetUp() {}
void RSHpaeHianimationTest::TearDown() {}

/**
 * @tc.name: OpenDeviceTest
 * @tc.desc: Verify function OpenDevice
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, OpenDeviceTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.openFailNum_ = 10;
    hianimationManager.OpenDevice();
    EXPECT_EQ(hianimationManager.hianimationDevice_, nullptr);

    hianimationManager.openFailNum_ = 0;
    hianimationManager.libHandle_ = reinterpret_cast<void *>(0x1234);
    hianimationManager.OpenDevice();
    EXPECT_NE(hianimationManager.hianimationDevice_, nullptr);

    hianimationManager.OpenDevice();
    EXPECT_NE(hianimationManager.hianimationDevice_, nullptr);
}

/**
 * @tc.name: HianimationInputCheckTest
 * @tc.desc: Verify function HianimationInputCheck
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, HianimationInputCheckTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.hianimationDevice_ = mockHianimationDevice_;
    BlurImgParam imgInfo;
    HaeNoiseValue noisePara;
    bool ret = hianimationManager.HianimationInputCheck(&imgInfo, &noisePara);
    EXPECT_TRUE(ret);

    hianimationManager.hianimationDevice_ = nullptr;
    ret = hianimationManager.HianimationInputCheck(&imgInfo, &noisePara);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: HianimationAlgoInitTest
 * @tc.desc: Verify function HianimationAlgoInit
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, HianimationAlgoInitTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.hianimationDevice_ = mockHianimationDevice_;
    hianimationManager.taskIdMap_.insert(0);
    int32_t ret = hianimationManager.HianimationAlgoInit(100, 100, 1.0, 0);
    EXPECT_EQ(ret, HIANIMATION_SUCC);

    hianimationManager.hianimationDevice_ = nullptr;
    ret = hianimationManager.HianimationAlgoInit(100, 100, 1.0, 0);
    EXPECT_EQ(ret, HIANIMATION_FAIL);
}

/**
 * @tc.name: HianimationAlgoDeInitTest
 * @tc.desc: Verify function HianimationAlgoDeInit
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, HianimationAlgoDeInitTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.hianimationDevice_ = mockHianimationDevice_;
    hianimationManager.taskIdMap_.insert(0);
    int32_t ret = hianimationManager.HianimationAlgoDeInit();
    EXPECT_EQ(ret, HIANIMATION_SUCC);

    hianimationManager.hianimationDevice_ = nullptr;
    ret = hianimationManager.HianimationAlgoDeInit();
    EXPECT_EQ(ret, HIANIMATION_FAIL);
}

/**
 * @tc.name: HianimationBuildTaskTest
 * @tc.desc: Verify function HianimationBuildTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, HianimationBuildTaskTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.hianimationDevice_ = mockHianimationDevice_;
    HaeBlurBasicAttr basicInfo;
    HaeBlurEffectAttr effectInfo;
    uint32_t taskId = 0;
    void *taskPtr = nullptr;
    int32_t ret = hianimationManager.HianimationBuildTask(&basicInfo, &effectInfo, &taskId, &taskPtr);
    EXPECT_EQ(ret, HIANIMATION_SUCC);

    ret = hianimationManager.HianimationBuildTask(&basicInfo, &effectInfo, nullptr, &taskPtr);
    EXPECT_EQ(ret, HIANIMATION_SUCC);

    hianimationManager.hianimationDevice_->hianimationBuildTask = MockHianimationBuildTaskFail;
    ret = hianimationManager.HianimationBuildTask(&basicInfo, &effectInfo, &taskId, &taskPtr);
    EXPECT_EQ(ret, HIANIMATION_FAIL);

    ret = hianimationManager.HianimationBuildTask(&basicInfo, &effectInfo, nullptr, &taskPtr);
    EXPECT_EQ(ret, HIANIMATION_FAIL);

    hianimationManager.hianimationDevice_ = nullptr;
    ret = hianimationManager.HianimationBuildTask(&basicInfo, &effectInfo, &taskId, &taskPtr);
    EXPECT_EQ(ret, HIANIMATION_FAIL);
}

/**
 * @tc.name: HianimationDestroyTaskTest
 * @tc.desc: Verify function HianimationDestroyTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, HianimationDestroyTaskTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.hianimationDevice_ = mockHianimationDevice_;
    hianimationManager.taskIdMap_.insert(0);
    int32_t ret = hianimationManager.HianimationDestroyTask(0);
    EXPECT_EQ(ret, HIANIMATION_SUCC);

    hianimationManager.hianimationDevice_ = nullptr;
    ret = hianimationManager.HianimationDestroyTask(0);
    EXPECT_EQ(ret, HIANIMATION_FAIL);
}

/**
 * @tc.name: HianimationInvalidTest
 * @tc.desc: Verify function HianimationInvalid
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, HianimationInvalidTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    bool ret = hianimationManager.HianimationInvalid();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: HianimationDestroyTaskAndNotifyTest
 * @tc.desc: Verify function HianimationDestroyTaskAndNotify
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, HianimationDestroyTaskAndNotifyTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.hianimationDevice_ = mockHianimationDevice_;
    hianimationManager.taskIdMap_.insert(0);
    int32_t ret = hianimationManager.HianimationDestroyTaskAndNotify(0);
    EXPECT_EQ(ret, HIANIMATION_SUCC);

    hianimationManager.hianimationDevice_ = nullptr;
    ret = hianimationManager.HianimationDestroyTaskAndNotify(0);
    EXPECT_EQ(ret, HIANIMATION_FAIL);
}

/**
 * @tc.name: WaitPreviousTaskTest
 * @tc.desc: Verify function WaitPreviousTask
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, WaitPreviousTaskTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    bool ret = hianimationManager.WaitPreviousTask();
    EXPECT_TRUE(ret);

    hianimationManager.taskIdMap_.insert(0);
    hianimationManager.taskIdMap_.insert(1);
    hianimationManager.taskIdMap_.insert(2);
    ret = hianimationManager.WaitPreviousTask();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: WaitAllTaskDoneTest
 * @tc.desc: Verify function WaitAllTaskDone
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, WaitAllTaskDoneTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    bool ret = hianimationManager.WaitAllTaskDone();
    EXPECT_TRUE(ret);

    hianimationManager.taskIdMap_.insert(0);
    ret = hianimationManager.WaitAllTaskDone();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: WaitAlgoInitTest
 * @tc.desc: Verify function WaitAlgoInit
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, WaitAlgoInitTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.algoInitDone_ = false;
    hianimationManager.WaitAlgoInit();
    EXPECT_FALSE(hianimationManager.algoInitDone_);
}

/**
 * @tc.name: HianimationPerfTrackTest
 * @tc.desc: Verify function HianimationPerfTrack
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, HianimationPerfTrackTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.HianimationPerfTrack();
    EXPECT_FALSE(hianimationManager.hpaePerfDone_);
}

/**
 * @tc.name: WaitHpaeDoneTest
 * @tc.desc: Verify function WaitHpaeDone
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeHianimationTest, WaitHpaeDoneTest, TestSize.Level1)
{
    HianimationManager hianimationManager;
    hianimationManager.hpaePerfDone_ = false;
    hianimationManager.WaitHpaeDone();
    EXPECT_FALSE(hianimationManager.hpaePerfDone_);

    hianimationManager.hpaePerfDone_ = true;
    hianimationManager.WaitHpaeDone();
    EXPECT_TRUE(hianimationManager.hpaePerfDone_);
}

} // namespace Rosen
} // namespace OHOS