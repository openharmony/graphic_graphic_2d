/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "rs_irender_service.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "transaction/rs_render_service_connection_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceConnectionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    
    static sptr<RSIConnectionToken> token_;
    static sptr<RSRenderServiceConnectionStub> connectionStub_;
};
sptr<RSIConnectionToken> RSRenderServiceConnectionStubTest::token_ = new IRemoteStub<RSIConnectionToken>();
sptr<RSRenderServiceConnectionStub> RSRenderServiceConnectionStubTest::connectionStub_ =
    new RSRenderServiceConnection(0, nullptr, RSMainThread::Instance(), nullptr, token_->AsObject(), nullptr);

void RSRenderServiceConnectionStubTest::SetUpTestCase() {}

void RSRenderServiceConnectionStubTest::TearDownTestCase()
{
    token_ = nullptr;
    connectionStub_ = nullptr;
}

void RSRenderServiceConnectionStubTest::SetUp() {}
void RSRenderServiceConnectionStubTest::TearDown() {}

/**
 * @tc.name: TestRSRenderServiceConnectionStub002
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceConnectionStubTest, TestRSRenderServiceConnectionStub002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    
    int res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_RENDER_MODE_CHANGE_CALLBACK, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_DEFAULT_SCREEN_ID, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_ALL_SCREEN_IDS, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::CREATE_VIRTUAL_SCREEN, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_SURFACE, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::REMOVE_VIRTUAL_SCREEN, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_SCREEN_CHANGE_CALLBACK, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_SCREEN_ACTIVE_MODE, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_RESOLUTION, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_SCREEN_POWER_STATUS, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(RSRenderServiceConnectionStub::TAKE_SURFACE_CAPTURE, data, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::REGISTER_APPLICATION_AGENT, data, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_VIRTUAL_SCREEN_RESOLUTION, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_ACTIVE_MODE, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub003
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceConnectionStubTest, TestRSRenderServiceConnectionStub003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_MODES, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_CAPABILITY, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_POWER_STATUS, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_DATA, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::EXECUTE_SYNCHRONOUS_TASK, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_BACK_LIGHT, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_SCREEN_BACK_LIGHT, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_BUFFER_AVAILABLE_LISTENER, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_GAMUTS, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_SUPPORTED_METADATAKEYS, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub004
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceConnectionStubTest, TestRSRenderServiceConnectionStub004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_GAMUT, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_SCREEN_GAMUT, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_SCREEN_GAMUT_MAP, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_GAMUT_MAP, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_HDR_CAPABILITY, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::GET_SCREEN_TYPE, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_SCREEN_SKIP_FRAME_INTERVAL, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::REGISTER_OCCLUSION_CHANGE_CALLBACK, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::UNREGISTER_OCCLUSION_CHANGE_CALLBACK, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub005
 * @tc.desc: Test if data has content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceConnectionStubTest, TestRSRenderServiceConnectionStub005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    int res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::SET_VIRTUAL_SCREEN_SURFACE, data, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);

    MessageParcel data2;
    data2.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::EXECUTE_SYNCHRONOUS_TASK, data2, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    data3.WriteUint16(RS_NODE_SYNCHRONOUS_READ_PROPERTY);
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::EXECUTE_SYNCHRONOUS_TASK, data3, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);

    MessageParcel data4;
    data4.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    res = connectionStub_->OnRemoteRequest(
        RSRenderServiceConnectionStub::UNREGISTER_OCCLUSION_CHANGE_CALLBACK, data4, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub006
 * @tc.desc: Test if the code not exists.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceConnectionStubTest, TestRSRenderServiceConnectionStub006, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = connectionStub_->OnRemoteRequest(-1, data, reply, option);
    ASSERT_EQ(res, ERR_UNKNOWN_TRANSACTION);
}
} // namespace OHOS::Rosen
