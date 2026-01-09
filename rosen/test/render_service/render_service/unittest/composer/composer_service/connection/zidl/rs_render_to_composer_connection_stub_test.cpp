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
#include <vector>
#include "rs_render_to_composer_connection.h"
#include "rs_render_composer_agent.h"
#include "rs_composer_to_render_connection.h"
#include "rs_render_to_composer_connection_stub.h"
#include "message_parcel.h"
#include "rs_layer_transaction_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
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
    RSRenderToComposerConnection conn("ut", 0u, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(u"wrong.descriptor");
    int ret = conn.OnRemoteRequest(
        IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_COMMIT_LAYERS,
        data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_STATE);
}

/**
 * Function: OnRemoteRequest_CommitLayers_UnmarshallingFail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. write correct token but no payload
 *                  2. expect binder error due to unmarshalling failure
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, OnRemoteRequest_CommitLayers_UnmarshallingFail, TestSize.Level1)
{
    RSRenderToComposerConnection conn("ut", 0u, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
    // No payload written -> Unmarshalling fails
    int ret = conn.OnRemoteRequest(
        IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_COMMIT_LAYERS,
        data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
}

/**
 * Function: OnRemoteRequest_CommitLayers_UnmarshallingSuccessButAgentReturnsFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. write minimal valid transaction payload
 *                  2. use non-null agent constructed with nullptr composer (returns false)
 *                  3. expect binder error
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, OnRemoteRequest_CommitLayers_UnmarshallingSuccessButAgentReturnsFalse, TestSize.Level1)
{
    auto agent = std::make_shared<RSRenderComposerAgent>(nullptr);
    RSRenderToComposerConnection conn("ut", 0u, agent);
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
    // Write minimal RSLayerTransactionData payload
    data.WriteInt32(0); // payload size
    data.WriteUint64(0); // timestamp
    data.WriteInt32(0); // pid
    data.WriteUint64(0); // index
    // ComposerScreenInfo
    data.WriteUint64(0); // id
    data.WriteUint32(0); // width
    data.WriteUint32(0); // height
    data.WriteUint32(0); // phyWidth
    data.WriteUint32(0); // phyHeight
    data.WriteBool(false); // isSamplingOn
    data.WriteFloat(0.0f); // samplingTranslateX
    data.WriteFloat(0.0f); // samplingTranslateY
    data.WriteFloat(1.0f); // samplingScale
    // activeRect
    data.WriteInt32(0); data.WriteInt32(0); data.WriteInt32(0); data.WriteInt32(0);
    // maskRect
    data.WriteInt32(0); data.WriteInt32(0); data.WriteInt32(0); data.WriteInt32(0);
    // reviseRect
    data.WriteInt32(0); data.WriteInt32(0); data.WriteInt32(0); data.WriteInt32(0);
    // PipelineParam
    data.WriteUint64(0); // frameTimestamp
    data.WriteInt64(0); // actualTimestamp
    data.WriteUint64(0); // fastComposeTimeStampDiff
    data.WriteUint64(0); // vsyncId
    data.WriteBool(false); // isForceRefresh
    data.WriteBool(false); // hasGameScene
    data.WriteUint32(0); // pendingScreenRefreshRate
    data.WriteUint64(0); // pendingConstraintRelativeTime
    data.WriteUint32(0); // SurfaceFpsOpNum

    int ret = conn.OnRemoteRequest(
        IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_COMMIT_LAYERS,
        data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
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
    RSRenderToComposerConnection conn("ut", 0u, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
    int ret = conn.OnRemoteRequest(
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
    RSRenderToComposerConnection conn("ut", 0u, nullptr);
    MessageParcel reply;
    MessageOption opt;
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        data.WriteUint64(123u);
        int ret = conn.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_CLEAN_LAYER_BUFFER_BY_SURFACE_ID,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        int ret = conn.OnRemoteRequest(
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
    RSRenderToComposerConnection conn("ut", 0u, nullptr);
    MessageParcel reply;
    MessageOption opt;
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        std::vector<uint64_t> ids {1u, 2u};
        data.WriteUInt64Vector(ids);
        int ret = conn.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_CLEAR_REDRAW_GPU_COMPOSITION_CACHE,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        int ret = conn.OnRemoteRequest(
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
    RSRenderToComposerConnection conn("ut", 0u, nullptr);
    MessageParcel reply;
    MessageOption opt;
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        data.WriteUint32(10u);
        int ret = conn.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_SET_BACKLIGHT_LEVEL,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        int ret = conn.OnRemoteRequest(
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
    RSRenderToComposerConnection conn("ut", 0u, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
    int ret = conn.OnRemoteRequest(999999u, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
}

/**
 * Function: OnRemoteRequest_SetComposerToRenderConnection_FailAndSuccess
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. fail: write token + null remote object
 *                  2. success: write token + valid remote object
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, OnRemoteRequest_SetComposerToRenderConnection_FailAndSuccess, TestSize.Level1)
{
    RSRenderToComposerConnection conn("ut", 0u, nullptr);
    MessageParcel reply;
    MessageOption opt;
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        data.WriteRemoteObject(nullptr);
        int ret = conn.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_SET_COMPOSER_TO_RENDER_CONNECTION,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
    }
    {
        MessageParcel data;
        data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
        sptr<RSComposerToRenderConnection> ctr = sptr<RSComposerToRenderConnection>::MakeSptr();
        data.WriteRemoteObject(ctr->AsObject());
        int ret = conn.OnRemoteRequest(
            IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_SET_COMPOSER_TO_RENDER_CONNECTION,
            data, reply, opt);
        EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    }
}

/**
 * Function: OnRemoteRequest_PreAllocProtectedFrameBuffers_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. write token only (no buffer serialized)
 *                  2. expect binder error to cover read fail branch
 */
HWTEST_F(RSRenderToComposerConnectionStubTest, OnRemoteRequest_PreAllocProtectedFrameBuffers_Fail, TestSize.Level1)
{
    RSRenderToComposerConnection conn("ut", 0u, nullptr);
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(IRSRenderToComposerConnection::GetDescriptor());
    int ret = conn.OnRemoteRequest(
        IRSRenderToComposerConnection::IRENDER_TO_COMPOSER_CONNECTION_PREALLOC_PROTECTED_FRAME_BUFFERS,
        data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
}
} // namespace OHOS::Rosen
