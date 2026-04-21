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

#include "rs_ipc_persistence_manager.h"
#include "rs_ipc_persistence_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class MockPersistenceData : public RSIpcPersistenceDataBase {
public:
    MockPersistenceData(RSIpcPersistenceType type, pid_t pid)
        : type_(type), pid_(pid) {}

    RSIpcPersistenceType GetType() const override { return type_; }
    pid_t GetCallingPid() const override { return pid_; }

    bool Marshalling(Parcel& parcel) const override
    {
        return parcel.WriteUint32(static_cast<uint32_t>(type_)) &&
               parcel.WriteInt32(pid_);
    }

    static MockPersistenceData* Unmarshalling(Parcel& parcel)
    {
        uint32_t type;
        pid_t pid;
        if (!parcel.ReadUint32(type) || !parcel.ReadInt32(pid)) {
            return nullptr;
        }
        return new MockPersistenceData(static_cast<RSIpcPersistenceType>(type), pid);
    }

    void Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent) override {}

private:
    RSIpcPersistenceType type_;
    pid_t pid_;
};

class RSIpcPersistenceManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RSIpcPersistenceManager> manager_;
};

std::shared_ptr<RSIpcPersistenceManager> RSIpcPersistenceManagerTest::manager_ = nullptr;

void RSIpcPersistenceManagerTest::SetUpTestCase()
{
    manager_ = std::make_shared<RSIpcPersistenceManager>();
}

void RSIpcPersistenceManagerTest::TearDownTestCase()
{
    manager_ = nullptr;
}

void RSIpcPersistenceManagerTest::SetUp()
{
    manager_ = std::make_shared<RSIpcPersistenceManager>();
}

void RSIpcPersistenceManagerTest::TearDown()
{
    manager_ = nullptr;
}

HWTEST_F(RSIpcPersistenceManagerTest, GetReplayDataTest001, TestSize.Level1)
{
    auto replayData = manager_->GetReplayData();
    EXPECT_TRUE(replayData.empty());
}

HWTEST_F(RSIpcPersistenceManagerTest, RegisterWithCallingPidTest001, TestSize.Level1)
{
    auto data = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    manager_->RegisterWithCallingPid(data);
    auto replayData = manager_->GetReplayData();
    EXPECT_FALSE(replayData.empty());
    EXPECT_EQ(replayData.size(), 1u);
    EXPECT_EQ(replayData[RSIpcPersistenceType::SET_WATERMARK].size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, RegisterWithCallingPidTest002, TestSize.Level1)
{
    auto data = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, IPC_PERSISTENCE_DEFAULT_PID);
    manager_->RegisterWithCallingPid(data);
    auto replayData = manager_->GetReplayData();
    EXPECT_TRUE(replayData.empty());
}

HWTEST_F(RSIpcPersistenceManagerTest, RegisterWithCallingPidTest003, TestSize.Level1)
{
    auto data1 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    auto data2 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    manager_->RegisterWithCallingPid(data1);
    manager_->RegisterWithCallingPid(data2);
    auto replayData = manager_->GetReplayData();
    EXPECT_EQ(replayData[RSIpcPersistenceType::SET_WATERMARK].size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, RegisterWithoutCallingPidTest001, TestSize.Level1)
{
    auto data = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SHOW_WATERMARK, IPC_PERSISTENCE_DEFAULT_PID);
    manager_->RegisterWithoutCallingPid(data);
    auto replayData = manager_->GetReplayData();
    EXPECT_FALSE(replayData.empty());
    EXPECT_EQ(replayData.size(), 1u);
    EXPECT_EQ(replayData[RSIpcPersistenceType::SHOW_WATERMARK].size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, RegisterWithoutCallingPidTest002, TestSize.Level1)
{
    auto data = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SHOW_WATERMARK, 1000);
    manager_->RegisterWithoutCallingPid(data);
    auto replayData = manager_->GetReplayData();
    EXPECT_TRUE(replayData.empty());
}

