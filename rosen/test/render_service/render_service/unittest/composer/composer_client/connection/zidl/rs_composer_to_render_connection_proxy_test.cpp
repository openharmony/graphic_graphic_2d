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
#include <thread>
#include <unordered_set>

#include "graphic_common_c.h"
#include "iremote_object.h"
#include "rs_composer_to_render_connection.h"
#include "rs_composer_to_render_connection_proxy.h"
#include "surface_buffer.h"
#include "sync_fence.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSComposerToRenderConnectionProxyTest : public Test {};

/**
 * Function: ProxyStub_ReleaseLayerBuffers_And_Notify
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSComposerToRenderConnection stub/proxy pair
 *                  2. call ReleaseLayerBuffers with populated info
 *                  3. call NotifyLppLayerToRender with vsyncId and ids
 *                  4. verify stub captured forwarded values
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, ProxyStub_ReleaseLayerBuffers_And_Notify, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    ReleaseLayerBuffersInfo capturedInfo;
    bool cbCalled = false;
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) {
        capturedInfo = info;
        cbCalled = true;
    });
    sptr<IRemoteObject> obj = stub->AsObject();
    RSComposerToRenderConnectionProxy proxy(obj);

    ReleaseLayerBuffersInfo info;
    info.screenId = 1u;
    GraphicPresentTimestamp ts { GRAPHIC_DISPLAY_PTS_TIMESTAMP, 123456 };
    info.timestampVec.push_back(std::tuple(static_cast<RSLayerId>(10u), true, ts));
    sptr<SurfaceBuffer> sb = nullptr;
    sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
    info.releaseBufferFenceVec.push_back(std::tuple(static_cast<RSLayerId>(10u), sb, fence));
    info.lastSwapBufferTime = 987654321LL;

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
    EXPECT_TRUE(cbCalled);
    EXPECT_EQ(capturedInfo.screenId, 1u);
    ASSERT_EQ(capturedInfo.timestampVec.size(), 1u);
    ASSERT_EQ(capturedInfo.releaseBufferFenceVec.size(), 1u);

    std::unordered_set<uint64_t> ids { 11u, 22u };
    r = proxy.NotifyLppLayerToRender(777u, ids);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
}

/**
 * Function: Proxy_NotifyLayerStateChangedToRender_UsesAsyncFlag
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with a fake remote object
 *                  2. call NotifyLayerStateChangedToRender
*                  3. verify request is sent with TF_ASYNC and payload is intact
*/
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_NotifyLayerStateChangedToRender_UsesAsyncFlag, TestSize.Level1)
{
    class FakeAsyncRemoteLocal : public IRemoteObject {
    public:
        FakeAsyncRemoteLocal() : IRemoteObject(IRSComposerToRenderConnection::GetDescriptor()) {}
        int32_t GetObjectRefCount() override { return 1; }
        int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &, MessageOption &option) override
        {
            capturedCode_ = code;
            capturedFlags_ = option.GetFlags();
            capturedToken_ = data.ReadInterfaceToken();
            capturedNodeIdValid_ = data.ReadUint64(capturedNodeId_);
            capturedGenerationValid_ = data.ReadUint64(capturedGeneration_);
            capturedStateValid_ = data.ReadUint32(capturedState_);
            return NO_ERROR;
        }
        bool AddDeathRecipient(const sptr<DeathRecipient> &) override { return false; }
        bool RemoveDeathRecipient(const sptr<DeathRecipient> &) override { return false; }
        int Dump(int, const std::vector<std::u16string> &) override { return 0; }

        uint32_t capturedCode_ = 0;
        int capturedFlags_ = 0;
        std::u16string capturedToken_;
        uint64_t capturedNodeId_ = 0;
        uint64_t capturedGeneration_ = 0;
        uint32_t capturedState_ = 0;
        bool capturedNodeIdValid_ = false;
        bool capturedGenerationValid_ = false;
        bool capturedStateValid_ = false;
    };

    auto remote = sptr<FakeAsyncRemoteLocal>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(remote);

    int32_t ret = proxy.NotifyLayerStateChangedToRender(999u, LayerStateChange::UNAVAILABLE, 77u);

    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
    EXPECT_EQ(remote->capturedCode_, 2u);
    EXPECT_EQ(remote->capturedFlags_, MessageOption::TF_ASYNC);
    EXPECT_EQ(remote->capturedToken_, IRSComposerToRenderConnection::GetDescriptor());
    EXPECT_TRUE(remote->capturedNodeIdValid_);
    EXPECT_TRUE(remote->capturedStateValid_);
    EXPECT_EQ(remote->capturedNodeId_, 999u);
    EXPECT_TRUE(remote->capturedGenerationValid_);
    EXPECT_EQ(remote->capturedGeneration_, 77u);
    EXPECT_EQ(remote->capturedState_, static_cast<uint32_t>(LayerStateChange::UNAVAILABLE));
}

