/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_ipc_persistence_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSIpcPersistenceDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSIpcPersistenceDataTest::SetUpTestCase() {}
void RSIpcPersistenceDataTest::TearDownTestCase() {}
void RSIpcPersistenceDataTest::SetUp() {}
void RSIpcPersistenceDataTest::TearDown() {}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataGetTypeTest001, TestSize.Level1)
{
    auto data = SetBehindWindowFilterEnabledPersistenceData(true);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED);
}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataGetTypeTest002, TestSize.Level1)
{
    auto data = SetBehindWindowFilterEnabledPersistenceData(false);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED);
}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataGetCallingPidTest001, TestSize.Level1)
{
    auto data = SetBehindWindowFilterEnabledPersistenceData(true);
    EXPECT_EQ(data.GetCallingPid(), IPC_PERSISTENCE_DEFAULT_PID);
}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataMarshallingTest001, TestSize.Level1)
{
    auto data = SetBehindWindowFilterEnabledPersistenceData(true);
    Parcel parcel;
    bool result = data.Marshalling(parcel);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataMarshallingTest002, TestSize.Level1)
{
    auto data = SetBehindWindowFilterEnabledPersistenceData(false);
    Parcel parcel;
    bool result = data.Marshalling(parcel);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataUnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(true);
    auto data = SetBehindWindowFilterEnabledPersistenceData::Unmarshalling(parcel);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->GetType(), RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED);
    delete data;
}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataUnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(false);
    auto data = SetBehindWindowFilterEnabledPersistenceData::Unmarshalling(parcel);
    EXPECT_NE(data, nullptr);
    delete data;
}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataRoundtripTest001, TestSize.Level1)
{
    auto originalData = SetBehindWindowFilterEnabledPersistenceData(true);
    Parcel parcel;
    bool marshallingResult = originalData.Marshalling(parcel);
    EXPECT_TRUE(marshallingResult);
    auto unmarshalledData = SetBehindWindowFilterEnabledPersistenceData::Unmarshalling(parcel);
    EXPECT_NE(unmarshalledData, nullptr);
    EXPECT_EQ(unmarshalledData->GetType(), originalData.GetType());
    delete unmarshalledData;
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataGetTypeTest001, TestSize.Level1)
{
    auto data = SetShowRefreshRateEnabledPersistenceData(true, 1);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED);
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataGetTypeTest002, TestSize.Level1)
{
    auto data = SetShowRefreshRateEnabledPersistenceData(false, 0);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED);
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataGetCallingPidTest001, TestSize.Level1)
{
    auto data = SetShowRefreshRateEnabledPersistenceData(true, 1);
    EXPECT_EQ(data.GetCallingPid(), IPC_PERSISTENCE_DEFAULT_PID);
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataMarshallingTest001, TestSize.Level1)
{
    auto data = SetShowRefreshRateEnabledPersistenceData(true, 1);
    Parcel parcel;
    bool result = data.Marshalling(parcel);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataMarshallingTest002, TestSize.Level1)
{
    auto data = SetShowRefreshRateEnabledPersistenceData(false, 0);
    Parcel parcel;
    bool result = data.Marshalling(parcel);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataUnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(true);
    parcel.WriteInt32(1);
    auto data = SetShowRefreshRateEnabledPersistenceData::Unmarshalling(parcel);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->GetType(), RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED);
    delete data;
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataUnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteBool(false);
    parcel.WriteInt32(0);
    auto data = SetShowRefreshRateEnabledPersistenceData::Unmarshalling(parcel);
    EXPECT_NE(data, nullptr);
    delete data;
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataRoundtripTest001, TestSize.Level1)
{
    auto originalData = SetShowRefreshRateEnabledPersistenceData(true, 1);
    Parcel parcel;
    bool marshallingResult = originalData.Marshalling(parcel);
    EXPECT_TRUE(marshallingResult);
    auto unmarshalledData = SetShowRefreshRateEnabledPersistenceData::Unmarshalling(parcel);
    EXPECT_NE(unmarshalledData, nullptr);
    EXPECT_EQ(unmarshalledData->GetType(), originalData.GetType());
    delete unmarshalledData;
}

HWTEST_F(RSIpcPersistenceDataTest, OnHwcEventPersistenceDataGetTypeTest001, TestSize.Level1)
{
    std::vector<int32_t> eventData = {1, 2, 3};
    auto data = OnHwcEventPersistenceData(1, 1, eventData);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::ON_HWC_EVENT);
}

HWTEST_F(RSIpcPersistenceDataTest, OnHwcEventPersistenceDataGetCallingPidTest001, TestSize.Level1)
{
    std::vector<int32_t> eventData;
    auto data = OnHwcEventPersistenceData(0, 0, eventData);
    EXPECT_EQ(data.GetCallingPid(), IPC_PERSISTENCE_DEFAULT_PID);
}

