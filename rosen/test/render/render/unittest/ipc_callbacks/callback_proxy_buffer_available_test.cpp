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

#include "ipc_callbacks/buffer_available_callback_proxy.h"
#include "mock_iremoter_objects.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSCallbackProxyBufferAvailabkeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCallbackProxyBufferAvailabkeTest::SetUpTestCase() {}
void RSCallbackProxyBufferAvailabkeTest::TearDownTestCase() {}
void RSCallbackProxyBufferAvailabkeTest::SetUp() {}
void RSCallbackProxyBufferAvailabkeTest::TearDown() {}


/**
 * @tc.name: CallBackOnBufferAvailable001
 * @tc.desc: Verify the CallBackOnBufferAvailable error case
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSCallbackProxyBufferAvailabkeTest, CallBackOnBufferAvailable002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    rsBufferAvailableCallbackProxy->CallBackOnBufferAvailable();
    ASSERT_EQ(remoteMocker->receivedCode_,
        static_cast<uint32_t>(RSIBufferAvailableCallbackInterfaceCode::ON_BUFFER_AVAILABLE));
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto rsBufferAvailableCallbackProxy = std::make_shared<RSBufferAvailableCallbackProxy>(remoteMocker);
    ASSERT_TRUE(rsBufferAvailableCallbackProxy != nullptr);
}

/**
 * @tc.name: CallBackCallBackOnBufferAvailable002
 * @tc.desc: Verify the CallBackOnBufferAvailable
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSCallbackProxyBufferAvailabkeTest, CallBackOnBufferAvailable001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_EQ(remoteMocker->receivedCode_,
        static_cast<uint32_t>(RSIBufferAvailableCallbackInterfaceCode::ON_BUFFER_AVAILABLE));
    ASSERT_TRUE(remoteMocker != nullptr);
    auto rsBufferAvailableCallbackProxy = std::make_shared<RSBufferAvailableCallbackProxy>(remoteMocker);
    ASSERT_TRUE(rsBufferAvailableCallbackProxy != nullptr);
    rsBufferAvailableCallbackProxy->CallBackOnBufferAvailable();
}
} // namespace Rosen
} // namespace OHOS
