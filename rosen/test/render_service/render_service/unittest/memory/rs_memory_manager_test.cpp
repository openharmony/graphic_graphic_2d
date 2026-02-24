/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <string>
#include <fstream>
#include "gtest/gtest.h"
#include "memory/rs_memory_manager.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/main_thread/rs_main_thread.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
std::string g_logMsg;
void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
                   const char *msg)
{
    g_logMsg = msg;
}

class RSMemoryManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMemoryManagerTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    RSTestUtil::InitRenderNodeGC();
}

void RSMemoryManagerTest::TearDownTestCase()
{
    auto mainThread = RSMainThread::Instance();
    if (!mainThread || !mainThread->context_) {
        return;
    }
    auto& renderNodeMap = mainThread->context_->GetMutableNodeMap();
    renderNodeMap.renderNodeMap_.clear();
    renderNodeMap.surfaceNodeMap_.clear();
    renderNodeMap.residentSurfaceNodeMap_.clear();
    renderNodeMap.screenNodeMap_.clear();
    renderNodeMap.canvasDrawingNodeMap_.clear();
    renderNodeMap.uiExtensionSurfaceNodes_.clear();
}

void RSMemoryManagerTest::SetUp() {}
void RSMemoryManagerTest::TearDown() {}

/**
 * @tc.name: DumpMemoryUsageTest001
 * @tc.desc: Dump all memory types when input type is empty.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest001, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "";
    bool isLite = false;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("memorySnapshots") != std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest002
 * @tc.desc: Dump only RenderService memory when type is "renderservice".
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest002, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "renderservice";
    bool isLite = false;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("memorySnapshots") == std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest003
 * @tc.desc: Dump only CPU memory when type is "cpu".
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest003, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "cpu";
    bool isLite = false;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("memorySnapshots") == std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest004
 * @tc.desc: Dump only GPU memory when type is "gpu".
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest004, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "gpu";
    bool isLite = false;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("memorySnapshots") == std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest005
 * @tc.desc: Dump only memory snapshot when type is MEM_SNAPSHOT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest005, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "snapshot";
    bool isLite = false;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("memorySnapshots") != std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest006
 * @tc.desc: No memory dumped when type is invalid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest006, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "invalid_type";
    bool isLite = false;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().empty());
}

/**
 * @tc.name: DumpMemoryUsageTest007
 * @tc.desc: Dump all memory types when input type is empty.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest007, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "";
    bool isLite = true;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") != std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest008
 * @tc.desc: Dump only RenderService memory when type is "renderservice".
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest008, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "renderservice";
    bool isLite = true;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest009
 * @tc.desc: Dump only CPU memory when type is "cpu".
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest009, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "cpu";
    bool isLite = true;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest010
 * @tc.desc: Dump only GPU memory when type is "gpu".
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest010, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "gpu";
    bool isLite = true;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest011
 * @tc.desc: Dump only memory snapshot when type is MEM_SNAPSHOT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest011, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "snapshot";
    bool isLite = true;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest012
 * @tc.desc: No memory dumped when type is invalid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest012, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "invalid_type";
    bool isLite = true;
    MemoryManager::DumpMemoryUsage(log, type, isLite);
    ASSERT_TRUE(log.GetString().empty());
}

/**
 * @tc.name: ReleaseAllGpuResourceWithValidContextAndTag
 * @tc.desc: Verify ReleaseAllGpuResource logs correct trace info when context and tag are valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseAllGpuResource001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "ReleaseAllGpuResource");
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseAllGpuResource(gpuContext, tag);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseAllGpuResourceWithValidContextAndTag
 * @tc.desc: Verify ReleaseAllGpuResource logs correct trace info when context are nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseAllGpuResource002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "ReleaseAllGpuResource");
    MemoryManager::ReleaseAllGpuResource(nullptr, tag);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseAllGpuResourceWithValidContextAndTag
 * @tc.desc: Verify ReleaseAllGpuResource logs correct trace info when context and pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseAllGpuResource003, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseAllGpuResource(gpuContext, 1);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource001
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "ReleaseUnlockGpuResource");
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, tag);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource002
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "ReleaseUnlockGpuResource");
    MemoryManager::ReleaseUnlockGpuResource(nullptr, tag);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource003
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context and pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource003, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, 1);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource004
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context is nulltr and pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource004, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::ReleaseUnlockGpuResource(nullptr, 1);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource005
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context is nullptr and exitedPidSet
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource005, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::ReleaseUnlockGpuResource(nullptr, {2, 3});
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByPid fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource006
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context is nullptr and exitedPidSet
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource006, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, {2, 3});
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource007
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context  and nodeid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource007, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    NodeId node = 1;
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, node);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource008
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context  and nodeid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource008, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    NodeId node = 1;
    MemoryManager::ReleaseUnlockGpuResource(nullptr, node);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource009
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context  and scratchResourcesOnly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource009, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, true);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource001
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context  and scratchResourcesOnly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource010, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::ReleaseUnlockGpuResource(nullptr, true);
    EXPECT_TRUE(g_logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: PurgeCacheBetweenFrames001
 * @tc.desc: Verify PurgeCacheBetweenFrames logs correct trace info when context  and scratchResourcesOnly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, PurgeCacheBetweenFrames001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    std::set<pid_t> pids = {1, 2};
    std::set<pid_t> protectedPidSet = {3};
    MemoryManager::PurgeCacheBetweenFrames(gpuContext, true, pids, protectedPidSet);
    EXPECT_TRUE(g_logMsg.find("PurgeCacheBetweenFrames fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: PurgeCacheBetweenFrames002
 * @tc.desc: Verify PurgeCacheBetweenFrames logs correct trace info when context is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, PurgeCacheBetweenFrames002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::set<pid_t> pids = {1, 2};
    std::set<pid_t> protectedPidSet = {3};
    MemoryManager::PurgeCacheBetweenFrames(nullptr, true, pids, protectedPidSet);
    EXPECT_TRUE(g_logMsg.find("PurgeCacheBetweenFrames fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockAndSafeCacheGpuResource001
 * @tc.desc: Verify ReleaseUnlockAndSafeCacheGpuResource logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockAndSafeCacheGpuResource001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(gpuContext);
    EXPECT_TRUE(g_logMsg.find("ReleaseUnlockAndSafeCacheGpuResource fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockAndSafeCacheGpuResource002
 * @tc.desc: Verify ReleaseUnlockAndSafeCacheGpuResource logs correct trace info when context iss nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockAndSafeCacheGpuResource002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(nullptr);
    EXPECT_TRUE(g_logMsg.find("ReleaseUnlockAndSafeCacheGpuResource fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: SetGpuCacheSuppressWindowSwitch001
 * @tc.desc: Verify SetGpuCacheSuppressWindowSwitch logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuCacheSuppressWindowSwitch001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::SetGpuCacheSuppressWindowSwitch(gpuContext, true);
    EXPECT_TRUE(g_logMsg.find("SetGpuCacheSuppressWindowSwitch fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: SetGpuCacheSuppressWindowSwitch002
 * @tc.desc: Verify SetGpuCacheSuppressWindowSwitch logs correct trace info when context is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuCacheSuppressWindowSwitch002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::SetGpuCacheSuppressWindowSwitch(nullptr, true);
#ifdef RS_ENABLE_VK
    EXPECT_TRUE(g_logMsg.find("SetGpuCacheSuppressWindowSwitch fail, gpuContext is nullptr") != std::string::npos);
#endif
}

/**
 * @tc.name: SetGpuMemoryAsyncReclaimerSwitch001
 * @tc.desc: Verify SetGpuMemoryAsyncReclaimerSwitch logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuMemoryAsyncReclaimerSwitch001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    const std::function<void()> setThreadPriority;
    MemoryManager::SetGpuMemoryAsyncReclaimerSwitch(gpuContext, true, setThreadPriority);
    EXPECT_TRUE(g_logMsg.find("SetGpuMemoryAsyncReclaimerSwitch fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: SetGpuMemoryAsyncReclaimerSwitch002
 * @tc.desc: Verify SetGpuMemoryAsyncReclaimerSwitch logs correct trace info when context is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuMemoryAsyncReclaimerSwitch002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    const std::function<void()> setThreadPriority;
    MemoryManager::SetGpuMemoryAsyncReclaimerSwitch(nullptr, true, setThreadPriority);
#ifdef RS_ENABLE_VK
    EXPECT_TRUE(g_logMsg.find("SetGpuMemoryAsyncReclaimerSwitch fail, gpuContext is nullptr") != std::string::npos);
#endif
}

/**
 * @tc.name: FlushGpuMemoryInWaitQueue001
 * @tc.desc: Verify FlushGpuMemoryInWaitQueue logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, FlushGpuMemoryInWaitQueue001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::FlushGpuMemoryInWaitQueue(gpuContext);
    EXPECT_TRUE(g_logMsg.find("FlushGpuMemoryInWaitQueue fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: FlushGpuMemoryInWaitQueue002
 * @tc.desc: Verify FlushGpuMemoryInWaitQueue logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, FlushGpuMemoryInWaitQueue002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::FlushGpuMemoryInWaitQueue(nullptr);
#ifdef RS_ENABLE_VK
    EXPECT_TRUE(g_logMsg.find("FlushGpuMemoryInWaitQueue fail, gpuContext is nullptr") != std::string::npos);
#endif
}

/**
 * @tc.name: SuppressGpuCacheBelowCertainRatio001
 * @tc.desc: Verify SuppressGpuCacheBelowCertainRatio logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SuppressGpuCacheBelowCertainRatio001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    const std::function<bool(void)> nextFrameHasArrived;
    MemoryManager::SuppressGpuCacheBelowCertainRatio(gpuContext, nextFrameHasArrived);
    EXPECT_TRUE(g_logMsg.find("SuppressGpuCacheBelowCertainRatio fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: SuppressGpuCacheBelowCertainRatio002
 * @tc.desc: Verify SuppressGpuCacheBelowCertainRatio logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SuppressGpuCacheBelowCertainRatio002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    const std::function<bool(void)> nextFrameHasArrived;
    MemoryManager::SuppressGpuCacheBelowCertainRatio(nullptr, nextFrameHasArrived);
#ifdef RS_ENABLE_VK
    EXPECT_TRUE(g_logMsg.find("SuppressGpuCacheBelowCertainRatio fail, gpuContext is nullptr") != std::string::npos);
#endif
}

/**
 * @tc.name: GetAppGpuMemoryInMB_NullContextTest
 * @tc.desc: Verify GetAppGpuMemoryInMB
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, GetAppGpuMemoryInMB001, testing::ext::TestSize.Level1)
{
    float result = MemoryManager::GetAppGpuMemoryInMB(nullptr);
    EXPECT_FLOAT_EQ(result, 0.0f);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    float result1 = MemoryManager::GetAppGpuMemoryInMB(gpuContext);
    ASSERT_TRUE(result1 >= 0.0f);
}

/**
 * @tc.name: DumpPidMemoryTest001
 * @tc.desc: Test DumpPidMemory with GPU context.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpPidMemoryTest001, testing::ext::TestSize.Level1)
{
    DfxString log;
    int pid = 1001;
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::DumpPidMemory(log, pid, gpuContext);
    ASSERT_TRUE(log.GetString().find("GPU Mem(MB)") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("CPU Mem(KB)") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Total Mem(MB)") != std::string::npos);
}

/**
 * @tc.name: DumpRenderServiceMemory001
 * @tc.desc: Verify DumpRenderServiceMemory
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpRenderServiceMemory001, testing::ext::TestSize.Level1)
{
    DfxString log;
    bool isLite = false;
    MemoryManager::DumpRenderServiceMemory(log, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Total Node Size = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("OnTree") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("the memory of size of all surfaces buffer:") != std::string::npos);
}

/**
 * @tc.name: DumpRenderServiceMemory002
 * @tc.desc: Verify DumpRenderServiceMemory
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpRenderServiceMemory002, testing::ext::TestSize.Level1)
{
    DfxString log;
    bool isLite = true;
    MemoryManager::DumpRenderServiceMemory(log, isLite);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Total Node Size = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("the memory of size of all surfaces buffer:") != std::string::npos);
}

/**
 * @tc.name: DumpDrawingCpuMemory
 * @tc.desc: Verify DumpDrawingCpuMemory
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpDrawingCpuMemory001, testing::ext::TestSize.Level1)
{
    DfxString log;
    MemoryManager::DumpDrawingCpuMemory(log);
    ASSERT_TRUE(log.GetString().find("Total CPU memory usage:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("cpu cache limit = ") != std::string::npos);
}

/**
 * @tc.name: DumpGpuCache001
 * @tc.desc: Verify DumpGpuCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpGpuCache001, testing::ext::TestSize.Level1)
{
    DfxString log;
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    std::string gpuInfo = "pid:" + std::to_string(getpid()) + ", threadId:" + std::to_string(gettid());
    char threadName[16];
    auto result = pthread_getname_np(pthread_self(), threadName, sizeof(threadName));
    if (result == 0) {
        gpuInfo = gpuInfo + ", threadName: " + threadName;
    }
    MemoryManager::DumpGpuCache(log, gpuContext, nullptr, gpuInfo);
    ASSERT_TRUE(log.GetString().find("Total GPU memory usage:") != std::string::npos);
}

/**
 * @tc.name: DumpGpuCache002
 * @tc.desc: Verify DumpGpuCache when gpucontext is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpGpuCache002, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string gpuInfo;
    MemoryManager::DumpGpuCache(log, nullptr, nullptr, gpuInfo);
    ASSERT_TRUE(log.GetString().find("gpuContext is nullptr.") != std::string::npos);
}

/**
 * @tc.name: DumpGpuCacheNew001
 * @tc.desc: Verify DumpGpuCacheNew
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpGpuCacheNew001, testing::ext::TestSize.Level1)
{
    DfxString log;
    Drawing::GPUContext gpuContext;
    float ret = MemoryManager::DumpGpuCacheNew(log, &gpuContext, nullptr);
    EXPECT_GE(ret, 0.0f);
}

/**
 * @tc.name: DumpGpuCacheNew002
 * @tc.desc: Verify DumpGpuCacheNew when gpucontext is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpGpuCacheNew002, testing::ext::TestSize.Level1)
{
    DfxString log;
    float ret = MemoryManager::DumpGpuCacheNew(log, nullptr, nullptr);
    EXPECT_TRUE(log.GetString().find("gpuContext is nullptr.") != std::string::npos);
    EXPECT_EQ(ret, 0.0f);
}

/**
 * @tc.name: DumpAllGpuInfo001
 * @tc.desc: Verify DumpAllGpuInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpAllGpuInfo001, testing::ext::TestSize.Level1)
{
    DfxString log;
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    MemoryManager::DumpAllGpuInfo(log, gpuContext, nodeTags);
    ASSERT_TRUE(log.GetString().find("Total GPU memory usage:") == std::string::npos);
}

/**
 * @tc.name: DumpAllGpuInfo002
 * @tc.desc: Verify DumpAllGpuInfo when gpucontext is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpAllGpuInfo002, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    MemoryManager::DumpAllGpuInfo(log, nullptr, nodeTags);
    ASSERT_TRUE(log.GetString().find("No valid gpu cache instance") != std::string::npos);
}

/**
 * @tc.name: DumpAllGpuInfoNew001
 * @tc.desc: Verify DumpAllGpuInfoNew
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpAllGpuInfoNew001, testing::ext::TestSize.Level1)
{
    DfxString log;
    Drawing::GPUContext gpuContext;
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    nodeTags.emplace_back(std::make_pair(1001, "testnode1"));
    nodeTags.emplace_back(std::make_pair(1002, "testnode2"));
    MemoryManager::DumpAllGpuInfoNew(log, &gpuContext, nodeTags);
    EXPECT_TRUE(log.GetString().find("GPU Memory Data:") != std::string::npos);
}

/**
 * @tc.name: DumpAllGpuInfoNew002
 * @tc.desc: Verify DumpAllGpuInfoNew when gpucontext is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpAllGpuInfoNew002, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    MemoryManager::DumpAllGpuInfoNew(log, nullptr, nodeTags);
    EXPECT_TRUE(log.GetString().find("No valid gpu cache instance") != std::string::npos);
}

/**
 * @tc.name: DumpDrawingGpuMemory001
 * @tc.desc: Verify DumpDrawingGpuMemory
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpDrawingGpuMemory001, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    bool isLite = false;
    MemoryManager::DumpDrawingGpuMemory(log, gpuContext, nodeTags, isLite);
    ASSERT_TRUE(log.GetString().find("Shader Caches:") != std::string::npos);
}

/**
 * @tc.name: DumpDrawingGpuMemory002
 * @tc.desc: Verify DumpDrawingGpuMemory when gpucontext is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpDrawingGpuMemory002, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    bool isLite = false;
    MemoryManager::DumpDrawingGpuMemory(log, nullptr, nodeTags, isLite);
    ASSERT_TRUE(log.GetString().find("No valid gpu cache instance") != std::string::npos);
}

/**
 * @tc.name: DumpDrawingGpuMemory003
 * @tc.desc: Verify DumpDrawingGpuMemory when isLite is true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpDrawingGpuMemory003, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    bool isLite = true;
    MemoryManager::DumpDrawingGpuMemory(log, nullptr, nodeTags, isLite);
    EXPECT_TRUE(log.GetString().find("dumpGpuStats") == std::string::npos);
}

/**
 * @tc.name: DumpMemorySnapshot001
 * @tc.desc: Verify DumpMemorySnapshot
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemorySnapshot001, testing::ext::TestSize.Level1)
{
    DfxString log;
    MemoryManager::DumpMemorySnapshot(log);
    ASSERT_TRUE(log.GetString().find("memorySnapshots, totalMemory") != std::string::npos);
}

/**
 * @tc.name: SetGpuMemoryLimit001
 * @tc.desc: Verify SuppressGpuCacheBelowCertainRatio logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuMemoryLimit001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::SetGpuMemoryLimit(gpuContext);
    EXPECT_TRUE(g_logMsg.find("MemoryManager::SetGpuMemoryLimit gpuContext is nullptr") != std::string::npos);
    MemoryManager::gpuMemoryControl_ = 2000 * 1024 * 1024;
    MemoryManager::SetGpuMemoryLimit(gpuContext);
}

/**
 * @tc.name: SetGpuMemoryLimit002
 * @tc.desc: Verify SuppressGpuCacheBelowCertainRatio logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuMemoryLimit002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::SetGpuMemoryLimit(nullptr);
    EXPECT_TRUE(g_logMsg.find("MemoryManager::SetGpuMemoryLimit gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: MemoryOverflow001
 * @tc.desc: Test MemoryOverflow
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, MemoryOverflow001, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::MemoryOverflow(1433, 1024, true);
    EXPECT_TRUE(g_logMsg.find("RSMemoryOverflow pid[1433]") != std::string::npos);
}

/**
 * @tc.name: MemoryOverflow002
 * @tc.desc: Test pid = 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, MemoryOverflow002, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::MemoryOverflow(0, 1024, true);
    EXPECT_TRUE(g_logMsg.find("RSMemoryOverflow pid[") == std::string::npos);
}

/**
 * @tc.name: MemoryOverflow003
 * @tc.desc: Test MemoryOverflow
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, MemoryOverflow003, testing::ext::TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    pid_t pid = 1434;
    MemoryManager::MemoryOverflow(pid, 1024, false);
    MemorySnapshot::Instance().AddCpuMemory(pid, 2048);
    EXPECT_TRUE(g_logMsg.find("RSMemoryOverflow pid[1434]") != std::string::npos);
}

/**
 * @tc.name: DumpExitPidMem001
 * @tc.desc: Verify DumpExitPidMem logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpExitPidMem001, testing::ext::TestSize.Level1)
{
    std::string logMsg;
    MemoryManager::DumpExitPidMem(logMsg, 123);
    EXPECT_TRUE(logMsg.find("allGpuSize:") != std::string::npos);
}

/**
 * @tc.name: InstanceTest001
 * @tc.desc: Verify RSReclaimMemoryManager singleton instance correctness
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, InstanceTest001, testing::ext::TestSize.Level1)
{
    RSReclaimMemoryManager& instance1 = RSReclaimMemoryManager::Instance();
    RSReclaimMemoryManager& instance2 = RSReclaimMemoryManager::Instance();
    ASSERT_EQ(&instance1, &instance2) << "Multiple singleton instances detected";
}

/**
 * @tc.name: InterruptReclaimTaskTest00
 * @tc.desc: Test multiple interrupt calls
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, InterruptReclaimTaskTest001, testing::ext::TestSize.Level1)
{
    RSReclaimMemoryManager::Instance().SetReclaimInterrupt(false);
    RSReclaimMemoryManager::Instance().InterruptReclaimTask("LAUNCHER_DIALER");
    ASSERT_TRUE(RSReclaimMemoryManager::Instance().IsReclaimInterrupt());
    RSReclaimMemoryManager::Instance().SetReclaimInterrupt(false);
    RSReclaimMemoryManager::Instance().InterruptReclaimTask("GESTURE_TO_RECENTS");
    ASSERT_FALSE(RSReclaimMemoryManager::Instance().IsReclaimInterrupt());
    RSReclaimMemoryManager::Instance().SetReclaimInterrupt(false);
}

/**
 * @tc.name: MemoryOverReport001
 * @tc.desc: Test MemoryOverReport
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, MemoryOverReport001, testing::ext::TestSize.Level1)
{
    pid_t pid = 1434;
    MemorySnapshotInfo info;
    auto& instance = MemorySnapshot::Instance();
    instance.GetMemorySnapshotInfoByPid(pid, info);
    std::string hidumperReport = "report";
    std::string filePath = "/data/service/el0/render_service/renderservice_mem.txt";
    MemoryManager::MemoryOverReport(pid, info, "RENDER_MEMORY_OVER_ERROR", hidumperReport, filePath);
    ASSERT_TRUE(std::ifstream(filePath).good());
}

/**
 * @tc.name: WriteInfoToFile
 * @tc.desc: Test WriteInfoToFile
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, WriteInfoToFile, testing::ext::TestSize.Level1)
{
    std::string filePath = "";
    std::string gpuMemInfo = "info";
    std::string hidumperReport = "";
    MemoryManager::WriteInfoToFile(filePath, gpuMemInfo, hidumperReport);
    filePath = "/data/service/el0/render_service/renderservice_mem.txt";
    MemoryManager::WriteInfoToFile(filePath, gpuMemInfo, hidumperReport);
    hidumperReport = "hidumper";
    MemoryManager::WriteInfoToFile(filePath, gpuMemInfo, hidumperReport);
    ASSERT_TRUE(std::ifstream(filePath).good());
}

/**
 * @tc.name: DumGpuNodeMemoryTest001
 * @tc.desc: DumGpuNodeMemory
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumGpuNodeMemoryTest001, testing::ext::TestSize.Level1)
{
    DfxString log;
    MemoryManager::DumpGpuNodeMemory(log);
    ASSERT_TRUE(log.GetString().find("GPU") != std::string::npos);
}

/**
 * @tc.name: GpuMemoryOverReportTest
 * @tc.desc: GpuMemoryOverReport
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, GpuMemoryOverReportTest, testing::ext::TestSize.Level1)
{
    std::unordered_map<std::string, std::pair<size_t, size_t>> typeInfo;
    typeInfo["IMAGE_GPU"] = std::make_pair(10000, 1);
    std::unordered_map<pid_t, size_t> pidInfo;
    pidInfo[1] = 10000;
    MemoryManager::GpuMemoryOverReport(1, 10000, typeInfo, pidInfo);
    std::string filePath = "/data/service/el0/render_service/renderservice_killProcessByPid.txt";
    ASSERT_TRUE(std::ifstream(filePath).good());
}

/**
 * @tc.name: GetNodeInfo001
 * @tc.desc: GetNodeInfo001 Test
 * @tc.type: FUNC
 * @tc.require: ICVK6I
 */