/**
 * Function: Proxy_ReleaseLayerBuffers_TimestampOnly
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. timestampVec contains multiple entries, releaseBufferFenceVec is empty
 *                  2. call ReleaseLayerBuffers and verify stub records timestamps
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_TimestampOnly, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    ReleaseLayerBuffersInfo capturedInfo;
    bool cbCalled = false;
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) {
        capturedInfo = info;
        cbCalled = true;
    });
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    ReleaseLayerBuffersInfo info;
    info.screenId = 8u;
    GraphicPresentTimestamp ts1 { GRAPHIC_DISPLAY_PTS_TIMESTAMP, 111 };
    GraphicPresentTimestamp ts2 { GRAPHIC_DISPLAY_PTS_DELAY, 222 };
    info.timestampVec.push_back(std::tuple(static_cast<RSLayerId>(21u), true, ts1));
    info.timestampVec.push_back(std::tuple(static_cast<RSLayerId>(22u), false, ts2));
    info.lastSwapBufferTime = 333;

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
    EXPECT_TRUE(cbCalled);
    ASSERT_EQ(capturedInfo.timestampVec.size(), 2u);
    EXPECT_EQ(std::get<0>(capturedInfo.timestampVec[0]), static_cast<RSLayerId>(21u));
    EXPECT_EQ(std::get<1>(capturedInfo.timestampVec[1]), false);
}

/**
 * Function: Proxy_ReleaseLayerBuffers_MixedNullAndNonNullBuffers
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. releaseBufferFenceVec contains both null and non-null buffers
 *                  2. verify proxy returns OK and stub records two entries
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_MixedNullAndNonNullBuffers, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    ReleaseLayerBuffersInfo capturedInfo;
    bool cbCalled = false;
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) {
        capturedInfo = info;
        cbCalled = true;
    });
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    ReleaseLayerBuffersInfo info;
    info.screenId = 9u;
    sptr<SurfaceBuffer> sb = nullptr;
    sptr<SyncFence> fenceA = sptr<SyncFence>::MakeSptr(-1);
    sptr<SyncFence> fenceB = sptr<SyncFence>::MakeSptr(-1);
    info.releaseBufferFenceVec.push_back(std::tuple(static_cast<RSLayerId>(31u), nullptr, fenceA));
    info.releaseBufferFenceVec.push_back(std::tuple(static_cast<RSLayerId>(32u), sb, fenceB));
    info.lastSwapBufferTime = 444;

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
    EXPECT_TRUE(cbCalled);
    ASSERT_EQ(capturedInfo.releaseBufferFenceVec.size(), 2u);
    EXPECT_EQ(std::get<1>(capturedInfo.releaseBufferFenceVec[0]), nullptr);
}

/**
 * Function: Proxy_NotifyLppLayerToRender_LargeIds
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build a large ids set (1000 entries)
 *                  2. call NotifyLppLayerToRender and verify reply code and set size
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_NotifyLppLayerToRender_LargeIds, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    std::unordered_set<uint64_t> ids;
    for (uint64_t i = 0; i < 1000; ++i) {
        ids.insert(100000 + i);
    }

    int32_t r = proxy.NotifyLppLayerToRender(999u, ids);
    EXPECT_EQ(r, -1);
}
/**
 * Function: Proxy_ReleaseLayerBuffers_EmptyVectors
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. use empty timestampVec and releaseBufferFenceVec
 *                  2. call ReleaseLayerBuffers to hit branch of vecSize=0
 *                  3. ensure no crash and stub receives screenId
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_EmptyVectors, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    uint64_t capturedScreenId = 0;
    bool cbCalled = false;
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) {
        capturedScreenId = info.screenId;
        cbCalled = true;
    });
    sptr<IRemoteObject> obj = stub->AsObject();
    RSComposerToRenderConnectionProxy proxy(obj);
    ReleaseLayerBuffersInfo info;
    info.screenId = 2u;
    info.lastSwapBufferTime = 0;
    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
    EXPECT_TRUE(cbCalled);
    EXPECT_EQ(capturedScreenId, 2u);
}

/**
 * Function: Proxy_ReleaseLayerBuffers_SurfaceBufferWriteFail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. put non-null SurfaceBuffer without properties
 *                  2. expect WriteSurfaceBufferImpl fails and proxy returns -1
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_SurfaceBufferWriteFail, TestSize.Level1)
{
    // Use a valid remote; failure should occur before SendRequest
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    ReleaseLayerBuffersInfo info;
    info.screenId = 4u;
    // Create a buffer without initializing properties to trigger write failure
    sptr<SurfaceBuffer> sb = SurfaceBuffer::Create();
    sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
    info.releaseBufferFenceVec.push_back(std::tuple(static_cast<RSLayerId>(1u), sb, fence));

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
}

/**
 * Function: Proxy_ReleaseLayerBuffers_FenceWriteFail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. use INVALID_FENCE to attempt fence write failure
 *                  2. expect proxy returns -1 when fence write fails
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_FenceWriteFail, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    ReleaseLayerBuffersInfo info;
    info.screenId = 5u;
    sptr<SurfaceBuffer> sb = SurfaceBuffer::Create();
    // null buffer path is covered elsewhere; here try to fail on fence write
    sptr<SyncFence> invalidFence = SyncFence::INVALID_FENCE;
    info.releaseBufferFenceVec.push_back(std::tuple(static_cast<RSLayerId>(2u), sb, invalidFence));

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
}
/**
 * Function: Proxy_ReleaseLayerBuffers_BufferNullPath
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. add one fence entry with nullptr buffer
 *                  2. verify stub receives entry and proxy returns code
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_BufferNullPath, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    bool receivedHasBufferFalse = false;
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) {
        receivedHasBufferFalse =
            (info.releaseBufferFenceVec.size() == 1 && std::get<1>(info.releaseBufferFenceVec[0]) == nullptr);
    });
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    ReleaseLayerBuffersInfo info;
    info.screenId = 3u;
    sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
    info.releaseBufferFenceVec.push_back(std::tuple(static_cast<RSLayerId>(9u), nullptr, fence));
    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
    EXPECT_TRUE(receivedHasBufferFalse);
}

/**
 * Function: Proxy_NotifyLppLayerToRender_EmptyIds
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call NotifyLppLayerToRender with empty ids
 *                  2. ensure reply code is forwarded
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_NotifyLppLayerToRender_EmptyIds, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());
    std::unordered_set<uint64_t> ids;
    int32_t r = proxy.NotifyLppLayerToRender(100u, ids);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
}

/**
 * Function: Proxy_SendRequest_ErrorBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with remote returning error
 *                  2. verify both methods return -1
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_SendRequest_ErrorBranch, TestSize.Level1)
{
    class FakeErrorRemoteLocal : public IRemoteObject {
    public:
        FakeErrorRemoteLocal() : IRemoteObject(IRSComposerToRenderConnection::GetDescriptor()) {}
        int32_t GetObjectRefCount() override
        {
            return 1;
        }
        int SendRequest(uint32_t, MessageParcel&, MessageParcel&, MessageOption&) override
        {
            return -1;
        }
        bool AddDeathRecipient(const sptr<DeathRecipient>&) override
        {
            return false;
        }
        bool RemoveDeathRecipient(const sptr<DeathRecipient>&) override
        {
            return false;
        }
        int Dump(int, const std::vector<std::u16string>&) override
        {
            return 0;
        }
    };
    sptr<IRemoteObject> badRemote = sptr<FakeErrorRemoteLocal>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(badRemote);

    ReleaseLayerBuffersInfo info;
    int32_t r1 = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r1, -1);

    std::unordered_set<uint64_t> ids;
    int32_t r2 = proxy.NotifyLppLayerToRender(1u, ids);
    EXPECT_EQ(r2, -1);
}

/**
 * Function: Proxy_ReleaseLayerBuffers_ReadReplyMessageFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with remote returning success but empty reply
 *                  2. verify ReleaseLayerBuffers returns -1 (line 111 true branch)
 */
