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

#include <gtest/gtest.h>

#include "transaction/rp_hgm_config_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr uint32_t MAX_ANIM_DYNAMIC_ITEM_SIZE = 256;
constexpr size_t PARCEL_MAX_CAPACITY = 2000 * 1024;

class RPHgmConfigDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RPHgmConfigDataTest::SetUpTestCase() {}
void RPHgmConfigDataTest::TearDownTestCase() {}
void RPHgmConfigDataTest::SetUp() {}
void RPHgmConfigDataTest::TearDown() {}

static void InitParcelData(Parcel& parcel)
{
    RPHgmConfigData rpHgmConfigData;
    parcel.WriteFloat(rpHgmConfigData.ppi_);
    parcel.WriteFloat(rpHgmConfigData.xDpi_);
    parcel.WriteFloat(rpHgmConfigData.yDpi_);
    parcel.WriteInt32(rpHgmConfigData.smallSizeArea_);
    parcel.WriteInt32(rpHgmConfigData.smallSizeLength_);
}

static void SetLeftSize(Parcel& parcel, uint32_t leftSize)
{
    parcel.SetMaxCapacity(PARCEL_MAX_CAPACITY);
    size_t useSize = PARCEL_MAX_CAPACITY - leftSize;
    size_t writeInt32Count = useSize / 4;
    size_t writeBoolCount = useSize % 4;

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
HWTEST_F(RPHgmConfigDataTest, UnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    RPHgmConfigData rpHgmConfigData;
    RPHgmConfigData* rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    parcel.WriteFloat(rpHgmConfigData.ppi_);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    parcel.WriteFloat(rpHgmConfigData.ppi_);
    parcel.WriteFloat(rpHgmConfigData.xDpi_);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    parcel.WriteFloat(rpHgmConfigData.ppi_);
    parcel.WriteFloat(rpHgmConfigData.xDpi_);
    parcel.WriteFloat(rpHgmConfigData.yDpi_);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    parcel.WriteFloat(rpHgmConfigData.ppi_);
    parcel.WriteFloat(rpHgmConfigData.xDpi_);
    parcel.WriteFloat(rpHgmConfigData.yDpi_);
    parcel.WriteInt32(rpHgmConfigData.smallSizeArea_);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(MAX_ANIM_DYNAMIC_ITEM_SIZE + 1);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RPHgmConfigDataTest, UnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    RPHgmConfigData rpHgmConfigData;
    InitParcelData(parcel);
    parcel.WriteUint32(1);
    RPHgmConfigData* rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    parcel.WriteInt32(0);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    parcel.WriteInt32(0);
    parcel.WriteInt32(1000);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    parcel.WriteInt32(0);
    parcel.WriteInt32(1000);
    parcel.WriteInt32(120);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RPHgmConfigDataTest, UnmarshallingTest003, TestSize.Level1)
{
    Parcel parcel;
    RPHgmConfigData rpHgmConfigData;
    InitParcelData(parcel);
    parcel.WriteUint32(0);
    parcel.WriteUint32(MAX_ANIM_DYNAMIC_ITEM_SIZE + 1);
    RPHgmConfigData* rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(0);
    parcel.WriteUint32(1);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(0);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(0);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(0);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    parcel.WriteInt32(0);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);

    InitParcelData(parcel);
    parcel.WriteUint32(0);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    parcel.WriteInt32(0);
    parcel.WriteInt32(1000);
    rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_EQ(rpHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RPHgmConfigDataTest, UnmarshallingTest004, TestSize.Level1)
{
    Parcel parcel;
    RPHgmConfigData rpHgmConfigData;
    InitParcelData(parcel);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    parcel.WriteInt32(0);
    parcel.WriteInt32(1000);
    parcel.WriteInt32(120);
    parcel.WriteUint32(1);
    parcel.WriteString("type");
    parcel.WriteString("name");
    parcel.WriteInt32(0);
    parcel.WriteInt32(1000);
    parcel.WriteInt32(120);
    RPHgmConfigData* rpHgmConfigDataPtr = rpHgmConfigData.Unmarshalling(parcel);
    EXPECT_NE(rpHgmConfigDataPtr, nullptr);
    EXPECT_EQ(rpHgmConfigDataPtr->configData_.size(), 1);
    EXPECT_EQ(rpHgmConfigDataPtr->smallSizeConfigData_.size(), 1);
}

/**
 * @tc.name: MarshallingTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RPHgmConfigDataTest, MarshallingTest001, TestSize.Level1)
{
    RPHgmConfigData rpHgmConfigData;
    rpHgmConfigData.AddAnimDynamicItem({ "test", "test", 0, -1, 120 });
    Parcel parcel;
    SetLeftSize(parcel, 2);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel));

    Parcel parcel2;
    SetLeftSize(parcel2, 6);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel2));

    Parcel parcel3;
    SetLeftSize(parcel3, 10);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel3));

    Parcel parcel4;
    SetLeftSize(parcel4, 14);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel4));

    Parcel parcel5;
    SetLeftSize(parcel5, 18);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel5));

    Parcel parcel6;
    SetLeftSize(parcel6, 22);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel6));

    Parcel parcel7;
    SetLeftSize(parcel7, 26);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel7));

    Parcel parcel8;
    SetLeftSize(parcel8, 30);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel8));

    Parcel parcel9;
    SetLeftSize(parcel9, 34);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel9));

    Parcel parcel10;
    SetLeftSize(parcel10, 38);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel10));

    Parcel parcel11;
    SetLeftSize(parcel11, 42);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel11));

    Parcel parcel12;
    SetLeftSize(parcel12, 46);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel12));

    Parcel parcel13;
    SetLeftSize(parcel13, 50);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel13));

    Parcel parcel14;
    SetLeftSize(parcel14, 54);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel14));
}

/**
 * @tc.name: MarshallingTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RPHgmConfigDataTest, MarshallingTest002, TestSize.Level1)
{
    RPHgmConfigData rpHgmConfigData;
    rpHgmConfigData.AddSmallSizeAnimDynamicItem({ "test", "test", 0, -1, 120 });
    rpHgmConfigData.AddAnimDynamicItem({ "test", "test", 0, -1, 120 });
    Parcel parcel;
    SetLeftSize(parcel, 58);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel));

    Parcel parcel2;
    SetLeftSize(parcel2, 62);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel2));

    Parcel parcel3;
    SetLeftSize(parcel3, 66);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel3));

    Parcel parcel4;
    SetLeftSize(parcel4, 70);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel4));

    Parcel parcel5;
    SetLeftSize(parcel5, 74);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel5));

    Parcel parcel7;
    SetLeftSize(parcel7, 78);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel7));

    Parcel parcel8;
    SetLeftSize(parcel8, 82);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel8));

    Parcel parcel9;
    SetLeftSize(parcel9, 86);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel9));

    Parcel parcel10;
    SetLeftSize(parcel10, 90);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel10));

    Parcel parcel11;
    SetLeftSize(parcel11, 94);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel11));

    Parcel parcel12;
    SetLeftSize(parcel12, 98);
    ASSERT_FALSE(rpHgmConfigData.Marshalling(parcel12));

    Parcel parcel13;
    SetLeftSize(parcel13, 102);
    ASSERT_TRUE(rpHgmConfigData.Marshalling(parcel13));
}
} // namespace Rosen
} // namespace OHOS