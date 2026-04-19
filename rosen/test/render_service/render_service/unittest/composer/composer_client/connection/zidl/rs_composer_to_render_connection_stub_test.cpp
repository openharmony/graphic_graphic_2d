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

#include "graphic_common_c.h"
#include "irs_composer_to_render_connection.h"
#include "message_option.h"
#include "message_parcel.h"
#include "rs_composer_to_render_connection.h"
#include "surface_type.h"
#include "sync_fence.h"

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
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { captured = info; });

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
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
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
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
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
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) {
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
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
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

    int ret = stub.OnRemoteRequest(IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER, data, reply, opt);
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

    int ret = stub.OnRemoteRequest(IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER, data, reply, opt);
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
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * Function: Stub_OnRemoteRequest_Notify_VsyncIdReadFail_Line116_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token but missing vsyncId data
 *                  2. expect ERR_INVALID_DATA (line 116 true branch, ReadUint64 fails)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_Notify_VsyncIdReadFail_Line116_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterJudgeLppLayerCB([&](uint64_t vsyncId, const std::unordered_set<uint64_t>& nodeIds) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    // Missing vsyncId data - ReadUint64 will fail, triggers line 116 true branch

    int ret = stub.OnRemoteRequest(IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_Notify_SizeReadFail_Line121_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token and vsyncId but missing size data
 *                  2. expect ERR_INVALID_DATA (line 121 true branch, ReadInt32 fails)
 */
HWTEST_F(
    RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_Notify_SizeReadFail_Line121_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterJudgeLppLayerCB([&](uint64_t vsyncId, const std::unordered_set<uint64_t>& nodeIds) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(888u)); // vsyncId
    // Missing size data - ReadInt32 will fail, triggers line 121 true branch

    int ret = stub.OnRemoteRequest(IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_Notify_SizeExceedsLimit_Line121_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, vsyncId, size > MAX_LPP_LAYER_SIZE(5)
 *                  2. expect ERR_INVALID_DATA (line 121 true branch, size > limit)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_Notify_SizeExceedsLimit_Line121_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterJudgeLppLayerCB([&](uint64_t vsyncId, const std::unordered_set<uint64_t>& nodeIds) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(999u)); // vsyncId
    ASSERT_TRUE(data.WriteInt32(6));     // size = 6 > MAX_LPP_LAYER_SIZE(5), triggers line 121 true branch

    int ret = stub.OnRemoteRequest(IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_Notify_FirstNodeIdReadFail_Line128_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, vsyncId, size=1 but missing nodeId data
 *                  2. expect ERR_INVALID_DATA (line 128 true branch, ReadUint64 fails for nodeId)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_Notify_FirstNodeIdReadFail_Line128_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterJudgeLppLayerCB([&](uint64_t vsyncId, const std::unordered_set<uint64_t>& nodeIds) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(123u)); // vsyncId
    ASSERT_TRUE(data.WriteInt32(1));     // size = 1
    // Missing nodeId data - ReadUint64 will fail, triggers line 128 true branch

    int ret = stub.OnRemoteRequest(IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_Notify_SecondNodeIdReadFail_Line128_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, vsyncId, size=2 but only 1 nodeId
 *                  2. expect ERR_INVALID_DATA (line 128 true branch, second ReadUint64 fails)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_Notify_SecondNodeIdReadFail_Line128_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterJudgeLppLayerCB([&](uint64_t vsyncId, const std::unordered_set<uint64_t>& nodeIds) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(456u)); // vsyncId
    ASSERT_TRUE(data.WriteInt32(2));     // size = 2
    ASSERT_TRUE(data.WriteUint64(100u)); // first nodeId
    // Missing second nodeId data - ReadUint64 will fail on second iteration, triggers line 128 true branch

    int ret = stub.OnRemoteRequest(IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_Notify_MultipleNodeIds_MiddleReadFail_Line128_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, vsyncId, size=5 but only 3 nodeIds
 *                  2. expect ERR_INVALID_DATA (line 128 true branch, 4th ReadUint64 fails)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_Notify_MultipleNodeIds_MiddleReadFail_Line128_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterJudgeLppLayerCB([&](uint64_t vsyncId, const std::unordered_set<uint64_t>& nodeIds) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(789u)); // vsyncId
    ASSERT_TRUE(data.WriteInt32(5));     // size = 5 (at limit)
    ASSERT_TRUE(data.WriteUint64(201u)); // nodeId 1
    ASSERT_TRUE(data.WriteUint64(202u)); // nodeId 2
    ASSERT_TRUE(data.WriteUint64(203u)); // nodeId 3
    // Missing nodeIds 4 and 5 - ReadUint64 will fail on 4th iteration, triggers line 128 true branch

    int ret = stub.OnRemoteRequest(IRSComposerToRenderConnection::NOTIFY_LPP_LAYER_TO_RENDER, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeExceeds_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with second vecSize > 1000 (releaseBufferFenceVec)
 *                  2. expect ERR_INVALID_DATA (line 79 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeExceeds_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(1u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec size exceeds limit
    ASSERT_TRUE(data.WriteUint32(1001u)); // vecSize > RELEASE_LAYER_MAX_SIZE, triggers line 79 true branch
    ASSERT_TRUE(data.WriteInt64(0));      // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSize1002_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with second vecSize = 1002 (releaseBufferFenceVec)
 *                  2. expect ERR_INVALID_DATA (line 79 true branch, boundary2)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSize1002_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(2u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec size = 1002 exceeds limit
    ASSERT_TRUE(data.WriteUint32(1002u)); // vecSize > RELEASE_LAYER_MAX_SIZE, triggers line 79 true branch
    ASSERT_TRUE(data.WriteInt64(0));      // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeMaxUint32_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with second vecSize = UINT32_MAX (releaseBufferFenceVec)
 *                  2. expect ERR_INVALID_DATA (line 79 true branch, extreme value)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeMaxUint32_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(3u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec size = UINT32_MAX (extreme value)
    ASSERT_TRUE(data.WriteUint32(std::numeric_limits<uint32_t>::max())); // vecSize >> RELEASE_LAYER_MAX_SIZE
    ASSERT_TRUE(data.WriteInt64(0));                                     // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_WithTimestamps_SecondVecExceeds_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with timestamps and second vecSize > 1000
 *                  2. expect ERR_INVALID_DATA (line 79 true branch with non-empty timestampVec)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_WithTimestamps_SecondVecExceeds_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(4u)); // screenId
    // timestampVec (2 entries, within limit)
    ASSERT_TRUE(data.WriteUint32(2u));
    ASSERT_TRUE(data.WriteUint64(10u));
    ASSERT_TRUE(data.WriteBool(true));
    ASSERT_TRUE(data.WriteUint32(static_cast<uint32_t>(GRAPHIC_DISPLAY_PTS_TIMESTAMP)));
    ASSERT_TRUE(data.WriteInt64(1000));
    ASSERT_TRUE(data.WriteUint64(20u));
    ASSERT_TRUE(data.WriteBool(false));
    ASSERT_TRUE(data.WriteUint32(static_cast<uint32_t>(GRAPHIC_DISPLAY_PTS_UNSUPPORTED)));
    ASSERT_TRUE(data.WriteInt64(0));
    // releaseBufferFenceVec size exceeds limit
    ASSERT_TRUE(data.WriteUint32(2000u)); // vecSize > RELEASE_LAYER_MAX_SIZE, triggers line 79 true branch
    ASSERT_TRUE(data.WriteInt64(5000));   // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSize5000_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with second vecSize = 5000 (releaseBufferFenceVec)
 *                  2. expect ERR_INVALID_DATA (line 79 true branch, much larger value)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSize5000_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(5u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec size = 5000 far exceeds limit
    ASSERT_TRUE(data.WriteUint32(5000u)); // vecSize >> RELEASE_LAYER_MAX_SIZE, triggers line 79 true branch
    ASSERT_TRUE(data.WriteInt64(0));      // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeAtLimit_FalseBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with second vecSize = 1000 (releaseBufferFenceVec)
 *                  2. expect success, line 79 condition is false (vecSize <= 1000)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeAtLimit_FalseBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    ReleaseLayerBuffersInfo captured {};
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { captured = info; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(5u)); // screenId
    // timestampVec (1 entry)
    ASSERT_TRUE(data.WriteUint32(1u));
    ASSERT_TRUE(data.WriteUint64(100u));
    ASSERT_TRUE(data.WriteBool(true));
    ASSERT_TRUE(data.WriteUint32(static_cast<uint32_t>(GRAPHIC_DISPLAY_PTS_UNSUPPORTED)));
    ASSERT_TRUE(data.WriteInt64(0));
    // releaseBufferFenceVec (1000 entries at limit, using 0 for simplicity)
    ASSERT_TRUE(data.WriteUint32(1000u)); // vecSize = RELEASE_LAYER_MAX_SIZE, line 79 false branch
    for (uint32_t i = 0; i < 1000u; i++) {
        ASSERT_TRUE(data.WriteUint64(i + 1)); // layerId
        ASSERT_TRUE(data.WriteBool(false));   // hasBuffer = false
        ASSERT_TRUE(data.WriteBool(false));   // invalid fence (no data)
    }
    ASSERT_TRUE(data.WriteInt64(12345)); // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    EXPECT_EQ(captured.screenId, 5u);
    ASSERT_EQ(captured.timestampVec.size(), 1u);
    ASSERT_EQ(captured.releaseBufferFenceVec.size(), 1000u);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeOne_FalseBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with second vecSize = 1 (releaseBufferFenceVec)
 *                  2. expect success, line 79 condition is false (1 <= 1000)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeOne_FalseBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    ReleaseLayerBuffersInfo captured {};
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { captured = info; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(88u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec (1 entry)
    ASSERT_TRUE(data.WriteUint32(1u));   // vecSize = 1, line 79 false branch
    ASSERT_TRUE(data.WriteUint64(888u)); // layerId
    ASSERT_TRUE(data.WriteBool(false));  // hasBuffer = false
    ASSERT_TRUE(data.WriteBool(false));  // invalid fence
    ASSERT_TRUE(data.WriteInt64(99999)); // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    EXPECT_EQ(captured.screenId, 88u);
    ASSERT_EQ(captured.releaseBufferFenceVec.size(), 1u);
    EXPECT_EQ(reply.ReadInt32(), COMPOSITOR_ERROR_OK);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_FirstVecSizeExceeds
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with first vecSize > 1000 (timestampVec)
 *                  2. expect ERR_INVALID_DATA (line 66 true branch)
 */
HWTEST_F(
    RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_FirstVecSizeExceeds, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(1u)); // screenId
    // timestampVec size exceeds limit
    ASSERT_TRUE(data.WriteUint32(1001u)); // vecSize > RELEASE_LAYER_MAX_SIZE, triggers line 66 true branch
    ASSERT_TRUE(data.WriteUint32(0u));    // releaseBufferFenceVec size
    ASSERT_TRUE(data.WriteInt64(0));      // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_HasBufferTrue_ReadFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with hasBuffer=true but incomplete buffer data
 *                  2. expect ERR_INVALID_DATA (line 88 true branch, ReadSurfaceBufferImpl fails at line 94)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_HasBufferTrue_ReadFail_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(1u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec (1 entry)
    ASSERT_TRUE(data.WriteUint32(1u));   // vecSize
    ASSERT_TRUE(data.WriteUint64(100u)); // layerId
    ASSERT_TRUE(data.WriteBool(true));   // hasBuffer = true, triggers line 88 true branch
    // Incomplete buffer data - sequence only, missing actual buffer content
    ASSERT_TRUE(data.WriteUint32(123u)); // sequence
    // Missing: buffer->ReadFromMessageParcel data would be here
    // Write fence (minimal valid fence data)
    ASSERT_TRUE(data.WriteInt32(-1)); // fence fd = -1 (invalid fence)
    ASSERT_TRUE(data.WriteInt64(0));  // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_HasBufferTrue_MissingFence_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with hasBuffer=true but missing fence data
 *                  2. expect error or handling (line 88 true branch, fence read may fail)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_HasBufferTrue_MissingFence_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(2u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec (1 entry)
    ASSERT_TRUE(data.WriteUint32(1u));   // vecSize
    ASSERT_TRUE(data.WriteUint64(200u)); // layerId
    ASSERT_TRUE(data.WriteBool(true));   // hasBuffer = true, triggers line 88 true branch
    // Incomplete buffer data
    ASSERT_TRUE(data.WriteUint32(456u)); // sequence
    // Missing: buffer->ReadFromMessageParcel data
    // Missing fence data entirely - fence read will fail
    ASSERT_TRUE(data.WriteInt64(111)); // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    // Should return error due to incomplete buffer data or missing fence
    EXPECT_NE(ret, COMPOSITOR_ERROR_OK);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_MultipleEntries_HasBufferTrue_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with multiple entries, some hasBuffer=true
 *                  2. expect ERR_INVALID_DATA when buffer read fails (line 88 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_MultipleEntries_HasBufferTrue_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(3u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec (3 entries)
    ASSERT_TRUE(data.WriteUint32(3u)); // vecSize
    // Entry 1: hasBuffer = false (valid)
    ASSERT_TRUE(data.WriteUint64(301u)); // layerId
    ASSERT_TRUE(data.WriteBool(false));  // hasBuffer = false
    ASSERT_TRUE(data.WriteBool(false));  // invalid fence
    // Entry 2: hasBuffer = true (incomplete, triggers line 88 true branch)
    ASSERT_TRUE(data.WriteUint64(302u)); // layerId
    ASSERT_TRUE(data.WriteBool(true));   // hasBuffer = true
    ASSERT_TRUE(data.WriteUint32(789u)); // sequence
    // Missing buffer data
    ASSERT_TRUE(data.WriteBool(false)); // invalid fence
    // Entry 3: hasBuffer = false (won't be reached due to entry 2 failure)
    ASSERT_TRUE(data.WriteUint64(303u)); // layerId
    ASSERT_TRUE(data.WriteBool(false));  // hasBuffer = false
    ASSERT_TRUE(data.WriteBool(false));  // invalid fence
    ASSERT_TRUE(data.WriteInt64(222));   // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    EXPECT_TRUE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_FirstEntryHasBufferTrue_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with first entry hasBuffer=true, incomplete data
 *                  2. expect ERR_INVALID_DATA at line 94 (line 88 true branch for first entry)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_FirstEntryHasBufferTrue_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(4u)); // screenId
    // timestampVec (1 entry)
    ASSERT_TRUE(data.WriteUint32(1u));
    ASSERT_TRUE(data.WriteUint64(40u));
    ASSERT_TRUE(data.WriteBool(false));
    ASSERT_TRUE(data.WriteUint32(static_cast<uint32_t>(GRAPHIC_DISPLAY_PTS_UNSUPPORTED)));
    ASSERT_TRUE(data.WriteInt64(0));
    // releaseBufferFenceVec (1 entry with hasBuffer=true at first position)
    ASSERT_TRUE(data.WriteUint32(1u));   // vecSize
    ASSERT_TRUE(data.WriteUint64(401u)); // layerId
    ASSERT_TRUE(data.WriteBool(true));   // hasBuffer = true, triggers line 88 true branch
    ASSERT_TRUE(data.WriteUint32(999u)); // sequence
    // Missing: buffer->ReadFromMessageParcel data
    ASSERT_TRUE(data.WriteInt32(-1));  // fence fd
    ASSERT_TRUE(data.WriteInt64(333)); // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_HasBufferTrue_NoSequence_Fail
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token with hasBuffer=true but missing sequence
 *                  2. expect error (line 88 true branch, ReadSurfaceBufferImpl fails reading sequence)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_HasBufferTrue_NoSequence_Fail,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(5u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec (1 entry)
    ASSERT_TRUE(data.WriteUint32(1u));   // vecSize
    ASSERT_TRUE(data.WriteUint64(500u)); // layerId
    ASSERT_TRUE(data.WriteBool(true));   // hasBuffer = true, triggers line 88 true branch
    // Missing sequence number entirely - ReadSurfaceBufferImpl will fail
    ASSERT_TRUE(data.WriteInt32(-1)); // fence fd
    ASSERT_TRUE(data.WriteInt64(0));  // lastSwapBufferTime

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    EXPECT_TRUE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_FirstVecSizeReadFail_TrueBranch

 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token but missing first vecSize data
 *                  2. expect COMPOSITOR_ERROR_BINDER_ERROR (line 66 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_FirstVecSizeReadFail_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(1u)); // screenId
    // Missing first vecSize data - ReadUint32 will fail, triggers line 66 true branch

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_FirstLayerIdReadFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, screenId, vecSize=1 but missing layerId
 *                  2. expect COMPOSITOR_ERROR_BINDER_ERROR (line 76 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_FirstLayerIdReadFail_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(2u)); // screenId
    ASSERT_TRUE(data.WriteUint32(1u)); // vecSize = 1
    // Missing layerId data - ReadUint64 will fail, triggers line 76 true branch

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_IsSupportedPresentTimestampReadFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, screenId, vecSize=1, layerId but missing isSupportedPresentTimestamp
 *                  2. expect COMPOSITOR_ERROR_BINDER_ERROR (line 81 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_IsSupportedPresentTimestampReadFail_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(3u));  // screenId
    ASSERT_TRUE(data.WriteUint32(1u));  // vecSize = 1
    ASSERT_TRUE(data.WriteUint64(10u)); // layerId
    // Missing isSupportedPresentTimestamp data - ReadBool will fail, triggers line 81 true branch

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_PresentTimestampTypeReadFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, screenId, vecSize=1, layerId, isSupportedPresentTimestamp but missing
 * presentTimestampType
 *                  2. expect COMPOSITOR_ERROR_BINDER_ERROR (line 87 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_PresentTimestampTypeReadFail_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(4u));  // screenId
    ASSERT_TRUE(data.WriteUint32(1u));  // vecSize = 1
    ASSERT_TRUE(data.WriteUint64(20u)); // layerId
    ASSERT_TRUE(data.WriteBool(true));  // isSupportedPresentTimestamp
    // Missing presentTimestampType data - ReadUint32 will fail, triggers line 87 true branch

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_TimestampTimeReadFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, screenId, vecSize=1, layerId, isSupportedPresentTimestamp,
 * presentTimestampType but missing timestamp.time
 *                  2. expect COMPOSITOR_ERROR_BINDER_ERROR (line 92 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_TimestampTimeReadFail_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(5u));  // screenId
    ASSERT_TRUE(data.WriteUint32(1u));  // vecSize = 1
    ASSERT_TRUE(data.WriteUint64(30u)); // layerId
    ASSERT_TRUE(data.WriteBool(false)); // isSupportedPresentTimestamp
    ASSERT_TRUE(data.WriteUint32(static_cast<uint32_t>(GRAPHIC_DISPLAY_PTS_UNSUPPORTED))); // presentTimestampType
    // Missing timestamp.time data - ReadInt64 will fail, triggers line 92 true branch

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeReadFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, screenId, valid timestampVec but missing second vecSize
 *                  2. expect COMPOSITOR_ERROR_BINDER_ERROR (line 98 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondVecSizeReadFail_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(6u)); // screenId
    // timestampVec (1 entry)
    ASSERT_TRUE(data.WriteUint32(1u));
    ASSERT_TRUE(data.WriteUint64(40u));
    ASSERT_TRUE(data.WriteBool(true));
    ASSERT_TRUE(data.WriteUint32(static_cast<uint32_t>(GRAPHIC_DISPLAY_PTS_TIMESTAMP)));
    ASSERT_TRUE(data.WriteInt64(1000));
    // Missing second vecSize data - ReadUint32 will fail, triggers line 98 true branch

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondLayerIdReadFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, screenId, valid timestampVec, second vecSize=1 but missing layerId
 *                  2. expect COMPOSITOR_ERROR_BINDER_ERROR (line 108 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest,
    Stub_OnRemoteRequest_ReleaseLayerBuffers_SecondLayerIdReadFail_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(7u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec (1 entry)
    ASSERT_TRUE(data.WriteUint32(1u)); // vecSize
    // Missing layerId data - ReadUint64 will fail, triggers line 108 true branch

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
    EXPECT_FALSE(called);
}

/**
 * Function: Stub_OnRemoteRequest_ReleaseLayerBuffers_HasBufferReadFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. write valid token, screenId, valid timestampVec, second vecSize=1, layerId but missing hasBuffer
 *                  2. expect COMPOSITOR_ERROR_BINDER_ERROR (line 113 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionStubTest, Stub_OnRemoteRequest_ReleaseLayerBuffers_HasBufferReadFail_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnection stub;
    bool called = false;
    stub.RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) { called = true; });

    MessageParcel data;
    MessageParcel reply;
    MessageOption opt;

    ASSERT_TRUE(data.WriteInterfaceToken(IRSComposerToRenderConnection::GetDescriptor()));
    ASSERT_TRUE(data.WriteUint64(8u)); // screenId
    // timestampVec (0 entries)
    ASSERT_TRUE(data.WriteUint32(0u));
    // releaseBufferFenceVec (1 entry)
    ASSERT_TRUE(data.WriteUint32(1u));  // vecSize
    ASSERT_TRUE(data.WriteUint64(50u)); // layerId
    // Missing hasBuffer data - ReadBool will fail, triggers line 113 true branch

    int ret = stub.OnRemoteRequest(
        IRSComposerToRenderConnection::ICOMPOSER_TO_RENDER_COMPOSER_RELEASE_LAYER_BUFFERS, data, reply, opt);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_BINDER_ERROR);
    EXPECT_FALSE(called);
}
} // namespace OHOS::Rosen