HWTEST_F(RSMemoryManagerTest, GetNodeInfo001, TestSize.Level1)
{
    std::unordered_map<int, std::pair<int, int>> nodeInfo;
    std::unordered_map<int, int> nullNodeInfo;
    std::unordered_map<pid_t, size_t> modifierSize;
    NodeId id = 1024;
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemoryManager::GetNodeInfo(nodeInfo, nullNodeInfo, modifierSize);
    ASSERT_TRUE(nodeInfo.empty());
}

/**
 * @tc.name: GetNodeInfo002
 * @tc.desc: GetNodeInfo002 Test
 * @tc.type: FUNC
 * @tc.require: ICVK6I
 */
HWTEST_F(RSMemoryManagerTest, GetNodeInfo002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::unordered_map<int, std::pair<int, int>> nodeInfo;
    std::unordered_map<int, int> nullNodeInfo;
    std::unordered_map<pid_t, size_t> modifierSize;
    NodeId id = 1025;
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    auto node = std::make_shared<RSRenderNode>(id);
    mainThread->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    MemoryManager::GetNodeInfo(nodeInfo, nullNodeInfo, modifierSize);
    ASSERT_TRUE(nodeInfo.size() > 0);
}

/**
 * @tc.name: GetNodeInfo003
 * @tc.desc: GetNodeInfo003 Test
 * @tc.type: FUNC
 * @tc.require: ICVK6I
 */
