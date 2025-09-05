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

#include "common/rs_common_def.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_buffer_layer.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_hpae.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_manager.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_util.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "hetero_hdr/rs_hdr_pattern_manager.h"  // rs base
#include "hetero_hdr/rs_hdr_vulkan_task.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "screen_manager/rs_screen.h"
#include "transaction/rs_interfaces.h"

#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/vk/GrVkBackendSurface.h"
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"
#else
#include "include/gpu/GrBackendSurface.h"
#endif

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;
using namespace std;

namespace {
    constexpr int GRAPH_NUM = 2;
}

namespace OHOS::Rosen {

class MockRSHDRPatternManager : public RSHDRPatternManager {
public:
    MockRSHDRPatternManager() = default;
    ~MockRSHDRPatternManager() = default;
    void SetMhcDev() {
        MHCDevice_ = mockMhcDevice;
    }
    std::shared_ptr<MHCDevice> mockMhcDevice = std::make_shared<MHCDevice>();
};

// 模拟单例类的函数
class SingletonMockRSHDRPatternManager {
public:
    static MockRSHDRPatternManager& Instance()
    {
        static MockRSHDRPatternManager instance;
        instance.SetMhcDev();  // only need set once
        return instance;
    }
};

class RSHDRPatternManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHDRPatternManagerTest::SetUpTestCase()
{
    RS_LOGI("************************RSHDRPatternManagerTest SetUpTestCase************************");
    SingletonMockRSHDRPatternManager::Instance().isFinishDLOpen_ = true;
}

void RSHDRPatternManagerTest::TearDownTestCase()
{
    RS_LOGI("************************RSHDRPatternManagerTest TearDownTestCase************************");
    // last ut must set graphPatternInstance_ and MHCLibFrameworkHandle_
    auto graphPatternDestroy = [](void*) { return; };
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);
    SingletonMockRSHDRPatternManager::Instance().MHCLibFrameworkHandle_ = nullptr;
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternDestroy = graphPatternDestroy;
}

void RSHDRPatternManagerTest::SetUp() { RS_LOGI("SetUp------------------------------------"); }

void RSHDRPatternManagerTest::TearDown() { RS_LOGI("TearDown------------------------------------"); }

