/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "src/ipc_callbacks/rs_sync_transaction_controller_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject {u"MockIRemoteObject"}
    {
    }

    ~MockIRemoteObject()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        return sendRequestResult_;
    }

    bool IsProxyObject() const
    {
        return true;
    }

    bool CheckObjectLegality() const
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface()
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }

    int sendRequestResult_ = 0;
    int count_ = 0;
};

class RSSyncTransactionControllerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<RSSyncTransactionControllerProxy> proxy_;
    sptr<RSSyncTransactionControllerProxy> proxyNull_;
};

void RSSyncTransactionControllerProxyTest::SetUpTestCase() {}

void RSSyncTransactionControllerProxyTest::TearDownTestCase() {}

void RSSyncTransactionControllerProxyTest::SetUp()
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    proxy_ = new RSSyncTransactionControllerProxy(remoteMocker);
    proxyNull_ = new RSSyncTransactionControllerProxy(nullptr);
}

void RSSyncTransactionControllerProxyTest::TearDown() {}

/**
 * @tc.name: CreateTransactionFinished001
 * @tc.desc: Verify the CreateTransactionFinished
 * @tc.type:FUNC
 */
HWTEST_F(RSSyncTransactionControllerProxyTest, CreateTransactionFinished001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSyncTransactionControllerProxyTest CreateTransactionFinished001 start";
    ASSERT_TRUE(proxy_ != nullptr);

    proxyNull_->CreateTransactionFinished();

    proxy_->CreateTransactionFinished();
    ASSERT_TRUE(proxy_ != nullptr);
    GTEST_LOG_(INFO) << "RSSyncTransactionControllerProxyTest CreateTransactionFinished001 end";
}

} // namespace Rosen
} // namespace OHOS