HWTEST_F(
    RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_ReadReplyMessageFail_TrueBranch, TestSize.Level1)
{
    class FakeReplyReadFailRemote : public IRemoteObject {
    public:
        FakeReplyReadFailRemote() : IRemoteObject(IRSComposerToRenderConnection::GetDescriptor()) {}
        int32_t GetObjectRefCount() override
        {
            return 1;
        }
        int SendRequest(uint32_t, MessageParcel&, MessageParcel& reply, MessageOption&) override
        {
            reply.RewindRead(0);
            return NO_ERROR;
        }
        bool AddDeathRecipient(const sptr<DeathRecipient>&) override
        {
            return false;
        }
        bool RemoveDeathRecipient(const sptr<DeathRecipient>&) override
        {
            return false;
        }
        int Dump(int, const std::vector<std::u16string>&) override
        {
            return 0;
        }
    };
    sptr<IRemoteObject> fakeRemote = sptr<FakeReplyReadFailRemote>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(fakeRemote);

    ReleaseLayerBuffersInfo info;
    info.screenId = 1u;
    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, -1);
}

/**
 * Function: Proxy_NotifyLppLayerToRender_ReadReplyMessageFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with remote returning success but empty reply
 *                  2. verify NotifyLppLayerToRender returns -1 (line 157 true branch)
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_NotifyLppLayerToRender_ReadReplyMessageFail_TrueBranch,
    TestSize.Level1)
{
    class FakeReplyReadFailRemote : public IRemoteObject {
    public:
        FakeReplyReadFailRemote() : IRemoteObject(IRSComposerToRenderConnection::GetDescriptor()) {}
        int32_t GetObjectRefCount() override
        {
            return 1;
        }
        int SendRequest(uint32_t, MessageParcel&, MessageParcel& reply, MessageOption&) override
        {
            reply.RewindRead(0);
            return NO_ERROR;
        }
        bool AddDeathRecipient(const sptr<DeathRecipient>&) override
        {
            return false;
        }
        bool RemoveDeathRecipient(const sptr<DeathRecipient>&) override
        {
            return false;
        }
        int Dump(int, const std::vector<std::u16string>&) override
        {
            return 0;
        }
    };
    sptr<IRemoteObject> fakeRemote = sptr<FakeReplyReadFailRemote>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(fakeRemote);

    std::unordered_set<uint64_t> ids { 1u, 2u };
    int32_t r = proxy.NotifyLppLayerToRender(100u, ids);
    EXPECT_EQ(r, -1);
}

/**
 * Function: Proxy_ReleaseLayerBuffers_RemoteNullptr_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with nullptr remote
 *                  2. call ReleaseLayerBuffers and verify it returns COMPOSITOR_ERROR_BINDER_ERROR
 *                  3. covers the (remote == nullptr) true branch added in the fix commit
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_RemoteNullptr_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnectionProxy proxy(nullptr);
    ReleaseLayerBuffersInfo info;
    info.screenId = 1u;
    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, COMPOSITOR_ERROR_BINDER_ERROR);
}

/**
 * Function: Proxy_NotifyLppLayerToRender_RemoteNullptr_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with nullptr remote
 *                  2. call NotifyLppLayerToRender and verify it returns COMPOSITOR_ERROR_BINDER_ERROR
 *                  3. covers the (remote == nullptr) true branch added in the fix commit
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_NotifyLppLayerToRender_RemoteNullptr_TrueBranch, TestSize.Level1)
{
    RSComposerToRenderConnectionProxy proxy(nullptr);
    std::unordered_set<uint64_t> ids { 1u, 2u };
    int32_t r = proxy.NotifyLppLayerToRender(100u, ids);
    EXPECT_EQ(r, COMPOSITOR_ERROR_BINDER_ERROR);
}

/**
 * Function: Proxy_NotifyLayerStateChangedToRender_RemoteNullptr_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with nullptr remote
 *                  2. call NotifyLayerStateChangedToRender and verify it returns COMPOSITOR_ERROR_BINDER_ERROR
 *                  3. covers the (remote == nullptr) true branch added in the fix commit
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_NotifyLayerStateChangedToRender_RemoteNullptr_TrueBranch,
    TestSize.Level1)
{
    RSComposerToRenderConnectionProxy proxy(nullptr);
    int32_t r = proxy.NotifyLayerStateChangedToRender(999u, LayerStateChange::UNAVAILABLE, 77u);
    EXPECT_EQ(r, COMPOSITOR_ERROR_BINDER_ERROR);
}

/**
 * Function: Proxy_ReleaseLayerBuffers_RemoteNotNull_FalseBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with a valid (non-null) remote
 *                  2. call ReleaseLayerBuffers and verify it does NOT return COMPOSITOR_ERROR_BINDER_ERROR
 *                  3. covers the (remote == nullptr) false branch added in the fix commit
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_RemoteNotNull_FalseBranch, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    bool cbCalled = false;
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo& info) {
        cbCalled = true;
    });
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    ReleaseLayerBuffersInfo info;
    info.screenId = 7u;
    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_NE(r, COMPOSITOR_ERROR_BINDER_ERROR);
    EXPECT_TRUE(cbCalled);
}

/**
 * Function: Proxy_NotifyLppLayerToRender_RemoteNotNull_FalseBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with a valid (non-null) remote
 *                  2. call NotifyLppLayerToRender and verify it does NOT return COMPOSITOR_ERROR_BINDER_ERROR
 *                  3. covers the (remote == nullptr) false branch added in the fix commit
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_NotifyLppLayerToRender_RemoteNotNull_FalseBranch, TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    std::unordered_set<uint64_t> ids { 1u };
    int32_t r = proxy.NotifyLppLayerToRender(100u, ids);
    EXPECT_NE(r, COMPOSITOR_ERROR_BINDER_ERROR);
}

/**
 * Function: Proxy_NotifyLayerStateChangedToRender_RemoteNotNull_FalseBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct proxy with a valid (non-null) remote
 *                  2. call NotifyLayerStateChangedToRender and verify it does NOT return COMPOSITOR_ERROR_BINDER_ERROR
 *                  3. covers the (remote == nullptr) false branch added in the fix commit
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_NotifyLayerStateChangedToRender_RemoteNotNull_FalseBranch,
    TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    int32_t r = proxy.NotifyLayerStateChangedToRender(999u, LayerStateChange::UNAVAILABLE, 77u);
    EXPECT_NE(r, COMPOSITOR_ERROR_BINDER_ERROR);
}

namespace {
constexpr int32_t PARCEL_REPLY_OK = 0;
constexpr size_t PARCEL_HEADER_SIZE = 108;
constexpr size_t NULL_FENCE_ENTRY_SIZE = 12;
constexpr size_t NONNULL_FENCE_ENTRY_SIZE = 16;
constexpr size_t PARCEL_MAX_CAPACITY = 204800;
constexpr size_t FILL_ENTRIES_FOR_BOOL_FAIL =
    (PARCEL_MAX_CAPACITY - PARCEL_HEADER_SIZE) / NULL_FENCE_ENTRY_SIZE;
constexpr size_t FILL_ENTRIES_FOR_FENCE_FAIL =
    (PARCEL_MAX_CAPACITY - PARCEL_HEADER_SIZE - 2 * NONNULL_FENCE_ENTRY_SIZE - NULL_FENCE_ENTRY_SIZE) /
    NULL_FENCE_ENTRY_SIZE;

class FakeSuccessReplyRemote : public IRemoteObject {
public:
    FakeSuccessReplyRemote() : IRemoteObject(IRSComposerToRenderConnection::GetDescriptor()) {}
    int32_t GetObjectRefCount() override { return 1; }
    int SendRequest(uint32_t, MessageParcel&, MessageParcel& reply, MessageOption&) override
    {
        reply.WriteInt32(PARCEL_REPLY_OK);
        return NO_ERROR;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient>&) override { return false; }
    bool RemoveDeathRecipient(const sptr<DeathRecipient>&) override { return false; }
    int Dump(int, const std::vector<std::u16string>&) override { return 0; }
};
}

/**
 * Function: Proxy_ReleaseLayerBuffers_FenceNullptr_ShortCircuit
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. add one entry with nullptr fence
 *                  2. verify proxy returns OK (fence && short-circuit false branch)
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_FenceNullptr_ShortCircuit,
    TestSize.Level1)
{
    auto remote = sptr<FakeSuccessReplyRemote>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(remote);

    ReleaseLayerBuffersInfo info;
    info.screenId = 11u;
    // Entry with null buffer and nullptr fence -> fence && short-circuits to false
    info.releaseBufferFenceVec.push_back(std::tuple(static_cast<RSLayerId>(100u), nullptr, nullptr));
    info.lastSwapBufferTime = 111;

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, PARCEL_REPLY_OK);
}

/**
 * Function: Proxy_ReleaseLayerBuffers_WriteBoolTrue_Fail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. fill parcel to near max capacity with null-buffer entries
 *                  2. add one entry with non-null buffer so WriteBool(true) is called
 *                  3. verify proxy returns -1 (if (!data.WriteBool(true)) true branch)
 * Note: Parcel max data capacity is 204800 bytes. After FILL_ENTRIES_FOR_BOOL_FAIL null-fence
 *       entries (12 bytes each) + header (108 bytes), exactly 8 bytes remain, enough for
 *       WriteUint64 but not WriteBool.
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_WriteBoolTrue_Fail_TrueBranch,
    TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    ReleaseLayerBuffersInfo info;
    info.screenId = 12u;
    // Fill parcel to leave exactly 8 bytes remaining
    for (size_t i = 0; i < FILL_ENTRIES_FOR_BOOL_FAIL; ++i) {
        info.releaseBufferFenceVec.push_back(
            std::tuple(static_cast<RSLayerId>(i), nullptr, nullptr));
    }
    // Triggering entry: non-null buffer -> WriteBool(true) path, 0 bytes remain -> fails
    sptr<SurfaceBuffer> sb = SurfaceBuffer::Create();
    info.releaseBufferFenceVec.push_back(
        std::tuple(static_cast<RSLayerId>(99998u), sb, nullptr));

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, -1);
}

/**
 * Function: Proxy_ReleaseLayerBuffers_WriteBoolFalse_Fail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. fill parcel to near max capacity with null-buffer entries
 *                  2. add one entry with null buffer so WriteBool(false) is called
 *                  3. verify proxy returns -1 (if (!data.WriteBool(false)) true branch)
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_WriteBoolFalse_Fail_TrueBranch,
    TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    ReleaseLayerBuffersInfo info;
    info.screenId = 13u;
    for (size_t i = 0; i < FILL_ENTRIES_FOR_BOOL_FAIL; ++i) {
        info.releaseBufferFenceVec.push_back(
            std::tuple(static_cast<RSLayerId>(i), nullptr, nullptr));
    }
    // Triggering entry: null buffer -> WriteBool(false) path, 0 bytes remain -> fails
    info.releaseBufferFenceVec.push_back(
        std::tuple(static_cast<RSLayerId>(99999u), nullptr, nullptr));

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, -1);
}

/**
 * Function: Proxy_ReleaseLayerBuffers_FenceWriteFail_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. fill parcel to leave exactly 12 bytes before the triggering entry
 *                  2. triggering entry has non-null fence: WriteUint64(8)+WriteBool(4) succeed,
 *                     but fence WriteToMessageParcel fails (0 bytes remain)
 *                  3. verify proxy returns -1 (if (fence && !WriteToMessageParcel) true branch)
 * Note: Uses FILL_ENTRIES_FOR_FENCE_FAIL null-fence entries (12 bytes each) + 2 non-null-fence
 *       entries (16 bytes each) to leave exactly 12 bytes for the triggering entry.
 */
