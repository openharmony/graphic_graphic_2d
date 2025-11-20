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
#include "surface_buffer.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasSurfaceBufferCallbackStubMock : public RSCanvasSurfaceBufferCallbackStub {
public:
    RSCanvasSurfaceBufferCallbackStubMock() = default;
    virtual ~RSCanvasSurfaceBufferCallbackStubMock() = default;

    void OnCanvasSurfaceBufferChanged(NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex) override
    {
        receivedNodeId_ = nodeId;
        receivedBuffer_ = buffer;
        receivedResetSurfaceIndex_ = resetSurfaceIndex;
        callbackInvoked_ = true;
    }

    NodeId receivedNodeId_ = 0;
    sptr<SurfaceBuffer> receivedBuffer_ = nullptr;
    uint32_t receivedResetSurfaceIndex_;
    bool callbackInvoked_ = false;
};

class RSCanvasSurfaceBufferCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<SurfaceBuffer> CreateTestSurfaceBuffer(int width, int height);
};

void RSCanvasSurfaceBufferCallbackStubTest::SetUpTestCase() {}
void RSCanvasSurfaceBufferCallbackStubTest::TearDownTestCase() {}
void RSCanvasSurfaceBufferCallbackStubTest::SetUp() {}
void RSCanvasSurfaceBufferCallbackStubTest::TearDown() {}

sptr<SurfaceBuffer> RSCanvasSurfaceBufferCallbackStubTest::CreateTestSurfaceBuffer(int width, int height)
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
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test InterfaceToken validation failure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasSurfaceBufferCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSCanvasSurfaceBufferCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSICanvasSurfaceBufferCallbackInterfaceCode::
        ON_CANVAS_SURFACE_BUFFER_CHANGED);

    // No InterfaceToken written
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_STATE);
    EXPECT_FALSE(stub->callbackInvoked_);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Test hasBuffer=false, buffer is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasSurfaceBufferCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSCanvasSurfaceBufferCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSICanvasSurfaceBufferCallbackInterfaceCode::
        ON_CANVAS_SURFACE_BUFFER_CHANGED);

    NodeId testNodeId = 12345;
    data.WriteInterfaceToken(RSICanvasSurfaceBufferCallback::GetDescriptor());
    data.WriteUint64(testNodeId);
    data.WriteBool(false); // hasBuffer = false

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
    EXPECT_TRUE(stub->callbackInvoked_);
    EXPECT_EQ(stub->receivedNodeId_, testNodeId);
    EXPECT_EQ(stub->receivedBuffer_, nullptr);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Test hasBuffer=true with valid SurfaceBuffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasSurfaceBufferCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSCanvasSurfaceBufferCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSICanvasSurfaceBufferCallbackInterfaceCode::
        ON_CANVAS_SURFACE_BUFFER_CHANGED);

    NodeId testNodeId = 67890;
    uint32_t resetSurfaceIndex = 1;
    sptr<SurfaceBuffer> testBuffer = CreateTestSurfaceBuffer(100, 100);
    ASSERT_NE(testBuffer, nullptr);

    data.WriteInterfaceToken(RSICanvasSurfaceBufferCallback::GetDescriptor());
    data.WriteUint64(testNodeId);
    data.WriteUint32(resetSurfaceIndex);
    data.WriteBool(true); // hasBuffer = true
    GSError ret = testBuffer->WriteToMessageParcel(data);
    ASSERT_EQ(ret, GSERROR_OK);

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
    EXPECT_TRUE(stub->callbackInvoked_);
    EXPECT_EQ(stub->receivedNodeId_, testNodeId);
    EXPECT_NE(stub->receivedBuffer_, nullptr);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Test hasBuffer=true but ReadFromMessageParcel fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasSurfaceBufferCallbackStubTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSCanvasSurfaceBufferCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSICanvasSurfaceBufferCallbackInterfaceCode::
        ON_CANVAS_SURFACE_BUFFER_CHANGED);

    NodeId testNodeId = 11111;
    uint32_t resetSurfaceIndex = 2;
    data.WriteInterfaceToken(RSICanvasSurfaceBufferCallback::GetDescriptor());
    data.WriteUint64(testNodeId);
    data.WriteUint32(resetSurfaceIndex);
    data.WriteBool(true); // hasBuffer = true
    // Not writing complete buffer data - will cause ReadFromMessageParcel to fail

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
    EXPECT_FALSE(stub->callbackInvoked_);
}

/**
 * @tc.name: OnRemoteRequest005
 * @tc.desc: Test invalid code
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasSurfaceBufferCallbackStubTest, OnRemoteRequest005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSCanvasSurfaceBufferCallbackStubMock>();
    uint32_t invalidCode = 9999;

    data.WriteInterfaceToken(RSICanvasSurfaceBufferCallback::GetDescriptor());

    int res = stub->OnRemoteRequest(invalidCode, data, reply, option);
    // Should call parent class IPCObjectStub::OnRemoteRequest
    EXPECT_EQ(res, IPC_STUB_UNKNOW_TRANS_ERR);
    EXPECT_FALSE(stub->callbackInvoked_);
}
} // namespace OHOS::Rosen
#endif // ROSEN_OHOS && RS_ENABLE_VK