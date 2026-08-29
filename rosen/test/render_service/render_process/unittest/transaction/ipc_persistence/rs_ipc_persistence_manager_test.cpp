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

#include <memory>
#include "message_parcel.h"
#include "rs_ipc_persistence_manager.h"
#include "transfers/rs_set_show_refresh_rate_enabled_transfer.h"
#include "transfers/rs_set_behind_window_filter_enabled_transfer.h"
#include "transfers/rs_on_hwc_event_transfer.h"
#include "transfers/rs_set_watermark_transfer.h"
#include "transfers/rs_show_watermark_transfer.h"
#include "transfers/rs_self_drawing_node_rect_change_callback_transfer.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
// Must stay a typeId that no transfer registers, otherwise the "unregistered typeId" branches
// below would silently stop testing what they claim.
constexpr auto UNREGISTERED_TYPE_ID = static_cast<RSIServiceToRenderConnectionInterfaceCode>(0xFFFF);
// Force ODR-use of each transfer's registrar so factories are registered at static init.
void ForceFactoryRegistration()
{
    [[maybe_unused]] auto* r1 = &TransferRegistrationChecker<SetShowRefreshRateEnabledTransfer>::registrar;
    [[maybe_unused]] auto* r2 = &TransferRegistrationChecker<SetBehindWindowFilterEnabledTransfer>::registrar;
    [[maybe_unused]] auto* r3 = &TransferRegistrationChecker<OnHwcEventTransfer>::registrar;
    [[maybe_unused]] auto* r4 = &TransferRegistrationChecker<SetWatermarkTransfer>::registrar;
    [[maybe_unused]] auto* r5 = &TransferRegistrationChecker<ShowWatermarkTransfer>::registrar;
    [[maybe_unused]] auto* r6 = &TransferRegistrationChecker<SelfDrawingNodeRectChangeCallbackTransfer>::registrar;
    [[maybe_unused]] auto* r7 =
        &TransferRegistrationChecker<UnRegisterSelfDrawingNodeRectChangeCallbackTransfer>::registrar;
}
} // namespace

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
    ForceFactoryRegistration();
}

void RSIpcPersistenceManagerTest::TearDownTestCase() {}

void RSIpcPersistenceManagerTest::SetUp()
{
    manager_ = std::make_shared<RSIpcPersistenceManager>();
}

void RSIpcPersistenceManagerTest::TearDown()
{
    manager_ = nullptr;
}

