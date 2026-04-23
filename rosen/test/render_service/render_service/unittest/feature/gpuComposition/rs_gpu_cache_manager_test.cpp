/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "gpuComposition/rs_gpu_cache_manager.h"
#include "pipeline/render_thread/rs_render_engine.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
struct CleanupCallbackRecorder {
    void Record(const std::unordered_set<uint64_t>& bufferIds)
    {
        ++callCount;
        lastBufferIds = bufferIds;
        calls.emplace_back(bufferIds);
    }

    size_t callCount = 0;
    std::unordered_set<uint64_t> lastBufferIds;
    std::vector<std::unordered_set<uint64_t>> calls;
};
} // namespace

class RSGPUCacheManagerTest : public testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: DeferredCleanupOccursOnlyWhenIdle001
 * @tc.desc: Schedule cleanup when GPU is busy should be deferred until the last GPUGuard is destroyed.
 * @tc.type: FUNC
 */
HWTEST_F(RSGPUCacheManagerTest, DeferredCleanupOccursOnlyWhenIdle001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    ASSERT_NE(renderEngine, nullptr);
    auto manager = GPUCacheManager::Create(*renderEngine);
    ASSERT_NE(manager, nullptr);

    CleanupCallbackRecorder recorder;
    manager->SetComposerCacheCleanupCallback([&recorder](const std::unordered_set<uint64_t>& bufferIds) {
        recorder.Record(bufferIds);
    });

    auto guard1 = manager->CreateGuard();
    {
        auto guard2 = manager->CreateGuard();
        ASSERT_EQ(manager->GetGPUDrawCount(), 2);

        // Both overloads: batch + single.
        manager->ScheduleBufferCleanup(std::set<uint64_t> { 100, 200 });
        manager->ScheduleBufferCleanup(300);
        EXPECT_EQ(recorder.callCount, 0u);
    }

    // 2 -> 1: should not trigger cleanup.
    EXPECT_EQ(manager->GetGPUDrawCount(), 1);
    EXPECT_EQ(recorder.callCount, 0u);

    // 1 -> 0: last guard destroyed, cleanup should run once.
    // Destroy guard1 explicitly to keep test intent clear.
    guard1 = GPUGuard(nullptr);
    EXPECT_EQ(manager->GetGPUDrawCount(), 0);

    ASSERT_EQ(recorder.callCount, 1u);
    EXPECT_EQ(recorder.lastBufferIds.size(), 3u);
    EXPECT_NE(recorder.lastBufferIds.find(100), recorder.lastBufferIds.end());
    EXPECT_NE(recorder.lastBufferIds.find(200), recorder.lastBufferIds.end());
    EXPECT_NE(recorder.lastBufferIds.find(300), recorder.lastBufferIds.end());
}

/**
 * @tc.name: ScheduleBatchCleanupEmptyAndImmediateWithoutCallback001
 * @tc.desc: Empty batch should do nothing; non-empty batch should cleanup immediately when idle (no callback set).
 * @tc.type: FUNC
 */
HWTEST_F(RSGPUCacheManagerTest, ScheduleBatchCleanupEmptyAndImmediateWithoutCallback001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    ASSERT_NE(renderEngine, nullptr);
    auto manager = GPUCacheManager::Create(*renderEngine);
    ASSERT_NE(manager, nullptr);
    ASSERT_EQ(manager->GetGPUDrawCount(), 0);

    manager->ScheduleBufferCleanup(std::set<uint64_t> {});
    EXPECT_TRUE(manager->pendingCleanupBuffers_.empty());

    manager->ScheduleBufferCleanup(std::set<uint64_t> { 1, 2 });
    EXPECT_TRUE(manager->pendingCleanupBuffers_.empty());
}

/**
 * @tc.name: GuardEndWithNoPendingDoesNothing001
 * @tc.desc: When no pending buffers exist, GPUGuard end should not invoke cleanup callback.
 * @tc.type: FUNC
 */
HWTEST_F(RSGPUCacheManagerTest, GuardEndWithNoPendingDoesNothing001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    ASSERT_NE(renderEngine, nullptr);
    auto manager = GPUCacheManager::Create(*renderEngine);
    ASSERT_NE(manager, nullptr);

    CleanupCallbackRecorder recorder;
    manager->SetComposerCacheCleanupCallback([&recorder](const std::unordered_set<uint64_t>& bufferIds) {
        recorder.Record(bufferIds);
    });

    {
        auto guard = manager->CreateGuard();
        ASSERT_EQ(manager->GetGPUDrawCount(), 1);
    }
    EXPECT_EQ(manager->GetGPUDrawCount(), 0);
    EXPECT_EQ(recorder.callCount, 0u);
}

/**
 * @tc.name: BufferDeleteCallbackWorksAndExpiredIsNoop001
 * @tc.desc: Buffer delete callbacks should schedule cleanup when manager is alive;
 *           be a noop after manager is destroyed.
 * @tc.type: FUNC
 */
HWTEST_F(RSGPUCacheManagerTest, BufferDeleteCallbackWorksAndExpiredIsNoop001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    ASSERT_NE(renderEngine, nullptr);

    CleanupCallbackRecorder recorder;
    std::function<void(uint64_t)> deleteCallback;
    std::function<void(int32_t)> deleteCallback32;

    {
        auto manager = GPUCacheManager::Create(*renderEngine);
        ASSERT_NE(manager, nullptr);
        manager->SetComposerCacheCleanupCallback([&recorder](const std::unordered_set<uint64_t>& bufferIds) {
            recorder.Record(bufferIds);
        });

        deleteCallback = manager->CreateBufferDeleteCallback();
        deleteCallback32 = manager->CreateBufferDeleteCallback32();

        deleteCallback(7);
        ASSERT_EQ(recorder.callCount, 1u);
        EXPECT_NE(recorder.lastBufferIds.find(7), recorder.lastBufferIds.end());

        deleteCallback32(8);
        ASSERT_EQ(recorder.callCount, 2u);
        EXPECT_NE(recorder.lastBufferIds.find(8), recorder.lastBufferIds.end());
    }

    // Manager is destroyed, callbacks should be no-op and must not crash.
    const size_t callsBefore = recorder.callCount;
    deleteCallback(9);
    deleteCallback32(10);
    EXPECT_EQ(recorder.callCount, callsBefore);
}

/**
 * @tc.name: GPUGuardMoveAssignmentCoverage001
 * @tc.desc: Cover GPUGuard move assignment branches (null manager, overwrite manager, and self move assignment).
 * @tc.type: FUNC
 */
HWTEST_F(RSGPUCacheManagerTest, GPUGuardMoveAssignmentCoverage001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    ASSERT_NE(renderEngine, nullptr);
    auto manager = GPUCacheManager::Create(*renderEngine);
    ASSERT_NE(manager, nullptr);

    // Null manager branch (ctor/dtor should do nothing).
    GPUGuard nullGuard(nullptr);
    EXPECT_EQ(manager->GetGPUDrawCount(), 0);

    // Move-assign into an "empty" guard: should not call OnGPUDrawEnd() for destination.
    GPUGuard dst(nullptr);
    dst = manager->CreateGuard();
    ASSERT_EQ(manager->GetGPUDrawCount(), 1);

    auto src = manager->CreateGuard();
    ASSERT_EQ(manager->GetGPUDrawCount(), 2);

    // Overwrite dst: should call OnGPUDrawEnd() for existing manager in dst.
    dst = std::move(src);
    EXPECT_EQ(manager->GetGPUDrawCount(), 1);
}
} // namespace OHOS::Rosen
