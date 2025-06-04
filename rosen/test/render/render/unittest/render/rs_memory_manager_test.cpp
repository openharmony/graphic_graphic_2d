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
#include "memory/rs_memory_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
std::string logMsg;
void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
                   const char *msg)
{
    logMsg = msg;
}

class RSMemoryManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMemoryManagerTest::SetUpTestCase() {}
void RSMemoryManagerTest::TearDownTestCase() {}
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
    MemoryManager::DumpMemoryUsage(log, type);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("malloc stats :") != std::string::npos);
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
    MemoryManager::DumpMemoryUsage(log, type);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("malloc stats :") == std::string::npos);
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
    MemoryManager::DumpMemoryUsage(log, type);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("malloc stats :") == std::string::npos);
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
    MemoryManager::DumpMemoryUsage(log, type);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("malloc stats :") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("memorySnapshots") == std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest005
 * @tc.desc: Dump only jemalloc memory when type is "jemalloc".
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest005, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "jemalloc";
    MemoryManager::DumpMemoryUsage(log, type);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("malloc stats :") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("memorySnapshots") == std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest006
 * @tc.desc: Dump only memory snapshot when type is MEM_SNAPSHOT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest006, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "snapshot";
    MemoryManager::DumpMemoryUsage(log, type);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("Skia CPU caches") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("gpu") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("malloc stats :") == std::string::npos);
    ASSERT_TRUE(log.GetString().find("memorySnapshots") != std::string::npos);
}