HWTEST_F(RSMemoryManagerTest, GetNodeInfo003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::unordered_map<int, std::pair<int, int>> nodeInfo;
    std::unordered_map<int, int> nullNodeInfo;
    std::unordered_map<pid_t, size_t> modifierSize;
    NodeId id = 1026;
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    auto node = std::make_shared<RSRenderNode>(id);
    mainThread->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    nodeInfo.insert({ExtractPid(id), std::make_pair(0, 0)});
    MemoryManager::GetNodeInfo(nodeInfo, nullNodeInfo, modifierSize);
    ASSERT_TRUE(nodeInfo.size() > 0);
}
/**
 * @tc.name: GetNodeInfo003
 * @tc.desc: GetNodeInfo003 Test
 * @tc.type: FUNC
 * @tc.require: ICVK6I
 */
HWTEST_F(RSMemoryManagerTest, GetNodeInfo004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::unordered_map<int, std::pair<int, int>> nodeInfo;
    std::unordered_map<int, int> nullNodeInfo;
    std::unordered_map<pid_t, size_t> modifierSize;
    NodeId id = 1027;
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    nullNodeInfo.insert({ExtractPid(id), 0});
    MemoryManager::GetNodeInfo(nodeInfo, nullNodeInfo, modifierSize);
    ASSERT_TRUE(nullNodeInfo.size() > 0);
}

