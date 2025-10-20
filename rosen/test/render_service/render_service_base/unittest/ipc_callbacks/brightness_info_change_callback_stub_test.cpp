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

#include <string>

#include "gtest/gtest.h"
#include "ipc_callbacks/brightness_info_change_callback_proxy.h"
#include "ipc_callbacks/brightness_info_change_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
class MocRSBrightnessInfoChangeCallbackStub : public RSBrightnessInfoChangeCallbackStub {
public:
    explicit MocRSBrightnessInfoChangeCallbackStub() {}
    ~MocRSBrightnessInfoChangeCallbackStub() override {}

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override
    {
        // do nothing
    }
};
} // namespace
class BrightnessInfoChangeCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BrightnessInfoChangeCallbackStubTest::SetUpTestCase() {}
void BrightnessInfoChangeCallbackStubTest::TearDownTestCase() {}
void BrightnessInfoChangeCallbackStubTest::SetUp() {}
void BrightnessInfoChangeCallbackStubTest::TearDown() {}

/**
 * @tc.name: WriteBrightnessInfoTest
 * @tc.desc: Verify function WriteBrightnessInfo
 * @tc.type: FUNC
 */
HWTEST_F(BrightnessInfoChangeCallbackStubTest, WriteBrightnessInfoTest, TestSize.Level1)
{
    sptr<RSBrightnessInfoChangeCallbackProxy> callback = new RSBrightnessInfoChangeCallbackProxy(nullptr);
    ASSERT_NE(callback, nullptr);
    BrightnessInfo brightnessInfo;
    MessageParcel data;
    callback->OnBrightnessInfoChange(0, brightnessInfo);
    ASSERT_TRUE(callback->WriteBrightnessInfo(brightnessInfo, data));
}

/**
 * @tc.name: OnRemoteRequestTest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(BrightnessInfoChangeCallbackStubTest, OnRemoteRequestTest001, TestSize.Level1)
{
    MocRSBrightnessInfoChangeCallbackStub stub;

    // case 1: invalid descriptor
    {
        std::u16string desc = u"";
        MessageParcel data;
        data.WriteInterfaceToken(desc);
        MessageParcel reply;
        MessageOption option;
        uint32_t code = static_cast<uint32_t>(RSIBrightnessInfoChangeCallbackInterfaceCode::ON_BRIGHTNESS_INFO_CHANGE);
        ASSERT_EQ(stub.OnRemoteRequest(code, data, reply, option), ERR_INVALID_STATE);
    }

    // case 2: invalid code
    {
        uint32_t code = 1;
        MessageParcel data;
        data.WriteInterfaceToken(RSIBrightnessInfoChangeCallback::GetDescriptor());
        MessageParcel reply;
        MessageOption option;
        ASSERT_EQ(stub.OnRemoteRequest(code, data, reply, option), IPC_STUB_UNKNOW_TRANS_ERR);
    }

    // case 3: screen id not int64
    {
        MessageParcel data;
        data.WriteInterfaceToken(RSIBrightnessInfoChangeCallback::GetDescriptor());
        data.WriteBool(true);
        MessageParcel reply;
        MessageOption option;
        uint32_t code = static_cast<uint32_t>(RSIBrightnessInfoChangeCallbackInterfaceCode::ON_BRIGHTNESS_INFO_CHANGE);
        ASSERT_EQ(stub.OnRemoteRequest(code, data, reply, option), ERR_INVALID_DATA);
    }

    // case 4: valid hdr info
    {
        MessageParcel data;
        data.WriteInterfaceToken(RSIBrightnessInfoChangeCallback::GetDescriptor());
        data.WriteUint64(0);
        data.WriteFloat(1.0f);
        data.WriteFloat(1.0f);
        data.WriteFloat(1.0f);
        MessageParcel reply;
        MessageOption option;
        uint32_t code = static_cast<uint32_t>(RSIBrightnessInfoChangeCallbackInterfaceCode::ON_BRIGHTNESS_INFO_CHANGE);
        ASSERT_EQ(stub.OnRemoteRequest(code, data, reply, option), ERR_NONE);
    }
}

/**
 * @tc.name: OnRemoteRequestTest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(BrightnessInfoChangeCallbackStubTest, OnRemoteRequestTest002, TestSize.Level1)
{
    MocRSBrightnessInfoChangeCallbackStub stub;

    // case 1: invalid hdr info
    {
        MessageParcel data;
        data.WriteInterfaceToken(RSIBrightnessInfoChangeCallback::GetDescriptor());
        data.WriteUint64(0);
        data.WriteFloat(1.0f);
        data.WriteFloat(1.0f);
        MessageParcel reply;
        MessageOption option;
        uint32_t code = static_cast<uint32_t>(RSIBrightnessInfoChangeCallbackInterfaceCode::ON_BRIGHTNESS_INFO_CHANGE);
        ASSERT_EQ(stub.OnRemoteRequest(code, data, reply, option), ERR_INVALID_DATA);
    }

    // case 2: invalid hdr info
    {
        MessageParcel data;
        data.WriteInterfaceToken(RSIBrightnessInfoChangeCallback::GetDescriptor());
        data.WriteUint64(0);
        data.WriteFloat(1.0f);
        MessageParcel reply;
        MessageOption option;
        uint32_t code = static_cast<uint32_t>(RSIBrightnessInfoChangeCallbackInterfaceCode::ON_BRIGHTNESS_INFO_CHANGE);
        ASSERT_EQ(stub.OnRemoteRequest(code, data, reply, option), ERR_INVALID_DATA);
    }

    // case 3: invalid hdr info
    {
        MessageParcel data;
        data.WriteInterfaceToken(RSIBrightnessInfoChangeCallback::GetDescriptor());
        data.WriteUint64(0);
        MessageParcel reply;
        MessageOption option;
        uint32_t code = static_cast<uint32_t>(RSIBrightnessInfoChangeCallbackInterfaceCode::ON_BRIGHTNESS_INFO_CHANGE);
        ASSERT_EQ(stub.OnRemoteRequest(code, data, reply, option), ERR_INVALID_DATA);
    }
}
} // namespace OHOS::Rosen
