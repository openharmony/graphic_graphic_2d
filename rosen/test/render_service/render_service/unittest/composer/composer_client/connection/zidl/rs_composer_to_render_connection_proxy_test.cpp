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
#include <thread>
#include "rs_composer_to_render_connection_proxy.h"
#include "rs_composer_to_render_connection_stub.h"
#include "sync_fence.h"
#include "surface_buffer.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TestComposerToRenderStub : public RSComposerToRenderConnectionStub {
public:
    int32_t ReleaseLayerBuffers(ReleaseLayerBuffersInfo &info) override
    {
        lastInfo_ = info;
        return 123;
    }
    int32_t NotifyLppLayerToRender(uint64_t vsyncId, const std::set<uint64_t> &lppNodeIds) override
    {
        lastVsyncId_ = vsyncId;
        lastLppIds_ = lppNodeIds;
        return 456;
    }
    ReleaseLayerBuffersInfo lastInfo_ {};
    uint64_t lastVsyncId_ {0};
    std::set<uint64_t> lastLppIds_ {};
};

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
    sptr<TestComposerToRenderStub> stub = sptr<TestComposerToRenderStub>::MakeSptr();
    sptr<IRemoteObject> obj = stub->AsObject();
    RSComposerToRenderConnectionProxy proxy(obj);

    ReleaseLayerBuffersInfo info;
    info.screenId = 1u;
    GraphicPresentTimestamp ts { GRAPHIC_DISPLAY_PTS_TIMESTAMP, 123456 };
    info.timestampVec.push_back(std::tuple(static_cast<RSLayerId>(10u), true, ts));
    sptr<SurfaceBuffer> sb = SurfaceBuffer::Create();
    sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
    info.releaseBufferFenceVec.push_back(std::tuple(static_cast<RSLayerId>(10u), sb, fence));
    info.lastSwapBufferTime = 987654321LL;

    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, 123);
    EXPECT_EQ(stub->lastInfo_.screenId, 1u);
    ASSERT_EQ(stub->lastInfo_.timestampVec.size(), 1u);
    ASSERT_EQ(stub->lastInfo_.releaseBufferFenceVec.size(), 1u);

    std::set<uint64_t> ids { 11u, 22u };
    r = proxy.NotifyLppLayerToRender(777u, ids);
    EXPECT_EQ(r, 456);
    EXPECT_EQ(stub->lastVsyncId_, 777u);
    EXPECT_EQ(stub->lastLppIds_.size(), 2u);
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
    class DummyStub : public RSComposerToRenderConnectionStub {
    public:
        int32_t ReleaseLayerBuffers(ReleaseLayerBuffersInfo &info) override
        {
            lastScreenId_ = info.screenId;
            return 0;
        }
        int32_t NotifyLppLayerToRender(uint64_t,
            const std::set<uint64_t>&) override
        {
            return 0;
        }
        uint64_t lastScreenId_ {0};
    };
    sptr<DummyStub> stub = sptr<DummyStub>::MakeSptr();
    sptr<IRemoteObject> obj = stub->AsObject();
    RSComposerToRenderConnectionProxy proxy(obj);
    ReleaseLayerBuffersInfo info;
    info.screenId = 2u;
    info.lastSwapBufferTime = 0;
    int32_t r = proxy.ReleaseLayerBuffers(info);
    EXPECT_EQ(r, 0);
    EXPECT_EQ(stub->lastScreenId_, 2u);
}
} // namespace OHOS::Rosen
