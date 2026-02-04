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

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "gtest/gtest.h"

#include "ipc_callbacks/rs_canvas_surface_buffer_callback_stub.h"
#include "memory/rs_canvas_dma_buffer_cache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDmaBufferCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

class MockCanvasSurfaceBufferCallback : public RSCanvasSurfaceBufferCallbackStub {
public:
    void OnCanvasSurfaceBufferChanged(
        NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex) override
    {}
};

void RSCanvasDmaBufferCacheTest::SetUpTestCase() {}
void RSCanvasDmaBufferCacheTest::TearDownTestCase() {}
void RSCanvasDmaBufferCacheTest::SetUp() {}
void RSCanvasDmaBufferCacheTest::TearDown() {}

/**
 * @tc.name: RegisterCanvasCallbackAndNotifyTest
 * @tc.desc: Test RegisterCanvasCallback and NotifyCanvasSurfaceBufferChanged functions
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDmaBufferCacheTest, RegisterCanvasCallbackAndNotifyTest, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    pid_t testPid = 12345;
    NodeId testNodeId = static_cast<NodeId>(testPid) << 32 | 1;  // Construct nodeId with pid

    // Create a mock callback (using nullptr for simplicity in this test)
    sptr<RSICanvasSurfaceBufferCallback> mockCallback = nullptr;

    // Test 1: Register callback with nullptr (should erase if exists)
    bufferCache.RegisterCanvasCallback(testPid, mockCallback);

    // Test 2: Register a valid callback
    // Note: In real scenario, we would create a mock implementation
    // For now, we test the registration mechanism itself
    bufferCache.RegisterCanvasCallback(testPid, mockCallback);

    // Test 3: NotifyCanvasSurfaceBufferChanged with nullptr buffer (should return early)
    sptr<SurfaceBuffer> nullBuffer = nullptr;
    bufferCache.NotifyCanvasSurfaceBufferChanged(testNodeId, nullBuffer, 0);
    // No assertion - just verify it doesn't crash

    // Test 4: NotifyCanvasSurfaceBufferChanged when callback not registered
    pid_t unregisteredPid = 99999;
    NodeId unregisteredNodeId = static_cast<NodeId>(unregisteredPid) << 32 | 1;
    sptr<SurfaceBuffer> testBuffer = SurfaceBuffer::Create();
    ASSERT_NE(testBuffer, nullptr);
    bufferCache.NotifyCanvasSurfaceBufferChanged(unregisteredNodeId, testBuffer, 0);
    // No assertion - just verify it doesn't crash when callback not found

    // Test 5: Unregister callback by passing nullptr
    bufferCache.RegisterCanvasCallback(testPid, nullptr);

    // Test 6: NotifyCanvasSurfaceBufferChanged after unregistration
    bufferCache.NotifyCanvasSurfaceBufferChanged(testNodeId, testBuffer, 0);
    // No assertion - just verify it doesn't crash

    // Test 7: RegisterCanvasCallback non null callback
    mockCallback = new MockCanvasSurfaceBufferCallback();
    bufferCache.RegisterCanvasCallback(testPid, mockCallback);
    ASSERT_EQ(bufferCache.canvasSurfaceBufferCallbackMap_.size(), 1);

    // Test 8: NotifyCanvasSurfaceBufferChanged when non null callback registered
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    bufferCache.NotifyCanvasSurfaceBufferChanged(testNodeId, buffer, 1);
    // No assertion - just verify it doesn't crash

    // Test 9: NotifyCanvasSurfaceBufferChanged when null callback registered
    bufferCache.canvasSurfaceBufferCallbackMap_[testPid] = nullptr;
    bufferCache.NotifyCanvasSurfaceBufferChanged(testNodeId, buffer, 1);
    // No assertion - just verify it doesn't crash
}

/**
 * @tc.name: AddPendingBufferTest
 * @tc.desc: Test AddPendingBuffer functions
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDmaBufferCacheTest, AddPendingBufferTest, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    bufferCache.pendingBufferMap_.clear();
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    NodeId nodeId = 1;
    sptr<SurfaceBuffer> buffer = nullptr;
    bufferCache.AddPendingBuffer(nodeId, buffer, 1);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 1);
    for (uint32_t i = 0; i <= 5; i++) {
        bufferCache.AddPendingBuffer(nodeId, buffer, i);
        ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 1);
    }
}

/**
 * @tc.name: AcquirePendingBufferTest
 * @tc.desc: Test AcquirePendingBuffer functions
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDmaBufferCacheTest, AcquirePendingBufferTest, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    bufferCache.pendingBufferMap_.clear();
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    NodeId nodeId = 1;
    auto buffer = bufferCache.AcquirePendingBuffer(nodeId, 1);
    ASSERT_EQ(buffer, nullptr);
    buffer = bufferCache.AcquirePendingBuffer(nodeId, 1);
    ASSERT_EQ(buffer, nullptr);
    sptr<SurfaceBuffer> buffer1 = SurfaceBuffer::Create();
    bufferCache.AddPendingBuffer(nodeId, buffer1, 2);
    buffer = bufferCache.AcquirePendingBuffer(nodeId, 2);
    ASSERT_NE(buffer, nullptr);
    bufferCache.AddPendingBuffer(nodeId, buffer1, 3);
    bufferCache.AddPendingBuffer(nodeId, buffer1, 4);
    bufferCache.AddPendingBuffer(nodeId, buffer1, 5);
    buffer = bufferCache.AcquirePendingBuffer(nodeId, 4);
    ASSERT_NE(buffer, nullptr);
}

/**
 * @tc.name: RemovePendingBufferTest
 * @tc.desc: Test RemovePendingBuffer functions
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDmaBufferCacheTest, RemovePendingBufferTest, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    bufferCache.pendingBufferMap_.clear();
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    NodeId nodeId = 1;
    bufferCache.RemovePendingBuffer(nodeId, 1);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    bufferCache.AcquirePendingBuffer(nodeId, 1);
    bufferCache.RemovePendingBuffer(nodeId, 1);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 1);
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    bufferCache.AddPendingBuffer(nodeId, buffer, 1);
    bufferCache.RemovePendingBuffer(nodeId, 1);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 1);
    bufferCache.AddPendingBuffer(nodeId, buffer, 2);
    bufferCache.RemovePendingBuffer(nodeId, 2);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 1);
}

/**
 * @tc.name: ClearPendingBufferByNodeIdTest
 * @tc.desc: Test ClearPendingBufferByNodeId functions
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDmaBufferCacheTest, ClearPendingBufferByNodeIdTest, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    bufferCache.pendingBufferMap_.clear();
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    NodeId nodeId = 1;
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    bufferCache.AddPendingBuffer(nodeId, buffer, 1);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 1);
    bufferCache.ClearPendingBufferByNodeId(nodeId);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 0);
    bufferCache.ClearPendingBufferByNodeId(0);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 0);
}

/**
 * @tc.name: ClearPendingBufferByPidTest
 * @tc.desc: Test ClearPendingBufferByPid functions
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDmaBufferCacheTest, ClearPendingBufferByPidTest, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    bufferCache.pendingBufferMap_.clear();
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    pid_t pid = 12345;
    NodeId nodeId = static_cast<NodeId>(pid) << 32 | 1;
    bufferCache.ClearPendingBufferByPid(pid);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    bufferCache.AddPendingBuffer(nodeId, buffer, 1);
    bufferCache.AddPendingBuffer(1, buffer, 1);
    bufferCache.ClearPendingBufferByPid(pid);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 1);
}

/**
 * @tc.name: AddPendingBufferBranchesTest
 * @tc.desc: Test AddPendingBuffer with different parameter combinations
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDmaBufferCacheTest, AddPendingBufferBranchesTest, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    bufferCache.pendingBufferMap_.clear();
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    NodeId nodeId = 1;
    // Test Case 1: buffer == nullptr && resetSurfaceIndex == 0
    sptr<SurfaceBuffer> nullBuffer = nullptr;
    bool result1 = bufferCache.AddPendingBuffer(nodeId, nullBuffer, 0);
    ASSERT_TRUE(result1);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    // Test Case 2: buffer == nullptr && resetSurfaceIndex > 0
    bufferCache.pendingBufferMap_.clear();
    bool result2 = bufferCache.AddPendingBuffer(nodeId, nullBuffer, 1);
    ASSERT_TRUE(result2);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 1);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 0);
    // Test Case 3: buffer != nullptr && resetSurfaceIndex == 0
    bufferCache.pendingBufferMap_.clear();
    sptr<SurfaceBuffer> validBuffer = SurfaceBuffer::Create();
    ASSERT_NE(validBuffer, nullptr);
    bool result3 = bufferCache.AddPendingBuffer(nodeId, validBuffer, 0);
    ASSERT_TRUE(result3);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 1);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 1);
    // Test Case 4: buffer != nullptr && resetSurfaceIndex > 0
    bufferCache.pendingBufferMap_.clear();
    bool result4 = bufferCache.AddPendingBuffer(nodeId, validBuffer, 2);
    ASSERT_TRUE(result4);
    ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 1);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 1);
    ASSERT_NE(bufferCache.pendingBufferMap_[nodeId].second.find(2),
              bufferCache.pendingBufferMap_[nodeId].second.end());
}

/**
 * @tc.name: AddPendingBufferWithExistingNodeTest
 * @tc.desc: Test AddPendingBuffer with existing node in cache
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDmaBufferCacheTest, AddPendingBufferWithExistingNodeTest, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    bufferCache.pendingBufferMap_.clear();
    NodeId nodeId = 1;
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    // Add initial buffer
    bufferCache.AddPendingBuffer(nodeId, buffer, 1);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 1);
    // Add buffer with higher resetSurfaceIndex (should be added)
    bool result1 = bufferCache.AddPendingBuffer(nodeId, buffer, 2);
    ASSERT_TRUE(result1);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 2);
    // Add buffer with same resetSurfaceIndex (should not be added again)
    bool result2 = bufferCache.AddPendingBuffer(nodeId, buffer, 2);
    ASSERT_FALSE(result2);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 2);
    // Add buffer with lower resetSurfaceIndex (should be ignored)
    bool result3 = bufferCache.AddPendingBuffer(nodeId, buffer, 1);
    ASSERT_FALSE(result3);
    ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 2);
}
} // namespace OHOS::Rosen
#endif // ROSEN_OHOS && RS_ENABLE_VK