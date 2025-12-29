/**
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

#include <gtest/gtest.h>
#include "rs_render_to_composer_connection_stub.h"
#include "message_parcel.h"
#include "rs_layer_transaction_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TestRenderToComposerStub : public RSRenderToComposerConnectionStub {
public:
    bool CommitLayers(std::unique_ptr<RSLayerTransactionData>& transactionData) override
    {
        committed_ = (transactionData != nullptr);
        return true;
    }
    void ClearFrameBuffers() override {}
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId) override {}
    void ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds) override {}
    void SetScreenBacklight(uint32_t level) override {}
    void SetComposerToRenderConnection(const sptr<RSIComposerToRenderConnection>& composerToRenderConn) override {}

    bool committed_ {false};
};

class RSRenderToComposerConnectionStubTest : public Test {};

/**
 * Function: Stub_InvalidToken_Branch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. craft MessageParcel with wrong interface token
 *                  2. invoke OnRemoteRequest with COMMIT_LAYERS code
 *                  3. expect ERR_INVALID_STATE to cover invalid token branch
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, Stub_InvalidToken_Branch, TestSize.Level1)
{
    TestRenderToComposerStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(u"wrong.descriptor");
    int ret = stub.OnRemoteRequest(
        IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_COMMIT_LAYERS,
        data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_STATE);
}

/**
 * Function: OnRemoteRequest_ClearFrameBuffers_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. write correct interface token
 *                  2. invoke CLEAR_FRAME_BUFFERS code
 *                  3. expect OK and path executed
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, OnRemoteRequest_ClearFrameBuffers_Success, TestSize.Level1)
{
    TestRenderToComposerStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
    int ret = stub.OnRemoteRequest(
        IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_CLEAR_FRAME_BUFFERS,
        data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
}

/**
 * Function: OnRemoteRequest_CleanLayerBufferBySurfaceId_SuccessAndFail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. success: write token + surfaceId
 *                  2. fail: write token only, missing surfaceId
 *                  3. cover both branches of GetCleanLayerBufferSurfaceId
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, OnRemoteRequest_CleanLayerBufferBySurfaceId_SuccessAndFail, TestSize.Level1)
{
    TestRenderToComposerStub stub;
    MessageParcel reply;
    MessageOption opt;
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        data.WriteUint64(123u);
        int ret = stub.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_CLEAN_LAYER_BUFFER_BY_SURFACE_ID,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        int ret = stub.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_CLEAN_LAYER_BUFFER_BY_SURFACE_ID,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
}

/**
 * Function: OnRemoteRequest_ClearRedrawGPUCompositionCache_SuccessAndFail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. success: write token + bufferIds vector
 *                  2. fail: write token only (vector read fails)
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, OnRemoteRequest_ClearRedrawGPUCompositionCache_SuccessAndFail, TestSize.Level1)
{
    TestRenderToComposerStub stub;
    MessageParcel reply;
    MessageOption opt;
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        std::vector<uint64_t> ids {1u, 2u};
        data.WriteUInt64Vector(ids);
        int ret = stub.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_CLEAR_REDRAW_GPU_COMPOSITION_CACHE,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        int ret = stub.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_CLEAR_REDRAW_GPU_COMPOSITION_CACHE,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
}

/**
 * Function: OnRemoteRequest_SetBacklightLevel_SuccessAndFail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. success: write token + level
 *                  2. fail: write token only (level read fails)
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, OnRemoteRequest_SetBacklightLevel_SuccessAndFail, TestSize.Level1)
{
    TestRenderToComposerStub stub;
    MessageParcel reply;
    MessageOption opt;
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        data.WriteUint32(10u);
        int ret = stub.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_SET_BACKLIGHT_LEVEL,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        int ret = stub.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_SET_BACKLIGHT_LEVEL,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
}

/**
 * Function: OnRemoteRequest_DefaultCode_ReturnsBinderError
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. write correct token
 *                  2. invoke unknown code
 *                  3. expect COMPOSITOR_ERROR_BINDER_ERROR
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, OnRemoteRequest_DefaultCode_ReturnsBinderError, TestSize.Level1)
{
    TestRenderToComposerStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
    int ret = stub.OnRemoteRequest(999999u, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
}
/**
 * Function: Stub_ManualCommitParcel_ZeroPayload
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. craft correct token parcel with zero payload
 *                  2. invoke OnRemoteRequest for COMMIT_LAYERS
 *                  3. expect OK and stub committed_ set true
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, Stub_ManualCommitParcel_ZeroPayload, TestSize.Level1)
{
    TestRenderToComposerStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteUint64(0);
    data.WriteInt32(0);
    data.WriteUint64(0);
    data.WriteBool(false);
    data.WriteUint64(0);
    data.WriteInt64(0);
    data.WriteUint64(0);
    data.WriteUint64(0);
    data.WriteBool(false);
    data.WriteBool(false);
    data.WriteUint32(0);
    data.WriteUint64(0);
    data.WriteUint32(0);

    int ret = stub.OnRemoteRequest(
        IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_COMMIT_LAYERS,
        data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    EXPECT_TRUE(stub.committed_);
}
} // namespace OHOS::Rosen