/**
 * @tc.name: RenderServiceAllNodeDump001
 * @tc.desc: RenderServiceAllNodeDump Test
 * @tc.type: FUNC
 * @tc.require: issueIB57QP
 */
HWTEST_F(RSMemoryManagerTest, RenderServiceAllNodeDump001, TestSize.Level1)
{
    DfxString log;
    MemoryManager::RenderServiceAllNodeDump(log);
    ASSERT_TRUE(log.GetString().find("OnTree") != std::string::npos);
}

/**
 * @tc.name: DumpMem001
 * @tc.desc: test DumpMem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMem001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isUniRender_ = true;
    std::unordered_set<std::u16string> argSets;
    std::string dumpString = "";
    std::string type = "";
    pid_t pid = 0;
    bool isLite = false;
    MemoryManager::DumpMem(argSets, dumpString, type, pid, isLite);
    ASSERT_TRUE(dumpString.find("dumpMem") != std::string::npos);
}

/**
 * @tc.name: DumpMem002
 * @tc.desc: test DumpMem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMem002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isUniRender_ = true;
    std::unordered_set<std::u16string> argSets;
    std::string dumpString = "x";
    std::string type = "x";
    pid_t pid = 0;
    bool isLite = true;
    MemoryManager::DumpMem(argSets, dumpString, type, pid, isLite);
    ASSERT_TRUE(dumpString.find("dumpMem") != std::string::npos);
}

/**
 * @tc.name: DumpMem003
 * @tc.desc: test DumpMem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMem003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isUniRender_ = true;
    std::unordered_set<std::u16string> argSets;
    std::string dumpString = "";
    std::string type = "gpu";
    pid_t pid = 1;
    bool isLite = false;
    MemoryManager::DumpMem(argSets, dumpString, type, pid, isLite);
    ASSERT_TRUE(dumpString.find("dumpMem") != std::string::npos);
}

/**
 * @tc.name: DumpGpuMem001
 * @tc.desc: test DumpGpuMem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpGpuMem001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isUniRender_ = true;
    std::unordered_set<std::u16string> argSets;
    std::string dumpString = "";
    std::string type = "gpu";
    MemoryManager::DumpGpuMem(argSets, dumpString, type);
    ASSERT_TRUE(dumpString.find("GPU") != std::string::npos);
}

/**
 * @tc.name: DumpGpuMem002
 * @tc.desc: test DumpGpuMem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpGpuMem002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isUniRender_ = true;
    std::unordered_set<std::u16string> argSets;
    std::string dumpString = "";
    std::string type = "";
    MemoryManager::DumpGpuMem(argSets, dumpString, type);
    ASSERT_TRUE(dumpString.find("GPU") != std::string::npos);
}

/**
 * @tc.name: DumpGpuMem003
 * @tc.desc: test DumpGpuMem
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpGpuMem003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->isUniRender_ = true;
    std::unordered_set<std::u16string> argSets;
    std::string dumpString = "x";
    std::string type = "x";
    MemoryManager::DumpGpuMem(argSets, dumpString, type);
    ASSERT_TRUE(dumpString.find("GPU") != std::string::npos);
}

/**
 * @tc.name: RenderServiceAllSurfaceDump001
 * @tc.desc: RenderServiceAllSurfaceDump Test
 * @tc.type: FUNC
 * @tc.require: issueIB57QP
 */
