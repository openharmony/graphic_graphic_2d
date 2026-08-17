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

#include <gtest/gtest.h>

#include "transaction/rs_hgm_config_data.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr uint32_t MAX_ANIM_DYNAMIC_ITEM_SIZE = 256;
constexpr uint32_t MAX_PAGE_NAME_SIZE = 64;
constexpr uint32_t MAX_APP_BUFFER_SIZE = 64;

class RSHgmConfigDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHgmConfigDataTest::SetUpTestCase() {}
void RSHgmConfigDataTest::TearDownTestCase() {}
void RSHgmConfigDataTest::SetUp() {}
void RSHgmConfigDataTest::TearDown() {}

/**
 * @tc.name: UnmarshallingTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    std::string type = "test";
    std::string name = "test";
    int minSpeed = 1;
    int maxSpeed = 2;
    int preferredFps = 3;
    AnimDynamicItem item = {type, name, minSpeed, maxSpeed, preferredFps};
    rsHgmConfigData.AddAnimDynamicItem(item);
    rsHgmConfigData.Marshalling(parcel);
    RSHgmConfigData* rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel);
    ASSERT_NE(rsHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    parcel.WriteFloat(rsHgmConfigData.ppi_);
    parcel.WriteFloat(rsHgmConfigData.xDpi_);
    parcel.WriteFloat(rsHgmConfigData.yDpi_);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    parcel.WriteInt32(0);
    parcel.WriteInt32(1000);
    parcel.WriteInt32(120);
    parcel.WriteUint32(1);
    parcel.WriteString("appBuffer");
    parcel.WriteUint32(1);
    parcel.WriteString("pageName");
    RSHgmConfigData* rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rsHgmConfigDataPtr->configData_.size(), 1);
    EXPECT_EQ(rsHgmConfigDataPtr->appBufferList_.size(), 1);
    EXPECT_EQ(rsHgmConfigDataPtr->pageNameList_.size(), 1);

    Parcel parcel2;
    parcel2.WriteFloat(rsHgmConfigData.ppi_);
    parcel2.WriteFloat(rsHgmConfigData.xDpi_);
    parcel2.WriteFloat(rsHgmConfigData.yDpi_);
    parcel2.WriteUint32(MAX_ANIM_DYNAMIC_ITEM_SIZE + 1);
    rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel2);

    Parcel parcel3;
    parcel3.WriteFloat(rsHgmConfigData.ppi_);
    parcel3.WriteFloat(rsHgmConfigData.xDpi_);
    parcel3.WriteFloat(rsHgmConfigData.yDpi_);
    parcel3.WriteUint32(0);
    parcel3.WriteUint32(MAX_PAGE_NAME_SIZE + 1);
    rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel3);
    
    Parcel parcel4;
    parcel4.WriteFloat(rsHgmConfigData.ppi_);
    parcel4.WriteFloat(rsHgmConfigData.xDpi_);
    parcel4.WriteFloat(rsHgmConfigData.yDpi_);
    parcel4.WriteUint32(MAX_ANIM_DYNAMIC_ITEM_SIZE);
    rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel4);

    Parcel parcel5;
    parcel5.WriteFloat(rsHgmConfigData.ppi_);
    parcel5.WriteFloat(rsHgmConfigData.xDpi_);
    parcel5.WriteFloat(rsHgmConfigData.yDpi_);
    parcel5.WriteUint32(0);
    parcel5.WriteUint32(MAX_PAGE_NAME_SIZE);
    rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel5);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, MarshallingTest, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    std::string type = "test";
    std::string name = "test";
    int minSpeed = 1;
    int maxSpeed = 2;
    int preferredFps = 3;
    AnimDynamicItem item = { type, name, minSpeed, maxSpeed, preferredFps };
    rsHgmConfigData.AddAnimDynamicItem(item);
    bool marshalling = rsHgmConfigData.Marshalling(parcel);
    ASSERT_TRUE(marshalling);
}

/**
 * @tc.name: AppBufferListApiTest
 * @tc.desc: test GetAppBufferList, SetAppBufferList, AddAppBuffer APIs
 * @tc.type: FUNC
 * @tc.require: issue24889
 */
HWTEST_F(RSHgmConfigDataTest, AppBufferListApiTest, TestSize.Level1)
{
    RSHgmConfigData data;
    EXPECT_TRUE(data.GetAppBufferList().empty());

    const std::string buf1{"buf1"};
    const std::string buf2{"buf2"};
    std::vector<std::string> list = {buf1, buf2};
    data.SetAppBufferList(list);
    EXPECT_EQ(data.GetAppBufferList().size(), 2);
    EXPECT_EQ(data.GetAppBufferList()[0], buf1);
    EXPECT_EQ(data.GetAppBufferList()[1], buf2);

    const std::string buf3{"buf3"};
    data.AddAppBuffer(buf3);
    EXPECT_EQ(data.GetAppBufferList().size(), 3);
    EXPECT_EQ(data.GetAppBufferList()[2], buf3);

    const std::string buf4{"buf4"};
    data.AddAppBuffer(buf4);
    EXPECT_EQ(data.GetAppBufferList().size(), 4);
    EXPECT_EQ(data.GetAppBufferList()[3], buf4);
}

