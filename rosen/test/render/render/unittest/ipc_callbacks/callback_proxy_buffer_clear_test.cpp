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

#include "ipc_callbacks/buffer_clear_callback_proxy.h"
#include "mock_iremoter_objects.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSCallbackProxyBufferClearTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCallbackProxyBufferClearTest::SetUpTestCase() {}
void RSCallbackProxyBufferClearTest::TearDownTestCase() {}
void RSCallbackProxyBufferClearTest::SetUp() {}
void RSCallbackProxyBufferClearTest::TearDown() {}

/**
 * @tc.name: OnBufferClear001
 * @tc.desc: Verify the OnBufferClear
 * @tc.type:FUNC
 */
HWTEST_F(RSCallbackProxyBufferClearTest, OnBufferClear001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto rsBufferClearCallbackProxy = std::make_shared<RSBufferClearCallbackProxy>(remoteMocker);
    ASSERT_TRUE(rsBufferClearCallbackProxy != nullptr);
    rsBufferClearCallbackProxy->OnBufferClear();
    ASSERT_EQ(remoteMocker->receivedCode_,
              static_cast<uint32_t>(RSIBufferClearCallbackInterfaceCode::ON_BUFFER_CLEAR));
}

/**
 * @tc.name: OnBufferClear002
 * @tc.desc: Verify the OnBufferClear error case
 * @tc.type:FUNC
 */
HWTEST_F(RSCallbackProxyBufferClearTest, OnBufferClear002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto rsBufferClearCallbackProxy = std::make_shared<RSBufferClearCallbackProxy>(remoteMocker);
    ASSERT_TRUE(rsBufferClearCallbackProxy != nullptr);
    rsBufferClearCallbackProxy->OnBufferClear();
    ASSERT_EQ(remoteMocker->receivedCode_,
              static_cast<uint32_t>(RSIBufferClearCallbackInterfaceCode::ON_BUFFER_CLEAR));
}

/**
* @tc.name: OnBufferClear003
* @tc.desc: Verify the OnBufferClear
* @tc.type:FUNC
*/
HWTEST_F(RSCallbackProxyBufferClearTest, OnBufferClear003, TestSize.Level1)
{
    auto  rsBufferClearCallbackProxy = std::make_shared<RSBufferClearCallbackProxy>(nullptr);
    ASSERT_TRUE(rsBufferClearCallbackProxy != nullptr);
    rsBufferClearCallbackProxy->OnBufferClear();
}
} // namespace Rosen
} // namespace OHOS