/**
 * @tc.name: RSHDRPatternManagerInterfaceTest
 * @tc.desc: Test RSHDRPatternManager interface
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, RSHDRPatternManagerInterfaceTest, TestSize.Level1)
{
    RSHDRPatternManager::Instance().MHCDlOpen();

    uint64_t frameId = 1;
    RSHDRPatternManager::Instance().MHCRequestEGraph(frameId);
    RSHDRPatternManager::Instance().MHCSubmitHDRTask(frameId, MHC_PATTERN_TASK_HDR_HPAE, nullptr, nullptr, nullptr);
    RSHDRPatternManager::Instance().MHCSubmitVulkanTask(frameId, MHC_PATTERN_TASK_HDR_HPAE, nullptr, nullptr);
    RSHDRPatternManager::Instance().MHCWait(frameId, MHC_PATTERN_TASK_HDR_HPAE);
    RSHDRPatternManager::Instance().MHCGetVulkanTaskWaitEvent(frameId, MHC_PATTERN_TASK_HDR_HPAE);
    RSHDRPatternManager::Instance().MHCGetVulkanTaskNotifyEvent(frameId, MHC_PATTERN_TASK_HDR_HPAE);
    RSHDRPatternManager::Instance().MHCReleaseEGraph(frameId);
    RSHDRPatternManager::Instance().MHCReleaseAll();
    RSHDRPatternManager::Instance().MHCCheck("ut test");

    RSHDRPatternManager::Instance().MHCResetCurFrameId();
    EXPECT_EQ(RSHDRPatternManager::Instance().lastFrameId_, 0);
}

/**
 * @tc.name: MHCDlOpenTest
 * @tc.desc: Test MHCDlOpen
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCDlOpenTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().isFinishDLOpen_ = true;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCDlOpen();
    EXPECT_EQ(ret, true);

    SingletonMockRSHDRPatternManager::Instance().isFinishDLOpen_ = false;
    SingletonMockRSHDRPatternManager::Instance().MHCLibFrameworkHandle_ = reinterpret_cast<void*>(0x1234);
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlOpen();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: MHCGraphPatternInitTest
 * @tc.desc: Test MHCGraphPatternInit
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCGraphPatternInitTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().isFinishDLOpen_ = false;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCGraphPatternInit(GRAPH_NUM);
    EXPECT_EQ(ret, false);

    SingletonMockRSHDRPatternManager::Instance().isFinishDLOpen_ = true;
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGraphPatternInit(GRAPH_NUM);
    EXPECT_EQ(ret, true);

    SingletonMockRSHDRPatternManager::Instance().isFinishDLOpen_ = true;
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;

    auto getGraphPatternInstance = [](PatternType, const char*) { return reinterpret_cast<void*>(0x1234); };
    auto graphPatternInit = [](void*, size_t) { return true; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->getGraphPatternInstance = getGraphPatternInstance;
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternInit = graphPatternInit;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGraphPatternInit(GRAPH_NUM);
    EXPECT_EQ(ret, true);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
}

/**
 * @tc.name: MHCRequestEGraphTest
 * @tc.desc: Test MHCRequestEGraph
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCRequestEGraphTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);

    auto graphPatternRequestEGraph = [](void*, uint64_t) { return true; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternRequestEGraph = graphPatternRequestEGraph;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCRequestEGraph(0);
    EXPECT_EQ(ret, true);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
}

/**
 * @tc.name: MHCSubmitHDRTaskTest
 * @tc.desc: Test MHCSubmitHDRTask
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCSubmitHDRTaskTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);

    auto graphPatternAnimationTaskSubmit = [](void*, uint64_t, MHCPatternTaskName, void*, void**, void*) { return; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternAnimationTaskSubmit = graphPatternAnimationTaskSubmit;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCSubmitHDRTask(0, MHC_PATTERN_TASK_HDR_HPAE, nullptr, nullptr, nullptr);
    EXPECT_EQ(ret, true);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
}

/**
 * @tc.name: MHCSubmitVulkanTaskTest
 * @tc.desc: Test MHCSubmitVulkanTask
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCSubmitVulkanTaskTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);

    auto graphPatternVulkanTaskSubmit = [](void*, uint64_t, MHCPatternTaskName, void*, void*) { return; };
    auto func = []() { return 0; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternVulkanTaskSubmit = graphPatternVulkanTaskSubmit;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCSubmitVulkanTask(0, MHC_PATTERN_TASK_HDR_HPAE, func, nullptr);
    EXPECT_EQ(ret, true);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
}

/**
 * @tc.name: MHCWaitTest
 * @tc.desc: Test MHCWait
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCWaitTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);

    auto graphPatternWait = [](void*, uint64_t, MHCPatternTaskName) { return; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternWait = graphPatternWait;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCWait(0, MHC_PATTERN_TASK_HDR_HPAE);
    EXPECT_EQ(ret, true);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
}

/**
 * @tc.name: MHCGetVulkanTaskWaitEventTest
 * @tc.desc: Test MHCGetVulkanTaskWaitEvent
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCGetVulkanTaskWaitEventTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);

    auto graphPatternGetVulkanWaitEvent = [](void*, uint64_t, MHCPatternTaskName) { return static_cast<uint16_t >(123); };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternGetVulkanWaitEvent = graphPatternGetVulkanWaitEvent;
    uint16_t ret = SingletonMockRSHDRPatternManager::Instance().MHCGetVulkanTaskWaitEvent(0, MHC_PATTERN_TASK_HDR_HPAE);
    EXPECT_EQ(ret, 123);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
}

/**
 * @tc.name: MHCGetVulkanTaskNotifyEventTest
 * @tc.desc: Test MHCGetVulkanTaskNotifyEvent
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCGetVulkanTaskNotifyEventTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);

    auto graphPatternGetVulkanNotifyEvent = [](void*, uint64_t, MHCPatternTaskName) { return static_cast<uint16_t >(123); };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternGetVulkanNotifyEvent = graphPatternGetVulkanNotifyEvent;
    uint16_t ret = SingletonMockRSHDRPatternManager::Instance().MHCGetVulkanTaskNotifyEvent(0, MHC_PATTERN_TASK_HDR_HPAE);
    EXPECT_EQ(ret, 123);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
}

/**
 * @tc.name: MHCReleaseEGraphTest
 * @tc.desc: Test MHCReleaseEGraph
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCReleaseEGraphTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);

    auto graphPatternReleaseEGraph = [](void*, uint64_t) { return true; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternReleaseEGraph = graphPatternReleaseEGraph;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCReleaseEGraph(0);
    EXPECT_EQ(ret, true);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
}

/**
 * @tc.name: MHCReleaseAllTest
 * @tc.desc: Test MHCReleaseAll
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCReleaseAllTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);

    auto graphPatternReleaseAll = [](void*) { return true; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternReleaseAll = graphPatternReleaseAll;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCReleaseAll();
    EXPECT_EQ(ret, true);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
}

/**
 * @tc.name: MHCDlsymInvalidTest
 * @tc.desc: Test MHCDlsymInvalid
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCDlsymInvalidTest, TestSize.Level1)
{
    auto graphPatternDestroy = [](void*) { return; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternDestroy = graphPatternDestroy;

    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternReleaseAll = nullptr;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternGetVulkanNotifyEvent = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternGetVulkanWaitEvent = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternWait = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternVulkanTaskSubmit = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternAnimationTaskSubmit = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternReleaseEGraph = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternRequestEGraph = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternDestroy = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternInit = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->getGraphPatternInstance = nullptr;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCDlsymInvalid();
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: MHCGetFrameIdForGPUTaskTest
 * @tc.desc: Test MHCGetFrameIdForGPUTaskTask
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, MHCGetFrameIdForGPUTaskTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().lastFrameIdUsed_ = false;
    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = false;
    SingletonMockRSHDRPatternManager::Instance().lastFrameConsumed_ = false;
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = false;
    // brach 1
    auto ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);

    SingletonMockRSHDRPatternManager::Instance().lastFrameIdUsed_ = true;
    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = false;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);

    SingletonMockRSHDRPatternManager::Instance().lastFrameIdUsed_ = false;
    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = true;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);

    SingletonMockRSHDRPatternManager::Instance().lastFrameIdUsed_ = true;
    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = true;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);

    // brach 2
    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = false;  // bypass brach 1

    SingletonMockRSHDRPatternManager::Instance().lastFrameIdUsed_ = false;
    SingletonMockRSHDRPatternManager::Instance().lastFrameConsumed_ = false;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);

    SingletonMockRSHDRPatternManager::Instance().lastFrameIdUsed_ = false;
    SingletonMockRSHDRPatternManager::Instance().lastFrameConsumed_ = true;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 1);

    SingletonMockRSHDRPatternManager::Instance().lastFrameIdUsed_ = true;
    SingletonMockRSHDRPatternManager::Instance().lastFrameConsumed_ = false;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);

    SingletonMockRSHDRPatternManager::Instance().lastFrameIdUsed_ = true;
    SingletonMockRSHDRPatternManager::Instance().lastFrameConsumed_ = true;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);

    // brach 3
    SingletonMockRSHDRPatternManager::Instance().lastFrameIdUsed_ = false;  // bypass brach 1
    SingletonMockRSHDRPatternManager::Instance().lastFrameConsumed_ = false;  // bypass brach 2
    
    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = false;
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = false;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);

    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = true;
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = false;
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);

    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = false;
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = true;
    SingletonMockRSHDRPatternManager::Instance().tid_ = gettid();
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 1);

    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = true;
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = true;
    SingletonMockRSHDRPatternManager::Instance().tid_ = gettid();
    ret = SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
}

/**
 * @tc.name: PrepareHDRSemaphoreVectorTest
 * @tc.desc: Test PrepareHDRSemaphoreVector
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHDRPatternManagerTest, PrepareHDRSemaphoreVectorTest, TestSize.Level1)
{
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);

    VkSemaphore notifySemaphore = VK_NULL_HANDLE;
#ifdef USE_M133_SKIA
    GrBackendSemaphore backendSemaphore = GrBackendSemaphores::MakeVk(notifySemaphore);
#else
    GrBackendSemaphore backendSemaphore;
    backendSemaphore.initVulkan(notifySemaphore);
#endif

    std::vector<uint64_t> frameIdVec {1, 2};
    std::vector<GrBackendSemaphore> semaphoreVec1 = {backendSemaphore};
    RSHDRVulkanTask::PrepareHDRSemaphoreVector(semaphoreVec1, surface, frameIdVec);

    auto rsSurfaceOhosVulkan = make_shared<RSSurfaceOhosVulkan>(IConsumerSurface::Create());
    std::vector<GrBackendSemaphore> semaphoreVec = {backendSemaphore};
    RSHDRVulkanTask::PrepareHDRSemaphoreVector(semaphoreVec, surface, frameIdVec);

    uint64_t frameId = 0;
    RSHDRVulkanTask::SubmitWaitEventToGPU(frameId);
}

} // namespace OHOS::Rosen