/**
 * @tc.name: MarshallingWithAppBufferListTest
 * @tc.desc: test Marshalling and Unmarshalling round-trip with appBufferList
 * @tc.type: FUNC
 * @tc.require: issue24889
 */
HWTEST_F(RSHgmConfigDataTest, MarshallingWithAppBufferListTest, TestSize.Level1)
{
    RSHgmConfigData srcData;
    const std::string name1{"buffer_a"};
    const std::string name2{"buffer_b"};
    srcData.AddAppBuffer(name1);
    srcData.AddAppBuffer(name2);

    Parcel parcel;
    EXPECT_TRUE(srcData.Marshalling(parcel));

    RSHgmConfigData* dstData = srcData.Unmarshalling(parcel);
    ASSERT_NE(dstData, nullptr);
    EXPECT_EQ(dstData->GetAppBufferList().size(), 2);
    EXPECT_EQ(dstData->GetAppBufferList()[0], name1);
    EXPECT_EQ(dstData->GetAppBufferList()[1], name2);
    delete dstData;
}

/**
 * @tc.name: UnmarshallingAppBufferListReadSizeFailTest
 * @tc.desc: UnmarshallingAppBufferList fails when ReadUint32(appBufferSize) fails
 * @tc.type: FUNC
 * @tc.require: issue24889
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingAppBufferListReadSizeFailTest, TestSize.Level1)
{
    RSHgmConfigData rsHgmConfigData;
    Parcel parcel;
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteUint32(0);

    RSHgmConfigData* result = rsHgmConfigData.Unmarshalling(parcel);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: UnmarshallingAppBufferListExceedMaxSizeTest
 * @tc.desc: UnmarshallingAppBufferList fails when appBufferSize > MAX_APP_BUFFER_SIZE
 * @tc.type: FUNC
 * @tc.require: issue24889
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingAppBufferListExceedMaxSizeTest, TestSize.Level1)
{
    RSHgmConfigData rsHgmConfigData;
    Parcel parcel;
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteUint32(0);
    parcel.WriteUint32(MAX_APP_BUFFER_SIZE + 1);

    RSHgmConfigData* result = rsHgmConfigData.Unmarshalling(parcel);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: UnmarshallingAppBufferListReadStringFailTest
 * @tc.desc: UnmarshallingAppBufferList fails when ReadString fails in loop
 * @tc.type: FUNC
 * @tc.require: issue24889
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingAppBufferListReadStringFailTest, TestSize.Level1)
{
    RSHgmConfigData rsHgmConfigData;
    Parcel parcel;
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteUint32(0);
    parcel.WriteUint32(2);
    parcel.WriteString("valid");

    RSHgmConfigData* result = rsHgmConfigData.Unmarshalling(parcel);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: UnmarshallingAppBufferListSkipEmptyTest
 * @tc.desc: UnmarshallingAppBufferList skips empty strings
 * @tc.type: FUNC
 * @tc.require: issue24889
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingAppBufferListSkipEmptyTest, TestSize.Level1)
{
    RSHgmConfigData rsHgmConfigData;
    Parcel parcel;
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteUint32(0);
    parcel.WriteUint32(3);
    const std::string valid1{"valid1"};
    const std::string valid2{"valid2"};
    parcel.WriteString(valid1);
    parcel.WriteString("");
    parcel.WriteString(valid2);
    parcel.WriteUint32(0);

    RSHgmConfigData* result = rsHgmConfigData.Unmarshalling(parcel);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->GetAppBufferList().size(), 2);
    EXPECT_EQ(result->GetAppBufferList()[0], valid1);
    EXPECT_EQ(result->GetAppBufferList()[1], valid2);
    delete result;
}

/**
 * @tc.name: UnmarshallingAppBufferListZeroSizeTest
 * @tc.desc: UnmarshallingAppBufferList with zero size succeeds
 * @tc.type: FUNC
 * @tc.require: issue24889
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingAppBufferListZeroSizeTest, TestSize.Level1)
{
    RSHgmConfigData rsHgmConfigData;
    Parcel parcel;
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteUint32(0);
    parcel.WriteUint32(0);
    parcel.WriteUint32(0);

    RSHgmConfigData* result = rsHgmConfigData.Unmarshalling(parcel);
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->GetAppBufferList().empty());
    delete result;
}
} // namespace Rosen
} // namespace OHOS