HWTEST_F(RSMemoryManagerTest, RenderServiceAllSurfaceDump001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    //create some surface nodes && register them
    RSSurfaceRenderNodeConfig configwithNoComsumer;
    configwithNoComsumer.id = 11;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(configwithNoComsumer);
    node1->SetIsOnTheTree(true);
    RSSurfaceRenderNodeConfig configwithComsumer;
    configwithComsumer.id = 12;
    auto node2 = RSTestUtil::CreateSurfaceNode(configwithComsumer);
    node2->SetIsOnTheTree(true);
    auto node3 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    mainThread->GetContext().GetMutableNodeMap().RegisterRenderNode(node1);
    mainThread->GetContext().GetMutableNodeMap().RegisterRenderNode(node2);
    mainThread->GetContext().GetMutableNodeMap().RegisterRenderNode(node3);

    DfxString log;
    MemoryManager::RenderServiceAllSurfaceDump(log);
    ASSERT_TRUE(log.GetString().find("memory") != std::string::npos);
}

/**
 * @tc.name: RenderServiceAllSurfaceDump002
 * @tc.desc: Test RenderServiceAllSurfaceDump with null consumer
 * @tc.type: FUNC
 * @tc.require: issueIB57QP
 */
HWTEST_F(RSMemoryManagerTest, RenderServiceAllSurfaceDump002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    // Create a surface node without consumer (surfaceConsumer == nullptr path)
    RSSurfaceRenderNodeConfig config;
    config.id = 20;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node, nullptr);
    node->SetIsOnTheTree(true);
    mainThread->GetContext().GetMutableNodeMap().RegisterRenderNode(node);

    DfxString log;
    MemoryManager::RenderServiceAllSurfaceDump(log);

    // Verify the function handles null consumer gracefully (no crash)
    // The log should still contain "memory" header even if no surface data
    ASSERT_TRUE(log.GetString().find("memory") != std::string::npos);
}