/**
 * @tc.name: DumpMemoryUsageTest007
 * @tc.desc: No memory dumped when type is invalid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMemoryUsageTest007, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string type = "invalid_type";
    MemoryManager::DumpMemoryUsage(log, type);
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
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "ReleaseAllGpuResource");
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseAllGpuResource(gpuContext, tag);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseAllGpuResourceWithValidContextAndTag
 * @tc.desc: Verify ReleaseAllGpuResource logs correct trace info when context are nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseAllGpuResource002, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "ReleaseAllGpuResource");
    MemoryManager::ReleaseAllGpuResource(nullptr, tag);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseAllGpuResourceWithValidContextAndTag
 * @tc.desc: Verify ReleaseAllGpuResource logs correct trace info when context and pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseAllGpuResource003, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseAllGpuResource(gpuContext, 1);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource001
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource001, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "ReleaseUnlockGpuResource");
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, tag);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource002
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource002, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "ReleaseUnlockGpuResource");
    MemoryManager::ReleaseUnlockGpuResource(nullptr, tag);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource003
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context and pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource003, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, 1);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource004
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context is nulltr and pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource004, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::ReleaseUnlockGpuResource(nullptr, 1);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource005
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context is nullptr and exitedPidSet
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource005, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::ReleaseUnlockGpuResource(nullptr, {2, 3});
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByPid fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource006
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context is nullptr and exitedPidSet
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource006, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, {2, 3});
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource007
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context  and nodeid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource007, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    NodeId node = 1;
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, node);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource008
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context  and nodeid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource008, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    NodeId node = 1;
    MemoryManager::ReleaseUnlockGpuResource(nullptr, node);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource009
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context  and scratchResourcesOnly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource009, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseUnlockGpuResource(gpuContext, true);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockGpuResource001
 * @tc.desc: Verify ReleaseUnlockGpuResource logs correct trace info when context  and scratchResourcesOnly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockGpuResource010, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::ReleaseUnlockGpuResource(nullptr, true);
    EXPECT_TRUE(logMsg.find("ReleaseGpuResByTag fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: PurgeCacheBetweenFrames001
 * @tc.desc: Verify PurgeCacheBetweenFrames logs correct trace info when context  and scratchResourcesOnly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, PurgeCacheBetweenFrames001, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    std::set<pid_t> pids = {1, 2};
    std::set<pid_t> protectedPidSet = {3};
    MemoryManager::PurgeCacheBetweenFrames(gpuContext, true, pids, protectedPidSet);
    EXPECT_TRUE(logMsg.find("PurgeCacheBetweenFrames fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: PurgeCacheBetweenFrames002
 * @tc.desc: Verify PurgeCacheBetweenFrames logs correct trace info when context is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, PurgeCacheBetweenFrames002, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::set<pid_t> pids = {1, 2};
    std::set<pid_t> protectedPidSet = {3};
    MemoryManager::PurgeCacheBetweenFrames(nullptr, true, pids, protectedPidSet);
    EXPECT_TRUE(logMsg.find("PurgeCacheBetweenFrames fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockAndSafeCacheGpuResource001
 * @tc.desc: Verify ReleaseUnlockAndSafeCacheGpuResource logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockAndSafeCacheGpuResource001, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(gpuContext);
    EXPECT_TRUE(logMsg.find("ReleaseUnlockAndSafeCacheGpuResource fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: ReleaseUnlockAndSafeCacheGpuResource002
 * @tc.desc: Verify ReleaseUnlockAndSafeCacheGpuResource logs correct trace info when context iss nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, ReleaseUnlockAndSafeCacheGpuResource002, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(nullptr);
    EXPECT_TRUE(logMsg.find("ReleaseUnlockAndSafeCacheGpuResource fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: SetGpuCacheSuppressWindowSwitch001
 * @tc.desc: Verify SetGpuCacheSuppressWindowSwitch logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuCacheSuppressWindowSwitch001, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::SetGpuCacheSuppressWindowSwitch(gpuContext, true);
    EXPECT_TRUE(logMsg.find("SetGpuCacheSuppressWindowSwitch fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: SetGpuCacheSuppressWindowSwitch002
 * @tc.desc: Verify SetGpuCacheSuppressWindowSwitch logs correct trace info when context is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuCacheSuppressWindowSwitch002, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::SetGpuCacheSuppressWindowSwitch(nullptr, true);
    EXPECT_TRUE(logMsg.find("SetGpuCacheSuppressWindowSwitch fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: SetGpuMemoryAsyncReclaimerSwitch001
 * @tc.desc: Verify SetGpuMemoryAsyncReclaimerSwitch logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuMemoryAsyncReclaimerSwitch001, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    const std::function<void()> setThreadPriority;
    MemoryManager::SetGpuMemoryAsyncReclaimerSwitch(gpuContext, true, setThreadPriority);
    EXPECT_TRUE(logMsg.find("SetGpuMemoryAsyncReclaimerSwitch fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: SetGpuMemoryAsyncReclaimerSwitch002
 * @tc.desc: Verify SetGpuMemoryAsyncReclaimerSwitch logs correct trace info when context is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuMemoryAsyncReclaimerSwitch002, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    const std::function<void()> setThreadPriority;
    MemoryManager::SetGpuMemoryAsyncReclaimerSwitch(nullptr, true, setThreadPriority);
    EXPECT_TRUE(logMsg.find("SetGpuMemoryAsyncReclaimerSwitch fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: FlushGpuMemoryInWaitQueue001
 * @tc.desc: Verify FlushGpuMemoryInWaitQueue logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, FlushGpuMemoryInWaitQueue001, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::FlushGpuMemoryInWaitQueue(gpuContext);
    EXPECT_TRUE(logMsg.find("FlushGpuMemoryInWaitQueue fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: FlushGpuMemoryInWaitQueue002
 * @tc.desc: Verify FlushGpuMemoryInWaitQueue logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, FlushGpuMemoryInWaitQueue002, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::FlushGpuMemoryInWaitQueue(nullptr);
    EXPECT_TRUE(logMsg.find("FlushGpuMemoryInWaitQueue fail, gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: SuppressGpuCacheBelowCertainRatio001
 * @tc.desc: Verify SuppressGpuCacheBelowCertainRatio logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SuppressGpuCacheBelowCertainRatio001, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    const std::function<bool(void)> nextFrameHasArrived;
    MemoryManager::SuppressGpuCacheBelowCertainRatio(gpuContext, nextFrameHasArrived);
    EXPECT_TRUE(logMsg.find("SuppressGpuCacheBelowCertainRatio fail, gpuContext is nullptr") == std::string::npos);
}

/**
 * @tc.name: SuppressGpuCacheBelowCertainRatio002
 * @tc.desc: Verify SuppressGpuCacheBelowCertainRatio logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SuppressGpuCacheBelowCertainRatio002, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    const std::function<bool(void)> nextFrameHasArrived;
    MemoryManager::SuppressGpuCacheBelowCertainRatio(nullptr, nextFrameHasArrived);
    EXPECT_TRUE(logMsg.find("SuppressGpuCacheBelowCertainRatio fail, gpuContext is nullptr") != std::string::npos);
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
 * @tc.name: DumpRenderServiceMemory
 * @tc.desc: Verify DumpRenderServiceMemory
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpRenderServiceMemory001, testing::ext::TestSize.Level1)
{
    DfxString log;
    MemoryManager::DumpRenderServiceMemory(log);
    ASSERT_TRUE(log.GetString().find("RenderService caches:") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("Total Node Size = ") != std::string::npos);
    ASSERT_TRUE(log.GetString().find("OnTree") != std::string::npos);
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
    MemoryManager::DumpDrawingGpuMemory(log, gpuContext, nodeTags);
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
    MemoryManager::DumpDrawingGpuMemory(log, nullptr, nodeTags);
    ASSERT_TRUE(log.GetString().find("No valid gpu cache instance") != std::string::npos);
}

/**
 * @tc.name: DumpMallocSta001
 * @tc.desc: Verify DumpMallocStats
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, DumpMallocStat001, testing::ext::TestSize.Level1)
{
    std::string log;
    MemoryManager::DumpMallocStat(log);
    ASSERT_TRUE(log.find("decaying:") != std::string::npos);
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
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    MemoryManager::SetGpuMemoryLimit(gpuContext);
    EXPECT_TRUE(logMsg.find("MemoryManager::SetGpuMemoryLimit gpuContext is nullptr") != std::string::npos);
}

/**
 * @tc.name: SetGpuMemoryLimit002
 * @tc.desc: Verify SuppressGpuCacheBelowCertainRatio logs correct trace info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryManagerTest, SetGpuMemoryLimit002, testing::ext::TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MemoryManager::SetGpuMemoryLimit(nullptr);
    EXPECT_TRUE(logMsg.find("MemoryManager::SetGpuMemoryLimit gpuContext is nullptr") != std::string::npos);
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
} // namespace OHOS::Rosen