HWTEST_F(RSIpcPersistenceDataTest, OnHwcEventPersistenceDataMarshallingTest001, TestSize.Level1)
{
    std::vector<int32_t> eventData = {1, 2, 3};
    auto data = OnHwcEventPersistenceData(1, 1, eventData);
    Parcel parcel;
    bool result = data.Marshalling(parcel);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceDataTest, OnHwcEventPersistenceDataMarshallingTest002, TestSize.Level1)
{
    std::vector<int32_t> eventData;
    auto data = OnHwcEventPersistenceData(0, 0, eventData);
    Parcel parcel;
    bool result = data.Marshalling(parcel);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceDataTest, OnHwcEventPersistenceDataUnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint32(1);
    parcel.WriteUint32(1);
    parcel.WriteUint32(3);
    parcel.WriteInt32(1);
    parcel.WriteInt32(2);
    parcel.WriteInt32(3);
    auto data = OnHwcEventPersistenceData::Unmarshalling(parcel);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->GetType(), RSIpcPersistenceType::ON_HWC_EVENT);
    delete data;
}

HWTEST_F(RSIpcPersistenceDataTest, OnHwcEventPersistenceDataUnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint32(0);
    parcel.WriteUint32(0);
    parcel.WriteUint32(0);
    auto data = OnHwcEventPersistenceData::Unmarshalling(parcel);
    EXPECT_NE(data, nullptr);
    delete data;
}

HWTEST_F(RSIpcPersistenceDataTest, OnHwcEventPersistenceDataRoundtripTest001, TestSize.Level1)
{
    std::vector<int32_t> eventData = {1, 2, 3};
    auto originalData = OnHwcEventPersistenceData(1, 1, eventData);
    Parcel parcel;
    bool marshallingResult = originalData.Marshalling(parcel);
    EXPECT_TRUE(marshallingResult);
    auto unmarshalledData = OnHwcEventPersistenceData::Unmarshalling(parcel);
    EXPECT_NE(unmarshalledData, nullptr);
    EXPECT_EQ(unmarshalledData->GetType(), originalData.GetType());
    delete unmarshalledData;
}

HWTEST_F(RSIpcPersistenceDataTest, ShowWatermarkPersistenceDataGetTypeTest001, TestSize.Level1)
{
    auto data = ShowWatermarkPersistenceData(nullptr, true);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::SHOW_WATERMARK);
}

HWTEST_F(RSIpcPersistenceDataTest, ShowWatermarkPersistenceDataGetTypeTest002, TestSize.Level1)
{
    auto data = ShowWatermarkPersistenceData(nullptr, false);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::SHOW_WATERMARK);
}

HWTEST_F(RSIpcPersistenceDataTest, ShowWatermarkPersistenceDataGetCallingPidTest001, TestSize.Level1)
{
    auto data = ShowWatermarkPersistenceData(nullptr, true);
    EXPECT_EQ(data.GetCallingPid(), IPC_PERSISTENCE_DEFAULT_PID);
}

HWTEST_F(RSIpcPersistenceDataTest, SetWatermarkPersistenceDataGetTypeTest001, TestSize.Level1)
{
    auto data = SetWatermarkPersistenceData(1000, "test", nullptr, true);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::SET_WATERMARK);
}

HWTEST_F(RSIpcPersistenceDataTest, SetWatermarkPersistenceDataGetTypeTest002, TestSize.Level1)
{
    auto data = SetWatermarkPersistenceData(1000, "test", nullptr, false);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::SET_WATERMARK);
}

HWTEST_F(RSIpcPersistenceDataTest, SetWatermarkPersistenceDataGetCallingPidTest001, TestSize.Level1)
{
    auto data = SetWatermarkPersistenceData(1000, "test", nullptr, true);
    EXPECT_EQ(data.GetCallingPid(), 1000);
}

HWTEST_F(RSIpcPersistenceDataTest, SetWatermarkPersistenceDataGetCallingPidTest002, TestSize.Level1)
{
    auto data = SetWatermarkPersistenceData(2000, "test", nullptr, true);
    EXPECT_EQ(data.GetCallingPid(), 2000);
}

HWTEST_F(RSIpcPersistenceDataTest, SelfDrawingNodeRectChangeCallbackPersistenceDataGetTypeTest001, TestSize.Level1)
{
    RectConstraint constraint;
    auto data = SelfDrawingNodeRectChangeCallbackPersistenceData(1000, constraint, nullptr);
    EXPECT_EQ(data.GetType(), RSIpcPersistenceType::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
}

HWTEST_F(RSIpcPersistenceDataTest, SelfDrawingNodeRectChangeCallbackPersistenceDataGetCallingPidTest001, TestSize.Level1)
{
    RectConstraint constraint;
    auto data = SelfDrawingNodeRectChangeCallbackPersistenceData(1000, constraint, nullptr);
    EXPECT_EQ(data.GetCallingPid(), 1000);
}

HWTEST_F(RSIpcPersistenceDataTest, SelfDrawingNodeRectChangeCallbackPersistenceDataGetCallingPidTest002, TestSize.Level1)
{
    RectConstraint constraint;
    auto data = SelfDrawingNodeRectChangeCallbackPersistenceData(2000, constraint, nullptr);
    EXPECT_EQ(data.GetCallingPid(), 2000);
}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataApplyTest001, TestSize.Level1)
{
    auto data = SetBehindWindowFilterEnabledPersistenceData(true);
    data.Apply(nullptr);
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataApplyTest001, TestSize.Level1)
{
    auto data = SetShowRefreshRateEnabledPersistenceData(true, 1);
    data.Apply(nullptr);
}

