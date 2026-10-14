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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>

#include "platform/ohos/transaction/zidl/rs_client_to_render_connection_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
class IRemoteObjectMock : public IRemoteObject {
public:
    IRemoteObjectMock() : IRemoteObject {u"IRemoteObjectMock"}
    {
    }

    ~IRemoteObjectMock()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    int Dump(int fd, const std::vector<std::u16string>& args)
    {
        return 0;
    }

    MOCK_METHOD4(SendRequest, int32_t(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
};
} // namespace

class RSClientToRenderConnectionCleanDelegateProxyTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: CleanDelegateModeBufferCmd001
 * @tc.desc: Verify proxy CleanDelegateModeBufferCmd returns true when SendRequest succeeds.
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToRenderConnectionCleanDelegateProxyTest, CleanDelegateModeBufferCmd001, TestSize.Level1)
{
    sptr<IRemoteObjectMock> remoteObject = new IRemoteObjectMock();
    auto mockProxy = std::make_shared<RSClientToRenderConnectionProxy>(remoteObject);
    ASSERT_NE(mockProxy, nullptr);
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(0));
    constexpr NodeId nodeId = 10086;
    EXPECT_TRUE(mockProxy->CleanDelegateModeBufferCmd(nodeId));
}

/**
 * @tc.name: CleanDelegateModeBufferCmd002
 * @tc.desc: Verify proxy CleanDelegateModeBufferCmd returns false when SendRequest fails.
 * @tc.type: FUNC
 */
HWTEST_F(RSClientToRenderConnectionCleanDelegateProxyTest, CleanDelegateModeBufferCmd002, TestSize.Level1)
{
    sptr<IRemoteObjectMock> remoteObject = new IRemoteObjectMock();
    auto mockProxy = std::make_shared<RSClientToRenderConnectionProxy>(remoteObject);
    ASSERT_NE(mockProxy, nullptr);
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _)).WillRepeatedly(testing::Return(-1));
    constexpr NodeId nodeId = 10087;
    EXPECT_FALSE(mockProxy->CleanDelegateModeBufferCmd(nodeId));
}
} // namespace Rosen
} // namespace OHOS
