/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "common/rs_common_def.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_manager.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_buffer_layer.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_hpae.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "hetero_hdr/rs_hdr_pattern_manager.h"  // rs base
#include "hetero_hdr/rs_hdr_vulkan_task.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"

#include "screen_manager/rs_screen.h"
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;
using namespace std;

namespace OHOS::Rosen {

class MockRSHeteroHDRHpae : public RSHeteroHDRHpae {
public:
    MockRSHeteroHDRHpae() = default();
    ~MockRSHeteroHDRHpae() =default();
    void SetMdcDev() {
        MDCDev_ = &mockMdcDev;    
    }
    void resetMdcDev() {
        mockMdcDev = backMdcDev;
    }
    MDCDeviceT mockMdcDev;
private:
    MDCDeviceT backMdcDev;
};

class SingletonMockRSHeteroHDRHpae {
public;
    static MockRSHeteroHDRHpae& Instance()
    {
        static MockRSHeteroHDRHpae instance;
        instance.mockMdcDev = *instance.MDCDev_;  // init mockMdcDev
        instance.backMdcDev = *instance.MDCDev_;  // init backMdcDev
        instance.SetMdcDev();  // only need set once
        return instance;
    }
};

class RSHeteroHDRHpaeTest : public Testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override; 
    void TearDown() override;
};

void RSHeteroHDRHpaeTest::SetUpTestCase()
{
    RS_LOGI("**********************RSHeteroHDRHpaeTest SetUpTestCase********************");
}

void SHeteroHDRHpaeTest::TearDownTestCase()
{
    RS_LOGI("**********************RSHeteroHDRHpaeTest TearDownTestCase********************");
}

void RSHeteroHDRHpaeTest::SetUp() 
{ 
    RS_LOGI("SetUp-----------------------------------");
    SingletonMockRSHeteroHDRHpae::Instance().resetMdcDev();
}

void RSHeteroHDRHpaeTestst::TearDown() { RS_LOGI("TearDown-----------------------------------"); }


