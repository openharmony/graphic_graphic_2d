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
#include "rs_composer_to_render_connection_proxy.h"
#include "rs_composer_to_render_connection.h"
#include "graphic_common_c.h"
#include "iremote_object.h"
#include "sync_fence.h"
#include "surface_buffer.h"

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
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo &info) {
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
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo &info) {
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
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo &info) {
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
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo &info) {
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
    EXPECT_EQ(r, -1);
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
    EXPECT_EQ(r, -1);
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
    stub->RegisterReleaseLayerBuffersCB([&](ReleaseLayerBuffersInfo &info) {
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
        int32_t GetObjectRefCount() override { return 1; }
        int SendRequest(uint32_t, MessageParcel &, MessageParcel &, MessageOption &) override { return -1; }
        bool AddDeathRecipient(const sptr<DeathRecipient> &) override { return false; }
        bool RemoveDeathRecipient(const sptr<DeathRecipient> &) override { return false; }
        int Dump(int, const std::vector<std::u16string> &) override { return 0; }
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
} // namespace OHOS::Rosen