HWTEST_F(RSIpcPersistenceManagerTest, RegisterWithoutCallingPidTest003, TestSize.Level1)
{
    auto data1 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SHOW_WATERMARK, IPC_PERSISTENCE_DEFAULT_PID);
    auto data2 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SHOW_WATERMARK, IPC_PERSISTENCE_DEFAULT_PID);
    manager_->RegisterWithoutCallingPid(data1);
    manager_->RegisterWithoutCallingPid(data2);
    auto replayData = manager_->GetReplayData();
    EXPECT_EQ(replayData[RSIpcPersistenceType::SHOW_WATERMARK].size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, UnregisterByTypeTest001, TestSize.Level1)
{
    auto data = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    manager_->RegisterWithCallingPid(data);
    manager_->UnregisterByType(RSIpcPersistenceType::SET_WATERMARK);
    auto replayData = manager_->GetReplayData();
    EXPECT_TRUE(replayData.empty());
}

HWTEST_F(RSIpcPersistenceManagerTest, UnregisterByTypeTest002, TestSize.Level1)
{
    auto data1 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    auto data2 = std::make_shared<MockPersistenceData>(
        RSIpcPersistenceType::SHOW_WATERMARK, IPC_PERSISTENCE_DEFAULT_PID);
    manager_->RegisterWithCallingPid(data1);
    manager_->RegisterWithoutCallingPid(data2);
    manager_->UnregisterByType(RSIpcPersistenceType::SET_WATERMARK);
    auto replayData = manager_->GetReplayData();
    EXPECT_EQ(replayData.size(), 1u);
    EXPECT_TRUE(replayData.find(RSIpcPersistenceType::SET_WATERMARK) == replayData.end());
}

HWTEST_F(RSIpcPersistenceManagerTest, UnregisterByCallingPidTest001, TestSize.Level1)
{
    auto data = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    manager_->RegisterWithCallingPid(data);
    manager_->UnregisterByCallingPid(1000);
    auto replayData = manager_->GetReplayData();
    EXPECT_TRUE(replayData.empty());
}

HWTEST_F(RSIpcPersistenceManagerTest, UnregisterByCallingPidTest002, TestSize.Level1)
{
    auto data1 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    auto data2 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::ON_HWC_EVENT, 1000);
    manager_->RegisterWithCallingPid(data1);
    manager_->RegisterWithCallingPid(data2);
    manager_->UnregisterByCallingPid(1000);
    auto replayData = manager_->GetReplayData();
    EXPECT_TRUE(replayData.empty());
}

HWTEST_F(RSIpcPersistenceManagerTest, UnregisterByCallingPidTest003, TestSize.Level1)
{
    auto data1 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    auto data2 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::ON_HWC_EVENT, 2000);
    manager_->RegisterWithCallingPid(data1);
    manager_->RegisterWithCallingPid(data2);
    manager_->UnregisterByCallingPid(1000);
    auto replayData = manager_->GetReplayData();
    EXPECT_EQ(replayData.size(), 1u);
    EXPECT_EQ(replayData[RSIpcPersistenceType::ON_HWC_EVENT].size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, UnregisterByTypeAndCallingPidTest001, TestSize.Level1)
{
    auto data = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    manager_->RegisterWithCallingPid(data);
    manager_->UnregisterByTypeAndCallingPid(RSIpcPersistenceType::SET_WATERMARK, 1000);
    auto replayData = manager_->GetReplayData();
    EXPECT_TRUE(replayData.empty());
}

HWTEST_F(RSIpcPersistenceManagerTest, UnregisterByTypeAndCallingPidTest002, TestSize.Level1)
{
    auto data1 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    auto data2 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::ON_HWC_EVENT, 1000);
    manager_->RegisterWithCallingPid(data1);
    manager_->RegisterWithCallingPid(data2);
    manager_->UnregisterByTypeAndCallingPid(RSIpcPersistenceType::SET_WATERMARK, 1000);
    auto replayData = manager_->GetReplayData();
    EXPECT_EQ(replayData.size(), 1u);
    EXPECT_TRUE(replayData.find(RSIpcPersistenceType::SET_WATERMARK) == replayData.end());
}

HWTEST_F(RSIpcPersistenceManagerTest, UnregisterWithoutCallingPidByTypeTest001, TestSize.Level1)
{
    auto data = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SHOW_WATERMARK, IPC_PERSISTENCE_DEFAULT_PID);
    manager_->RegisterWithoutCallingPid(data);
    manager_->UnregisterWithoutCallingPidByType(RSIpcPersistenceType::SHOW_WATERMARK);
    auto replayData = manager_->GetReplayData();
    EXPECT_TRUE(replayData.empty());
}

