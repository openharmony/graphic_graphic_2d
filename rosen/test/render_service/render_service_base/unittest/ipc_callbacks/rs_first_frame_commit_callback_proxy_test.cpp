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

#include "gtest/gtest.h"
#include "ipc_callbacks/rs_first_frame_commit_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFirstFrameCommitCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFirstFrameCommitCallbackProxyTest::SetUpTestCase() {}
void RSFirstFrameCommitCallbackProxyTest::TearDownTestCase() {}
void RSFirstFrameCommitCallbackProxyTest::SetUp() {}
void RSFirstFrameCommitCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnFirstFrameCommit001
 * @tc.desc: Verify function OnFirstFrameCommit
 * @tc.type: FUNC
 * @tc.require: issuesIBTF2E
 */
HWTEST_F(RSFirstFrameCommitCallbackProxyTest, OnFirstFrameCommit001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    auto rsFirstFrameCommitCallbackProxy = std::make_shared<RSFirstFrameCommitCallbackProxy>(remoteObject);
    ASSERT_NE(rsFirstFrameCommitCallbackProxy, nullptr);
    uint32_t screenId = 0;
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    rsFirstFrameCommitCallbackProxy->OnFirstFrameCommit(screenId, timestamp);
    ASSERT_EQ(static_cast<uint32_t>(RSIFirstFrameCommitCallbackInterfaceCode::ON_FIRST_FRAME_COMMIT),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnFirstFrameCommit002
 * @tc.desc: Verify function OnFirstFrameCommit
 * @tc.type: FUNC
 * @tc.require: issuesIBTF2E
 */
HWTEST_F(RSFirstFrameCommitCallbackProxyTest, OnFirstFrameCommit002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    remoteObject->sendRequestResult_ = 1;
    auto rsFirstFrameCommitCallbackProxy = std::make_shared<RSFirstFrameCommitCallbackProxy>(remoteObject);
    ASSERT_NE(rsFirstFrameCommitCallbackProxy, nullptr);
    uint32_t screenId = 0;
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    rsFirstFrameCommitCallbackProxy->OnFirstFrameCommit(screenId, timestamp);
    ASSERT_EQ(static_cast<uint32_t>(RSIFirstFrameCommitCallbackInterfaceCode::ON_FIRST_FRAME_COMMIT),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnFirstFrameCommit003
 * @tc.desc: Verify function OnFirstFrameCommit
 * @tc.type: FUNC
 * @tc.require: issuesIBTF2E
 */
HWTEST_F(RSFirstFrameCommitCallbackProxyTest, OnFirstFrameCommit003, TestSize.Level1)
{
    auto rsFirstFrameCommitCallbackProxy = std::make_shared<RSFirstFrameCommitCallbackProxy>(nullptr);
    ASSERT_NE(rsFirstFrameCommitCallbackProxy, nullptr);
    uint32_t screenId = 0;
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    rsFirstFrameCommitCallbackProxy->OnFirstFrameCommit(screenId, timestamp);
}

} // namespace OHOS::Rosen
