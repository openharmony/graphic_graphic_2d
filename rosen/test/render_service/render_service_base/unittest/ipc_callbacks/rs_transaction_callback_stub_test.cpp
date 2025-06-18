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

#include "gtest/gtest.h"

#include "ipc_callbacks/rs_transaction_callback_stub.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSTransactionCallbackStubMock : public RSTransactionCallbackStub {
public:
    RSTransactionCallbackStubMock() = default;
    virtual ~RSTransactionCallbackStubMock() = default;

    GSError OnCompleted(const OnCompletedRet& ret) override { return GSERROR_OK; };
    GSError GoBackground(uint64_t queueId) override { return GSERROR_OK; };
    GSError OnDropBuffers(const OnDropBuffersRet& ret) override { return GSERROR_OK; };
    GSError SetDefaultWidthAndHeight(const OnSetDefaultWidthAndHeightRet& ret) override { return GSERROR_OK; };
    GSError OnSurfaceDump(OnSurfaceDumpRet& ret) override { return GSERROR_OK; };
};

class RSTransactionCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionCallbackStubTest::SetUpTestCase() {}
void RSTransactionCallbackStubTest::TearDownTestCase() {}
void RSTransactionCallbackStubTest::SetUp() {}
void RSTransactionCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify the OnRemoteRequest001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    vector<RSITransactionCallbackInterfaceCode> codeVec = {
        RSITransactionCallbackInterfaceCode::ON_COMPLETED,
        RSITransactionCallbackInterfaceCode::GO_BACKGROUND,
        RSITransactionCallbackInterfaceCode::ON_DROPBUFFERS,
        RSITransactionCallbackInterfaceCode::SET_DEFAULT_WIDTH_AND_HEIGHT,
        RSITransactionCallbackInterfaceCode::ON_SURFACE_DUMP
    };
    for (auto code : codeVec) {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        auto stub = std::make_shared<RSTransactionCallbackStubMock>();
        int ret = stub->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
        ASSERT_EQ(ret, ERR_INVALID_STATE);

        bool writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
        ASSERT_EQ(writeToken, true);

        ret = stub->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
        ASSERT_EQ(ret, ERR_INVALID_DATA);
    }
}