HWTEST_F(RSIpcPersistenceManagerTest, UnregisterWithoutCallingPidByTypeTest002, TestSize.Level1)
{
    auto data1 = std::make_shared<MockPersistenceData>(
        RSIpcPersistenceType::SHOW_WATERMARK, IPC_PERSISTENCE_DEFAULT_PID);
    auto data2 = std::make_shared<MockPersistenceData>(
        RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED, IPC_PERSISTENCE_DEFAULT_PID);
    manager_->RegisterWithoutCallingPid(data1);
    manager_->RegisterWithoutCallingPid(data2);
    manager_->UnregisterWithoutCallingPidByType(RSIpcPersistenceType::SHOW_WATERMARK);
    auto replayData = manager_->GetReplayData();
    EXPECT_EQ(replayData.size(), 1u);
    EXPECT_TRUE(replayData.find(RSIpcPersistenceType::SHOW_WATERMARK) == replayData.end());
}

HWTEST_F(RSIpcPersistenceManagerTest, MarshallingTest001, TestSize.Level1)
{
    IpcPersistenceTypeToDataMap typeToDataMap;
    Parcel parcel;
    bool result = RSIpcPersistenceManager::Marshalling(parcel, typeToDataMap);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceManagerTest, MarshallingTest002, TestSize.Level1)
{
    IpcPersistenceTypeToDataMap typeToDataMap;
    auto data = std::make_shared<SetBehindWindowFilterEnabledPersistenceData>(true);
    typeToDataMap[RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED].emplace_back(data);
    Parcel parcel;
    bool result = RSIpcPersistenceManager::Marshalling(parcel, typeToDataMap);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceManagerTest, MarshallingTest003, TestSize.Level1)
{
    IpcPersistenceTypeToDataMap typeToDataMap;
    auto data1 = std::make_shared<SetBehindWindowFilterEnabledPersistenceData>(true);
    auto data2 = std::make_shared<SetBehindWindowFilterEnabledPersistenceData>(false);
    typeToDataMap[RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED].emplace_back(data1);
    typeToDataMap[RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED].emplace_back(data2);
    Parcel parcel;
    bool result = RSIpcPersistenceManager::Marshalling(parcel, typeToDataMap);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceManagerTest, MarshallingTest004, TestSize.Level1)
{
    IpcPersistenceTypeToDataMap typeToDataMap;
    auto data1 = std::make_shared<SetBehindWindowFilterEnabledPersistenceData>(true);
    auto data2 = std::make_shared<SetShowRefreshRateEnabledPersistenceData>(true, 1);
    typeToDataMap[RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED].emplace_back(data1);
    typeToDataMap[RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED].emplace_back(data2);
    Parcel parcel;
    bool result = RSIpcPersistenceManager::Marshalling(parcel, typeToDataMap);
    EXPECT_TRUE(result);
}

HWTEST_F(RSIpcPersistenceManagerTest, UnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint32(0);
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().empty());
}

HWTEST_F(RSIpcPersistenceManagerTest, UnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint32(1);
    parcel.WriteUint32(static_cast<uint32_t>(RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED));
    parcel.WriteUint32(1);
    parcel.WriteBool(true);
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value().size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, UnmarshallingTest003, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint32(1);
    parcel.WriteUint32(static_cast<uint32_t>(RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED));
    parcel.WriteUint32(1);
    parcel.WriteBool(true);
    parcel.WriteInt32(1);
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value().size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, UnmarshallingTest004, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint32(101);
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_FALSE(result.has_value());
}

HWTEST_F(RSIpcPersistenceManagerTest, UnmarshallingTest005, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint32(1);
    parcel.WriteUint32(static_cast<uint32_t>(RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED));
    parcel.WriteUint32(201);
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_FALSE(result.has_value());
}

