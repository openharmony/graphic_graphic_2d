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

#include "ipc_callbacks/rs_transaction_callback.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSTransactionCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionCallbackTest::SetUpTestCase() {}
void RSTransactionCallbackTest::TearDownTestCase() {}
void RSTransactionCallbackTest::SetUp() {}
void RSTransactionCallbackTest::TearDown() {}

GSError OnCompletedCallbackStub(const OnCompletedRet& ret)
{
    return GSERROR_OK;
}

GSError GoBackgroundCallbackStub(uint64_t queueId)
{
    return GSERROR_OK;
}

GSError OnDropBuffersCallbackStub(const OnDropBuffersRet& ret)
{
    return GSERROR_OK;
}

GSError SetDefaultWidthAndHeightCallbackStub(const OnSetDefaultWidthAndHeightRet& ret)
{
    return GSERROR_OK;
}

GSError OnSurfaceDumpCallbackStub(const OnSurfaceDumpRet& ret)
{
    return GSERROR_OK;
}

/**
 * @tc.name: RSDefaultTransactionCallback001
 * @tc.desc: Verify the RSDefaultTransactionCallback
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackTest, RSDefaultTransactionCallback001, TestSize.Level1)
{
    TransactionCallbackFuncs funcs;
    auto transactionDataCallback = new RSDefaultTransactionCallback(funcs);
    ASSERT_TRUE(transactionDataCallback != nullptr);
}

/**
 * @tc.name: OnCompleted001
 * @tc.desc: Verify the OnCompleted
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackTest, OnCompleted001, TestSize.Level1)
{
    TransactionCallbackFuncs funcs = {
        .onCompleted = [](const OnCompletedRet& ret) { return GSERROR_OK; },
        .goBackground = [](uint64_t id) { return GSERROR_OK; },
        .onDropBuffers = [](const OnDropBuffersRet& ret) { return GSERROR_OK; },
        .setDefaultWidthAndHeight = [](const OnSetDefaultWidthAndHeightRet& ret) { return GSERROR_OK; },
        .onSurfaceDump = [](const OnSurfaceDumpRet& ret)  { return GSERROR_OK; }
    };
    auto transactionDataCallback = new RSDefaultTransactionCallback(funcs);
    ASSERT_TRUE(transactionDataCallback != nullptr);

    // onCompletedCallback_ is nullptr
    transactionDataCallback->onCompletedCallback_ = nullptr;
    OnCompletedRet ret;
    GSError retVal = transactionDataCallback->OnCompleted(ret);
    ASSERT_EQ(retVal, GSERROR_INTERNAL);
    // onCompletedCallback_ is not nullptr
    transactionDataCallback->onCompletedCallback_ = OnCompletedCallbackStub;
    retVal = transactionDataCallback->OnCompleted(ret);
    ASSERT_EQ(retVal, GSERROR_OK);
}

/**
 * @tc.name: GoBackground001
 * @tc.desc: Verify the GoBackground001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackTest, GoBackground001, TestSize.Level1)
{
    TransactionCallbackFuncs funcs = {
        .onCompleted = [](const OnCompletedRet& ret) { return GSERROR_OK; },
        .goBackground = [](uint64_t id) { return GSERROR_OK; },
        .onDropBuffers = [](const OnDropBuffersRet& ret) { return GSERROR_OK; },
        .setDefaultWidthAndHeight = [](const OnSetDefaultWidthAndHeightRet& ret) { return GSERROR_OK; },
        .onSurfaceDump = [](const OnSurfaceDumpRet& ret)  { return GSERROR_OK; }
    };
    auto transactionDataCallback = new RSDefaultTransactionCallback(funcs);
    ASSERT_TRUE(transactionDataCallback != nullptr);

    // goBackgroundCallback_ is nullptr
    transactionDataCallback->goBackgroundCallback_ = nullptr;
    uint64_t queueId = 1;
    GSError retVal = transactionDataCallback->GoBackground(queueId);
    ASSERT_EQ(retVal, GSERROR_INTERNAL);
    // goBackgroundCallback_ is not nullptr
    transactionDataCallback->goBackgroundCallback_ = GoBackgroundCallbackStub;
    retVal = transactionDataCallback->GoBackground(queueId);
    ASSERT_EQ(retVal, GSERROR_OK);
}

/**
 * @tc.name: OnDropBuffers001
 * @tc.desc: Verify the GoBackground001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackTest, OnDropBuffers001, TestSize.Level1)
{
    TransactionCallbackFuncs funcs = {
        .onCompleted = [](const OnCompletedRet& ret) { return GSERROR_OK; },
        .goBackground = [](uint64_t id) { return GSERROR_OK; },
        .onDropBuffers = [](const OnDropBuffersRet& ret) { return GSERROR_OK; },
        .setDefaultWidthAndHeight = [](const OnSetDefaultWidthAndHeightRet& ret) { return GSERROR_OK; },
        .onSurfaceDump = [](const OnSurfaceDumpRet& ret)  { return GSERROR_OK; }
    };
    auto transactionDataCallback = new RSDefaultTransactionCallback(funcs);
    ASSERT_TRUE(transactionDataCallback != nullptr);

    // onDropBuffersCallback_ is nullptr
    transactionDataCallback->onDropBuffersCallback_ = nullptr;
    OnDropBuffersRet ret;
    GSError retVal = transactionDataCallback->OnDropBuffers(ret);
    ASSERT_EQ(retVal, GSERROR_INTERNAL);
    // onDropBuffersCallback_ is not nullptr
    transactionDataCallback->onDropBuffersCallback_ = OnDropBuffersCallbackStub;
    retVal = transactionDataCallback->OnDropBuffers(ret);
    ASSERT_EQ(retVal, GSERROR_OK);
}

/**
 * @tc.name: SetDefaultWidthAndHeight001
 * @tc.desc: Verify the SetDefaultWidthAndHeight001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackTest, SetDefaultWidthAndHeight001, TestSize.Level1)
{
    TransactionCallbackFuncs funcs = {
        .onCompleted = [](const OnCompletedRet& ret) { return GSERROR_OK; },
        .goBackground = [](uint64_t id) { return GSERROR_OK; },
        .onDropBuffers = [](const OnDropBuffersRet& ret) { return GSERROR_OK; },
        .setDefaultWidthAndHeight = [](const OnSetDefaultWidthAndHeightRet& ret) { return GSERROR_OK; },
        .onSurfaceDump = [](const OnSurfaceDumpRet& ret)  { return GSERROR_OK; }
    };
    auto transactionDataCallback = new RSDefaultTransactionCallback(funcs);
    ASSERT_TRUE(transactionDataCallback != nullptr);

    // setDefaultWidthAndHeightCallback_ is nullptr
    transactionDataCallback->setDefaultWidthAndHeightCallback_ = nullptr;
    OnSetDefaultWidthAndHeightRet ret = {.queueId = 2, .height = 1, .width = 3};
    GSError retVal = transactionDataCallback->SetDefaultWidthAndHeight(ret);
    ASSERT_EQ(retVal, GSERROR_INTERNAL);
    // setDefaultWidthAndHeightCallback_ is not nullptr
    transactionDataCallback->setDefaultWidthAndHeightCallback_ = SetDefaultWidthAndHeightCallbackStub;
    retVal = transactionDataCallback->SetDefaultWidthAndHeight(ret);
    ASSERT_EQ(retVal, GSERROR_OK);
}

/**
 * @tc.name: OnSurfaceDump001
 * @tc.desc: Verify the OnSurfaceDump001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackTest, OnSurfaceDump001, TestSize.Level1)
{
    TransactionCallbackFuncs funcs = {
        .onCompleted = [](const OnCompletedRet& ret) { return GSERROR_OK; },
        .goBackground = [](uint64_t id) { return GSERROR_OK; },
        .onDropBuffers = [](const OnDropBuffersRet& ret) { return GSERROR_OK; },
        .setDefaultWidthAndHeight = [](const OnSetDefaultWidthAndHeightRet& ret) { return GSERROR_OK; },
        .onSurfaceDump = [](const OnSurfaceDumpRet& ret)  { return GSERROR_OK; }
    };
    auto transactionDataCallback = new RSDefaultTransactionCallback(funcs);
    ASSERT_TRUE(transactionDataCallback != nullptr);

    // onSurfaceDumpCallback_ is nullptr
    transactionDataCallback->onSurfaceDumpCallback_ = nullptr;
    OnSurfaceDumpRet ret;
    GSError retVal = transactionDataCallback->OnSurfaceDump(ret);
    ASSERT_EQ(retVal, GSERROR_INTERNAL);
    // onSurfaceDumpCallback_ is not nullptr
    transactionDataCallback->onSurfaceDumpCallback_ = OnSurfaceDumpCallbackStub;
    retVal = transactionDataCallback->OnSurfaceDump(ret);
    ASSERT_EQ(retVal, GSERROR_OK);
}
} // namespace Rosen
} // namespace OHOS
