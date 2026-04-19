/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ipc_callbacks/dfx/rs_dump_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSDumpCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDumpCallbackProxyTest::SetUpTestCase() {}
void RSDumpCallbackProxyTest::TearDownTestCase() {}
void RSDumpCallbackProxyTest::SetUp() {}
void RSDumpCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnDumpResult_Normal
 * @tc.desc: Verify OnDumpResult with normal parameters
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSDumpCallbackProxyTest, OnDumpResult_Normal, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto proxy = std::make_shared<RSDumpCallbackProxy>(remoteMocker);
    ASSERT_TRUE(proxy != nullptr);
    
    std::string dumpResult = "test dump result";
    proxy->OnDumpResult(dumpResult);
    
    ASSERT_EQ(remoteMocker->receivedCode_,
              static_cast<uint32_t>(RSDumpCallbackInterfaceCode::REPLY_DUMP_RESULT_TO_SERVICE));
}

/**
 * @tc.name: OnDumpResult_SendRequestFailed
 * @tc.desc: Verify OnDumpResult handles SendRequest failure
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSDumpCallbackProxyTest, OnDumpResult_SendRequestFailed, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    
    auto proxy = std::make_shared<RSDumpCallbackProxy>(remoteMocker);
    ASSERT_TRUE(proxy != nullptr);
    
    std::string dumpResult = "test dump result";
    proxy->OnDumpResult(dumpResult);
}

/**
 * @tc.name: OnDumpResult_EmptyString
 * @tc.desc: Verify OnDumpResult with empty string parameter
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSDumpCallbackProxyTest, OnDumpResult_EmptyString, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto proxy = std::make_shared<RSDumpCallbackProxy>(remoteMocker);
    ASSERT_TRUE(proxy != nullptr);
    
    std::string dumpResult = "";
    proxy->OnDumpResult(dumpResult);
}

/**
 * @tc.name: OnDumpResult_LongString
 * @tc.desc: Verify OnDumpResult with long string parameter
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSDumpCallbackProxyTest, OnDumpResult_LongString, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto proxy = std::make_shared<RSDumpCallbackProxy>(remoteMocker);
    ASSERT_TRUE(proxy != nullptr);
    
    std::string dumpResult(10000, 'a');
    proxy->OnDumpResult(dumpResult);
}

} // namespace OHOS::Rosen
