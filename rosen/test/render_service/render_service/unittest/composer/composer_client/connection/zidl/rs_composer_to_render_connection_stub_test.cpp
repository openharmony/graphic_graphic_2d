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
#include "rs_composer_to_render_connection_stub.h"
#include "message_parcel.h"
#include "sync_fence.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TestComposerToRenderStub : public RSComposerToRenderConnectionStub {
public:
    int32_t ReleaseLayerBuffers(ReleaseLayerBuffersInfo &info) override { return COMPOSITOR_ERROR_OK; }
    int32_t NotifyLppLayerToRender(uint64_t vsyncId, const std::set<uint64_t> &lppNodeIds) override
    {
        return COMPOSITOR_ERROR_OK;
    }
};

class RSComposerToRenderConnectionStubTest : public Test {};

/**
 * Function: Stub_InvalidToken_Branch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. craft MessageParcel with wrong interface token
 *                  2. invoke OnRemoteRequest with RELEASE_LAYER_BUFFERS code
 *                  3. expect non-OK error to cover invalid token branch
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_InvalidToken_Branch, TestSize.Level1)
{
    TestComposerToRenderStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    data.WriteInterfaceToken(u"wrong.descriptor");
    int ret = stub.OnRemoteRequest(
        RSIComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS,
        data, reply, opt);
    EXPECT_NE(ret, COMPOSITOR_ERROR_OK);
}

/**
 * Function: OnRemoteRequest_ReleaseLayerBuffers_Parse_AllPaths_NoBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. craft parcel with correct token
 *                  2. write screenId, timestampVec(1), releaseBufferFenceVec(1 with no buffer)
 *                  3. write lastSwapBufferTime
 *                  4. expect OK reply code path executed
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, OnRemoteRequest_ReleaseLayerBuffers_Parse_AllPaths_NoBuffer, TestSize.Level1)
{
    class DummyStub : public RSComposerToRenderConnectionStub {
    public:
        int32_t ReleaseLayerBuffers(ReleaseLayerBuffersInfo &info) override { return COMPOSITOR_ERROR_OK; }
        int32_t NotifyLppLayerToRender(uint64_t vsyncId, const std::set<uint64_t> &lppNodeIds) override { return COMPOSITOR_ERROR_OK; }
    };
    DummyStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    data.WriteInterfaceToken(RSIComposerToRenderConnection::GetDescriptor());
    data.WriteUint64(1u); // screenId
    data.WriteUint32(1u); // timestampVec size
    data.WriteUint64(10u); // layerId
    data.WriteBool(true); // IsSupportedPresentTimestamp
    data.WriteUint32(static_cast<uint32_t>(GRAPHIC_DISPLAY_PTS_TIMESTAMP));
    data.WriteInt64(123456);
    data.WriteUint32(1u); // releaseBufferFenceVec size
    data.WriteUint64(10u); // layerId
    data.WriteBool(false); // hasBuffer
    sptr<SyncFence> fence = new SyncFence(-1);
    fence->WriteToMessageParcel(data);
    data.WriteInt64(987654321LL); // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        RSIComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS,
        data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
}

/**
 * Function: OnRemoteRequest_ReleaseLayerBuffers_BufferPresent_ReadFail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. craft parcel with hasBuffer=true but missing buffer payload
 *                  2. expect stub returns ERR_INVALID_DATA due to ReadSurfaceBufferImpl failure
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, OnRemoteRequest_ReleaseLayerBuffers_BufferPresent_ReadFail, TestSize.Level1)
{
    class DummyStub : public RSComposerToRenderConnectionStub {
    public:
        int32_t ReleaseLayerBuffers(ReleaseLayerBuffersInfo &info) override { return COMPOSITOR_ERROR_OK; }
        int32_t NotifyLppLayerToRender(uint64_t vsyncId, const std::set<uint64_t> &lppNodeIds) override { return COMPOSITOR_ERROR_OK; }
    };
    DummyStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    data.WriteInterfaceToken(RSIComposerToRenderConnection::GetDescriptor());
    data.WriteUint64(1u); // screenId
    data.WriteUint32(0u); // timestampVec size
    data.WriteUint32(1u); // releaseBufferFenceVec size
    data.WriteUint64(10u); // layerId
    data.WriteBool(true); // hasBuffer=true but we won't write buffer content to trigger error
    // no buffer content here
    sptr<SyncFence> fence = new SyncFence(-1);
    fence->WriteToMessageParcel(data);
    data.WriteInt64(0); // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(RSIComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * Function: OnRemoteRequest_NotifyLppLayerToRender_Parse_AllPaths
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. craft parcel with correct token
 *                  2. write vsyncId, ids size and ids values
 *                  3. expect OK reply code path executed
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, OnRemoteRequest_NotifyLppLayerToRender_Parse_AllPaths, TestSize.Level1)
{
    class DummyStub : public RSComposerToRenderConnectionStub {
    public:
        int32_t ReleaseLayerBuffers(ReleaseLayerBuffersInfo &info) override { return COMPOSITOR_ERROR_OK; }
        int32_t NotifyLppLayerToRender(uint64_t vsyncId, const std::set<uint64_t> &lppNodeIds) override { return COMPOSITOR_ERROR_OK; }
    };
    DummyStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    data.WriteInterfaceToken(RSIComposerToRenderConnection::GetDescriptor());
    data.WriteUint64(100u); // vsyncId
    data.WriteInt32(2); // size
    data.WriteUint64(1u);
    data.WriteUint64(2u);
    int ret = stub.OnRemoteRequest(
        RSIComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER,
        data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
}

/**
 * Function: OnRemoteRequest_DefaultCode_ReturnsInvalidArguments
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. write correct token (not required by default)
 *                  2. invoke unknown code
 *                  3. expect COMPOSITOR_ERROR_INVALID_ARGUMENTS
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, OnRemoteRequest_DefaultCode_ReturnsInvalidArguments, TestSize.Level1)
{
    TestComposerToRenderStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    int ret = stub.OnRemoteRequest(999999u, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_INVALID_ARGUMENTS);
}
} // namespace OHOS::Rosen