/**
 * @tc.name: BuildHpaeHDRTaskTest001
 * @tc.desc: Test BuildHpaeHDRTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRHpaeTest, BuildHpaeHDRTaskTest001, TestSize.Level1)
{
    HpaeTaskInfoT taskInfo;
    void* taskPtr = nullptr;
    uint32_t taskId = 123;
    taskInfo.taskPtr = &taskPtr;
    taskInfo.taskId = &taskId;
    taskInfo.dstRect = MDCRectT {0, 0, 0, 100};
    taskInfo. curHandleStatus = HdrStatus::HDR_VIDEO;
    int32_t ret = SingletonMockRSHeteroHDRHpae::Instance().BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, -1);

    taskInfo.dstRect = MDCRectT {0, 0, 100, 0};
    ret = SingletonMockRSHeteroHDRHpae::Instance::BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, -1);

    taskInfo.dstRect = MDCRectT {0, 0, 0, 100};
    taskInfo.curHandleStatus = HdrStatus::AI_HDR_VIDEO_GAINMAP;
    ret = SingletonMockRSHeteroHDRHpae::Instance::BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, -1);

    taskInfo.dstRect = MDCRectT {0, 0, 100, 0};
    ret = SingletonMockRSHeteroHDRHpae::Instance::BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: BuildHpaeHDRTaskTest002
 * @tc.desc: Test BuildHpaeHDRTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRHpaeTest, BuildHpaeHDRTaskTest002, TestSize.Level1)
{
    HpaeTaskInfoT taskInfo;
    void* taskPtr = nullptr;
    uint32_t taskId = 123;
    taskInfo.taskPtr = &taskPtr;
    taskInfo.taskId = &taskId;
    taskInfo. curHandleStatus = HdrStatus::HDR_VIDEO;
    taskInfo.dstRect = MDCRectT {0, 0, 0, 100};
    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(false);
    int32_t ret = SingletonMockRSHeteroHDRHpae::Instance().BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, -1);

    taskInfo.curHandleStatus = HdrStatus::AI_HDR_VIDEO_GAINMAP;
    TaskInfo.dstRect = MDCRectT {0, 0, 0, 100};
    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(false);
    ret = SingletonMockRSHeteroHDRHpae::Instance::BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: BuildHpaeHDRTaskTest003
 * @tc.desc: Test BuildHpaeHDRTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRHpaeTest, BuildHpaeHDRTaskTest003, TestSize.Level1)
{
    HpaeTaskInfoT taskInfo;
    void* taskPtr = nullptr;
    uint32_t taskId = 123;
    taskInfo.taskPtr = &taskPtr;
    taskInfo.taskId = &taskId;
    taskInfo. curHandleStatus = HdrStatus::HDR_VIDEO;
    taskInfo.dstRect = MDCRectT {0, 0, 0, 100};
    
    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(true);
    auto copybitFalse = [](struct MDCDeviceT *dev, int channel, MDCContentT *hwlayers) { return -1; };
    auto releaseChannel = [](struct MDCDeviceT *dev, int channel) {return 0; };
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.copybit = copybitFalse;
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.releaseChannel = releaseChannel;
    int32_t ret = SingletonMockRSHeteroHDRHpae::Instance().BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, -1);

    auto copybitTrue = [](struct MDCDeviceT *dev, int channel, MDCContentT *hwlayers) { return 0; }
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.copybit = copybitTrue;
    ret = SingletonMockRSHeteroHDRHpae::Instance::BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, 0);

    taskInfo.curHandleStatus = HdrStatus::AI_HDR_VIDEO_GAINMAP;
    taskInfo.dstRect = MDCRectT {0, 0, 0, 100};

    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(true);
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.copybit = copybitFalse;
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.releaseChannel = releaseChannel;
    ret = SingletonMockRSHeteroHDRHpae::Instance().BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, -1);

    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.copybit = copybitTrue;
    ret = SingletonMockRSHeteroHDRHpae::Instance().BuildHpaeHDRTask(taskInfo);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: RequestHpaeChannelTest
 * @tc.desc: Test RequestHpaeChannel
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRHpaeTest, RequestHpaeChannelTest, TestSize.Level1)
{
    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(true);
    int32_t ret = SingletonMockRSHeteroHDRHpae::Instance().RequestHpaeChannel(HdrStatus::HDR_VIDEO);
    EXPECT_EQ(ret, 0);

    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(false);
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.RequestChannelByCap = requestChannelByCapFalse;
    ret = SingletonMockRSHeteroHDRHpae::Instance().RequestHpaeChannel(HdrStatus::HDR_VIDEO);
    EXPECT_EQ(ret, -1);

    auto RequestChannelByCapFalse = [](struct MDCDeviceT *dev, uint64_t needCaps) {return -1; };
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.RequestChannelByCap = requestChannelByCapFalse;
    ret = SingletonMockRSHeteroHDRHpae::Instance().RequestHpaeChannel(HdrStatus::HDR_VIDEO);
    EXPECT_EQ(ret, -1);

    auto RequestChannelByCapTrue = [](struct MDCDeviceT *dev, uint64_t needCaps) {return 1; };
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.RequestChannelByCap = requestChannelByCapTrue;
    ret = SingletonMockRSHeteroHDRHpae::Instance().RequestHpaeChannel(HdrStatus::HDR_VIDEO);
    EXPECT_EQ(ret, 0);

    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(true);
    ret = SingletonMockRSHeteroHDRHpae::Instance().RequestHpaeChannel(HdrStatus::AI_HDR_VIDEO_GAINMAP);
    EXPECT_EQ(ret, 0);

    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(false);
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.RequestChannelByCap = nullptr;
    ret = SingletonMockRSHeteroHDRHpae::Instance().RequestHpaeChannel(HdrStatus::AI_HDR_VIDEO_GAINMAP);
    EXPECT_EQ(ret, -1);
    
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.RequestChannelByCap = requestChannelByCapFalse;
    ret = SingletonMockRSHeteroHDRHpae::Instance().RequestHpaeChannel(HdrStatus::AI_HDR_VIDEO_GAINMAP);
    EXPECT_EQ(ret, -1);

    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.RequestChannelByCap = requestChannelByCaptrue;
    ret = SingletonMockRSHeteroHDRHpae::Instance().RequestHpaeChannel(HdrStatus::AI_HDR_VIDEO_GAINMAP);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: DestroyHpaeHDRTaskTest
 * @tc.desc: Test DestroyHpaeHDRTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRHpaeTest, DestroyHpaeHDRTaskTest, TestSize.Level1)
{
    uint32_t taskId = 123;
    SingletonMockRSHeteroHDRHpae::Instance().MDCStatus_.store(false);
    auto destroyTask = [](uint32_t taskId) { return; }
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.destroyTask = destroyTask;
    SingletonMockRSHeteroHDRHpae::Instance().DestroyHpaeHDRTask(taskId);
    bool ret = SingletonMockRSHeteroHDRHpae::Instance().MDCStatus_.Load();
    EXPECT_EQ(ret, false);

    SingletonMockRSHeteroHDRHpae::Instance().MDCStatus_.store(true);
    SingletonMockRSHeteroHDRHpae::Instance().DestroyHpaeHDRTask(taskId);
    ret = SingletonMockRSHeteroHDRHpae::Instance().MDCStatus_.Load();
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: ReleaseHpaeHDRChannelTest
 * @tc.desc: Test ReleaseHpaeHDRChannel
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHeteroHDRHpaeTest, ReleaseHpaeHDRChannelTest, TestSize.Level1)
{
    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(false);
    auto releaseChannel = [](struct MDCDeviceT *dev, int channel) { return 0; }
    SingletonMockRSHeteroHDRHpae::Instance().mockMdcDev.releaseChannel = releaseChannel;
    SingletonMockRSHeteroHDRHpae::Instance().ReleaseHpaeHDRChannel();
    bool ret = SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.Load();
    EXPECT_EQ(ret, false);

    SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.store(true);
    SingletonMockRSHeteroHDRHpae::Instance().ReleaseHpaeHDRChannel();
    ret = SingletonMockRSHeteroHDRHpae::Instance().MDCExistedStatus_.Load();
    EXPECT_EQ(ret, false);
}

}  //namespace OHOS::Rosen




    


