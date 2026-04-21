/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include <message_parcel.h>
#include <refbase.h>

#include "rs_render_connect_parcel_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderConnectParcelInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderConnectParcelInfoTest::SetUpTestCase() {}
void RSRenderConnectParcelInfoTest::TearDownTestCase() {}
void RSRenderConnectParcelInfoTest::SetUp() {}
void RSRenderConnectParcelInfoTest::TearDown() {}

/**
 * @tc.name: ReplyToRenderInfoMarshalling001
 * @tc.desc: Test ReplyToRenderInfo Marshalling with all valid members
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoMarshalling001, TestSize.Level1)
{
    auto remoteObj = sptr<IRemoteObject>::MakeSptr();
    ASSERT_NE(remoteObj, nullptr);
    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
    ASSERT_NE(screenProperty, nullptr);
    auto vsyncConn = sptr<IRemoteObject>::MakeSptr();
    ASSERT_NE(vsyncConn, nullptr);
    auto replayData = std::make_shared<IpcPersistenceTypeToDataMap>();

    ReplyToRenderInfo info(remoteObj, screenProperty, vsyncConn, replayData);
    MessageParcel parcel;
    EXPECT_TRUE(info.Marshalling(parcel));
}

/**
 * @tc.name: ReplyToRenderInfoMarshalling002
 * @tc.desc: Test ReplyToRenderInfo Marshalling with null composeConnection
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoMarshalling002, TestSize.Level1)
{
    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
    auto vsyncConn = sptr<IRemoteObject>::MakeSptr();
    auto replayData = std::make_shared<IpcPersistenceTypeToDataMap>();

    ReplyToRenderInfo info(nullptr, screenProperty, vsyncConn, replayData);
    MessageParcel parcel;
    EXPECT_FALSE(info.Marshalling(parcel));
}

/**
 * @tc.name: ReplyToRenderInfoMarshalling003
 * @tc.desc: Test ReplyToRenderInfo Marshalling with null rsScreenProperty
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoMarshalling003, TestSize.Level1)
{
    auto remoteObj = sptr<IRemoteObject>::MakeSptr();
    auto vsyncConn = sptr<IRemoteObject>::MakeSptr();
    auto replayData = std::make_shared<IpcPersistenceTypeToDataMap>();

    ReplyToRenderInfo info(remoteObj, nullptr, vsyncConn, replayData);
    MessageParcel parcel;
    EXPECT_FALSE(info.Marshalling(parcel));
}

/**
 * @tc.name: ReplyToRenderInfoMarshalling004
 * @tc.desc: Test ReplyToRenderInfo Marshalling with null vsyncConn
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoMarshalling004, TestSize.Level1)
{
    auto remoteObj = sptr<IRemoteObject>::MakeSptr();
    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
    auto replayData = std::make_shared<IpcPersistenceTypeToDataMap>();

    ReplyToRenderInfo info(remoteObj, screenProperty, nullptr, replayData);
    MessageParcel parcel;
    EXPECT_FALSE(info.Marshalling(parcel));
}

/**
 * @tc.name: ReplyToRenderInfoMarshalling005
 * @tc.desc: Test ReplyToRenderInfo Marshalling with null replayData
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoMarshalling005, TestSize.Level1)
{
    auto remoteObj = sptr<IRemoteObject>::MakeSptr();
    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
    auto vsyncConn = sptr<IRemoteObject>::MakeSptr();

    ReplyToRenderInfo info(remoteObj, screenProperty, vsyncConn, nullptr);
    MessageParcel parcel;
    EXPECT_FALSE(info.Marshalling(parcel));
}

/**
 * @tc.name: ReplyToRenderInfoUnmarshalling001
 * @tc.desc: Test ReplyToRenderInfo Unmarshalling with valid data
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoUnmarshalling001, TestSize.Level1)
{
    auto remoteObj = sptr<IRemoteObject>::MakeSptr();
    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
    auto vsyncConn = sptr<IRemoteObject>::MakeSptr();
    auto replayData = std::make_shared<IpcPersistenceTypeToDataMap>();

    ReplyToRenderInfo info(remoteObj, screenProperty, vsyncConn, replayData);
    MessageParcel parcel;
    ASSERT_TRUE(info.Marshalling(parcel));

    auto* result = ReplyToRenderInfo::Unmarshalling(parcel);
    ASSERT_NE(result, nullptr);
    EXPECT_NE(result->composeConnection_, nullptr);
    EXPECT_NE(result->rsScreenProperty_, nullptr);
    EXPECT_NE(result->vsyncConn_, nullptr);
    EXPECT_NE(result->replayData_, nullptr);
    delete result;
}

/**
 * @tc.name: ReplyToRenderInfoUnmarshalling002
 * @tc.desc: Test ReplyToRenderInfo Unmarshalling with empty parcel (null composeConnection)
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoUnmarshalling002, TestSize.Level1)
{
    MessageParcel parcel;
    auto* result = ReplyToRenderInfo::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ReplyToRenderInfoUnmarshalling003
 * @tc.desc: Test ReplyToRenderInfo Unmarshalling with composeConnection but null rsScreenProperty
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoUnmarshalling003, TestSize.Level1)
{
    MessageParcel parcel;
    auto remoteObj = sptr<IRemoteObject>::MakeSptr();
    ASSERT_TRUE(parcel.WriteRemoteObject(remoteObj));
    parcel.WriteParcelable(nullptr);

    auto* result = ReplyToRenderInfo::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ReplyToRenderInfoUnmarshalling004
 * @tc.desc: Test ReplyToRenderInfo Unmarshalling with composeConnection and rsScreenProperty but null vsyncConn
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoUnmarshalling004, TestSize.Level1)
{
    MessageParcel parcel;
    auto remoteObj = sptr<IRemoteObject>::MakeSptr();
    ASSERT_TRUE(parcel.WriteRemoteObject(remoteObj));
    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
    ASSERT_TRUE(parcel.WriteParcelable(screenProperty.GetRefPtr()));

    auto* result = ReplyToRenderInfo::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ReplyToRenderInfoUnmarshalling005
 * @tc.desc: Test ReplyToRenderInfo Unmarshalling with composeConnection, rsScreenProperty, vsyncConn but invalid
 *          replayData
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoUnmarshalling005, TestSize.Level1)
{
    MessageParcel parcel;
    auto remoteObj = sptr<IRemoteObject>::MakeSptr();
    ASSERT_TRUE(parcel.WriteRemoteObject(remoteObj));
    auto screenProperty = sptr<RSScreenProperty>::MakeSptr();
    ASSERT_TRUE(parcel.WriteParcelable(screenProperty.GetRefPtr()));
    auto vsyncConn = sptr<IRemoteObject>::MakeSptr();
    ASSERT_TRUE(parcel.WriteRemoteObject(vsyncConn));
    constexpr uint32_t invalidMapSize = 200;
    ASSERT_TRUE(parcel.WriteUint32(invalidMapSize));

    auto* result = ReplyToRenderInfo::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ConnectToServiceInfoMarshalling001
 * @tc.desc: Test ConnectToServiceInfo Marshalling with all valid members
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ConnectToServiceInfoMarshalling001, TestSize.Level1)
{
    auto connection = sptr<IRemoteObject>::MakeSptr();
    ASSERT_NE(connection, nullptr);
    auto vsyncToken = sptr<IRemoteObject>::MakeSptr();
    ASSERT_NE(vsyncToken, nullptr);

    ConnectToServiceInfo info(connection, vsyncToken);
    MessageParcel parcel;
    EXPECT_TRUE(info.Marshalling(parcel));
}

/**
 * @tc.name: ConnectToServiceInfoMarshalling002
 * @tc.desc: Test ConnectToServiceInfo Marshalling with null composerToRenderConnection
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ConnectToServiceInfoMarshalling002, TestSize.Level1)
{
    auto vsyncToken = sptr<IRemoteObject>::MakeSptr();

    ConnectToServiceInfo info(nullptr, vsyncToken);
    MessageParcel parcel;
    EXPECT_FALSE(info.Marshalling(parcel));
}

/**
 * @tc.name: ConnectToServiceInfoMarshalling003
 * @tc.desc: Test ConnectToServiceInfo Marshalling with null vsyncToken
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ConnectToServiceInfoMarshalling003, TestSize.Level1)
{
    auto connection = sptr<IRemoteObject>::MakeSptr();

    ConnectToServiceInfo info(connection, nullptr);
    MessageParcel parcel;
    EXPECT_FALSE(info.Marshalling(parcel));
}

/**
 * @tc.name: ConnectToServiceInfoUnmarshalling001
 * @tc.desc: Test ConnectToServiceInfo Unmarshalling with valid data
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ConnectToServiceInfoUnmarshalling001, TestSize.Level1)
{
    auto connection = sptr<IRemoteObject>::MakeSptr();
    auto vsyncToken = sptr<IRemoteObject>::MakeSptr();

    ConnectToServiceInfo info(connection, vsyncToken);
    MessageParcel parcel;
    ASSERT_TRUE(info.Marshalling(parcel));

    auto* result = ConnectToServiceInfo::Unmarshalling(parcel);
    ASSERT_NE(result, nullptr);
    EXPECT_NE(result->composerToRenderConnection_, nullptr);
    EXPECT_NE(result->vsyncToken_, nullptr);
    delete result;
}

/**
 * @tc.name: ConnectToServiceInfoUnmarshalling002
 * @tc.desc: Test ConnectToServiceInfo Unmarshalling with empty parcel (null composerToRenderConnection)
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ConnectToServiceInfoUnmarshalling002, TestSize.Level1)
{
    MessageParcel parcel;
    auto* result = ConnectToServiceInfo::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ConnectToServiceInfoUnmarshalling003
 * @tc.desc: Test ConnectToServiceInfo Unmarshalling with composerToRenderConnection but null vsyncToken
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ConnectToServiceInfoUnmarshalling003, TestSize.Level1)
{
    MessageParcel parcel;
    auto connection = sptr<IRemoteObject>::MakeSptr();
    ASSERT_TRUE(parcel.WriteRemoteObject(connection));

    auto* result = ConnectToServiceInfo::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ReplyToRenderInfoDefaultConstructor001
 * @tc.desc: Test ReplyToRenderInfo default constructor initializes members to null
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ReplyToRenderInfoDefaultConstructor001, TestSize.Level1)
{
    ReplyToRenderInfo info;
    EXPECT_EQ(info.composeConnection_, nullptr);
    EXPECT_EQ(info.rsScreenProperty_, nullptr);
    EXPECT_EQ(info.vsyncConn_, nullptr);
    EXPECT_EQ(info.replayData_, nullptr);
}

/**
 * @tc.name: ConnectToServiceInfoDefaultConstructor001
 * @tc.desc: Test ConnectToServiceInfo default constructor initializes members to null
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderConnectParcelInfoTest, ConnectToServiceInfoDefaultConstructor001, TestSize.Level1)
{
    ConnectToServiceInfo info;
    EXPECT_EQ(info.composerToRenderConnection_, nullptr);
    EXPECT_EQ(info.vsyncToken_, nullptr);
}
} // namespace OHOS::Rosen
