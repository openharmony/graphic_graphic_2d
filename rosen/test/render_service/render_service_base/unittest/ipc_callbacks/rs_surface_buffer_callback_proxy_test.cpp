/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "ipc_callbacks/rs_surface_buffer_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSSurfaceBufferCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceBufferCallbackProxyTest::SetUpTestCase() {}
void RSSurfaceBufferCallbackProxyTest::TearDownTestCase() {}
void RSSurfaceBufferCallbackProxyTest::SetUp() {}
void RSSurfaceBufferCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnFinish001
 * @tc.desc: Verify the OnFinish
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSSurfaceBufferCallbackProxyTest, OnFinish001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto rsSurfaceBufferCallbackProxy = std::make_shared<RSSurfaceBufferCallbackProxy>(remoteMocker);
    ASSERT_TRUE(rsSurfaceBufferCallbackProxy != nullptr);
    rsSurfaceBufferCallbackProxy->OnFinish({
        .uid = 0,
        .surfaceBufferIds = { 1 },
        .isRenderedFlags = { 1 },
        .isUniRender = true,
        .releaseFences = { SyncFence::INVALID_FENCE },
    });
    ASSERT_EQ(remoteMocker->receivedCode_, static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_FINISH));
}

/**
 * @tc.name: OnFinish002
 * @tc.desc: Verify the OnFinish error case
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSSurfaceBufferCallbackProxyTest, OnFinish002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto rsSurfaceBufferCallbackProxy = std::make_shared<RSSurfaceBufferCallbackProxy>(remoteMocker);
    ASSERT_TRUE(rsSurfaceBufferCallbackProxy != nullptr);
    rsSurfaceBufferCallbackProxy->OnFinish({
        .uid = 0,
        .surfaceBufferIds = { 1 },
        .isRenderedFlags = { true },
        .isUniRender = true,
        .releaseFences = { SyncFence::INVALID_FENCE },
    });
    ASSERT_EQ(remoteMocker->receivedCode_, static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_FINISH));
}
} // namespace Rosen
} // namespace OHOS
