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
#include "ipc_callbacks/hgm_config_change_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSCallbackStubHgmConfigChangeMock : public RSHgmConfigChangeCallbackStub {
public:
    RSCallbackStubHgmConfigChangeMock() = default;
    virtual ~RSCallbackStubHgmConfigChangeMock() = default;
    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override {};
    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override {};
    void OnHgmRefreshRateUpdate(int32_t refreshRate) override {};
};

class RSCallbackStubHgmConfigChangeTest : public testing::Test {
public:
    static sptr<RSCallbackStubHgmConfigChangeMock> stub;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

sptr<RSCallbackStubHgmConfigChangeMock> RSCallbackStubHgmConfigChangeTest::stub = nullptr;

void RSCallbackStubHgmConfigChangeTest::SetUpTestCase()
{
    stub = new RSCallbackStubHgmConfigChangeMock();
}
void RSCallbackStubHgmConfigChangeTest::TearDownTestCase()
{
    stub = nullptr;
}
void RSCallbackStubHgmConfigChangeTest::SetUp() {}
void RSCallbackStubHgmConfigChangeTest::TearDown() {}


/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest incorrect code and data present
 * @tc.type: FUNC
 */
HWTEST_F(RSCallbackStubHgmConfigChangeTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = -1;
    data.WriteInterfaceToken(RSIHgmConfigChangeCallback::GetDescriptor());
    
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(IPC_STUB_UNKNOW_TRANS_ERR, res);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest code:ON_HGM_CONFIG_CHANGED
 * @tc.type: FUNC
 */
HWTEST_F(RSCallbackStubHgmConfigChangeTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_CONFIG_CHANGED);
    data.WriteInterfaceToken(RSIHgmConfigChangeCallback::GetDescriptor());

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest code:ON_HGM_REFRESH_RATE_MODE_CHANGED
 * @tc.type: FUNC
 */
HWTEST_F(RSCallbackStubHgmConfigChangeTest, OnRemoteRequest005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_MODE_CHANGED);
    data.WriteInterfaceToken(RSIHgmConfigChangeCallback::GetDescriptor());

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Verify function OnRemoteRequest code:ON_HGM_REFRESH_RATE_CHANGED
 * @tc.type: FUNC
 */
HWTEST_F(RSCallbackStubHgmConfigChangeTest, OnRemoteRequest006, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_CHANGED);
    data.WriteInterfaceToken(RSIHgmConfigChangeCallback::GetDescriptor());

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: OnRemoteRequest005
 * @tc.desc: Verify function OnRemoteRequest incorrect code and no data
 * @tc.type: FUNC
 */
HWTEST_F(RSCallbackStubHgmConfigChangeTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = -1;

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_INVALID_STATE, res);
}

/**
 * @tc.name: OnRemoteRequest006
 * @tc.desc: Verify function OnRemoteRequest incorrect Descriptor
 * @tc.type: FUNC
 */
HWTEST_F(RSCallbackStubHgmConfigChangeTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_CONFIG_CHANGED);
    data.WriteInterfaceToken(u"ohos.rosen.TestDescriptor");

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_INVALID_STATE, res);
}

} // namespace Rosen
} // namespace OHOS
