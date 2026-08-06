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
#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>

#include "display_engine/ipc_callbacks/rs_de_status_change_callback_stub.h"
#include "display_engine/ipc_callbacks/rs_ide_status_change_callback.h"
#include "display_engine/transaction/zidl/rs_display_engine_control_stub.h"
#include "display_engine/transaction/rs_idisplay_engine_control_ipc_interface_code.h"
#include "display_engine/transaction/zidl/rs_idisplay_engine_control.h"

namespace OHOS::Rosen {
class RSDisplayEngineControlStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayEngineControlStubTest::SetUpTestCase() {}
void RSDisplayEngineControlStubTest::TearDownTestCase() {}
void RSDisplayEngineControlStubTest::SetUp() {}
void RSDisplayEngineControlStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequestInvalidInterfaceToken
 * @tc.desc: Test OnRemoteRequest with invalid interface token
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlStubTest, OnRemoteRequestInvalidInterfaceToken, testing::ext::TestSize.Level1)
{
    RSDisplayEngineControlStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(u"invalid.token");
    uint32_t code = static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::NOTIFY_DE_STATUS_CHANGE);
    int res = stub.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: NotifyDEStatusChangeSuccess
 * @tc.desc: Test NOTIFY_DE_STATUS_CHANGE success path
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlStubTest, NotifyDEStatusChangeSuccess, testing::ext::TestSize.Level1)
{
    RSDisplayEngineControlStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIDisplayEngineControl::GetDescriptor());
    uint32_t sceneKey = 123;
    std::vector<uint8_t> values = {1, 2, 3};
    data.WriteUint32(sceneKey);
    data.WriteUInt8Vector(values);
    uint32_t code = static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::NOTIFY_DE_STATUS_CHANGE);
    int res = stub.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: RegisterDEStatusChangeCallbackNullptr
 * @tc.desc: Test REGISTER_DE_STATUS_CHANGE_CALLBACK when ReadRemoteObject returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlStubTest, RegisterDEStatusChangeCallbackNullptr, testing::ext::TestSize.Level1)
{
    RSDisplayEngineControlStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIDisplayEngineControl::GetDescriptor());
    data.WriteRemoteObject(nullptr);
    uint32_t code = static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::REGISTER_DE_STATUS_CHANGE_CALLBACK);
    int res = stub.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: RegisterDEStatusChangeCallbackInvalidStub
 * @tc.desc: Test REGISTER_DE_STATUS_CHANGE_CALLBACK with invalid stub
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlStubTest, RegisterDEStatusChangeCallbackInvalidStub, testing::ext::TestSize.Level1)
{
    RSDisplayEngineControlStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIDisplayEngineControl::GetDescriptor());
    sptr<IRemoteObject> callbackObj = new RSDisplayEngineControlStub();
    data.WriteRemoteObject(callbackObj);
    uint32_t code = static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::REGISTER_DE_STATUS_CHANGE_CALLBACK);
    int res = stub.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: RegisterDEStatusChangeCallbackSuccess
 * @tc.desc: Test REGISTER_DE_STATUS_CHANGE_CALLBACK with success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayEngineControlStubTest, RegisterDEStatusChangeCallbackSuccess, testing::ext::TestSize.Level1)
{
    RSDisplayEngineControlStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIDisplayEngineControl::GetDescriptor());
    sptr<IRemoteObject> callbackObj = new RSDEStatusChangeCallbackStub();
    data.WriteRemoteObject(callbackObj);
    uint32_t code = static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::REGISTER_DE_STATUS_CHANGE_CALLBACK);
    int res = stub.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: UnregisterDEStatusChangeCallbackSuccess
 * @tc.desc: Test UNREGISTER_DE_STATUS_CHANGE_CALLBACK success path
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlStubTest, UnregisterDEStatusChangeCallbackSuccess, testing::ext::TestSize.Level1)
{
    RSDisplayEngineControlStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIDisplayEngineControl::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIDisplayEngineControlInterfaceCode::UNREGISTER_DE_STATUS_CHANGE_CALLBACK);
    int res = stub.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequestDefaultCase
 * @tc.desc: Test OnRemoteRequest with unknown code falls through to IPCObjectStub
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayEngineControlStubTest, OnRemoteRequestDefaultCase, testing::ext::TestSize.Level1)
{
    RSDisplayEngineControlStub stub;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIDisplayEngineControl::GetDescriptor());
    uint32_t code = 9999;
    int res = stub.OnRemoteRequest(code, data, reply, option);
    EXPECT_NE(res, ERR_NONE);
}
} // namespace OHOS::Rosen