/**
 * @tc.name: RenderServiceAllSurfaceDump004
 * @tc.desc: Test RenderServiceAllSurfaceDump with both buffer and preBuffer
 * @tc.type: FUNC
 * @tc.require: issueIB57QP
 */
HWTEST_F(RSMemoryManagerTest, RenderServiceAllSurfaceDump004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    // Create a surface node with buffer
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    node->SetIsOnTheTree(true);
    mainThread->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    // Get the surface handler and set another buffer (this will make current buffer become preBuffer)
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    int64_t timestamp = 0;
    OHOS::sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp);
    OHOS::sptr<SurfaceBuffer> cbuffer = SurfaceBuffer::Create();
    surfaceHandler->SetBuffer(cbuffer, acquireFence, damage, timestamp);

    DfxString log;
    MemoryManager::RenderServiceAllSurfaceDump(log);

    // Verify buffer branch is covered
    ASSERT_TRUE(log.GetString().find("seqNum = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("bufferWidth = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("bufferHeight = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("bufferSize = ") != std::string::npos);

    // Verify both buffer and preBuffer branches are covered
    ASSERT_TRUE(log.GetString().find("seqNum = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("preSeqNum = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("preBufferWidth = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("preBufferHeight = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("preBufferSize = ") != std::string::npos);
}

/**
 * @tc.name: RenderServiceAllSurfaceDump005
 * @tc.desc: Test RenderServiceAllSurfaceDump with consumer Dump returning empty
 * @tc.type: FUNC
 * @tc.require: issueIB57QP
 */
HWTEST_F(RSMemoryManagerTest, RenderServiceAllSurfaceDump005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    // Create a surface node with consumer but no sequence info
    // This tests the path where consumer->Dump succeeds but doesn't contain "sequence"
    RSSurfaceRenderNodeConfig config;
    config.id = 21;
    auto node = RSTestUtil::CreateSurfaceNode(config);
    ASSERT_NE(node, nullptr);
    node->SetIsOnTheTree(true);
    mainThread->GetContext().GetMutableNodeMap().RegisterRenderNode(node);

    DfxString log;
    MemoryManager::RenderServiceAllSurfaceDump(log);

    // Verify function handles empty consumer dump gracefully
    ASSERT_TRUE(log.GetString().find("memory") != std::string::npos);
}

/**
 * @tc.name: RenderServiceAllSurfaceDump006
 * @tc.desc: Test RenderServiceAllSurfaceDump with null node in surfaceNodeMap
 * @tc.type: FUNC
 * @tc.require: issueIB57QP
 */
HWTEST_F(RSMemoryManagerTest, RenderServiceAllSurfaceDump006, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    // Manually insert a nullptr into surfaceNodeMap_ to test the defensive check
    // This simulates an edge case where a null pointer exists in the map
    NodeId nullNodeId = 9999;
    mainThread->GetContext().GetMutableNodeMap().surfaceNodeMap_.emplace(nullNodeId, nullptr);

    DfxString log;
    // This should not crash even with nullptr in the map (node == nullptr check should return early)
    MemoryManager::RenderServiceAllSurfaceDump(log);

    // Verify the function handles null node gracefully (no crash)
    ASSERT_TRUE(log.GetString().find("memory") != std::string::npos);

    // Clean up: remove the nullptr entry to prevent affecting other tests
    mainThread->GetContext().GetMutableNodeMap().surfaceNodeMap_.erase(nullNodeId);
    // Verify cleanup was successful
    ASSERT_EQ(mainThread->GetContext().GetMutableNodeMap().surfaceNodeMap_.count(nullNodeId), 0);
}
} // namespace OHOS::Rosen