/**
 * @tc.name: OnRemoteRequest002_ON_COMPLETED
 * @tc.desc: Verify the OnRemoteRequest002_ON_COMPLETED
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackStubTest, OnRemoteRequest002_ON_COMPLETED, TestSize.Level1)
{
    // ON_COMPLETED
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    bool writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);
    OnCompletedRet onCompletedRet = {
        .queueId = 1,
        .bufferSeqNum = 3,
        .isActiveGame = true,
        .releaseFence = SyncFence::INVALID_FENCE,
    };
    bool write = data.WriteUint64(onCompletedRet.queueId);
    ASSERT_EQ(write, true);
    write = data.WriteUint32(onCompletedRet.bufferSeqNum);
    ASSERT_EQ(write, true);
    write = onCompletedRet.releaseFence->WriteToMessageParcel(data);
    ASSERT_EQ(write, true);
    write = data.WriteBool(onCompletedRet.isActiveGame);
    ASSERT_EQ(write, true);
    auto stub = std::make_shared<RSTransactionCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_COMPLETED);
    int ret = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest002_GO_BACKGROUND
 * @tc.desc: Verify the OnRemoteRequest002_GO_BACKGROUND
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackStubTest, OnRemoteRequest002_GO_BACKGROUND, TestSize.Level1)
{
    // GO_BACKGROUND
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    bool writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);

    auto stub = std::make_shared<RSTransactionCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::GO_BACKGROUND);
    int ret = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);

    writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);
    uint64_t queueId = 1;
    bool write = data.WriteUint64(queueId);
    ASSERT_EQ(write, true);
    ret = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest002_ON_DROPBUFFERS
 * @tc.desc: Verify the OnRemoteRequest002_ON_DROPBUFFERS
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackStubTest, OnRemoteRequest002_ON_DROPBUFFERS, TestSize.Level1)
{
    // ON_DROPBUFFERS
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    bool writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);

    auto stub = std::make_shared<RSTransactionCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_DROPBUFFERS);
    int ret = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);

    writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);
    OnDropBuffersRet onDropBuffersRet = {.queueId = 1};
    uint32_t num = 1;
    bool write = data.WriteUint64(onDropBuffersRet.queueId);
    ASSERT_EQ(write, true);
    write = data.WriteUint32(num);
    ASSERT_EQ(write, true);
    uint32_t bufferSequence = 1;
    write = data.WriteUint32(bufferSequence);
    ASSERT_EQ(write, true);
    ret = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest003_ON_DROPBUFFERS
 * @tc.desc: Verify the OnRemoteRequest002_ON_DROPBUFFERS
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackStubTest, OnRemoteRequest003_ON_DROPBUFFERS, TestSize.Level1)
{
    // ON_DROPBUFFERS
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    bool writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);

    auto stub = std::make_shared<RSTransactionCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_DROPBUFFERS);
    int ret = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);

    writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);
    OnDropBuffersRet onDropBuffersRet = {.queueId = 1};
    uint32_t num = 1;
    bool write = data.WriteUint64(onDropBuffersRet.queueId);
    ASSERT_EQ(write, true);
    write = data.WriteUint32(num);
    ASSERT_EQ(write, true);
    ret = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: OnRemoteRequest002_SET_DEFAULT_WIDTH_AND_HEIGHT
 * @tc.desc: Verify the OnRemoteRequest002_SET_DEFAULT_WIDTH_AND_HEIGHT
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackStubTest, OnRemoteRequest002_SET_DEFAULT_WIDTH_AND_HEIGHT, TestSize.Level1)
{
    // SET_DEFAULT_WIDTH_AND_HEIGHT
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    bool writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);

    OnSetDefaultWidthAndHeightRet onSetDefaultWidthAndHeightRet = {
        .height = 1,
        .queueId = 2,
        .width = 3,
    };
    bool write = data.WriteUint64(onSetDefaultWidthAndHeightRet.queueId);
    ASSERT_EQ(write, true);
    write = data.WriteUint32(onSetDefaultWidthAndHeightRet.width);
    ASSERT_EQ(write, true);
    write = data.WriteUint32(onSetDefaultWidthAndHeightRet.height);
    ASSERT_EQ(write, true);

    auto stub = std::make_shared<RSTransactionCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::SET_DEFAULT_WIDTH_AND_HEIGHT);
    int ret = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest002_ON_SURFACE_DUMP
 * @tc.desc: Verify the OnRemoteRequest002_ON_SURFACE_DUMP
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackStubTest, OnRemoteRequest002_ON_SURFACE_DUMP, TestSize.Level1)
{
    // ON_SURFACE_DUMP
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    bool writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);

    OnSurfaceDumpRet onSurfaceDumpRet = {
        .queueId = 1,
    };
    bool write = data.WriteUint64(onSurfaceDumpRet.queueId);
    ASSERT_EQ(write, true);

    auto stub = std::make_shared<RSTransactionCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSITransactionCallbackInterfaceCode::ON_SURFACE_DUMP);
    int ret = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify the OnRemoteRequest003
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    // case default
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSTransactionCallbackStubMock>();
    int ret = stub->OnRemoteRequest(-1, data, reply, option);
    ASSERT_EQ(ret, ERR_INVALID_STATE);

    bool writeToken = data.WriteInterfaceToken(RSITransactionCallback::GetDescriptor());
    ASSERT_EQ(writeToken, true);
    ret = stub->OnRemoteRequest(-1, data, reply, option);
    ASSERT_NE(ret, ERR_NONE);
}
} // namespace Rosen
} // namespace OHOS
