/**
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
#include <memory>
#include <set>
#include "message_parcel.h"
#include "rs_layer_transaction_data.h"
#include "rs_layer_parcel.h"
#define UnmarshallingFunc RSLayerParcelFactory_UnmarshallingFunc
#include "rs_surface_layer_parcel.h"
#undef UnmarshallingFunc
#include "rs_render_layer_cmd.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {
class RSLayerTransactionDataTest : public Test {};

/**
 * Function: Marshalling_Unmarshalling_BasicPaths
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. prepare RSLayerTransactionData with screenInfo/pipelineParam
 *                  2. set empty payload and marshalling
 *                  3. unmarshalling back and verify non-null
 */
HWTEST_F(RSLayerTransactionDataTest, Marshalling_Unmarshalling_BasicPaths, TestSize.Level1)
{
    RSLayerTransactionData data;
    data.SetTimestamp(1u);
    data.SetSendingPid(2);
    data.SetIndex(3u);
    ComposerInfo composerInfo {};
    composerInfo.composerScreenInfo.id = 10u;
    composerInfo.composerScreenInfo.width = 1920;
    composerInfo.composerScreenInfo.height = 1080;
    composerInfo.pipelineParam.vsyncId = 5u;
    composerInfo.pipelineParam.SurfaceFpsOpNum = 0;
    data.SetComposerInfo(composerInfo);

    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(data.Marshalling(parcel));
    RSLayerTransactionData* out = RSLayerTransactionData::Unmarshalling(*parcel);
    ASSERT_NE(out, nullptr);
    delete out;
}

/**
 * Function: Unmarshalling_InvalidPayloadSize_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. craft parcel with invalid payload size beyond readable
 *                  2. expect Unmarshalling returns nullptr
 */
HWTEST_F(RSLayerTransactionDataTest, Unmarshalling_InvalidPayloadSize_Fail, TestSize.Level1)
{
    MessageParcel parcel;
    parcel.WriteInt32(0); // isNormalParcel
    parcel.WriteInt32(1000000); // huge payload size -> fail
    RSLayerTransactionData* out = RSLayerTransactionData::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Marshalling_WithNullParcelEntries
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. payload with one null RSLayerParcel entry
 *                  2. marshalling should succeed and set hasRsLayerParcel=0 path
 */
HWTEST_F(RSLayerTransactionDataTest, Marshalling_WithNullParcelEntries, TestSize.Level1)
{
    RSLayerTransactionData data;
    auto& payload = data.GetPayload();
    payload.emplace_back(static_cast<uint64_t>(1u), std::shared_ptr<RSLayerParcel>(nullptr));
    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    EXPECT_TRUE(data.Marshalling(parcel));
}

/**
 * Function: Unmarshalling_PayloadWithNullParcel_ZeroCommands
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. payload contains a null RSLayerParcel entry
 *                  2. marshal/unmarshal and verify command count is zero
 */
HWTEST_F(RSLayerTransactionDataTest, Unmarshalling_PayloadWithNullParcel_ZeroCommands, TestSize.Level1)
{
    RSLayerTransactionData data;
    auto& payload = data.GetPayload();
    payload.emplace_back(static_cast<uint64_t>(42u), std::shared_ptr<RSLayerParcel>(nullptr));
    ComposerInfo info {};
    info.composerScreenInfo.id = 1u;
    info.pipelineParam.vsyncId = 1u;
    data.SetComposerInfo(info);
    std::shared_ptr<MessageParcel> mp = std::make_shared<MessageParcel>();
    ASSERT_TRUE(data.Marshalling(mp));
    RSLayerTransactionData* out = RSLayerTransactionData::Unmarshalling(*mp);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetCommandCount(), 0u);
    delete out;
}

/**
 * Function: Unmarshalling_FullSuccess_OneValidParcel
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. make a transaction with one valid RSUpdateRSLayerCmd
 *                  2. marshal/unmarshal and verify payload size=1
 */
HWTEST_F(RSLayerTransactionDataTest, Unmarshalling_FullSuccess_OneValidParcel, TestSize.Level1)
{
    // Build transaction data with one valid parcel
    RSLayerTransactionData data;
    RSLayerId id = static_cast<RSLayerId>(2u);
    auto prop = std::make_shared<OHOS::Rosen::RSRenderLayerCmdProperty<int32_t>>(3);
    auto zCmd = std::make_shared<OHOS::Rosen::RSRenderLayerZorderCmd>(prop);
    std::shared_ptr<OHOS::Rosen::RSLayerParcel> parcel = std::make_shared<OHOS::Rosen::RSUpdateRSLayerCmd>(id, zCmd);
    data.AddRSLayerParcel(parcel, id);
    ComposerInfo info {};
    info.composerScreenInfo.id = 1u;
    info.pipelineParam.vsyncId = 1u;
    info.pipelineParam.SurfaceFpsOpNum = 0;
    data.SetComposerInfo(info);
    std::shared_ptr<MessageParcel> mp = std::make_shared<MessageParcel>();
    ASSERT_TRUE(data.Marshalling(mp));
    RSLayerTransactionData* out = RSLayerTransactionData::Unmarshalling(*mp);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetCommandCount(), 1u);
    delete out;
}