HWTEST_F(RSIpcPersistenceDataTest, OnHwcEventPersistenceDataApplyTest001, TestSize.Level1)
{
    std::vector<int32_t> eventData;
    auto data = OnHwcEventPersistenceData(0, 0, eventData);
    data.Apply(nullptr);
}

HWTEST_F(RSIpcPersistenceDataTest, ShowWatermarkPersistenceDataApplyTest001, TestSize.Level1)
{
    auto data = ShowWatermarkPersistenceData(nullptr, true);
    data.Apply(nullptr);
}

HWTEST_F(RSIpcPersistenceDataTest, SetWatermarkPersistenceDataApplyTest001, TestSize.Level1)
{
    auto data = SetWatermarkPersistenceData(1000, "test", nullptr, true);
    data.Apply(nullptr);
}

HWTEST_F(RSIpcPersistenceDataTest, SelfDrawingNodeRectChangeCallbackPersistenceDataApplyTest001, TestSize.Level1)
{
    RectConstraint constraint;
    auto data = SelfDrawingNodeRectChangeCallbackPersistenceData(1000, constraint, nullptr);
    data.Apply(nullptr);
}

HWTEST_F(RSIpcPersistenceDataTest, RSIpcPersistenceDataBaseDefaultGetCallingPidTest001, TestSize.Level1)
{
    auto baseData = std::make_shared<SetBehindWindowFilterEnabledPersistenceData>(true);
    EXPECT_EQ(baseData->GetCallingPid(), IPC_PERSISTENCE_DEFAULT_PID);
}

HWTEST_F(RSIpcPersistenceDataTest, MultipleDataCreationTest001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSIpcPersistenceDataBase>> dataList;
    dataList.push_back(std::make_shared<SetBehindWindowFilterEnabledPersistenceData>(true));
    dataList.push_back(std::make_shared<SetShowRefreshRateEnabledPersistenceData>(true, 1));
    dataList.push_back(std::make_shared<OnHwcEventPersistenceData>(1, 1, std::vector<int32_t>{1, 2, 3}));
    dataList.push_back(std::make_shared<ShowWatermarkPersistenceData>(nullptr, true));
    dataList.push_back(std::make_shared<SetWatermarkPersistenceData>(1000, "test", nullptr, true));

    EXPECT_EQ(dataList.size(), 5u);
    EXPECT_EQ(dataList[0]->GetType(), RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED);
    EXPECT_EQ(dataList[1]->GetType(), RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED);
    EXPECT_EQ(dataList[2]->GetType(), RSIpcPersistenceType::ON_HWC_EVENT);
    EXPECT_EQ(dataList[3]->GetType(), RSIpcPersistenceType::SHOW_WATERMARK);
    EXPECT_EQ(dataList[4]->GetType(), RSIpcPersistenceType::SET_WATERMARK);
}

HWTEST_F(RSIpcPersistenceDataTest, SetBehindWindowFilterEnabledPersistenceDataSharedPtrTest001, TestSize.Level1)
{
    auto data = std::make_shared<SetBehindWindowFilterEnabledPersistenceData>(true);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->GetType(), RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED);
}

HWTEST_F(RSIpcPersistenceDataTest, SetShowRefreshRateEnabledPersistenceDataSharedPtrTest001, TestSize.Level1)
{
    auto data = std::make_shared<SetShowRefreshRateEnabledPersistenceData>(true, 1);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->GetType(), RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED);
}

HWTEST_F(RSIpcPersistenceDataTest, OnHwcEventPersistenceDataSharedPtrTest001, TestSize.Level1)
{
    auto data = std::make_shared<OnHwcEventPersistenceData>(1, 1, std::vector<int32_t>{1, 2, 3});
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->GetType(), RSIpcPersistenceType::ON_HWC_EVENT);
}

HWTEST_F(RSIpcPersistenceDataTest, ShowWatermarkPersistenceDataSharedPtrTest001, TestSize.Level1)
{
    auto data = std::make_shared<ShowWatermarkPersistenceData>(nullptr, true);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->GetType(), RSIpcPersistenceType::SHOW_WATERMARK);
}

HWTEST_F(RSIpcPersistenceDataTest, SetWatermarkPersistenceDataSharedPtrTest001, TestSize.Level1)
{
    auto data = std::make_shared<SetWatermarkPersistenceData>(1000, "test", nullptr, true);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->GetType(), RSIpcPersistenceType::SET_WATERMARK);
}

HWTEST_F(RSIpcPersistenceDataTest, SelfDrawingNodeRectChangeCallbackPersistenceDataSharedPtrTest001, TestSize.Level1)
{
    RectConstraint constraint;
    auto data = std::make_shared<SelfDrawingNodeRectChangeCallbackPersistenceData>(1000, constraint, nullptr);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->GetType(), RSIpcPersistenceType::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
}

} // namespace OHOS::Rosen