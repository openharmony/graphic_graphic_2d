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
 * @tc.desc: Test when !parcel.ReadBool(data->isSyncConfig_)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData* rsHgmConfigDataPtr = RSHgmConfigData::Unmarshalling(parcel);
    ASSERT_EQ(rsHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest002
 * @tc.desc: Test when !data->isSyncConfig_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(false);
    RSHgmConfigData* rsHgmConfigDataPtr = RSHgmConfigData::Unmarshalling(parcel);
    ASSERT_EQ(rsHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest003
 * @tc.desc: Test when !data->UnmarshallingConfig(parcel)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest003, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(true);
    RSHgmConfigData* rsHgmConfigDataPtr = RSHgmConfigData::Unmarshalling(parcel);
    ASSERT_EQ(rsHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest004
 * @tc.desc: Test when data->UnmarshallingConfig(parcel)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest004, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(true);
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(1.0f);
    parcel.WriteUint32(0);
    RSHgmConfigData* rsHgmConfigDataPtr = RSHgmConfigData::Unmarshalling(parcel);
    ASSERT_EQ(rsHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest005
 * @tc.desc: Test when !parcel.ReadBool(data->isSyncEnergyData_)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest005, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(false);
    parcel.WriteBool(false);
    RSHgmConfigData* rsHgmConfigDataPtr = RSHgmConfigData::Unmarshalling(parcel);
    ASSERT_NE(rsHgmConfigDataPtr, nullptr);
    EXPECT_FALSE(rsHgmConfigDataPtr->isSyncEnergyData_);
    delete rsHgmConfigDataPtr;
}

/**
 * @tc.name: UnmarshallingTest006
 * @tc.desc: Test when !data->UnmarshallingEnergyData(parcel)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest006, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(false);
    parcel.WriteBool(true);
    RSHgmConfigData* rsHgmConfigDataPtr = RSHgmConfigData::Unmarshalling(parcel);
    ASSERT_EQ(rsHgmConfigDataPtr, nullptr);
}

/**
 * @tc.name: UnmarshallingTest007
 * @tc.desc: Test when !data->isSyncEnergyData_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest007, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(false);
    parcel.WriteBool(false);
    RSHgmConfigData* rsHgmConfigDataPtr = RSHgmConfigData::Unmarshalling(parcel);
    ASSERT_NE(rsHgmConfigDataPtr, nullptr);
    EXPECT_FALSE(rsHgmConfigDataPtr->isSyncEnergyData_);
    delete rsHgmConfigDataPtr;
}

/**
 * @tc.name: UnmarshallingTest008
 * @tc.desc: Test when data->UnmarshallingEnergyData(parcel)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingTest008, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(false);
    parcel.WriteBool(true);
    parcel.WriteString("componentName");
    parcel.WriteInt32(60);
    RSHgmConfigData* rsHgmConfigDataPtr = RSHgmConfigData::Unmarshalling(parcel);
    ASSERT_NE(rsHgmConfigDataPtr, nullptr);
    EXPECT_TRUE(rsHgmConfigDataPtr->isSyncEnergyData_);
    EXPECT_EQ(rsHgmConfigDataPtr->energyInfo_.componentName, "componentName");
    EXPECT_EQ(rsHgmConfigDataPtr->energyInfo_.componentDefaultFps, 60);
    delete rsHgmConfigDataPtr;
}

/**
 * @tc.name: MarshallingTest001
 * @tc.desc: Test when isSyncConfig_ = true, isSyncEnergyData_ = false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, MarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    rsHgmConfigData.SetIsSyncConfig(true);

    std::string type = "test";
    std::string name = "test";
    int minSpeed = 1;
    int maxSpeed = 2;
    int preferredFps = 3;
    AnimDynamicItem item = { type, name, minSpeed, maxSpeed, preferredFps };
    rsHgmConfigData.AddAnimDynamicItem(item);
    std::string pageName = "pageName";
    rsHgmConfigData.AddPageName(pageName);

    bool marshallingResult = rsHgmConfigData.Marshalling(parcel);
    ASSERT_TRUE(marshallingResult);
}

/**
 * @tc.name: MarshallingTest002
 * @tc.desc: Test when isSyncConfig_ == false,isSyncEnergyData_== true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, MarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    rsHgmConfigData.SetIsSyncConfig(false);

    std::string type = "test";
    std::string name = "test";
    int minSpeed = 1;
    int maxSpeed = 2;
    int preferredFps = 3;
    AnimDynamicItem item = { type, name, minSpeed, maxSpeed, preferredFps };
    rsHgmConfigData.AddAnimDynamicItem(item);
    std::string pageName = "pageName";
    rsHgmConfigData.AddPageName(pageName);

    EnergyInfo energyInfo = { "componentName", 0, 60 };
    rsHgmConfigData.SetEnergyInfo(energyInfo);

    bool marshallingResult = rsHgmConfigData.Marshalling(parcel);
    ASSERT_TRUE(marshallingResult);
}

/**
 * @tc.name: UnmarshallingConfigTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingConfigTest001, TestSize.Level1)
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
    rsHgmConfigData.MarshallingConfig(parcel);
    RSHgmConfigData* rsHgmConfigDataPtr = new RSHgmConfigData();
    rsHgmConfigDataPtr->UnmarshallingConfig(parcel);
    ASSERT_EQ(rsHgmConfigDataPtr->configData_[0].animType, type);
    delete rsHgmConfigDataPtr;
}

/**
 * @tc.name: UnmarshallingConfigTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingConfigTest002, TestSize.Level1)
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
    parcel.WriteString("pageName");
    RSHgmConfigData* rsHgmConfigDataPtr = new RSHgmConfigData();
    rsHgmConfigDataPtr->UnmarshallingConfig(parcel);
    EXPECT_EQ(rsHgmConfigDataPtr->configData_.size(), 1);
    EXPECT_EQ(rsHgmConfigDataPtr->pageNameList_.size(), 1);
    delete rsHgmConfigDataPtr;

    Parcel parcel2;
    parcel2.WriteFloat(rsHgmConfigData.ppi_);
    parcel2.WriteFloat(rsHgmConfigData.xDpi_);
    parcel2.WriteFloat(rsHgmConfigData.yDpi_);
    parcel2.WriteUint32(MAX_ANIM_DYNAMIC_ITEM_SIZE + 1);
    rsHgmConfigDataPtr = new RSHgmConfigData();
    rsHgmConfigDataPtr->UnmarshallingConfig(parcel2);
    delete rsHgmConfigDataPtr;

    Parcel parcel3;
    parcel3.WriteFloat(rsHgmConfigData.ppi_);
    parcel3.WriteFloat(rsHgmConfigData.xDpi_);
    parcel3.WriteFloat(rsHgmConfigData.yDpi_);
    parcel3.WriteUint32(0);
    parcel3.WriteUint32(MAX_PAGE_NAME_SIZE + 1);
    rsHgmConfigDataPtr = new RSHgmConfigData();
    rsHgmConfigDataPtr->UnmarshallingConfig(parcel3);
    delete rsHgmConfigDataPtr;
}

/**
 * @tc.name: UnmarshallingConfigTest003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingConfigTest003, TestSize.Level1)
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
    parcel.WriteString("pageName");
    RSHgmConfigData* rsHgmConfigDataPtr = new RSHgmConfigData();
    rsHgmConfigDataPtr->UnmarshallingConfig(parcel);
    EXPECT_EQ(rsHgmConfigDataPtr->configData_.size(), 1);
    EXPECT_EQ(rsHgmConfigDataPtr->pageNameList_.size(), 1);
    delete rsHgmConfigDataPtr;

    Parcel parcel2;
    parcel2.WriteFloat(rsHgmConfigData.ppi_);
    parcel2.WriteFloat(rsHgmConfigData.xDpi_);
    parcel2.WriteFloat(rsHgmConfigData.yDpi_);
    parcel2.WriteUint32(MAX_ANIM_DYNAMIC_ITEM_SIZE);
    rsHgmConfigDataPtr = new RSHgmConfigData();
    rsHgmConfigDataPtr->UnmarshallingConfig(parcel2);
    delete rsHgmConfigDataPtr;

    Parcel parcel3;
    parcel3.WriteFloat(rsHgmConfigData.ppi_);
    parcel3.WriteFloat(rsHgmConfigData.xDpi_);
    parcel3.WriteFloat(rsHgmConfigData.yDpi_);
    parcel3.WriteUint32(0);
    parcel3.WriteUint32(MAX_PAGE_NAME_SIZE);
    rsHgmConfigDataPtr = new RSHgmConfigData();
    rsHgmConfigDataPtr->UnmarshallingConfig(parcel3);
    delete rsHgmConfigDataPtr;
}

/**
 * @tc.name: MarshallingConfigTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, MarshallingConfigTest, TestSize.Level1)
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
    bool marshalling = rsHgmConfigData.MarshallingConfig(parcel);
    ASSERT_TRUE(marshalling);
}

/**
 * @tc.name: UnmarshallingEnergyDataTest001
 * @tc.desc: Test when !parcel.ReadString(energyInfo_.componentName)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingEnergyDataTest001, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;

    bool result = rsHgmConfigData.UnmarshallingEnergyData(parcel);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: UnmarshallingEnergyDataTest002
 * @tc.desc: Test when !parcel.ReadInt32(energyInfo_.componentDefaultFps)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingEnergyDataTest002, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    parcel.WriteString("componentName");

    bool result = rsHgmConfigData.UnmarshallingEnergyData(parcel);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: UnmarshallingEnergyDataTest003
 * @tc.desc: Test return true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, UnmarshallingEnergyDataTest003, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;
    parcel.WriteString("componentName");
    parcel.WriteInt32(60);

    bool result = rsHgmConfigData.UnmarshallingEnergyData(parcel);
    EXPECT_TRUE(result);
    EXPECT_EQ(rsHgmConfigData.GetEnergyInfo().componentName, "componentName");
    EXPECT_EQ(rsHgmConfigData.GetEnergyInfo().componentDefaultFps, 60);
}

/**
 * @tc.name: MarshallingEnergyDataTest001
 * @tc.desc: Test MarshallingEnergyData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHgmConfigDataTest, MarshallingEnergyDataTest001, TestSize.Level1)
{
    Parcel parcel;
    RSHgmConfigData rsHgmConfigData;

    EnergyInfo energyInfo = { "componentName", 0, 60 };
    rsHgmConfigData.SetEnergyInfo(energyInfo);
    bool result = rsHgmConfigData.MarshallingEnergyData(parcel);
    EXPECT_TRUE(result);
}

} // namespace Rosen
} // namespace OHOS