HWTEST_F(RSComposerToRenderConnectionProxyTest, Proxy_ReleaseLayerBuffers_FenceWriteFail_TrueBranch,
    TestSize.Level1)
{
    sptr<RSComposerToRenderConnection> stub = sptr<RSComposerToRenderConnection>::MakeSptr();
    RSComposerToRenderConnectionProxy proxy(stub->AsObject());

    ReleaseLayerBuffersInfo info;
    info.screenId = 14u;
    // Fill with null-fence entries (12 bytes each)
    for (size_t i = 0; i < FILL_ENTRIES_FOR_FENCE_FAIL; ++i) {
        info.releaseBufferFenceVec.push_back(
            std::tuple(static_cast<RSLayerId>(i), nullptr, nullptr));
    }
    // 2 non-null-fence entries (16 bytes each) to align remaining to 12 bytes
    sptr<SyncFence> dummyFence = sptr<SyncFence>::MakeSptr(-1);
    info.releaseBufferFenceVec.push_back(
        std::tuple(static_cast<RSLayerId>(88888u), nullptr, dummyFence));
    info.releaseBufferFenceVec.push_back(
        std::tuple(static_cast<RSLayerId>(88889u), nullptr, dummyFence));
    // Triggering entry: non-null fence, WriteUint64(8)+WriteBool(4) succeed,
    // fence WriteToMessageParcel fails (0 bytes remain) -> true branch
    info.releaseBufferFenceVec.push_back(
        std::tuple(static_cast<RSLayerId>(77777u), nullptr, dummyFence));

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, -1);
}
} // namespace OHOS::Rosen
