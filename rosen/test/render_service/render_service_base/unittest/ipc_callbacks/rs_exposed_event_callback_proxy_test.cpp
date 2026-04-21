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

#include "ipc_callbacks/rs_exposed_event_callback_proxy.h"
#include "ipc_callbacks/rs_iexposed_event_callback_ipc_interface_code.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSExposedEventCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSExposedEventCallbackProxyTest::SetUpTestCase() {}
void RSExposedEventCallbackProxyTest::TearDownTestCase() {}
void RSExposedEventCallbackProxyTest::SetUp() {}
void RSExposedEventCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnExposedEvent001
 * @tc.desc: Verify OnDisplayEvent with valid remote object
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSExposedEventCallbackProxyTest, OnExposedEvent001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_NE(remoteMocker, nullptr);
    auto proxy = std::make_shared<RSExposedEventCallbackProxy>(remoteMocker);
    ASSERT_NE(proxy, nullptr);
    auto data = std::make_shared<RSExtScreenUnsupportData>();
    proxy->OnDisplayEvent(data);
    ASSERT_EQ(remoteMocker->receivedCode_,
        static_cast<uint32_t>(RSIExposedEventCallbackInterfaceCode::ON_EXPOSED_EVENT));
}

/**
 * @tc.name: OnExposedEvent002
 * @tc.desc: Verify OnDisplayEvent with error status
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSExposedEventCallbackProxyTest, OnExposedEvent002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_NE(remoteMocker, nullptr);
    remoteMocker->SetErrorStatus(UNKNOWN_ERROR);
    auto proxy = std::make_shared<RSExposedEventCallbackProxy>(remoteMocker);
    ASSERT_NE(proxy, nullptr);
    auto data = std::make_shared<RSExtScreenUnsupportData>();
    proxy->OnDisplayEvent(data);
    ASSERT_EQ(remoteMocker->receivedCode_,
        static_cast<uint32_t>(RSIExposedEventCallbackInterfaceCode::ON_EXPOSED_EVENT));
}

/**
 * @tc.name: OnExposedEvent003
 * @tc.desc: Verify OnDisplayEvent with nullptr remote
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSExposedEventCallbackProxyTest, OnExposedEvent003, TestSize.Level1)
{
    auto proxy = std::make_shared<RSExposedEventCallbackProxy>(nullptr);
    ASSERT_NE(proxy, nullptr);
    auto data = std::make_shared<RSExtScreenUnsupportData>();
    proxy->OnDisplayEvent(data);
}

} // namespace OHOS::Rosen
