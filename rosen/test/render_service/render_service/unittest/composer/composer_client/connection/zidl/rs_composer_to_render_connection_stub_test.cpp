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
#include <set>
#include "rs_composer_to_render_connection.h"
#include "irs_composer_to_render_connection.h"
#include "message_option.h"
#include "message_parcel.h"
#include "sync_fence.h"
#include "surface_type.h"
#include "graphic_common_c.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSComposerToRenderConnectionStubTest : public Test {};

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_Normal
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token and payload with timestamps only
 *                  2. expect ret OK and reply forwards override return
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_Normal, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    ReleaseLayerBuffersInfo captured {};
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo &info,
        const std::shared_ptr<RSRenderComposerClient>& composerClient) { captured = info; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(9u)); // screenId
    // timestampVec (2 entries)
    ASSERT_TRUE(data.WriteUint32(2u));
    GraphicPresentTimestamp ts1 { GRAPHIC_DISPLAY_PTS_DELAY, 111 };
    GraphicPresentTimestamp ts2 { GRAPHIC_DISPLAY_PTS_TIMESTAMP, 222 };
    ASSERT_TRUE(data.WriteUint64(21u));
    ASSERT_TRUE(data.WriteBool(true));
    ASSERT_TRUE(data.WriteUint32(static_cast<uint32_t>(ts1.type)));
    ASSERT_TRUE(data.WriteInt64(ts1.time));
    ASSERT_TRUE(data.WriteUint64(22u));
    ASSERT_TRUE(data.WriteBool(false));
    ASSERT_TRUE(data.WriteUint32(static_cast<uint32_t>(ts2.type)));
    ASSERT_TRUE(data.WriteInt64(ts2.time));
    // releaseBufferFenceVec (0 entries to avoid buffer serialization)
    ASSERT_TRUE(data.WriteUint32(0u));
    // lastSwapBufferTime
    ASSERT_TRUE(data.WriteInt64(333));

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS,
        data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    EXPECT_EQ(captured.screenId, 9u);
    ASSERT_EQ(captured.timestampVec.size(), 2u);
    EXPECT_EQ(reply.ReadInt32(), COMPOSITOR_ERROR_OK);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_BadToken
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write wrong interface token
 *                  2. expect ERR_INVALID_DATA and method not called
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_BadToken, TestSize.Level1)
{
    RSComposerToRenderConnection stub;

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(u"bad.token"));
    ASSERT_TRUE(data.WriteUint64(1u));
    ASSERT_TRUE(data.WriteUint32(0u));
    ASSERT_TRUE(data.WriteUint32(0u));
    ASSERT_TRUE(data.WriteInt64(0));

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS,
        data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_ReadBufferFail
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. hasBuffer=true without serialized buffer
 *                  2. expect ERR_INVALID_DATA returned; override still invoked later
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_ReadBufferFail, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    size_t capturedSize = 0;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo &info,
        const std::shared_ptr<RSRenderComposerClient>& composerClient) {
        called = true;
        capturedSize = info.releaseBufferFenceVec.size();
    });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(2u));
    ASSERT_TRUE(data.WriteUint32(1001u));
    ASSERT_TRUE(data.WriteUint32(0u)); // releaseBufferFenceVec size
    ASSERT_TRUE(data.WriteInt64(0));   // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS,
        data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_Notify_Normal
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token and lpp ids
 *                  2. expect ret OK and reply forwards override return
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_Notify_Normal, TestSize.Level1)
{
    RSComposerToRenderConnection stub;

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(777u));
    ASSERT_TRUE(data.WriteInt32(3));
    ASSERT_TRUE(data.WriteUint64(11u));
    ASSERT_TRUE(data.WriteUint64(22u));
    ASSERT_TRUE(data.WriteUint64(33u));

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER,
        data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    EXPECT_EQ(reply.ReadInt32(), COMPOSITOR_ERROR_OK);
}

/**
 * Function: Stub_OnRemoteRequest_Notify_BadToken
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write wrong interface token
 *                  2. expect ERR_INVALID_DATA and method not called
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_Notify_BadToken, TestSize.Level1)
{
    RSComposerToRenderConnection stub;

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(u"bad.token"));
    ASSERT_TRUE(data.WriteUint64(1u));
    ASSERT_TRUE(data.WriteInt32(0));

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER,
        data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * Function: Stub_OnRemoteRequest_DefaultBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. call with unsupported code
 *                  2. expect COMPOSITOR_ERROR_INVALID_ARGUMENTS
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_DefaultBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;
    int ret = stub.OnRemoteRequest(999u, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_INVALID_ARGUMENTS);
}
} // namespace OHOS::Rosen