HWTEST_F(RSIpcPersistenceManagerTest, MarshallingUnmarshallingTest001, TestSize.Level1)
{
    IpcPersistenceTypeToDataMap typeToDataMap;
    auto data = std::make_shared<SetBehindWindowFilterEnabledPersistenceData>(true);
    typeToDataMap[RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED].emplace_back(data);
    Parcel parcel;
    bool marshallingResult = RSIpcPersistenceManager::Marshalling(parcel, typeToDataMap);
    EXPECT_TRUE(marshallingResult);
    auto unmarshallingResult = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_TRUE(unmarshallingResult.has_value());
    EXPECT_EQ(unmarshallingResult.value().size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, MarshallingUnmarshallingTest002, TestSize.Level1)
{
    IpcPersistenceTypeToDataMap typeToDataMap;
    auto data = std::make_shared<SetShowRefreshRateEnabledPersistenceData>(true, 1);
    typeToDataMap[RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED].emplace_back(data);
    Parcel parcel;
    bool marshallingResult = RSIpcPersistenceManager::Marshalling(parcel, typeToDataMap);
    EXPECT_TRUE(marshallingResult);
    auto unmarshallingResult = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_TRUE(unmarshallingResult.has_value());
    EXPECT_EQ(unmarshallingResult.value().size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, MarshallingUnmarshallingTest003, TestSize.Level1)
{
    IpcPersistenceTypeToDataMap typeToDataMap;
    auto data1 = std::make_shared<SetBehindWindowFilterEnabledPersistenceData>(true);
    auto data2 = std::make_shared<SetShowRefreshRateEnabledPersistenceData>(true, 1);
    typeToDataMap[RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED].emplace_back(data1);
    typeToDataMap[RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED].emplace_back(data2);
    Parcel parcel;
    bool marshallingResult = RSIpcPersistenceManager::Marshalling(parcel, typeToDataMap);
    EXPECT_TRUE(marshallingResult);
    auto unmarshallingResult = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_TRUE(unmarshallingResult.has_value());
    EXPECT_EQ(unmarshallingResult.value().size(), 2u);
}

HWTEST_F(RSIpcPersistenceManagerTest, MultipleOperationsTest001, TestSize.Level1)
{
    auto data1 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::SET_WATERMARK, 1000);
    auto data2 = std::make_shared<MockPersistenceData>(RSIpcPersistenceType::ON_HWC_EVENT, 2000);
    auto data3 = std::make_shared<MockPersistenceData>(
        RSIpcPersistenceType::SHOW_WATERMARK, IPC_PERSISTENCE_DEFAULT_PID);
    
    manager_->RegisterWithCallingPid(data1);
    manager_->RegisterWithCallingPid(data2);
    manager_->RegisterWithoutCallingPid(data3);
    
    auto replayData = manager_->GetReplayData();
    EXPECT_EQ(replayData.size(), 3u);
    
    manager_->UnregisterByCallingPid(1000);
    replayData = manager_->GetReplayData();
    EXPECT_EQ(replayData.size(), 2u);
    
    manager_->UnregisterByType(RSIpcPersistenceType::ON_HWC_EVENT);
    replayData = manager_->GetReplayData();
    EXPECT_EQ(replayData.size(), 1u);
}

HWTEST_F(RSIpcPersistenceManagerTest, DefaultPidTest001, TestSize.Level1)
{
    EXPECT_EQ(IPC_PERSISTENCE_DEFAULT_PID, -1);
}

HWTEST_F(RSIpcPersistenceManagerTest, TypeEnumTest001, TestSize.Level1)
{
    EXPECT_EQ(static_cast<uint32_t>(RSIpcPersistenceType::SET_WATERMARK), 0u);
    EXPECT_EQ(static_cast<uint32_t>(RSIpcPersistenceType::SHOW_WATERMARK), 1u);
    EXPECT_EQ(static_cast<uint32_t>(RSIpcPersistenceType::ON_HWC_EVENT), 2u);
    EXPECT_EQ(static_cast<uint32_t>(RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED), 3u);
    EXPECT_EQ(static_cast<uint32_t>(RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED), 4u);
    EXPECT_EQ(static_cast<uint32_t>(RSIpcPersistenceType::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK), 5u);
    EXPECT_EQ(static_cast<uint32_t>(RSIpcPersistenceType::DEFAULT), 0xFFFFFFFF);
}

} // namespace OHOS::Rosen