/**
 * Function: Marshalling_NullParcel_Fail
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. pass nullptr parcel to Marshalling
 *                  2. expect Marshalling returns false
 */
HWTEST_F(RSLayerTransactionDataTest, Marshalling_NullParcel_Fail, TestSize.Level1)
{
    RSLayerTransactionData data;
    std::shared_ptr<MessageParcel> nullParcel;
    EXPECT_FALSE(data.Marshalling(nullParcel));
}

/**
 * Function: Unmarshalling_InvalidType_Fail
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. craft parcel with one payload entry and invalid rsLayerParcelType
 *                  2. expect Unmarshalling returns nullptr
 */
HWTEST_F(RSLayerTransactionDataTest, Unmarshalling_InvalidType_Fail, TestSize.Level1)
{
    MessageParcel parcel;
    // payload size = 1
    ASSERT_TRUE(parcel.WriteInt32(1));
    // layerId + hasRsLayerParcel
    ASSERT_TRUE(parcel.WriteUint64(123u));
    ASSERT_TRUE(parcel.WriteUint8(1));
    // invalid type (unregistered)
    ASSERT_TRUE(parcel.WriteUint16(0xFFFF));
    RSLayerTransactionData* out = RSLayerTransactionData::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshalling_TimestampClamped
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. marshal data with large future timestamp
 *                  2. expect unmarshalling clamps timestamp near now
 */
HWTEST_F(RSLayerTransactionDataTest, Unmarshalling_TimestampClamped, TestSize.Level1)
{
    RSLayerTransactionData data;
    // set a far future timestamp
    uint64_t nowNs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    data.SetTimestamp(nowNs + 10000000000ULL); // +10s
    ComposerInfo composerInfo {};
    composerInfo.composerScreenInfo.id = 1u;
    composerInfo.pipelineParam.vsyncId = 1u;
    composerInfo.pipelineParam.SurfaceFpsOpNum = 0;
    data.SetComposerInfo(composerInfo);
    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(data.Marshalling(parcel));
    RSLayerTransactionData* out = RSLayerTransactionData::Unmarshalling(*parcel);
    ASSERT_NE(out, nullptr);
    uint64_t clampedTs = out->GetTimestamp();
    // should be clamped to within ~1s of now
    EXPECT_LE(clampedTs, nowNs + 1500000000ULL);
    delete out;
}

/**
 * Function: Marshalling_Unmarshalling_PipelineParam_SurfaceFpsOps
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. set PipelineParam with SurfaceFpsOpNum>0 and list
 *                  2. marshal/unmarshal and verify ops are present
 */
HWTEST_F(RSLayerTransactionDataTest, Marshalling_Unmarshalling_PipelineParam_SurfaceFpsOps, TestSize.Level1)
{
    RSLayerTransactionData data;
    ComposerInfo info {};
    info.composerScreenInfo.id = 2u;
    info.pipelineParam.vsyncId = 9u;
    info.pipelineParam.isForceRefresh = true;
    info.pipelineParam.hasGameScene = false;
    info.pipelineParam.pendingScreenRefreshRate = 60u;
    info.pipelineParam.pendingConstraintRelativeTime = 100u;
    info.pipelineParam.SurfaceFpsOpNum = 2u;
    info.pipelineParam.SurfaceFpsOpList = {
        SurfaceFpsOp { SURFACE_FPS_ADD, 1001, "s1" },
        SurfaceFpsOp { SURFACE_FPS_REMOVE, 1002, "s2" },
    };
    data.SetComposerInfo(info);
    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(data.Marshalling(parcel));
    RSLayerTransactionData* out = RSLayerTransactionData::Unmarshalling(*parcel);
    ASSERT_NE(out, nullptr);
    auto pp = out->GetPipelineParam();
    EXPECT_EQ(pp.vsyncId, 9u);
    EXPECT_EQ(pp.GetSurfaceFpsOpNum(), 2u);
    ASSERT_EQ(pp.SurfaceFpsOpList.size(), 2u);
    EXPECT_EQ(pp.SurfaceFpsOpList[0].surfaceFpsOpType, SURFACE_FPS_ADD);
    EXPECT_EQ(pp.SurfaceFpsOpList[1].surfaceFpsOpType, SURFACE_FPS_REMOVE);
    delete out;
}
} // namespace OHOS::Rosen