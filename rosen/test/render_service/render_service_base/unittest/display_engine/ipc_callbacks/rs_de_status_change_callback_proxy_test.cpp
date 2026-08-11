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
#include "display_engine/ipc_callbacks/rs_de_status_change_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class DEStatusChangeCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DEStatusChangeCallbackProxyTest::SetUpTestCase() {}
void DEStatusChangeCallbackProxyTest::TearDownTestCase() {}
void DEStatusChangeCallbackProxyTest::SetUp() {}
void DEStatusChangeCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnNotifyDEStatusChangeDone001
 * @tc.desc: Verify function OnNotifyDEStatusChangeDone full process
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(DEStatusChangeCallbackProxyTest, OnNotifyDEStatusChangeDone001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    auto rsDEStatusChangeCallbackProxy = std::make_shared<RSDEStatusChangeCallbackProxy>(remoteObject);
    ASSERT_NE(rsDEStatusChangeCallbackProxy, nullptr);
    uint32_t sceneKey = 0;
    std::vector<uint8_t> result;
    result.push_back(static_cast<uint8_t>(1));
    result.push_back(static_cast<uint8_t>(1));
    rsDEStatusChangeCallbackProxy->OnNotifyDEStatusChangeDone(sceneKey, result);
    ASSERT_EQ(static_cast<uint32_t>(RSIDEStatusChangeCallbackInterfaceCode::ON_DE_STATUS_CHANGE_DONE),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnNotifyDEStatusChangeDone002
 * @tc.desc: Verify function OnNotifyDEStatusChangeDone sendRequestResult
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(DEStatusChangeCallbackProxyTest, OnNotifyDEStatusChangeDone002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    remoteObject->sendRequestResult_ = 1;
    auto rsDEStatusChangeCallbackProxy = std::make_shared<RSDEStatusChangeCallbackProxy>(remoteObject);
    ASSERT_NE(rsDEStatusChangeCallbackProxy, nullptr);
    uint32_t sceneKey = 0;
    std::vector<uint8_t> result;
    result.push_back(static_cast<uint8_t>(1));
    result.push_back(static_cast<uint8_t>(1));
    rsDEStatusChangeCallbackProxy->OnNotifyDEStatusChangeDone(sceneKey, result);
    ASSERT_EQ(static_cast<uint32_t>(RSIDEStatusChangeCallbackInterfaceCode::ON_DE_STATUS_CHANGE_DONE),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnNotifyDEStatusChangeDone003
 * @tc.desc: Verify function OnNotifyDEStatusChangeDone nullptr
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(DEStatusChangeCallbackProxyTest, OnNotifyDEStatusChangeDone003, TestSize.Level1)
{
    auto rsDEStatusChangeCallbackProxy = std::make_shared<RSDEStatusChangeCallbackProxy>(nullptr);
    ASSERT_NE(rsDEStatusChangeCallbackProxy, nullptr);
    uint32_t sceneKey = 0;
    std::vector<uint8_t> result;
    result.push_back(static_cast<uint8_t>(1));
    result.push_back(static_cast<uint8_t>(1));
    rsDEStatusChangeCallbackProxy->OnNotifyDEStatusChangeDone(sceneKey, result);
}
} // namespace OHOS::Rosen