/**
 * @tc.name: CreateTransferByTypeId001
 * @tc.desc: valid typeId + valid parcel returns non-null transfer
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, CreateTransferByTypeId001, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteBool(true));
    ASSERT_TRUE(parcel.WriteInt32(1));
    int32_t errCode = 0;
    auto transfer = RSIpcPersistenceManager::CreateTransferByTypeId(
        RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED, parcel, errCode);
    EXPECT_NE(transfer, nullptr);
    EXPECT_EQ(errCode, 0);
}

/**
 * @tc.name: CreateTransferByTypeId002
 * @tc.desc: unregistered typeId returns nullptr
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, CreateTransferByTypeId002, TestSize.Level2)
{
    MessageParcel parcel;
    int32_t errCode = 0;
    auto transfer = RSIpcPersistenceManager::CreateTransferByTypeId(
        UNREGISTERED_TYPE_ID, parcel, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: CreateTransferByTypeId003
 * @tc.desc: malformed parcel (missing fields) returns nullptr + sets errCode (D4 per-field)
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, CreateTransferByTypeId003, TestSize.Level2)
{
    MessageParcel parcel; // empty parcel, no fields written
    int32_t errCode = 0;
    auto transfer = RSIpcPersistenceManager::CreateTransferByTypeId(
        RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED, parcel, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: CreateTransferByTypeId004
 * @tc.desc: SetBehindWindowFilterEnabled valid parcel roundtrip
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, CreateTransferByTypeId004, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteBool(true));
    int32_t errCode = 0;
    auto transfer = RSIpcPersistenceManager::CreateTransferByTypeId(
        RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED, parcel, errCode);
    EXPECT_NE(transfer, nullptr);
    EXPECT_EQ(errCode, 0);
}

/**
 * @tc.name: PersistTransfer001
 * @tc.desc: null transfer is a no-op
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, PersistTransfer001, TestSize.Level2)
{
    EXPECT_NO_FATAL_FAILURE(manager_->PersistTransfer(nullptr));
    // null transfer is a no-op: manager stays empty -> snapshot is empty -> Marshalling writes count=0
    auto snap = manager_->GetPersistenceMap();
    MessageParcel parcel;
    EXPECT_TRUE(RSIpcPersistenceManager::Marshalling(parcel, snap));
    uint32_t count = 0;
    ASSERT_TRUE(parcel.ReadUint32(count));
    EXPECT_EQ(count, 0u);
}

/**
 * @tc.name: PersistTransfer002
 * @tc.desc: persistent transfer is stored under its typeId and replayable through the snapshot
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, PersistTransfer002, TestSize.Level1)
{
    constexpr auto typeId = RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED;
    auto input = std::make_shared<SetShowRefreshRateEnabledInput>(true, 1);
    auto transfer = std::make_shared<SetShowRefreshRateEnabledTransfer>(input);
    ASSERT_NE(transfer, nullptr);
    EXPECT_NO_FATAL_FAILURE(manager_->PersistTransfer(transfer));
    auto snap = manager_->GetPersistenceMap();
    EXPECT_EQ(snap.count(typeId), 1u); // transfer really stored under its typeId
    MessageParcel parcel;
    ASSERT_TRUE(RSIpcPersistenceManager::Marshalling(parcel, snap));
    auto replayed = RSIpcPersistenceManager::Unmarshalling(parcel);
    ASSERT_TRUE(replayed.has_value());
    EXPECT_NE(replayed.value().find(typeId), replayed.value().end()); // snapshot is replayable
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: empty manager snapshot + Marshalling writes count=0
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Marshalling001, TestSize.Level2)
{
    auto snap = manager_->GetPersistenceMap();
    MessageParcel parcel;
    EXPECT_TRUE(RSIpcPersistenceManager::Marshalling(parcel, snap));
    uint32_t count = 0;
    ASSERT_TRUE(parcel.ReadUint32(count));
    EXPECT_EQ(count, 0u);
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: populated manager snapshot + Marshalling writes count=1
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Marshalling002, TestSize.Level1)
{
    auto input = std::make_shared<SetShowRefreshRateEnabledInput>(false, 2);
    manager_->PersistTransfer(std::make_shared<SetShowRefreshRateEnabledTransfer>(input));
    auto snap = manager_->GetPersistenceMap();
    MessageParcel parcel;
    EXPECT_TRUE(RSIpcPersistenceManager::Marshalling(parcel, snap));
    uint32_t count = 0;
    ASSERT_TRUE(parcel.ReadUint32(count));
    EXPECT_EQ(count, 1u);
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: empty parcel (count=0) yields empty map
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Unmarshalling001, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(0));
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().empty());
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: typeCount exceeding MAX_MAP_SIZE returns nullopt
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Unmarshalling002, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(0xFFFFFFFF));
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_FALSE(result.has_value());
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc: roundtrip: persist + snapshot + Marshalling + Unmarshalling yields same typeId
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Unmarshalling003, TestSize.Level1)
{
    auto input = std::make_shared<SetShowRefreshRateEnabledInput>(true, 3);
    manager_->PersistTransfer(std::make_shared<SetShowRefreshRateEnabledTransfer>(input));
    auto snap = manager_->GetPersistenceMap();
    MessageParcel parcel;
    ASSERT_TRUE(RSIpcPersistenceManager::Marshalling(parcel, snap));
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value().find(
        RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED), result.value().end());
}

/**
 * @tc.name: Unmarshalling004
 * @tc.desc: invalid typeId in replay stream yields nullopt
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Unmarshalling004, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(1)); // typeCount=1
    ASSERT_TRUE(parcel.WriteUint32(static_cast<uint32_t>(UNREGISTERED_TYPE_ID))); // unregistered typeId
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_FALSE(result.has_value());
}

/**
 * @tc.name: Unmarshalling005
 * @tc.desc: valid typeId but empty payload -> CreateTransferByTypeId nullptr -> nullopt
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Unmarshalling005, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(1)); // typeCount=1
    ASSERT_TRUE(parcel.WriteUint32(static_cast<uint32_t>(
        RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED))); // valid typeId, no payload
    auto result = RSIpcPersistenceManager::Unmarshalling(parcel);
    EXPECT_FALSE(result.has_value());
}

/**
 * @tc.name: Marshalling003
 * @tc.desc: Marshalling rejects an oversized input map (> MAX_MAP_SIZE) before touching the parcel
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Marshalling003, TestSize.Level2)
{
    // Size check fires before per-entry validation, so synthetic typeIds and nullptr transfers are fine.
    IpcPersistenceMap oversized;
    for (uint32_t i = 0; i <= 100; ++i) { // 101 entries > MAX_MAP_SIZE(100)
        oversized[static_cast<RSIServiceToRenderConnectionInterfaceCode>(0xF0000000u | i)] = nullptr;
    }
    MessageParcel parcel;
    EXPECT_FALSE(RSIpcPersistenceManager::Marshalling(parcel, oversized));
}

/**
 * @tc.name: Marshalling004
 * @tc.desc: nullptr transfer entry in snapshot causes Marshalling to return false
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Marshalling004, TestSize.Level2)
{
    manager_->persistedData_[RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED] = nullptr;
    auto snap = manager_->GetPersistenceMap();
    MessageParcel parcel;
    EXPECT_FALSE(RSIpcPersistenceManager::Marshalling(parcel, snap));
}

/**
 * @tc.name: Marshalling005
 * @tc.desc: transfer whose ProxyMarshalling fails (null inputData_) causes Marshalling to return false
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, Marshalling005, TestSize.Level2)
{
    auto transfer = std::make_shared<SetShowRefreshRateEnabledTransfer>(
        std::shared_ptr<SetShowRefreshRateEnabledInput>{nullptr}); // inputData_ null -> ProxyMarshalling false
    ASSERT_TRUE(transfer->IsPersistent());
    manager_->PersistTransfer(transfer); // PersistTransfer inserts it (with null inputData_) into the map
    auto snap = manager_->GetPersistenceMap();
    MessageParcel parcel;
    EXPECT_FALSE(RSIpcPersistenceManager::Marshalling(parcel, snap));
}

/**
 * @tc.name: PersistTransfer003
 * @tc.desc: persisting two distinct transfers stores both under their typeIds
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, PersistTransfer003, TestSize.Level2)
{
    manager_->PersistTransfer(std::make_shared<SetShowRefreshRateEnabledTransfer>(
        std::make_shared<SetShowRefreshRateEnabledInput>(true, 1)));
    manager_->PersistTransfer(std::make_shared<SetBehindWindowFilterEnabledTransfer>(
        std::make_shared<SetBehindWindowFilterEnabledInput>(false)));
    auto snap = manager_->GetPersistenceMap();
    EXPECT_EQ(snap.size(), 2u);
    EXPECT_NE(snap.find(RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED), snap.end());
    EXPECT_NE(snap.find(RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED), snap.end());
}

/**
 * @tc.name: PersistTransfer004
 * @tc.desc: UnRegister SelfDrawing must keep the REGISTER entry (concurrency) and only clear the pid inside
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, PersistTransfer004, TestSize.Level2)
{
    constexpr auto regId = RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK;
    constexpr pid_t registeredPid = 42;
    std::map<pid_t, std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput>> inputs;
    inputs[registeredPid] =
        std::make_shared<SelfDrawingNodeRectChangeCallbackInput>(registeredPid, RectConstraint{}, nullptr);
    auto registerTransfer = std::make_shared<SelfDrawingNodeRectChangeCallbackTransfer>(std::move(inputs));
    manager_->persistedData_[regId] = registerTransfer;
    ASSERT_EQ(manager_->GetPersistenceMap().count(regId), 1u);
    ASSERT_EQ(registerTransfer->inputs_.count(registeredPid), 1u);
    manager_->PersistTransfer(std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackTransfer>(
        std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackInput>(registeredPid)));
    EXPECT_EQ(manager_->GetPersistenceMap().count(regId), 1u); // entry kept, only pid cleared
    EXPECT_EQ(registerTransfer->inputs_.count(registeredPid), 0u); // pid really cleared inside the entry
}

/**
 * @tc.name: PersistTransfer005
 * @tc.desc: when persisted map size >= MAX_PERSIST_MAP_SIZE(100), PersistTransfer does not insert a new transfer
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcPersistenceManagerTest, PersistTransfer005, TestSize.Level2)
{
    auto filler = std::make_shared<SetShowRefreshRateEnabledTransfer>(
        std::make_shared<SetShowRefreshRateEnabledInput>(true, 1));
    for (uint32_t i = 0; i < 100; ++i) { // fill to MAX_PERSIST_MAP_SIZE
        manager_->persistedData_[static_cast<RSIServiceToRenderConnectionInterfaceCode>(0xF0000000u | i)] = filler;
    }
    ASSERT_EQ(manager_->GetPersistenceMap().size(), 100u);
    manager_->PersistTransfer(std::make_shared<SetBehindWindowFilterEnabledTransfer>(
        std::make_shared<SetBehindWindowFilterEnabledInput>(true)));
    // map full -> new typeId not inserted
    EXPECT_EQ(manager_->GetPersistenceMap().count(
        RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED), 0u);
    EXPECT_EQ(manager_->GetPersistenceMap().size(), 100u);
}

} // namespace OHOS::Rosen
