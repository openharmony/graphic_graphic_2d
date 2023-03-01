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

#include "rs_window_animation_finished_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSWindowAnimationFinishedCallbackStubMock : public RSWindowAnimationFinishedCallbackStub {
public:
    RSWindowAnimationFinishedCallbackStubMock() = default;
    virtual ~RSWindowAnimationFinishedCallbackStubMock() = default;

    void OnAnimationFinished() override {};
};

class RSWindowAnimationFinishedCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSWindowAnimationFinishedCallbackStubTest::SetUpTestCase() {}
void RSWindowAnimationFinishedCallbackStubTest::TearDownTestCase() {}
void RSWindowAnimationFinishedCallbackStubTest::SetUp() {}
void RSWindowAnimationFinishedCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify the OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationFinishedCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationFinishedCallbackStubTest OnRemoteRequest001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    
    auto finishedCallbackStub = std::make_shared<RSWindowAnimationFinishedCallbackStubMock>();
    int res = finishedCallbackStub->OnRemoteRequest(
        RSIWindowAnimationFinishedCallback::ON_ANIMATION_FINISHED, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "RSWindowAnimationFinishedCallbackStubTest OnRemoteRequest001 end";
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify the OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationFinishedCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationFinishedCallbackStubTest OnRemoteRequest002 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationFinishedCallback::GetDescriptor());

    auto finishedCallbackStub = std::make_shared<RSWindowAnimationFinishedCallbackStubMock>();
    int res = finishedCallbackStub->OnRemoteRequest(-1, data, reply, option);
    ASSERT_EQ(res, ERR_UNKNOWN_TRANSACTION);
    GTEST_LOG_(INFO) << "RSWindowAnimationFinishedCallbackStubTest OnRemoteRequest002 end";
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify the OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationFinishedCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationFinishedCallbackStubTest OnRemoteRequest003 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationFinishedCallback::GetDescriptor());
    
    auto finishedCallbackStub = std::make_shared<RSWindowAnimationFinishedCallbackStubMock>();
    int res = finishedCallbackStub->OnRemoteRequest(
        RSIWindowAnimationFinishedCallback::ON_ANIMATION_FINISHED, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationFinishedCallbackStubTest OnRemoteRequest003 end";
}
} // namespace Rosen
} // namespace OHOS