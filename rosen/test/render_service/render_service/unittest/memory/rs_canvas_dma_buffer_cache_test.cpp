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

void RSCanvasDmaBufferCacheTest::SetUpTestCase() {}
void RSCanvasDmaBufferCacheTest::TearDownTestCase() {}
void RSCanvasDmaBufferCacheTest::SetUp() {}
void RSCanvasDmaBufferCacheTest::TearDown() {}

/**
 * @tc.name: RegisterCanvasCallbackAndNotifyTest
 * @tc.desc: Test RegisterCanvasCallback and NotifyCanvasSurfaceBufferChanged functions
 * @tc.type: FUNC
 * @tc.require:
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
}
} // namespace OHOS::Rosen
#endif // ROSEN_OHOS && RS_ENABLE_VK