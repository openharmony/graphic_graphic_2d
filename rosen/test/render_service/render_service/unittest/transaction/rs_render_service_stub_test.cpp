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
#include "hgm_core.h"
#include "limit_number.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "render_server/rs_render_service.h"
#include "render_server/transaction/zidl/rs_render_service_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    
    static sptr<RSIConnectionToken> token_;
    static sptr<RSRenderServiceStub> stub_;
};
sptr<RSIConnectionToken> RSRenderServiceStubTest::token_ = new IRemoteStub<RSIConnectionToken>();
sptr<RSRenderServiceStub> RSRenderServiceStubTest::stub_ = new RSRenderService();

void RSRenderServiceStubTest::SetUpTestCase() {}

void RSRenderServiceStubTest::TearDownTestCase()
{
    token_ = nullptr;
    stub_ = nullptr;
}

void RSRenderServiceStubTest::SetUp() {}
void RSRenderServiceStubTest::TearDown() {}

/**
 * @tc.name: TestRSRenderServiceStub001
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceStubTest, TestRSRenderServiceStub001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::CREATE_CONNECTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: TestRSRenderServiceStub002
 * @tc.desc: Test if remoteObj is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceStubTest, TestRSRenderServiceStub002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(RSIRenderService::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::CREATE_CONNECTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);
}

/**
 * @tc.name: TestRSRenderServiceStub003
 * @tc.desc: Test if the code not exists.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceStubTest, TestRSRenderServiceStub003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = stub_->OnRemoteRequest(-1, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: TestRSRenderServiceStub004
 * @tc.desc: Test if data has no content.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceStubTest, TestRSRenderServiceStub004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
 
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::REMOVE_CONNECTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: TestRSRenderServiceStub005
 * @tc.desc: Test if remoteObj is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceStubTest, TestRSRenderServiceStub005, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
 
    data.WriteInterfaceToken(RSIRenderService::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::REMOVE_CONNECTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_NULL_OBJECT);
}

/**
 * @tc.name: TestRSRenderServiceStub006
 * @tc.desc: Test if remoteObj is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueI60KUK
 */
HWTEST_F(RSRenderServiceStubTest, TestRSRenderServiceStub006, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
 
    data.WriteInterfaceToken(RSIRenderService::GetDescriptor());
    auto token = new IRemoteStub<RSIConnectionToken>();
    auto tokenObj = token->AsObject();
    data.WriteRemoteObject(tokenObj);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceInterfaceCode::REMOVE_CONNECTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_UNKNOWN_OBJECT);
}

/**
 * @tc.name: GetHgmContextTest001
 * @tc.desc: Test GetHgmContext when hgmPolicyEnabled is true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceStubTest, GetHgmContextTest001, TestSize.Level1)
{
    auto renderService = static_cast<RSRenderService*>(stub_.GetRefPtr());
    ASSERT_NE(renderService, nullptr);

    auto orgHgmPolicyEnabled = HgmCore::Instance().hgmAbilityEnabled_;
    HgmCore::Instance().hgmAbilityEnabled_ = true;

    auto hgmContext = renderService->GetHgmContext();
    // When hgmPolicyEnabled is true, GetHgmContext returns hgmContext_ (may be nullptr if not initialized)
    EXPECT_EQ(hgmContext, renderService->hgmContext_);
    // When hgmPolicyEnabled is false, GetHgmContext returns nullptr
    HgmCore::Instance().hgmAbilityEnabled_ = false;
    EXPECT_EQ(hgmContext, nullptr);
    HgmCore::Instance().hgmAbilityEnabled_ = orgHgmPolicyEnabled;
}
} // namespace OHOS::Rosen
