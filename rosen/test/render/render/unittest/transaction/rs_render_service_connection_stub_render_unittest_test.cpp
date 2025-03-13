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
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "transaction/rs_render_service_connection_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    int OnRemoteRequestTest(uint32_t code);
    static sptr<RSIConnectionToken> token_;
    static sptr<RSRenderServiceConnectionStub> connectionStub_;
};

sptr<RSIConnectionToken> RSRenderServiceConnectionTest::token_ = new IRemoteStub<RSIConnectionToken>();
sptr<RSRenderServiceConnectionStub> RSRenderServiceConnectionTest::connectionStub_ = new RSRenderServiceConnection(
    0, nullptr, RSMainThread::Instance(), CreateOrGetScreenManager(), token_->AsObject(), nullptr);

void RSRenderServiceConnectionTest::SetUpTestCase() {}

void RSRenderServiceConnectionTest::TearDownTestCase()
{
    token_ = nullptr;
    connectionStub_ = nullptr;
}

void RSRenderServiceConnectionTest::SetUp() {}
void RSRenderServiceConnectionTest::TearDown() {}
int RSRenderServiceConnectionTest::OnRemoteRequestTest(uint32_t code)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    return connectionStub_->OnRemoteRequest(code, data, reply, option);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub0011
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KU1
 */
HWTEST_F(RSRenderServiceConnectionTest, TestRSRenderServiceConnectionStub0011, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res;
    uint32_t code;

    code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION);
    res = connectionStub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub0021
 * @tc.desc: Test screen related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KU1
 */
HWTEST_F(RSRenderServiceConnectionTest, TestRSRenderServiceConnectionStub0021, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE)), ERR_UNKNOWN_REASON);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY)),
        ERR_UNKNOWN_REASON);
    ASSERT_NE(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE)), ERR_NONE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub0031
 * @tc.desc: Test virtual screen related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KU1
 */
HWTEST_F(RSRenderServiceConnectionTest, TestRSRenderServiceConnectionStub0031, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS)), ERR_NONE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub0041
 * @tc.desc: Test refresh rate related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KU1
 */
HWTEST_F(RSRenderServiceConnectionTest, TestRSRenderServiceConnectionStub0041, TestSize.Level1)
{
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO)), ERR_NONE);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE)), ERR_INVALID_STATE);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE)),
        ERR_INVALID_STATE);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED)), ERR_INVALID_STATE);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED)), ERR_INVALID_STATE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub0051
 * @tc.desc: Test register/unregister callback related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KU1
 */
HWTEST_F(RSRenderServiceConnectionTest, TestRSRenderServiceConnectionStub0051, TestSize.Level1)
{
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK)), ERR_NULL_OBJECT);
    // Unable to access IPC due to lack of permissions.
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    // Unable to access IPC due to lack of permissions.
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK)), ERR_NONE);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK)), ERR_NONE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub0061
 * @tc.desc: Test register/unregister callback related transaction, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KU1
 */
HWTEST_F(RSRenderServiceConnectionTest, TestRSRenderServiceConnectionStub0061, TestSize.Level1)
{
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    // Unable to access IPC due to lack of permissions.
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK)), ERR_NULL_OBJECT);
    // Unable to access IPC due to lack of permissions.
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK)), ERR_NONE);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK)), ERR_NONE);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT)), ERR_NULL_OBJECT);
    EXPECT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER)), ERR_NULL_OBJECT);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub0071
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueI60KU1
 */
HWTEST_F(RSRenderServiceConnectionTest, TestRSRenderServiceConnectionStub0071, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE)), ERR_INVALID_DATA);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK)), ERR_INVALID_STATE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK)), ERR_INVALID_STATE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED)), IPC_STUB_INVALID_DATA_ERR);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_DEFAULT_DEVICE_ROTATION_OFFSET)), ERR_NONE);
#ifdef TP_FEATURE_ENABLE
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG)), ERR_INVALID_STATE);
#endif
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE)), ERR_NULL_OBJECT);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO)), ERR_NONE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub0081
 * @tc.desc: Test performance/memory related ipc, with non empty data.
 * @tc.type: FUNC
 * @tc.require: issueI60KU1
 */
HWTEST_F(RSRenderServiceConnectionTest, TestRSRenderServiceConnectionStub0081, TestSize.Level1)
{
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE)), ERR_NONE);
    ASSERT_EQ(OnRemoteRequestTest(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS)), ERR_NONE);
}

/**
 * @tc.name: TestRSRenderServiceConnectionStub0091
 * @tc.desc: Test if the code not exists.
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSRenderServiceConnectionTest, TestRSRenderServiceConnectionStub0091, TestSize.Level1)
{
    MessageParcel reply;
    MessageOption option;
    MessageParcel data;
    int res = connectionStub_->OnRemoteRequest(-1, data, reply, option);
    ASSERT_EQ(res, IPC_STUB_UNKNOW_TRANS_ERR);
}
} // namespace OHOS::Rosen
