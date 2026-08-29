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

#include <limits>
#include "message_parcel.h"
#include "pixel_map.h"
#include "transfers/rs_set_show_refresh_rate_enabled_transfer.h"
#include "transfers/rs_set_behind_window_filter_enabled_transfer.h"
#include "transfers/rs_on_hwc_event_transfer.h"
#include "transfers/rs_set_watermark_transfer.h"
#include "transfers/rs_show_watermark_transfer.h"
#include "transfers/rs_self_drawing_node_rect_change_callback_transfer.h"
#include "platform/common/rs_log.h"
#include "rs_render_pipeline_agent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSIpcTransferTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

namespace {
constexpr uint32_t MAX_ENTRIES = std::numeric_limits<uint32_t>::max();
constexpr uint32_t MAX_PID_SIZE_NUMBER = 100000; // must match SelfDrawingNodeRectChangeCallbackInput cap
}

// ============================ SetShowRefreshRateEnabled ============================

/**
 * @tc.name: SetShowRefreshRateEnabledTransfer_001
 * @tc.desc: StubUnmarshalling with valid payload returns non-null transfer and errCode 0
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetShowRefreshRateEnabledTransfer_001, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteBool(true));
    ASSERT_TRUE(parcel.WriteInt32(1));
    int32_t errCode = 0;
    auto transfer = SetShowRefreshRateEnabledTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(transfer, nullptr);
    EXPECT_EQ(errCode, 0);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTransfer_002
 * @tc.desc: StubUnmarshalling with empty parcel returns nullptr and ERR_INVALID_DATA
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetShowRefreshRateEnabledTransfer_002, TestSize.Level2)
{
    MessageParcel parcel; // empty
    int32_t errCode = 0;
    auto transfer = SetShowRefreshRateEnabledTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTransfer_003
 * @tc.desc: ProxyMarshalling with valid input succeeds
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetShowRefreshRateEnabledTransfer_003, TestSize.Level1)
{
    auto input = std::make_shared<SetShowRefreshRateEnabledInput>(true, 1);
    SetShowRefreshRateEnabledTransfer transfer(input);
    MessageParcel parcel;
    EXPECT_TRUE(transfer.ProxyMarshalling(parcel));
}

/**
 * @tc.name: SetShowRefreshRateEnabledTransfer_004
 * @tc.desc: ProxyMarshalling with null input fails
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetShowRefreshRateEnabledTransfer_004, TestSize.Level2)
{
    SetShowRefreshRateEnabledTransfer transfer(std::shared_ptr<SetShowRefreshRateEnabledInput>{nullptr});
    MessageParcel parcel;
    EXPECT_FALSE(transfer.ProxyMarshalling(parcel));
}

/**
 * @tc.name: SetShowRefreshRateEnabledTransfer_005
 * @tc.desc: Persist stores the transfer under its typeId in the persistence map
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetShowRefreshRateEnabledTransfer_005, TestSize.Level1)
{
    auto input = std::make_shared<SetShowRefreshRateEnabledInput>(true, 1);
    auto transfer = std::make_shared<SetShowRefreshRateEnabledTransfer>(input);
    IpcPersistenceMap map;
    std::mutex mtx;
    EXPECT_NO_FATAL_FAILURE(transfer->Persist(map, mtx));
    EXPECT_NE(map.find(RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED), map.end());
}

/**
 * @tc.name: SetShowRefreshRateEnabledTransfer_006
 * @tc.desc: transfer attribute getters return the declared sync/persist/fanout contract
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetShowRefreshRateEnabledTransfer_006, TestSize.Level1)
{
    auto input = std::make_shared<SetShowRefreshRateEnabledInput>(true, 1);
    SetShowRefreshRateEnabledTransfer t(input);
    EXPECT_EQ(t.GetTypeId(), RSIServiceToRenderConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);
    EXPECT_TRUE(t.IsPersistent());
    EXPECT_TRUE(t.IsSync());
    EXPECT_EQ(t.GetFanoutPolicy(), FanoutPolicy::ANY_SUCCESS);
    EXPECT_EQ(t.GetReplyResult(), Detail::REPLY_RESULT_PENDING);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTransfer_007
 * @tc.desc: reply marshalling roundtrip: no replyData before ProxyUnmarshalling, then succeeds
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetShowRefreshRateEnabledTransfer_007, TestSize.Level1)
{
    auto input = std::make_shared<SetShowRefreshRateEnabledInput>(true, 1);
    auto transfer = std::make_shared<SetShowRefreshRateEnabledTransfer>(input);
    MessageParcel reply;
    EXPECT_FALSE(transfer->StubMarshalling(reply)); // replyData_ null before Apply
    EXPECT_TRUE(transfer->ProxyUnmarshalling(reply)); // constructs replyData_, no-op Unmarshalling
    EXPECT_TRUE(transfer->StubMarshalling(reply)); // replyData_ set, empty reply marshals true
}

/**
 * @tc.name: SetShowRefreshRateEnabledTransfer_008
 * @tc.desc: Apply with null agent returns false
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetShowRefreshRateEnabledTransfer_008, TestSize.Level2)
{
    auto input = std::make_shared<SetShowRefreshRateEnabledInput>(true, 1);
    SetShowRefreshRateEnabledTransfer transfer(input);
    EXPECT_FALSE(transfer.Apply(nullptr));
}

// ============================ SetBehindWindowFilterEnabled ============================

/**
 * @tc.name: SetBehindWindowFilterEnabledTransfer_001
 * @tc.desc: StubUnmarshalling with valid payload returns non-null transfer and errCode 0
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetBehindWindowFilterEnabledTransfer_001, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteBool(false));
    int32_t errCode = 0;
    auto transfer = SetBehindWindowFilterEnabledTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(transfer, nullptr);
    EXPECT_EQ(errCode, 0);
}

/**
 * @tc.name: SetBehindWindowFilterEnabledTransfer_002
 * @tc.desc: StubUnmarshalling with empty parcel returns nullptr and ERR_INVALID_DATA
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetBehindWindowFilterEnabledTransfer_002, TestSize.Level2)
{
    MessageParcel parcel; // empty
    int32_t errCode = 0;
    auto transfer = SetBehindWindowFilterEnabledTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetBehindWindowFilterEnabledTransfer_003
 * @tc.desc: transfer attribute getters return the declared sync/persist/fanout contract
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetBehindWindowFilterEnabledTransfer_003, TestSize.Level1)
{
    auto input = std::make_shared<SetBehindWindowFilterEnabledInput>(true);
    SetBehindWindowFilterEnabledTransfer t(input);
    EXPECT_EQ(t.GetTypeId(), RSIServiceToRenderConnectionInterfaceCode::SET_BEHIND_WINDOW_FILTER_ENABLED);
    EXPECT_TRUE(t.IsPersistent());
    EXPECT_TRUE(t.IsSync());
    EXPECT_EQ(t.GetFanoutPolicy(), FanoutPolicy::ANY_SUCCESS);
    EXPECT_EQ(t.GetReplyResult(), Detail::REPLY_RESULT_PENDING);
}

/**
 * @tc.name: SetBehindWindowFilterEnabledTransfer_004
 * @tc.desc: full proxy->stub roundtrip and reply marshalling roundtrip
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetBehindWindowFilterEnabledTransfer_004, TestSize.Level1)
{
    auto input = std::make_shared<SetBehindWindowFilterEnabledInput>(false);
    auto transfer = std::make_shared<SetBehindWindowFilterEnabledTransfer>(input);
    MessageParcel parcel;
    ASSERT_TRUE(transfer->ProxyMarshalling(parcel));
    int32_t errCode = 0;
    auto deser = SetBehindWindowFilterEnabledTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(deser, nullptr);
    EXPECT_EQ(errCode, 0);
    MessageParcel reply;
    EXPECT_FALSE(transfer->StubMarshalling(reply));
    EXPECT_TRUE(transfer->ProxyUnmarshalling(reply));
    EXPECT_TRUE(transfer->StubMarshalling(reply));
}

/**
 * @tc.name: SetBehindWindowFilterEnabledTransfer_005
 * @tc.desc: Apply with null agent returns false
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetBehindWindowFilterEnabledTransfer_005, TestSize.Level2)
{
    auto input = std::make_shared<SetBehindWindowFilterEnabledInput>(false);
    SetBehindWindowFilterEnabledTransfer transfer(input);
    EXPECT_FALSE(transfer.Apply(nullptr));
}

// ============================ OnHwcEvent ============================

/**
 * @tc.name: OnHwcEventTransfer_001
 * @tc.desc: StubUnmarshalling with valid payload returns non-null transfer and errCode 0
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, OnHwcEventTransfer_001, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(1u));
    ASSERT_TRUE(parcel.WriteUint32(2u));
    ASSERT_TRUE(parcel.WriteInt32Vector({1, 2, 3}));
    int32_t errCode = 0;
    auto transfer = OnHwcEventTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(transfer, nullptr);
    EXPECT_EQ(errCode, 0);
}

/**
 * @tc.name: OnHwcEventTransfer_002
 * @tc.desc: StubUnmarshalling with empty parcel returns nullptr and ERR_INVALID_DATA
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, OnHwcEventTransfer_002, TestSize.Level2)
{
    MessageParcel parcel; // empty
    int32_t errCode = 0;
    auto transfer = OnHwcEventTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: OnHwcEventTransfer_003
 * @tc.desc: transfer attribute getters return the declared async/persist/fanout contract
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, OnHwcEventTransfer_003, TestSize.Level1)
{
    auto input = std::make_shared<OnHwcEventInput>(1u, 2u, std::vector<int32_t>{1});
    OnHwcEventTransfer t(input);
    EXPECT_EQ(t.GetTypeId(), RSIServiceToRenderConnectionInterfaceCode::HANDLE_HWC_EVENT);
    EXPECT_TRUE(t.IsPersistent());
    EXPECT_FALSE(t.IsSync());
    EXPECT_EQ(t.GetFanoutPolicy(), FanoutPolicy::ANY_SUCCESS);
    EXPECT_EQ(t.GetReplyResult(), Detail::REPLY_RESULT_PENDING);
}

/**
 * @tc.name: OnHwcEventTransfer_004
 * @tc.desc: full proxy->stub roundtrip preserves the event payload
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, OnHwcEventTransfer_004, TestSize.Level1)
{
    auto input = std::make_shared<OnHwcEventInput>(1u, 2u, std::vector<int32_t>{1, 2, 3});
    auto transfer = std::make_shared<OnHwcEventTransfer>(input);
    MessageParcel parcel;
    ASSERT_TRUE(transfer->ProxyMarshalling(parcel));
    int32_t errCode = 0;
    auto deser = OnHwcEventTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(deser, nullptr);
    EXPECT_EQ(errCode, 0);
}

/**
 * @tc.name: OnHwcEventTransfer_005
 * @tc.desc: input Unmarshalling rejects data vector larger than HWC_EVENT_DATA_SIZE_MAX(100)
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, OnHwcEventTransfer_005, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(1u)); // deviceId
    ASSERT_TRUE(parcel.WriteUint32(2u)); // eventId
    std::vector<int32_t> big(101, 1); // > HWC_EVENT_DATA_SIZE_MAX(100)
    ASSERT_TRUE(parcel.WriteInt32Vector(big));
    int32_t errCode = 0;
    auto input = OnHwcEventInput::Unmarshalling(parcel, errCode);
    EXPECT_EQ(input, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: OnHwcEventTransfer_006
 * @tc.desc: Apply with null agent returns false
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, OnHwcEventTransfer_006, TestSize.Level2)
{
    auto input = std::make_shared<OnHwcEventInput>(1u, 2u, std::vector<int32_t>{1, 2, 3});
    OnHwcEventTransfer transfer(input);
    EXPECT_FALSE(transfer.Apply(nullptr));
}

// ============================ UnRegisterSelfDrawing ============================

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_001
 * @tc.desc: StubUnmarshalling with valid pid returns non-null transfer and errCode 0
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_001, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteInt32(42));
    int32_t errCode = 0;
    auto transfer = UnRegisterSelfDrawingNodeRectChangeCallbackTransfer::StubUnmarshalling(
        parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(transfer, nullptr);
    EXPECT_EQ(errCode, 0);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_002
 * @tc.desc: StubUnmarshalling with empty parcel returns nullptr and ERR_INVALID_DATA
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_002, TestSize.Level2)
{
    MessageParcel parcel; // empty
    int32_t errCode = 0;
    auto transfer = UnRegisterSelfDrawingNodeRectChangeCallbackTransfer::StubUnmarshalling(
        parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_003
 * @tc.desc: transfer attribute getters return the declared sync/persist/fanout contract
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_003, TestSize.Level1)
{
    auto input = std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackInput>(42);
    UnRegisterSelfDrawingNodeRectChangeCallbackTransfer t(input);
    EXPECT_EQ(t.GetTypeId(),
        RSIServiceToRenderConnectionInterfaceCode::UNREGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    EXPECT_TRUE(t.IsPersistent());
    EXPECT_TRUE(t.IsSync());
    EXPECT_EQ(t.GetFanoutPolicy(), FanoutPolicy::FAIL_FAST);
    EXPECT_EQ(t.GetReplyResult(), Detail::REPLY_RESULT_PENDING);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_004
 * @tc.desc: full roundtrip including sync reply result marshalling both directions
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_004, TestSize.Level1)
{
    auto input = std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackInput>(42);
    auto transfer = std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackTransfer>(input);
    MessageParcel parcel;
    ASSERT_TRUE(transfer->ProxyMarshalling(parcel));
    int32_t errCode = 0;
    auto deser = UnRegisterSelfDrawingNodeRectChangeCallbackTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(deser, nullptr);
    EXPECT_EQ(errCode, 0);
    // SYNC reply carries int32 result
    MessageParcel reply;
    ASSERT_TRUE(reply.WriteInt32(7));
    EXPECT_TRUE(transfer->ProxyUnmarshalling(reply)); // reads result=7
    EXPECT_EQ(transfer->GetReplyResult(), 7);
    MessageParcel replyOut;
    EXPECT_TRUE(transfer->StubMarshalling(replyOut)); // writes result=7
    int32_t out = 0;
    ASSERT_TRUE(replyOut.ReadInt32(out));
    EXPECT_EQ(out, 7);
}

/**
 * @tc.name: UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_005
 * @tc.desc: Apply with null agent returns false
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, UnRegisterSelfDrawingNodeRectChangeCallbackTransfer_005, TestSize.Level2)
{
    auto input = std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackInput>(42);
    UnRegisterSelfDrawingNodeRectChangeCallbackTransfer transfer(input);
    EXPECT_FALSE(transfer.Apply(nullptr));
}

// ============================ SetWatermark (multi-entry) ============================

/**
 * @tc.name: SetWatermarkTransfer_001
 * @tc.desc: StubUnmarshalling with empty parcel (count read fails) returns nullptr
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetWatermarkTransfer_001, TestSize.Level2)
{
    MessageParcel parcel; // empty, count read fails
    int32_t errCode = 0;
    auto transfer = SetWatermarkTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetWatermarkTransfer_002
 * @tc.desc: StubUnmarshalling with count=1 but no entry data fails on pid read
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetWatermarkTransfer_002, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(1)); // count=1, but no entry data
    int32_t errCode = 0;
    auto transfer = SetWatermarkTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA); // pid read fail -> ERR_INVALID_DATA
}

/**
 * @tc.name: SetWatermarkTransfer_003
 * @tc.desc: transfer attribute getters return the declared async/persist/FAIL_FAST contract
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetWatermarkTransfer_003, TestSize.Level1)
{
    std::map<pid_t, std::shared_ptr<SetWatermarkInput>> empty;
    SetWatermarkTransfer t(std::move(empty));
    EXPECT_EQ(t.GetTypeId(), RSIServiceToRenderConnectionInterfaceCode::SET_WATERMARK);
    EXPECT_TRUE(t.IsPersistent());
    EXPECT_FALSE(t.IsSync());
    EXPECT_EQ(t.GetFanoutPolicy(), FanoutPolicy::FAIL_FAST);
    EXPECT_EQ(t.GetReplyResult(), Detail::REPLY_RESULT_PENDING);
}

/**
 * @tc.name: SetWatermarkTransfer_004
 * @tc.desc: proxy->stub roundtrip with empty entry map (count=0)
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetWatermarkTransfer_004, TestSize.Level1)
{
    std::map<pid_t, std::shared_ptr<SetWatermarkInput>> empty;
    auto transfer = std::make_shared<SetWatermarkTransfer>(std::move(empty));
    MessageParcel parcel;
    ASSERT_TRUE(transfer->ProxyMarshalling(parcel)); // count=0
    int32_t errCode = 0;
    auto deser = SetWatermarkTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(deser, nullptr);
    EXPECT_EQ(errCode, 0);
}

/**
 * @tc.name: SetWatermarkTransfer_005
 * @tc.desc: StubUnmarshalling rejects count > MAX_WATERMARK_ENTRIES(100) cap
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetWatermarkTransfer_005, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(101)); // count > MAX_WATERMARK_ENTRIES(100) cap
    int32_t errCode = 0;
    auto transfer = SetWatermarkTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetWatermarkTransfer_006
 * @tc.desc: Apply with null agent must not crash whatever the feature flag is
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetWatermarkTransfer_006, TestSize.Level2)
{
    std::map<pid_t, std::shared_ptr<SetWatermarkInput>> inputs;
    inputs[42] = std::make_shared<SetWatermarkInput>(42, "test", nullptr, 1, 1);
    SetWatermarkTransfer transfer(std::move(inputs));
    // Feature disabled -> returns true (skip); enabled -> returns false (null agent).
    // Either way must not crash.
    EXPECT_NO_FATAL_FAILURE(transfer.Apply(nullptr));
}

/**
 * @tc.name: SetWatermarkTransfer_007
 * @tc.desc: StubUnmarshalling rejects an entry with rowCount=256 exceeding the 255 grid cap
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetWatermarkTransfer_007, TestSize.Level2)
{
    // Write a single entry with rowCount=256 (exceeds MAX_WATERMARK_GRID_COUNT=255).
    // Need valid pid + name + PixelMap before reaching the grid check.
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    auto pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);

    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(1)); // count
    ASSERT_TRUE(parcel.WriteInt32(42)); // pid
    ASSERT_TRUE(parcel.WriteString("test"));
    ASSERT_TRUE(parcel.WriteParcelable(pixelMap.get()));
    ASSERT_TRUE(parcel.WriteUint32(256)); // rowCount > 255
    ASSERT_TRUE(parcel.WriteUint32(1)); // colCount
    int32_t errCode = 0;
    auto transfer = SetWatermarkTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: SetWatermarkTransfer_008
 * @tc.desc: StubUnmarshalling accepts an entry at the 255/255 grid boundary
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SetWatermarkTransfer_008, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    auto pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);

    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(1)); // count
    ASSERT_TRUE(parcel.WriteInt32(42)); // pid
    ASSERT_TRUE(parcel.WriteString("test"));
    ASSERT_TRUE(parcel.WriteParcelable(pixelMap.get()));
    ASSERT_TRUE(parcel.WriteUint32(255)); // rowCount = 255 (boundary ok)
    ASSERT_TRUE(parcel.WriteUint32(255)); // colCount = 255
    int32_t errCode = 0;
    auto transfer = SetWatermarkTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(transfer, nullptr);
    EXPECT_EQ(errCode, 0);
}

// ============================ ShowWatermark ============================

/**
 * @tc.name: ShowWatermarkTransfer_001
 * @tc.desc: StubUnmarshalling with empty parcel returns nullptr and ERR_INVALID_DATA
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, ShowWatermarkTransfer_001, TestSize.Level2)
{
    MessageParcel parcel; // empty
    int32_t errCode = 0;
    auto transfer = ShowWatermarkTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: ShowWatermarkTransfer_002
 * @tc.desc: transfer attribute getters return the declared async/persist/fanout contract
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, ShowWatermarkTransfer_002, TestSize.Level1)
{
    auto input = std::make_shared<ShowWatermarkInput>(std::shared_ptr<Media::PixelMap>{}, true);
    ShowWatermarkTransfer t(input);
    EXPECT_EQ(t.GetTypeId(), RSIServiceToRenderConnectionInterfaceCode::SHOW_WATERMARK);
    EXPECT_TRUE(t.IsPersistent());
    EXPECT_FALSE(t.IsSync());
    EXPECT_EQ(t.GetFanoutPolicy(), FanoutPolicy::ANY_SUCCESS);
    EXPECT_EQ(t.GetReplyResult(), Detail::REPLY_RESULT_PENDING);
}

/**
 * @tc.name: ShowWatermarkTransfer_003
 * @tc.desc: Apply with null agent returns false
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, ShowWatermarkTransfer_003, TestSize.Level2)
{
    auto input = std::make_shared<ShowWatermarkInput>(nullptr, true);
    ShowWatermarkTransfer transfer(input);
    EXPECT_FALSE(transfer.Apply(nullptr));
}

/**
 * @tc.name: ShowWatermarkTransfer_004
 * @tc.desc: input Unmarshalling rejects a null PixelMap with ERR_INVALID_DATA
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, ShowWatermarkTransfer_004, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteParcelable(nullptr)); // null PixelMap
    ASSERT_TRUE(parcel.WriteBool(true));
    int32_t errCode = 0;
    auto input = ShowWatermarkInput::Unmarshalling(parcel, errCode);
    EXPECT_EQ(input, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

// ============================ SelfDrawing Register (multi-entry) ============================

/**
 * @tc.name: SelfDrawingNodeRectChangeCallbackTransfer_001
 * @tc.desc: StubUnmarshalling with empty parcel (count read fails) returns nullptr
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SelfDrawingNodeRectChangeCallbackTransfer_001, TestSize.Level2)
{
    MessageParcel parcel; // empty, count read fails
    int32_t errCode = 0;
    auto transfer = SelfDrawingNodeRectChangeCallbackTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: SelfDrawingNodeRectChangeCallbackTransfer_002
 * @tc.desc: StubUnmarshalling with count=1 but no entry data fails on pid read
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SelfDrawingNodeRectChangeCallbackTransfer_002, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint32(1)); // count=1, but no entry data
    int32_t errCode = 0;
    auto transfer = SelfDrawingNodeRectChangeCallbackTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_EQ(transfer, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA); // pid read fail -> ERR_INVALID_DATA
}

/**
 * @tc.name: SelfDrawingNodeRectChangeCallbackTransfer_003
 * @tc.desc: transfer attribute getters return the declared sync/persist/FAIL_FAST contract
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SelfDrawingNodeRectChangeCallbackTransfer_003, TestSize.Level1)
{
    std::map<pid_t, std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput>> empty;
    SelfDrawingNodeRectChangeCallbackTransfer t(std::move(empty));
    EXPECT_EQ(t.GetTypeId(),
        RSIServiceToRenderConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    EXPECT_TRUE(t.IsPersistent());
    EXPECT_TRUE(t.IsSync());
    EXPECT_EQ(t.GetFanoutPolicy(), FanoutPolicy::FAIL_FAST);
    EXPECT_EQ(t.GetReplyResult(), Detail::REPLY_RESULT_PENDING);
}

/**
 * @tc.name: SelfDrawingNodeRectChangeCallbackTransfer_004
 * @tc.desc: proxy->stub roundtrip with empty entry map (count=0)
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SelfDrawingNodeRectChangeCallbackTransfer_004, TestSize.Level1)
{
    std::map<pid_t, std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput>> empty;
    auto transfer = std::make_shared<SelfDrawingNodeRectChangeCallbackTransfer>(std::move(empty));
    MessageParcel parcel;
    ASSERT_TRUE(transfer->ProxyMarshalling(parcel)); // count=0
    int32_t errCode = 0;
    auto deser = SelfDrawingNodeRectChangeCallbackTransfer::StubUnmarshalling(parcel, MAX_ENTRIES, errCode);
    EXPECT_NE(deser, nullptr);
    EXPECT_EQ(errCode, 0);
}

/**
 * @tc.name: SelfDrawingNodeRectChangeCallbackTransfer_005
 * @tc.desc: input Unmarshalling rejects pidsSize > MAX_PID_SIZE_NUMBER(100000) cap
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SelfDrawingNodeRectChangeCallbackTransfer_005, TestSize.Level2)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteInt32(42)); // pid
    ASSERT_TRUE(parcel.WriteUint32(MAX_PID_SIZE_NUMBER + 1)); // pidsSize > MAX_PID_SIZE_NUMBER(100000)
    int32_t errCode = 0;
    auto input = SelfDrawingNodeRectChangeCallbackInput::Unmarshalling(parcel, errCode);
    EXPECT_EQ(input, nullptr);
    EXPECT_EQ(errCode, ERR_INVALID_DATA);
}

/**
 * @tc.name: SelfDrawingNodeRectChangeCallbackTransfer_006
 * @tc.desc: Apply with null agent returns false
 * @tc.type: FUNC
 * @tc.require: issueI9KXXE
 */
HWTEST_F(RSIpcTransferTest, SelfDrawingNodeRectChangeCallbackTransfer_006, TestSize.Level2)
{
    std::map<pid_t, std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput>> inputs;
    inputs[42] = std::make_shared<SelfDrawingNodeRectChangeCallbackInput>(42, RectConstraint{}, nullptr);
    SelfDrawingNodeRectChangeCallbackTransfer transfer(std::move(inputs));
    EXPECT_FALSE(transfer.Apply(nullptr));
}

} // namespace OHOS::Rosen
