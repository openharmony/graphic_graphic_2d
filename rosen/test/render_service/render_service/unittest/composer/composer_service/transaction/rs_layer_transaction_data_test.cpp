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
    ScreenInfo si {};
    si.id = 10u;
    si.width = 1920;
    si.height = 1080;
    data.SetScreenInfo(si);
    data.SetIsScreenInfoChanged(true);
    PipelineParam pp {};
    pp.vsyncId = 5u;
    pp.SurfaceFpsOpNum = 0;
    data.SetPipelineParam(pp);

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
} // namespace OHOS::Rosen