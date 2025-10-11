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
    void SetMhcDev()
    {
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
    SingletonMockRSHDRPatternManager::Instance().MHCDlOpen();

    uint64_t frameId = 1;
    SingletonMockRSHDRPatternManager::Instance().MHCRequestEGraph(frameId);
    SingletonMockRSHDRPatternManager::Instance().MHCSubmitHDRTask(frameId, MHC_PATTERN_TASK_HDR_HPAE,
        nullptr, nullptr, nullptr);
    SingletonMockRSHDRPatternManager::Instance().MHCSubmitVulkanTask(frameId, MHC_PATTERN_TASK_HDR_HPAE,
        nullptr, nullptr);
    SingletonMockRSHDRPatternManager::Instance().MHCWait(frameId, MHC_PATTERN_TASK_HDR_HPAE);
    SingletonMockRSHDRPatternManager::Instance().MHCGetVulkanTaskWaitEvent(frameId, MHC_PATTERN_TASK_HDR_HPAE);
    SingletonMockRSHDRPatternManager::Instance().MHCGetVulkanTaskNotifyEvent(frameId, MHC_PATTERN_TASK_HDR_HPAE);
    SingletonMockRSHDRPatternManager::Instance().MHCReleaseEGraph(frameId);
    SingletonMockRSHDRPatternManager::Instance().MHCReleaseAll();
    SingletonMockRSHDRPatternManager::Instance().MHCCheck("ut test");

    SingletonMockRSHDRPatternManager::Instance().MHCResetCurFrameId();
    EXPECT_EQ(SingletonMockRSHDRPatternManager::Instance().lastFrameId_, 0);
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
 * @tc.name: MHCGraphQueryTaskErrorTest
 * @tc.desc: Test MHCGraphQueryTaskError
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHDRPatternManagerTest, MHCGraphQueryTaskErrorTest, TestSize.Level1)
{
    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = reinterpret_cast<void*>(0x1234);
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCCheck("query task test");
    auto graphPatternHpaeTaskExecutionQuery = [] (void*, uint64_t, MHCPatternTaskName) { return 0; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternHpaeTaskExecutionQuery =
        graphPatternHpaeTaskExecutionQuery;
    SingletonMockRSHDRPatternManager::Instance().MHCGraphQueryTaskError(1, MHC_PATTERN_TASK_HDR_HPAE);
    EXPECT_EQ(ret, true);

    bool ret2 = SingletonMockRSHDRPatternManager::Instance().MHCCheck("query task test2");
    auto graphPatternHpaeTaskExecutionQuery2 = [] (void*, uint64_t, MHCPatternTaskName) { return -1; };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternHpaeTaskExecutionQuery =
        graphPatternHpaeTaskExecutionQuery2;
    SingletonMockRSHDRPatternManager::Instance().MHCGraphQueryTaskError(1, MHC_PATTERN_TASK_HDR_HPAE);
    EXPECT_EQ(ret2, true);

    SingletonMockRSHDRPatternManager::Instance().graphPatternInstance_ = nullptr;
    bool ret3 = SingletonMockRSHDRPatternManager::Instance().MHCCheck("query task test3");
    SingletonMockRSHDRPatternManager::Instance().MHCGraphQueryTaskError(1, MHC_PATTERN_TASK_HDR_HPAE);
    EXPECT_EQ(ret3, false);
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
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternAnimationTaskSubmit =
        graphPatternAnimationTaskSubmit;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCSubmitHDRTask(0,
        MHC_PATTERN_TASK_HDR_HPAE, nullptr, nullptr, nullptr);
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
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternVulkanTaskSubmit =
        graphPatternVulkanTaskSubmit;
    bool ret = SingletonMockRSHDRPatternManager::Instance().MHCSubmitVulkanTask(0,
        MHC_PATTERN_TASK_HDR_HPAE, func, nullptr);
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

    auto graphPatternGetVulkanWaitEvent = [](void*, uint64_t, MHCPatternTaskName) {
        return static_cast<uint16_t >(123); };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternGetVulkanWaitEvent =
        graphPatternGetVulkanWaitEvent;
    uint16_t ret = SingletonMockRSHDRPatternManager::Instance().MHCGetVulkanTaskWaitEvent(0,
        MHC_PATTERN_TASK_HDR_HPAE);
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

    auto graphPatternGetVulkanNotifyEvent = [](void*, uint64_t, MHCPatternTaskName) {
        return static_cast<uint16_t >(123); };
    SingletonMockRSHDRPatternManager::Instance().MHCDevice_->graphPatternGetVulkanNotifyEvent =
        graphPatternGetVulkanNotifyEvent;
    uint16_t ret = SingletonMockRSHDRPatternManager::Instance().MHCGetVulkanTaskNotifyEvent(0,
        MHC_PATTERN_TASK_HDR_HPAE);
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
 * @tc.name: SetThreadIdTest
 * @tc.desc: etThreadId
 * @tc.type: FUNC
 * @tc.require:
 */
 HWTEST_F(RSHDRPatternManagerTest, SetThreadIdTest, TestSize.Level1)
 {
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    MHCDevice_->graphPatternRequestEGraph = nullptr;
    auto tempSet = SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_;
    SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_.clear();

    // test 1
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = true;
    SingletonMockRSHDRPatternManager::Instance().SetThreadId(canvas);
    EXPECT_EQ(SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_.size(), 0);

    // test 2
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = false;
    SingletonMockRSHDRPatternManager::Instance().flushedBuffer_ = false;
    SingletonMockRSHDRPatternManager::Instance().SetThreadId(canvas);
    EXPECT_EQ(SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_.size(), 0);

    // test 3
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = false;
    SingletonMockRSHDRPatternManager::Instance().flushedBuffer_ = true;
    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = false;
    SingletonMockRSHDRPatternManager::Instance().curFrameId_ = 0;
    SingletonMockRSHDRPatternManager::Instance().SetThreadId(canvas);
    EXPECT_EQ(SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_.size(), 0);

    // test 4
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = false;
    SingletonMockRSHDRPatternManager::Instance().flushedBuffer_ = true;
    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = true;
    SingletonMockRSHDRPatternManager::Instance().curFrameId_ = 0;
    SingletonMockRSHDRPatternManager::Instance().SetThreadId(canvas);
    EXPECT_EQ(SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_.size(), 0);

    // test 5
    SingletonMockRSHDRPatternManager::Instance().processConsumed_ = false;
    SingletonMockRSHDRPatternManager::Instance().flushedBuffer_ = true;
    SingletonMockRSHDRPatternManager::Instance().curFrameIdUsed_ = false;
    SingletonMockRSHDRPatternManager::Instance().curFrameId_ = 1;
    SingletonMockRSHDRPatternManager::Instance().SetThreadId(canvas);
    EXPECT_EQ(SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_.size(), 1);

    SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_ = tempSet;
}

/**
 * @tc.name: MHCGetFrameIdForGPUTaskTest_Branch1
 * @tc.desc: Test MHCGetFrameIdForGPUTask branch 1 scenarios
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHDRPatternManagerTest, MHCGetFrameIdForGPUTest_Branch1, TestSize.Level1)
{
    auto& instance = SingletonMockRSHDRPatternManager::Instance();
    
    // Test branch 1 scenarios
    instance.lastFrameIdUsed_ = false;
    instance.curFrameIdUsed_ = false;
    instance.lastFrameConsumed_ = false;
    instance.processConsumed_ = false;
    
    auto ret = instance.MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
    
    instance.lastFrameIdUsed_ = true;
    instance.curFrameIdUsed_ = false;
    ret = instance.MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
    
    instance.lastFrameIdUsed_ = false;
    instance.curFrameIdUsed_ = true;
    ret = instance.MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
    
    instance.lastFrameIdUsed_ = true;
    instance.curFrameIdUsed_ = true;
    ret = instance.MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
}

/**
 * @tc.name: MHCGetFrameIdForGPUTaskTest_Branch2
 * @tc.desc: Test MHCGetFrameIdForGPUTask branch 2 scenarios
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHDRPatternManagerTest, MHCGetFrameIdForGPUTest_Branch2, TestSize.Level1)
{
    auto& instance = SingletonMockRSHDRPatternManager::Instance();
    
    // Test branch 2 scenarios
    instance.curFrameIdUsed_ = false;  // bypass branch 1
    instance.lastFrameIdUsed_ = false;
    instance.lastFrameConsumed_ = false;
    
    auto ret = instance.MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
    
    instance.lastFrameIdUsed_ = false;
    instance.lastFrameConsumed_ = true;
    ret = instance.MHCGetFrameIdForGPUTask();
    
    instance.lastFrameIdUsed_ = true;
    instance.lastFrameConsumed_ = false;
    ret = instance.MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
    
    instance.lastFrameIdUsed_ = true;
    instance.lastFrameConsumed_ = true;
    ret = instance.MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
}

/**
 * @tc.name: MHCGetFrameIdForGPUTaskTest_Branch3
 * @tc.desc: Test MHCGetFrameIdForGPUTask branch 3 scenarios
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHDRPatternManagerTest, MHCGetFrameIdForGPUTest_Branch3, TestSize.Level1)
{
    auto& instance = SingletonMockRSHDRPatternManager::Instance();
    
    // Test branch 3 scenarios
    instance.lastFrameIdUsed_ = false;  // bypass branch 1
    instance.lastFrameConsumed_ = false;  // bypass branch 2
    
    instance.curFrameIdUsed_ = false;
    instance.processConsumed_ = false;
    auto ret = instance.MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
    
    instance.curFrameIdUsed_ = true;
    instance.processConsumed_ = false;
    ret = instance.MHCGetFrameIdForGPUTask();
    EXPECT_EQ(ret.size(), 0);
    
    instance.curFrameIdUsed_ = false;
    instance.processConsumed_ = true;
    instance.tid_ = gettid();
    ret = instance.MHCGetFrameIdForGPUTask();
    
    instance.curFrameIdUsed_ = true;
    instance.processConsumed_ = true;
    instance.tid_ = gettid();
    ret = instance.MHCGetFrameIdForGPUTask();
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

    auto waitSemaphoreSetTemp = SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_;
    SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_.clear();
    // MHCCheckWaitSemaphoreSet false
    std::vector<GrBackendSemaphore> semaphoreVec2 = {backendSemaphore};
    RSHDRVulkanTask::PrepareHDRSemaphoreVector(semaphoreVec2, surface, frameIdVec);
    // MHCCheckWaitSemaphoreSet true
    uint64_t frameId1 = 1;
    std::vector<GrBackendSemaphore> semaphoreVec3 = {backendSemaphore};
    SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_.insert(frameId1);
    RSHDRVulkanTask::PrepareHDRSemaphoreVector(semaphoreVec3, surface, frameIdVec);
    SingletonMockRSHDRPatternManager::Instance().waitSemaphoreSet_. = waitSemaphoreSetTemp;

    uint64_t frameId = 0;
    RSHDRVulkanTask::SubmitWaitEventToGPU(frameId);
}

/**
 * @tc.name: IsInterfaceTypeBasicRenderTest
 * @tc.desc: Test IsInterfaceTypeBasicRender
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHDRPatternManagerTest, IsInterfaceTypeBasicRenderTest, TestSize.Level1)
{
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    auto interfaceTypeTmp = RsVulkanContext::GetSingleton().vulkanInterfaceType_;

    auto ret = RSHDRVulkanTask::IsInterfaceTypeBasicRender();
    SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_NE(ret, 2);


    RsVulkanContext::GetSingleton().SetIsProtected(false);
    auto ret1 = RSHDRVulkanTask::IsInterfaceTypeBasicRender();
    SingletonMockRSHDRPatternManager::Instance().MHCGetFrameIdForGPUTask();
    EXPECT_NE(ret1, 2);
    RsVulkanContext::GetSingleton().vulkanInterfaceType_ = interfaceTypeTmp;
}

/**
 * @tc.name: MHCSubmitGPUTaskTest
 * @tc.desc: Test MHCSubmitGPUTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHDRPatternManagerTest, MHCSubmitGPUTaskTest, TestSize.Level1)
{
    auto tmpFunc = SingletonMockRSHDRPatternManager::Instance().submitFuncs_;
    SingletonMockRSHDRPatternManager::Instance().submitFuncs_.clear();
    VkSemaphore waitSemaphore1 = {};
    std::function<void()> func = []() { return; };
    VkSemaphore waitSemaphores = { waitSemaphore1 };
    VkSubmitInfo submitInfo1 = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, 1, waitSemaphores, nullptr, 0, nullptr, 0, nullptr
    };
    // cannot process func
    SingletonMockRSHDRPatternManager::Instance().MHCSubmitGPUTask(0, &submitInfo1);
    EXPECT_EQ(SingletonMockRSHDRPatternManager::Instance().submitFuncs_.size(), 0);

    // traverse cannot process func
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
    auto& vkContext = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    VkSemaphore waitSemaphore = vkContext.RequireSemaphore();
    void* key = (void*)waitSemaphore;
    SingletonMockRSHDRPatternManager::Instance().submitFuncs_[key] = func;
    SingletonMockRSHDRPatternManager::Instance().MHCSubmitGPUTask(1, &submitInfo1);
    EXPECT_EQ(SingletonMockRSHDRPatternManager::Instance().submitFuncs_.size(), 1);

    // process func
    VkSemaphore waitSemaphores[1] = { waitSemaphore };
    VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, 1, waitSemaphores, nullptr, 0, nullptr, 0, nullptr
    };
    
    VkSubmitInfo submitInfos[1] = { submitInfo };
    SingletonMockRSHDRPatternManager::Instance().MHCSubmitGPUTask(1, submitInfos);
    EXPECT_EQ(SingletonMockRSHDRPatternManager::Instance().submitFuncs_.size(), 0);

    SingletonMockRSHDRPatternManager::Instance().submitFuncs_ = tmpFunc;
}
} // namespace OHOS::Rosen