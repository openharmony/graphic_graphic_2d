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
constexpr size_t PARCEL_MAX_CAPACITY = 2000 * 1024;

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

static void WriteBaseData(Parcel& parcel, uint32_t size)
{
    RSHgmConfigData rsHgmConfigData;
    parcel.WriteFloat(rsHgmConfigData.ppi_);
    parcel.WriteFloat(rsHgmConfigData.xDpi_);
    parcel.WriteFloat(rsHgmConfigData.yDpi_);
    parcel.WriteUint32(size);
}

static void SetLeftSize(Parcel& parcel, uint32_t leftSize)
{
    parcel.SetMaxCapacity(PARCEL_MAX_CAPACITY);
    size_t useSize = PARCEL_MAX_CAPACITY - leftSize;
    size_t writeInt32Count = useSize / sizeof(int32_t);
    size_t writeBoolCount = useSize % sizeof(int32_t);
    for (size_t i = 0; i < writeInt32Count; i++) {
        parcel.WriteInt32(0);
    }
    for (size_t j = 0; j < writeBoolCount; j++) {
        parcel.WriteBoolUnaligned(false);
    }
}

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
    EXPECT_EQ(rsHgmConfigDataPtr, nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(rsHgmConfigData.ppi_);
    parcel3.WriteFloat(rsHgmConfigData.xDpi_);
    parcel3.WriteFloat(rsHgmConfigData.yDpi_);
    parcel3.WriteUint32(0);
    parcel3.WriteUint32(MAX_PAGE_NAME_SIZE + 1);
    rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel3);
    EXPECT_EQ(rsHgmConfigDataPtr, nullptr);
    
    Parcel parcel4;
    parcel4.WriteFloat(rsHgmConfigData.ppi_);
    parcel4.WriteFloat(rsHgmConfigData.xDpi_);
    parcel4.WriteFloat(rsHgmConfigData.yDpi_);
    parcel4.WriteUint32(MAX_ANIM_DYNAMIC_ITEM_SIZE);
    rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel4);
    EXPECT_EQ(rsHgmConfigDataPtr, nullptr);

    Parcel parcel5;
    parcel5.WriteFloat(rsHgmConfigData.ppi_);
    parcel5.WriteFloat(rsHgmConfigData.xDpi_);
    parcel5.WriteFloat(rsHgmConfigData.yDpi_);
    parcel5.WriteUint32(0);
    parcel5.WriteUint32(MAX_PAGE_NAME_SIZE);
    rsHgmConfigDataPtr = rsHgmConfigData.Unmarshalling(parcel5);
    EXPECT_EQ(rsHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest003
 * @tc.desc: Verify Unmarshalling base fields are missing or size exceeds limit
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest003, TestSize.Level1)
{
    RSHgmConfigData rsHgmConfigData;
    Parcel parcel1;
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel1), nullptr);

    Parcel parcel2;
    parcel2.WriteFloat(rsHgmConfigData.ppi_);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel2), nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(rsHgmConfigData.ppi_);
    parcel3.WriteFloat(rsHgmConfigData.xDpi_);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel3), nullptr);

    Parcel parcel4;
    parcel4.WriteFloat(rsHgmConfigData.ppi_);
    parcel4.WriteFloat(rsHgmConfigData.xDpi_);
    parcel4.WriteFloat(rsHgmConfigData.yDpi_);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel4), nullptr);

    Parcel parcel5;
    WriteBaseData(parcel5, MAX_ANIM_DYNAMIC_ITEM_SIZE + 1);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel5), nullptr);
}

/**
 * @tc.name: UnmarshallingTest004
 * @tc.desc: Verify Unmarshalling anim dynamic item fields are missing
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest004, TestSize.Level1)
{
    RSHgmConfigData rsHgmConfigData;
    Parcel parcel1;
    WriteBaseData(parcel1, 1);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel1), nullptr);

    Parcel parcel2;
    WriteBaseData(parcel2, 1);
    parcel2.WriteString("type");
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel2), nullptr);

    Parcel parcel3;
    WriteBaseData(parcel3, 1);
    parcel3.WriteString("type");
    parcel3.WriteString("name");
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel3), nullptr);

    Parcel parcel4;
    WriteBaseData(parcel4, 1);
    parcel4.WriteString("type");
    parcel4.WriteString("name");
    parcel4.WriteInt32(0);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel4), nullptr);

    Parcel parcel5;
    WriteBaseData(parcel5, 1);
    parcel5.WriteString("type");
    parcel5.WriteString("name");
    parcel5.WriteInt32(0);
    parcel5.WriteInt32(1000);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel5), nullptr);
}

/**
 * @tc.name: UnmarshallingTest005
 * @tc.desc: Verify Unmarshalling page name fields are missing or size exceeds limit
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest005, TestSize.Level1)
{
    RSHgmConfigData rsHgmConfigData;
    Parcel parcel1;
    WriteBaseData(parcel1, 0);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel1), nullptr);

    Parcel parcel2;
    WriteBaseData(parcel2, 0);
    parcel2.WriteUint32(MAX_PAGE_NAME_SIZE + 1);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel2), nullptr);

    Parcel parcel3;
    WriteBaseData(parcel3, 0);
    parcel3.WriteUint32(1);
    EXPECT_EQ(rsHgmConfigData.Unmarshalling(parcel3), nullptr);
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

/**
 * @tc.name: MarshallingTest002
 * @tc.desc: Verify Marshalling returns false when parcel space is insufficient for each field
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, MarshallingTest002, TestSize.Level1)
{
    RSHgmConfigData rsHgmConfigData;
    rsHgmConfigData.AddAnimDynamicItem({ "xx1", "xx2", 0, 0, 0 });
    std::string pageName = "xx3";
    rsHgmConfigData.AddPageName(pageName);
    for (int32_t i = 0; i < 100; i = i + 4) {
        Parcel parcel;
        SetLeftSize(parcel, i);
        rsHgmConfigData.Marshalling(parcel)
    }
    Parcel parcelOk;
    SetLeftSize(parcelOk, 100);
    ASSERT_TRUE(rsHgmConfigData.Marshalling(parcelOk));
}
} // namespace Rosen
} // namespace OHOS