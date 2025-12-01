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

#include "ipc_callbacks/rs_canvas_surface_buffer_callback_proxy.h"
#include "mock_iremote_object.h"
#include "surface_buffer.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSCanvasSurfaceBufferCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<SurfaceBuffer> CreateTestSurfaceBuffer(int width, int height);
};

void RSCanvasSurfaceBufferCallbackProxyTest::SetUpTestCase() {}
void RSCanvasSurfaceBufferCallbackProxyTest::TearDownTestCase() {}
void RSCanvasSurfaceBufferCallbackProxyTest::SetUp() {}
void RSCanvasSurfaceBufferCallbackProxyTest::TearDown() {}

sptr<SurfaceBuffer> RSCanvasSurfaceBufferCallbackProxyTest::CreateTestSurfaceBuffer(int width, int height)
{
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    if (buffer == nullptr) {
        return nullptr;
    }

    BufferRequestConfig config = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE |
                 BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_MMZ_CACHE,
        .timeout = 0,
    };

    if (buffer->Alloc(config) != GSERROR_OK) {
        return nullptr;
    }

    return buffer;
}

/**
 * @tc.name: OnCanvasSurfaceBufferChanged001
 * @tc.desc: Test OnCanvasSurfaceBufferChanged with buffer=nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasSurfaceBufferCallbackProxyTest, OnCanvasSurfaceBufferChanged001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_NE(remoteMocker, nullptr);
    auto proxy = std::make_shared<RSCanvasSurfaceBufferCallbackProxy>(remoteMocker);
    ASSERT_NE(proxy, nullptr);

    NodeId testNodeId = 12345;
    proxy->OnCanvasSurfaceBufferChanged(testNodeId, nullptr, 0);

    EXPECT_EQ(remoteMocker->receivedCode_, static_cast<uint32_t>(
        RSICanvasSurfaceBufferCallbackInterfaceCode::ON_CANVAS_SURFACE_BUFFER_CHANGED));
}

/**
 * @tc.name: OnCanvasSurfaceBufferChanged002
 * @tc.desc: Test OnCanvasSurfaceBufferChanged with valid buffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasSurfaceBufferCallbackProxyTest, OnCanvasSurfaceBufferChanged002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_NE(remoteMocker, nullptr);
    auto proxy = std::make_shared<RSCanvasSurfaceBufferCallbackProxy>(remoteMocker);
    ASSERT_NE(proxy, nullptr);

    NodeId testNodeId = 67890;
    sptr<SurfaceBuffer> testBuffer = CreateTestSurfaceBuffer(100, 100);
    ASSERT_NE(testBuffer, nullptr);

    proxy->OnCanvasSurfaceBufferChanged(testNodeId, testBuffer, 0);

    EXPECT_EQ(remoteMocker->receivedCode_, static_cast<uint32_t>(
        RSICanvasSurfaceBufferCallbackInterfaceCode::ON_CANVAS_SURFACE_BUFFER_CHANGED));
}

/**
 * @tc.name: OnCanvasSurfaceBufferChanged003
 * @tc.desc: Test OnCanvasSurfaceBufferChanged with SendRequest error
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasSurfaceBufferCallbackProxyTest, OnCanvasSurfaceBufferChanged003, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_NE(remoteMocker, nullptr);
    remoteMocker->sendRequestResult_ = 1; // Simulate SendRequest failure
    auto proxy = std::make_shared<RSCanvasSurfaceBufferCallbackProxy>(remoteMocker);
    ASSERT_NE(proxy, nullptr);

    NodeId testNodeId = 11111;
    proxy->OnCanvasSurfaceBufferChanged(testNodeId, nullptr, 0);

    EXPECT_EQ(remoteMocker->receivedCode_, static_cast<uint32_t>(
        RSICanvasSurfaceBufferCallbackInterfaceCode::ON_CANVAS_SURFACE_BUFFER_CHANGED));
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS && RS_ENABLE_VK